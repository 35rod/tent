#include "parser.hpp"

#include <cstdlib>
#include <fstream>

#include "lexer.hpp"
#include "native.hpp"
#include "errors.hpp"
#include "esc_codes.hpp"
#include "args.hpp"

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

std::vector<std::string> nativeLibs;

Parser::Parser(std::vector<Token> parserTokens, Diagnostics& diagnostics, std::string fname, std::vector<std::string> search_dirs)
: tokens(parserTokens), filename(fname), diags(diagnostics), file_search_dirs(search_dirs) {}

Token Parser::current() {
	if (pos >= tokens.size()) {
		return Token("\0", TokenType::EOF_TOK, tokens.back().span);
	}

	return tokens[pos];
}

Token Parser::peek(int num) {
	if ((pos+num) >= tokens.size()) {
		return Token("\0", TokenType::EOF_TOK, tokens.back().span);
	}

	return tokens[pos+num];
}

Token Parser::advance(int num) {
	if (pos >= tokens.size()) {
		return Token("\0", TokenType::EOF_TOK, tokens.back().span);
	}

	Token token = tokens[pos];
	pos += num;

	return token;
}

Token Parser::expect(TokenType ttype) {
	if (current().kind == ttype) {
		return current();
	}

	diags.report<SyntaxError>(
		"Expected " + tokenTypeToString(ttype) +
		", got " + tokenTypeToString(current().kind), current().span,
		"", filename
	);

	return current();
}

ASTPtr Parser::parse_program() {
	std::vector<ExpressionStmt> stmts;

	while (pos < tokens.size()) {
		try {
			ExpressionStmt&& stmt = parse_statement();
			
			if (!stmt.noOp) {
				if (auto imported = dynamic_cast<Program*>(stmt.expr.get())) {
					std::vector<ExpressionStmt> imported_stmts = std::move(imported->statements);

					stmts.insert(stmts.begin(),
						std::make_move_iterator(imported_stmts.begin()),
						std::make_move_iterator(imported_stmts.end())
					);
				} else {
					stmts.push_back(std::move(stmt));
				}
			}
		} catch (const std::runtime_error& e) {
			std::cerr << "Parse error: " << e.what() << std::endl;

			break;
		}
	}

	return std::make_unique<Program>(std::move(stmts), current().span);
}

std::vector<ExpressionStmt> Parser::parse_block() {
	expect(TokenType::OPEN_BRAC);
	advance();

	std::vector<ExpressionStmt> stmts;

	while (current().kind != TokenType::CLOSE_BRAC) {
		if (current().kind == TokenType::EOF_TOK) {
			diags.report<SyntaxError>(
				"Closing braces required for code block", current().span,
				"Did you forget a closing brace?", filename
			);

			exitErrors();
		}

		ExpressionStmt&& stmt = parse_statement();

		if ((stmt.noOp && stmt.isBreak) || !stmt.noOp) {
			stmts.push_back(std::move(stmt));
		}
	}
	
	advance();

	return stmts;
}

