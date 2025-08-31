#include <fstream>

#include "parser.hpp"
#include "lexer.hpp"
#include "errors.hpp"
#include "esc_codes.hpp"

Parser::Parser(std::vector<Token> parserTokens, std::vector<std::string> search_dirs)
	: tokens(parserTokens), file_search_dirs(search_dirs) {}

Token Parser::current() {
	if (pos >= tokens.size()) {
		return Token("\0", "EOF", tokens[pos-1].lineNo);
	}

	return tokens[pos];
}

Token Parser::peek(int num) {
	if ((pos+num) >= tokens.size()) {
		return Token("\0", "EOF", 0);
	}

	return tokens[pos+num];
}

Token Parser::advance(int num) {
	if (pos >= tokens.size()) {
		return Token("\0", "EOF", 0);
	}

	Token token = tokens[pos];
	pos += num;

	return token;
}

Token Parser::expect(std::string ttype) {
	if (current().kind == ttype) {
		return current();
	}

	SyntaxError("Expected " + ttype + ", got " + current().kind, current().lineNo);

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
	expect("OPEN_BRAC");
	advance();

	std::vector<ExpressionStmt> stmts;

	while (current().kind != "CLOSE_BRAC") {
		if (current().kind == "EOF")
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

	if (token.kind == "LOAD") {
		advance();
		Token filename = expect("STR");

		if (peek().kind == "SEM") {
			advance();
		} else {
			MissingTerminatorError("Missing statement terminator after load statement", current().lineNo);
		}

		std::ifstream fileHandle(filename.text);
		for (std::string search_dir : file_search_dirs) {
			if (fileHandle.is_open()) break;

			fileHandle = std::ifstream(search_dir + "/" + filename.text);
		}

		if (!fileHandle.is_open())
			std::cerr << "File error: could not find file '" << filename.text << "'." << std::endl;
		
		std::string output;
		std::string line;

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
	} else if (token.kind == "SEM") {
		advance();

		ASTPtr noOp = std::make_unique<NoOp>(NoOp());
		ExpressionStmt expressionStmt = ExpressionStmt(std::move(noOp), true);

		return expressionStmt;
	} else if (token.kind == "SET") {
		advance();
		Token nameToken = expect("IDENT");
		advance();
		expect("ASSIGN");
		advance();

		ASTPtr value = parse_expression(0);

		if (peek().kind == "SEM") {
			advance();
		} else {
			MissingTerminatorError("Missing statement terminator after variable assignment", current().lineNo);
		}

		ASTPtr assign = std::make_unique<BinaryOp>("ASSIGN", std::make_unique<Variable>(nameToken.text, nullptr), std::move(value));

		return ExpressionStmt(std::move(assign));
	} else if (token.kind == "FORM") {
		advance();
		Token name = expect("IDENT");
		advance();
		expect("OPEN_PAREN");
		advance();

		std::vector<ASTPtr> params;

		while (current().kind != "CLOSE_PAREN") {
			if (current().kind == "EOF") {
				SyntaxError("Closing parentheses required for function definition", current().lineNo);
			}

			Token param = expect("IDENT");

			params.push_back(std::make_unique<Variable>(param.text, nullptr));

			advance();

			if (current().kind == "CLOSE_PAREN") {
				break;
			}

			expect("COMMA");

			advance();
		}

		advance();
		std::vector<ExpressionStmt> stmts = parse_block();

		ASTPtr form = std::make_unique<FunctionLiteral>(name.text, std::move(params), std::move(stmts));

		return ExpressionStmt(std::move(form));
	} else if (token.kind == "RETURN") {
		advance();

		ASTPtr value = parse_expression(0);

		if (peek().kind == "SEM") {
			advance();
		} else {
			MissingTerminatorError("Missing statement terminator after return statement", current().lineNo);
		}

		ASTPtr returnLiteral = std::make_unique<ReturnLiteral>(std::move(value));

		return ExpressionStmt(std::move(returnLiteral));
	} else if (token.kind == "WHILE") {
		advance();

		ASTPtr condition = parse_expression(0);

		std::vector<ExpressionStmt> stmts;

		advance();

		if (current().kind != "OPEN_BRAC") {
			stmts.push_back(parse_statement());
		} else {
			stmts = parse_block();
		}

		ASTPtr whileLiteral = std::make_unique<WhileLiteral>(std::move(condition), std::move(stmts));

		return ExpressionStmt(std::move(whileLiteral));
	} else if (token.kind == "IF") {
		advance();

		ASTPtr condition = parse_expression(0);
		std::vector<ExpressionStmt> thenStmts, elseStmts;

		advance();

		if (current().kind != "OPEN_BRAC") {
			thenStmts.push_back(parse_statement());
		} else {
			thenStmts = parse_block();
		}

		if (current().kind != "ELSE") {
			advance();
			
			ASTPtr ifLiteral = std::make_unique<IfLiteral>(std::move(condition), std::move(thenStmts));

			return ExpressionStmt(std::move(ifLiteral));
		}

		advance();

		if (current().kind != "OPEN_BRAC") {
			elseStmts.push_back(parse_statement());
		} else {
			elseStmts = parse_block();
		}

		ASTPtr ifLiteral = std::make_unique<IfLiteral>(std::move(condition), std::move(thenStmts), std::move(elseStmts));

		return ExpressionStmt(std::move(ifLiteral));
	} else if (token.kind == "BREAK" || token.kind == "CONTINUE") {
		ASTPtr noOp = std::make_unique<NoOp>();

		if (peek().kind == "SEM")
			advance();
		else
			MissingTerminatorError("Missing statement terminator after break statement", current().lineNo);

		return ExpressionStmt(std::move(noOp), true, true, token.kind == "CONTINUE");
	}

	ASTPtr expr = parse_expression(0);

	if (peek().kind == "SEM") {
		advance();
	} else {
		MissingTerminatorError("Missing statement terminator after expression", current().lineNo);
	}

	return ExpressionStmt(std::move(expr));
}

ASTPtr Parser::parse_expression(int min_bp) {
	Token token = current();

	if (token.kind == "BIT_NOT" || token.kind == "NOT" || token.kind == "SUB" ||
			token.kind == "INCREMENT" || token.kind == "DECREMENT") {
		advance();
	
		ASTPtr operand = parse_expression(15);

		return std::make_unique<UnaryOp>(token.kind, std::move(operand));
	}

	ASTPtr left;

	std::string first_3_chars = token.kind.substr(0, 3);

	if (first_3_chars == "INT") {
		int8_t base = -1;
		if (token.kind == "INT_HEX")
			base = 16;
		if (token.kind == "INT_DEC")
			base = 10;
		if (token.kind == "INT_OCT")
			base = 8;
		if (token.kind == "INT_BIN")
			base = 2;
		if (base == -1)
			SyntaxError("Somehow an integer with an invalid radix (base) has slipped through the cracks..."
					"this message shouldn't ever appear at all, really."
					"Please report this in the Issue Tracker.", token.lineNo);

		left = std::make_unique<IntLiteral>(std::strtoll(token.text.c_str(), NULL, base));
	} else if (token.kind == "FLOAT") {
		left = std::make_unique<FloatLiteral>(std::strtof(token.text.c_str(), NULL));
	} else if (token.kind == "STR") {
		left = std::make_unique<StrLiteral>(read_escape(token.text));
	} else if (token.kind == "CHR") {
		char c = 0;
		get_escape(token.text, &c);
		left = std::make_unique<IntLiteral>(c);
	} else if (token.kind == "BOOL") {
		left = std::make_unique<BoolLiteral>(token.text == "true");
	} else if (token.kind == "OPEN_BRACKET") {
		advance();

		std::vector<ASTPtr> elems;
		
		while (current().kind != "CLOSE_BRACKET") {
			if (current().kind == "EOF") {
				SyntaxError("Unterminated vector literal", current().lineNo);
			}

			ASTPtr elem = parse_expression(0);
			elems.push_back(std::move(elem));

			advance();

			if (current().kind == "CLOSE_BRACKET")
				break;

			expect("COMMA");

			advance();
		}
		
		left = std::make_unique<VecLiteral>(std::move(elems));
	} else if (token.kind == "IDENT") {
		if (peek().kind == "OPEN_PAREN") {
			advance(2);

			std::vector<ASTPtr> params;

			while (current().kind != "CLOSE_PAREN") {
				if (current().kind == "EOF") {
					SyntaxError("Closing parenthesis required for function call", current().lineNo);
				}

				ASTPtr param = parse_expression(0);
				params.push_back(std::move(param));

				advance();

				if (current().kind == "CLOSE_PAREN") {
					break;
				}

				expect("COMMA");

				advance();
			}

			ASTPtr call = std::make_unique<FunctionCall>(token.text, std::move(params));

			left = std::move(call);
		} else {
			left = std::make_unique<Variable>(token.text);
		}
	} else if (token.kind == "OPEN_PAREN") {
		advance();
		left = parse_expression(0);
		advance();
		expect("CLOSE_PAREN");
	} else {
		SyntaxError("Unexpected token in expression: " + token.kind + 
			(token.text.empty() ? "" : (", '" + token.text + "'")), token.lineNo);
	}

	while (true) {
		Token nextToken = peek();

		if (nextToken.kind == "INCREMENT" || nextToken.kind == "DECREMENT") {
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

		auto isRightAssoc = [](const std::string& op) {
			return (op.find("ASSIGN") != std::string::npos || op == "POW");
		};
		
		ASTPtr right = parse_expression(isRightAssoc(op.kind) ? bp : bp+1);
		left = std::make_unique<BinaryOp>(op.kind, std::move(left), std::move(right));
	}

	return left;
}
