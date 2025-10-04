#include <fstream>
#include "parser.hpp"
#include "native.hpp"
#include "lexer.hpp"
#include "errors.hpp"
#include "esc_codes.hpp"

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

std::vector<std::string> nativeLibs;

Parser::Parser(std::vector<Token> parserTokens, std::vector<std::string> search_dirs)
	: tokens(parserTokens), file_search_dirs(search_dirs) {}

Token Parser::current() {
	if (pos >= tokens.size()) {
		return Token("\0", TokenType::EOF_TOK, tokens[pos-1].lineNo);
	}

	return tokens[pos];
}

Token Parser::peek(int num) {
	if ((pos+num) >= tokens.size()) {
		return Token("\0", TokenType::EOF_TOK, 0);
	}

	return tokens[pos+num];
}

Token Parser::advance(int num) {
	if (pos >= tokens.size()) {
		return Token("\0", TokenType::EOF_TOK, 0);
	}

	Token token = tokens[pos];
	pos += num;

	return token;
}

Token Parser::expect(TokenType ttype) {
	if (current().kind == ttype) {
		return current();
	}

	SyntaxError("Expected " + std::to_string((uint16_t)ttype)
            + ", got " + std::to_string((uint16_t)current().kind), current().lineNo);

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

	return std::make_unique<Program>(std::move(stmts));
}