ExpressionStmt Parser::parse_statement() {
	Token token = current();

	if (token.kind == TokenType::LOAD) {
		advance();
		std::string fname = expect(TokenType::STR).text;

		if (peek().kind == TokenType::SEM) {
			advance();
		} else {
			diags.report<MissingTerminatorError>(
				"Missing statement terminator after load statement", current().span,
				"Did you forget a semicolon?", filename
			);

			advance();
		}

		if (fname.size() >= 5 && fname.substr(fname.size() - 5) == ".tent") {
			const auto foundFile = checkSearchPathsFor(fname, file_search_dirs);
			if (!foundFile.has_value()) {
				std::cerr << "File error: could not find file'" << fname << "'" << std::endl;
				exit(1);
			}

			std::ifstream fileHandle(foundFile.value().first + "/" + foundFile.value().second);

			std::string output, line;

			while (std::getline(fileHandle, line)) {
				output += line;
				output.push_back('\n');
			}

			Lexer lexer(output, diags);
			lexer.nextChar();
			lexer.getTokens();

			Parser parser(lexer.tokens, diags, fname, file_search_dirs);

			ASTPtr imported_program = std::make_unique<Program>(std::move(dynamic_cast<Program*>(parser.parse_program().get())->statements), current().span);

			return ExpressionStmt(std::move(imported_program), Span::combine(token.span, current().span), false, false, false);
		}

		using RegisterFn = void(*)(std::unordered_map<std::string, NativeFn>&);

#if defined(_WIN32) || defined(_WIN64)
		HMODULE handle = LoadLibraryA(("lib" + fname).c_str());
		if (!handle) handle = LoadLibraryA(("lib" + fname + ".dll").c_str());

		if (!handle) {
			for (const std::string& dir : file_search_dirs) {
				if (handle) break;
				handle = LoadLibraryA((dir + "/lib" + fname).c_str());
			}
		}

		if (!handle) {
			std::cerr << "Failed to load library: " << fname << std::endl;
			exit(1);
		}

		RegisterFn reg = reinterpret_cast<RegisterFn>(GetProcAddress(handle, "registerFunctions"));

		if (!reg) {
			std::cerr << "Library missing registerFunctions symbol" << std::endl;
			FreeLibrary(handle);
			exit(1);
		}

		reg(nativeFunctions);
#else
		void* handle = NULL;
		for (const std::string& dir : file_search_dirs) {
			if (handle) break;
			handle = dlopen((dir + "/lib" + fname).c_str(), RTLD_LAZY);
			if (!handle) handle = dlopen((dir + "/lib" + fname + ".dylib").c_str(), RTLD_LAZY);
			if (!handle) handle = dlopen((dir + "/lib" + fname + ".so").c_str(), RTLD_LAZY);
		}

		if (!handle) {
			char *err_str = dlerror();
			std::cerr << "Failed to load library: " << fname <<
					(err_str ? (": \n" + std::string(err_str)) : "") << std::endl;
			exit(1);
		}

		RegisterFn reg = reinterpret_cast<RegisterFn>(dlsym(handle, "registerFunctions"));

		if (!reg) {
			std::cerr << "Library missing registerFunctions symbol" << std::endl;
			dlclose(handle);
			exit(1);
		}

		reg(nativeFunctions);
#endif

		nativeLibs.push_back(fname);

		return ExpressionStmt(std::make_unique<NoOp>(), Span::combine(token.span, current().span), true, false, false);
	} else if (token.kind == TokenType::SEM) {
		advance();

		ExpressionStmt expressionStmt = ExpressionStmt(std::make_unique<NoOp>(), current().span, true, false, false);

		return expressionStmt;
	} else if (token.kind == TokenType::FORM || token.kind == TokenType::CLASS) {
		advance();
		Token name = expect(TokenType::IDENT);
		advance();
		expect(TokenType::OPEN_PAREN);
		advance();

		std::vector<ASTPtr> params;

		while (current().kind != TokenType::CLOSE_PAREN) {
			if (current().kind == TokenType::EOF_TOK) {
				diags.report<SyntaxError>(
					"Closing parenthesis required for function/class definition", current().span,
					"Did you forget closing parenthesis?", filename
				);

				exitErrors();
			}

			Token param = expect(TokenType::IDENT);

			params.push_back(std::make_unique<Variable>(param.text, current().span, nullptr));

			advance();

			if (current().kind == TokenType::CLOSE_PAREN) {
				break;
			}

			expect(TokenType::COMMA);

			advance();
		}

		Span parenSpan = current().span;

		advance();
		std::vector<ExpressionStmt> stmts = parse_block();

		ASTPtr res = nullptr;

		if (token.kind == TokenType::FORM) {
			res = std::make_unique<FunctionStmt>(name.text, std::move(params), std::move(stmts), Span::combine(token.span, parenSpan), nullptr);
		} else {
			res = std::make_unique<ClassStmt>(name.text, std::move(params), std::move(stmts), Span::combine(token.span, parenSpan));
		}
		
		return ExpressionStmt(std::move(res), Span::combine(token.span, parenSpan), false, false, false);
	} else if (token.kind == TokenType::RETURN) {
		advance();

		ASTPtr value = parse_expression(0);

		if (peek().kind == TokenType::SEM) {
			advance();
		} else {
			diags.report<MissingTerminatorError>(
				"Missing statement terminator after return statement", current().span,
				"Did you forget a semicolon?", filename
			);

			advance();
		}

		ASTPtr returnStmt = std::make_unique<ReturnStmt>(std::move(value), Span::combine(token.span, current().span));

		return ExpressionStmt(std::move(returnStmt), Span::combine(token.span, current().span), false, false, false);
	} else if (token.kind == TokenType::WHILE) {
		advance();
		ASTPtr condition = parse_expression(0);
		advance();

		std::vector<ExpressionStmt> stmts;

		Span endSpan = current().span;

		if (current().kind != TokenType::OPEN_BRAC) {
			stmts.push_back(parse_statement());
		} else {
			stmts = parse_block();
		}

		ASTPtr whileStmt = std::make_unique<WhileStmt>(std::move(condition), std::move(stmts), Span::combine(token.span, endSpan));

		return ExpressionStmt(std::move(whileStmt), Span::combine(token.span, endSpan), false, false, false);
	} else if (token.kind == TokenType::FOR) {
		advance();
		ASTPtr var = parse_expression(0);
		advance();
		expect(TokenType::ITER);
		advance();
		ASTPtr iter = parse_expression(0);
		advance();

		std::vector<ExpressionStmt> stmts;

		Span endSpan = current().span;

		if (current().kind != TokenType::OPEN_BRAC) {
			stmts.push_back(parse_statement());
		} else {
			stmts = parse_block();
		}

		ASTPtr forStmt = std::make_unique<ForStmt>(dynamic_cast<Variable*>(var.get())->name, std::move(iter), std::move(stmts), Span::combine(token.span, endSpan));

		return ExpressionStmt(std::move(forStmt), Span::combine(token.span, endSpan), false, false, false);
	} else if (token.kind == TokenType::IF) {
		advance();

		ASTPtr condition = parse_expression(0);
		std::vector<ExpressionStmt> thenStmts, elseStmts;

		advance();

		Span endSpan = current().span;

		if (current().kind != TokenType::OPEN_BRAC) {
			thenStmts.push_back(parse_statement());
		} else {
			thenStmts = parse_block();
		}

		if (current().kind != TokenType::ELSE) {
			ASTPtr ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenStmts), Span::combine(token.span, endSpan), std::vector<ExpressionStmt>());

			return ExpressionStmt(std::move(ifStmt), Span::combine(token.span, endSpan), false, false, false);
		}

		advance();

		if (current().kind != TokenType::OPEN_BRAC) {
			elseStmts.push_back(parse_statement());
		} else {
			elseStmts = parse_block();
		}

		ASTPtr ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenStmts), Span::combine(token.span, endSpan), std::move(elseStmts));

		return ExpressionStmt(std::move(ifStmt), Span::combine(token.span, endSpan), false, false, false);
	} else if (token.kind == TokenType::BREAK || token.kind == TokenType::CONTINUE) {
		if (peek().kind == TokenType::SEM)
			advance();
		else {
			diags.report<MissingTerminatorError>(
				"Missing statement terminator after break statement", current().span,
				"Did you forget a semicolon?", filename
			);

			advance();
		}

		return ExpressionStmt(std::make_unique<NoOp>(), Span::combine(token.span, current().span), true, true, token.kind == TokenType::CONTINUE);
	}

	ASTPtr expr = parse_expression(0);

	if (peek().kind == TokenType::SEM) {
		advance();
	} else {
		diags.report<MissingTerminatorError>(
			"Missing statement terminator after expression", current().span,
			"Did you forget a semicolon?", filename
		);

		advance();
	}

	return ExpressionStmt(std::move(expr), Span::combine(token.span, current().span), false, false, false);
}