std::vector<ExpressionStmt> Parser::parse_block() {
	expect(TokenType::OPEN_BRAC);
	advance();

	std::vector<ExpressionStmt> stmts;

	while (current().kind != TokenType::CLOSE_BRAC) {
		if (current().kind == TokenType::EOF_TOK)
			SyntaxError("Closing braces required for code block", current().lineNo);

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
		Token filename = expect(TokenType::STR);

		if (peek().kind == TokenType::SEM) {
			advance();
		} else {
			MissingTerminatorError("Missing statement terminator after load statement", current().lineNo);
		}

		std::string fname = filename.text;

		if (fname.size() >= 3 && fname.substr(fname.size() - 3) == ".nl") {
			std::ifstream fileHandle(fname);

			for (const std::string& dir : file_search_dirs) {
				if (fileHandle.is_open()) break;
				fileHandle = std::ifstream(dir + "/" + fname);
			}

			if (!fileHandle.is_open()) {
				std::cerr << "File error: could not find file'" << fname << "'" << std::endl;
			}

			std::string output, line;

			while (std::getline(fileHandle, line)) {
				output += line;
				output.push_back('\n');
			}

			Lexer lexer(output);
			lexer.nextChar();
			lexer.getTokens();

			Parser parser(lexer.tokens, file_search_dirs);

			ASTPtr imported_program = std::make_unique<Program>(std::move(dynamic_cast<Program*>(parser.parse_program().get())->statements));

			return ExpressionStmt(std::move(imported_program));
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

		return ExpressionStmt(std::make_unique<NoOp>(), true);
		//}

	/*	std::cerr << "Unsupported file type for LOAD: " << fname << std::endl;

		return ExpressionStmt(std::make_unique<NoOp>(), true);*/
	} else if (token.kind == TokenType::SEM) {
		advance();

		ASTPtr noOp = std::make_unique<NoOp>(NoOp());
		ExpressionStmt expressionStmt = ExpressionStmt(std::move(noOp), true);

		return expressionStmt;
	} else if (token.kind == TokenType::FORM || token.kind == TokenType::INLINE || token.kind == TokenType::CLASS) {
		advance();
		Token name = expect(TokenType::IDENT);
		advance();
		expect(TokenType::OPEN_PAREN);
		advance();

		std::vector<ASTPtr> params;

		while (current().kind != TokenType::CLOSE_PAREN) {
			if (current().kind == TokenType::EOF_TOK) {
				SyntaxError("Closing parentheses required for function/class definition", current().lineNo);
			}

			Token param = expect(TokenType::IDENT);

			params.push_back(std::make_unique<Variable>(param.text, nullptr));

			advance();

			if (current().kind == TokenType::CLOSE_PAREN) {
				break;
			}

			expect(TokenType::COMMA);

			advance();
		}

		advance();
		std::vector<ExpressionStmt> stmts = parse_block();

		ASTPtr res = nullptr;

		if (token.kind == TokenType::FORM) {
			res = std::make_unique<FunctionLiteral>(name.text, std::move(params), std::move(stmts));
		} else if (token.kind == TokenType::INLINE) {
			res = std::make_unique<InlineLiteral>(name.text, std::move(params), std::move(stmts));
		} else {
			res = std::make_unique<ClassLiteral>(name.text, std::move(params), std::move(stmts));
		}
		
		return ExpressionStmt(std::move(res));
	} else if (token.kind == TokenType::RETURN) {
		advance();

		ASTPtr value = parse_expression(0);

		if (peek().kind == TokenType::SEM) {
			advance();
		} else {
			MissingTerminatorError("Missing statement terminator after return statement", current().lineNo);
		}

		ASTPtr returnLiteral = std::make_unique<ReturnLiteral>(std::move(value));

		return ExpressionStmt(std::move(returnLiteral));
	} else if (token.kind == TokenType::WHILE) {
		advance();
		ASTPtr condition = parse_expression(0);
		advance();

		std::vector<ExpressionStmt> stmts;

		if (current().kind != TokenType::OPEN_BRAC) {
			stmts.push_back(parse_statement());
		} else {
			stmts = parse_block();
		}

		ASTPtr whileLiteral = std::make_unique<WhileLiteral>(std::move(condition), std::move(stmts));

		return ExpressionStmt(std::move(whileLiteral));
	} else if (token.kind == TokenType::FOR) {
		advance();
		ASTPtr var = parse_expression(0);
		advance();
		expect(TokenType::ITER);
		advance();
		ASTPtr iter = parse_expression(0);
		advance();

		std::vector<ExpressionStmt> stmts;

		if (current().kind != TokenType::OPEN_BRAC) {
			stmts.push_back(parse_statement());
		} else {
			stmts = parse_block();
		}

		ASTPtr forLiteral = std::make_unique<ForLiteral>(std::move(var), std::move(iter), std::move(stmts));

		return ExpressionStmt(std::move(forLiteral));
	} else if (token.kind == TokenType::IF) {
		advance();

		ASTPtr condition = parse_expression(0);
		std::vector<ExpressionStmt> thenStmts, elseStmts;

		advance();

		if (current().kind != TokenType::OPEN_BRAC) {
			thenStmts.push_back(parse_statement());
		} else {
			thenStmts = parse_block();
		}

		if (current().kind != TokenType::ELSE) {
			ASTPtr ifLiteral = std::make_unique<IfLiteral>(std::move(condition), std::move(thenStmts));

			return ExpressionStmt(std::move(ifLiteral));
		}

		advance();

		if (current().kind != TokenType::OPEN_BRAC) {
			elseStmts.push_back(parse_statement());
		} else {
			elseStmts = parse_block();
		}

		ASTPtr ifLiteral = std::make_unique<IfLiteral>(std::move(condition), std::move(thenStmts), std::move(elseStmts));

		return ExpressionStmt(std::move(ifLiteral));
	} else if (token.kind == TokenType::BREAK || token.kind == TokenType::CONTINUE) {
		ASTPtr noOp = std::make_unique<NoOp>();

		if (peek().kind == TokenType::SEM)
			advance();
		else
			MissingTerminatorError("Missing statement terminator after break statement", current().lineNo);

		return ExpressionStmt(std::move(noOp), true, true, token.kind == TokenType::CONTINUE);
	}

	ASTPtr expr = parse_expression(0);

	if (peek().kind == TokenType::SEM) {
		advance();
	} else {
		MissingTerminatorError("Missing statement terminator after expression", current().lineNo);
	}

	return ExpressionStmt(std::move(expr));
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
		left = std::make_unique<UnaryOp>(new_token_type, std::move(operand));
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
		if (base == -1)
			SyntaxError("Somehow an integer with an invalid radix (base) has slipped through the cracks..."
					"this message shouldn't ever appear at all, really."
					"Please report this in the Issue Tracker.", token.lineNo);

		left = std::make_unique<IntLiteral>(std::strtoll(token.text.c_str(), NULL, base));
	} else if (token.kind == TokenType::FLOAT) {
		left = std::make_unique<FloatLiteral>(std::strtof(token.text.c_str(), NULL));
	} else if (token.kind == TokenType::STR) {
		left = std::make_unique<StrLiteral>(read_escape(token.text));
	} else if (token.kind == TokenType::TYPE_INT) {
		left = std::make_unique<TypeInt>();
	} else if (token.kind == TokenType::TYPE_FLOAT) {
		left = std::make_unique<TypeFloat>();
	} else if (token.kind == TokenType::TYPE_STR) {
		left = std::make_unique<TypeStr>();
	} else if (token.kind == TokenType::TYPE_BOOL) {
		left = std::make_unique<TypeBool>();
	} else if (token.kind == TokenType::TYPE_VEC) {
		left = std::make_unique<TypeVec>();
	} else if (token.kind == TokenType::CHR) {
		char c = 0;
		get_escape(token.text, &c);
		left = std::make_unique<IntLiteral>(c);
	} else if (token.kind == TokenType::BOOL) {
		left = std::make_unique<BoolLiteral>(token.text == "true");
	} else if (token.kind == TokenType::OPEN_BRACKET) {
		advance();

		std::vector<ASTPtr> elems;
		
		while (current().kind != TokenType::CLOSE_BRACKET) {
			if (current().kind == TokenType::EOF_TOK) {
				SyntaxError("Unterminated vector literal", current().lineNo);
			}

			ASTPtr elem = parse_expression(0);
			elems.push_back(std::move(elem));

			advance();

			if (current().kind == TokenType::CLOSE_BRACKET)
				break;

			expect(TokenType::COMMA);

			advance();
		}
		
		left = std::make_unique<VecLiteral>(std::move(elems));
	} else if (token.kind == TokenType::IDENT) {
		if (peek().kind == TokenType::OPEN_PAREN) {
			advance(2);

			std::vector<ASTPtr> params;

			while (current().kind != TokenType::CLOSE_PAREN) {
				if (current().kind == TokenType::EOF_TOK) {
					SyntaxError("Closing parenthesis required for function call", current().lineNo);
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

			ASTPtr call = std::make_unique<FunctionCall>(token.text, std::move(params));

			left = std::move(call);
		} else {
			left = std::make_unique<Variable>(token.text);
		}
	} else if (token.kind == TokenType::OPEN_PAREN) {
		advance();
		left = parse_expression(0);
		advance();
		expect(TokenType::CLOSE_PAREN);
	} else {
		SyntaxError("Unexpected token in expression: " + std::to_string((uint16_t)token.kind) + 
			(token.text.empty() ? "" : (", '" + token.text + "'")), token.lineNo);
	}

	while (true) {
		Token nextToken = peek();

		if (nextToken.kind == TokenType::INCREMENT || nextToken.kind == TokenType::DECREMENT) {
			advance();

			left = std::make_unique<UnaryOp>(nextToken.kind, std::move(left));
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
		left = std::make_unique<BinaryOp>(op.kind, std::move(left), std::move(right));
	}

	return left;
}