ASTPtr Parser::parse_expression(int min_bp) {
	Token token = current();

	ASTPtr left;

	if (token.kind == TokenType::BIT_NOT || token.kind == TokenType::NOT || token.kind == TokenType::SUB ||
			token.kind == TokenType::INCREMENT || token.kind == TokenType::DECREMENT) {
		advance();
	
		ASTPtr operand = parse_expression(15);

		TokenType new_token_type = (token.kind == TokenType::SUB)
			? TokenType::NEGATE
			: token.kind;
		
		left = std::make_unique<UnaryOp>(new_token_type, std::move(operand), Span::combine(token.span, current().span));
	} else if (token.kind >= TokenType::INT_HEX && token.kind <= TokenType::INT_BIN) {
		int8_t base = -1;
		if (token.kind == TokenType::INT_HEX)
			base = 16;
		if (token.kind == TokenType::INT_DEC)
			base = 10;
		if (token.kind == TokenType::INT_OCT)
			base = 8;
		if (token.kind == TokenType::INT_BIN)
			base = 2;
		if (base == -1) {
			diags.report<SyntaxError>(
				"Invalid radix base", current().span,
				"Please report this in the Issue Tracker", filename
			);
		}

		left = std::make_unique<IntLiteral>(std::strtoll(token.text.c_str(), NULL, base), current().span);
	} else if (token.kind == TokenType::FLOAT) {
		left = std::make_unique<FloatLiteral>(std::strtof(token.text.c_str(), NULL), current().span);
	} else if (token.kind == TokenType::STR) {
		left = std::make_unique<StrLiteral>(read_escape(token.text), current().span);
	} else if (token.kind == TokenType::TYPE_INT) {
		left = std::make_unique<TypeInt>(current().span);
	} else if (token.kind == TokenType::TYPE_FLOAT) {
		left = std::make_unique<TypeFloat>(current().span);
	} else if (token.kind == TokenType::TYPE_STR) {
		left = std::make_unique<TypeStr>(current().span);
	} else if (token.kind == TokenType::TYPE_BOOL) {
		left = std::make_unique<TypeBool>(current().span);
	} else if (token.kind == TokenType::TYPE_VEC) {
		left = std::make_unique<TypeVec>(current().span);
	} else if (token.kind == TokenType::TYPE_DIC) {
	    left = std::make_unique<TypeDic>(current().span);
	} else if (token.kind == TokenType::CHR) {
		char c = 0;
		get_escape(token.text, &c);
		left = std::make_unique<IntLiteral>(c, current().span);
	} else if (token.kind == TokenType::BOOL) {
		left = std::make_unique<BoolLiteral>(token.text == "true", current().span);
	} else if (token.kind == TokenType::OPEN_BRACKET) {
		advance();

		std::vector<ASTPtr> elems;
		
		while (current().kind != TokenType::CLOSE_BRACKET) {
			if (current().kind == TokenType::EOF_TOK) {
				diags.report<SyntaxError>(
					"Unterminated vector literal", current().span,
					"Did you forget a closing bracket?", filename
				);

				exitErrors();
			}

			ASTPtr elem = parse_expression(0);
			elems.push_back(std::move(elem));

			advance();

			if (current().kind == TokenType::CLOSE_BRACKET)
				break;

			expect(TokenType::COMMA);

			advance();
		}
		
		left = std::make_unique<VecLiteral>(std::move(elems), Span::combine(token.span, current().span));
	} else if (token.kind == TokenType::OPEN_BRAC) {
	    advance();

	    std::map<ASTPtr, ASTPtr> dic;

	    while (current().kind != TokenType::CLOSE_BRAC) {
	        if (current().kind == TokenType::EOF_TOK) {
				diags.report<SyntaxError>(
					"Unterminated dictionary literal", current().span,
					"Did you forget a closing brace?", filename
				);

				exitErrors();
	        }

	        ASTPtr key = parse_expression(0);
	        advance();
	        expect(TokenType::COLON);
	        advance();
	        ASTPtr value = parse_expression(0);
	        dic[std::move(key)] = std::move(value);

	        advance();

	        if (current().kind == TokenType::CLOSE_BRAC)
	            break;

	        expect(TokenType::COMMA);

	        advance();
	    }

	    left = std::make_unique<DicLiteral>(std::move(dic), Span::combine(token.span, current().span));
	} else if (token.kind == TokenType::IDENT) {
		if (peek().kind == TokenType::OPEN_PAREN) {
			advance(2);

			std::vector<ASTPtr> params;

			while (current().kind != TokenType::CLOSE_PAREN) {
				if (current().kind == TokenType::EOF_TOK) {
					diags.report<SyntaxError>(
						"Closing parenthesis required for function call", current().span,
						"Did you forget a closing parenthesis?", filename
					);

					exitErrors();
				}

				ASTPtr param = parse_expression(0);
				params.push_back(std::move(param));

				advance();

				if (current().kind == TokenType::CLOSE_PAREN) {
					break;
				}

				expect(TokenType::COMMA);
				advance();
			}

			ASTPtr call = std::make_unique<FunctionCall>(token.text, std::move(params), Span::combine(token.span, current().span));

			left = std::move(call);
		} else {
			left = std::make_unique<Variable>(token.text, Span::combine(token.span, current().span), nullptr);
		}
	} else if (token.kind == TokenType::OPEN_PAREN) {
		advance();
		left = parse_expression(0);
		advance();
		expect(TokenType::CLOSE_PAREN);
	} else {
		diags.report<SyntaxError>(
			"Unexpected token in expression: " + tokenTypeToString(token.kind) +
			(token.text.empty() ? "" : (", '" + token.text + "'")), current().span,
			"", filename
		);
	}

	while (true) {
		Span startSpan = current().span;

		Token nextToken = peek();

		if (nextToken.kind == TokenType::INCREMENT || nextToken.kind == TokenType::DECREMENT) {
			advance();

			left = std::make_unique<UnaryOp>(nextToken.kind, std::move(left), Span::combine(startSpan, nextToken.span));
			continue;
		}

		if (precedence.count(nextToken.kind) == 0) {
			break;
		}

		int bp = precedence[nextToken.kind];

		if (bp < min_bp) {
			break;
		}

		advance();

		Token op = current();

		advance();

		ASTPtr right = parse_expression(isRightAssoc(op.kind) ? bp : bp+1);
		left = std::make_unique<BinaryOp>(op.kind, std::move(left), std::move(right), Span::combine(startSpan, current().span));
	}

	return left;
}

void Parser::exitErrors() {
	diags.print_errors();
	exit(1);
}