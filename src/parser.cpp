#include "parser.hpp"
#include "errors.hpp"

Parser::Parser(std::vector<Token> parserTokens) : tokens(parserTokens) {}

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

Program Parser::parse_program() {
    std::vector<ExpressionStmt> stmts;

    while (pos < tokens.size()) {
        try {
            ExpressionStmt&& stmt = parse_statement();
            
            if (!stmt.noOp) {
                stmts.push_back(std::move(stmt));
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;

            break;
        }
    }

    return Program(std::move(stmts));
}

ExpressionStmt Parser::parse_statement() {
    Token token = current();

    if (token.kind == "SEM") {
        advance();

        ASTPtr noOp = std::make_unique<NoOp>(NoOp());
        ExpressionStmt expressionStmt = ExpressionStmt(std::move(noOp), true);

        return expressionStmt;
    }

    if (token.kind == "SET") {
        advance();
        Token nameToken = expect("IDENT");
        advance();
        expect("EQ");
        advance();

        ASTPtr value = parse_expression(0);

        if (peek().kind == "SEM") {
            advance();
        } else {
            MissingTerminatorError("Missing statement terminator after variable assignment", current().lineNo);
        }

        ASTPtr var = std::make_unique<Variable>(nameToken.text, std::move(value), "ASSIGN");

        return ExpressionStmt(std::move(var));
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

            params.push_back(std::make_unique<Variable>(param.text, nullptr, "FUNCTION"));

            advance();

            if (current().kind == "CLOSE_PAREN") {
                break;
            }

            expect("COMMA");

            advance();
        }

        advance();
        expect("OPEN_BRAC");
        advance();

        std::vector<ExpressionStmt> stmts;

        while (current().kind != "CLOSE_BRAC") {
            if (current().kind == "EOF") {
                SyntaxError("Closing braces required for function body", current().lineNo);
            }

            ExpressionStmt&& stmt = parse_statement();

            if (!stmt.noOp) {
                stmts.push_back(std::move(stmt));
            }
        }

        advance();

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
    } else if (token.kind == "WHILE" || token.kind == "IF") {
        advance();

        ASTPtr condition = parse_expression(0);

        advance();
        expect("OPEN_BRAC");
        advance();

        std::vector<ExpressionStmt> stmts;

        while (current().kind != "CLOSE_BRAC") {
            if (current().kind == "EOF") {
                SyntaxError("Closing braces required for " +
                        std::string((token.kind == "WHILE")
                            ? "while loop"
                            : "if statement")
                        + " body", current().lineNo);
            }

            ExpressionStmt&& stmt = parse_statement();

            if ((stmt.noOp && stmt.isBreak) || !stmt.noOp) {
                stmts.push_back(std::move(stmt));
            }
        }

        advance();

        if (token.kind == "WHILE") {
            ASTPtr whileLiteral = std::make_unique<WhileLiteral>(std::move(condition), std::move(stmts));
            return ExpressionStmt(std::move(whileLiteral));
        } else {
            ASTPtr ifLiteral = std::make_unique<IfLiteral>(std::move(condition), std::move(stmts));
            return ExpressionStmt(std::move(ifLiteral));
        }
    } else if (token.kind == "BREAK") {
        ASTPtr noOp = std::make_unique<NoOp>();

        if (peek().kind == "SEM")
            advance();
        else
            MissingTerminatorError("Missing statement terminator after break statement", current().lineNo);

        return ExpressionStmt(std::move(noOp), true, true);
    } else if (token.kind == "IDENT") {
        if (peek().kind == "EQ") {
        	advance();
        	advance();

        	ASTPtr value = parse_expression(0);

        	if (peek().kind == "SEM") {
        	    advance();
        	} else {
        		MissingTerminatorError("Missing statement terminator after variable assignment", current().lineNo);
        	}

            ASTPtr var = std::make_unique<Variable>(token.text, std::move(value), "REASSIGN");

            return ExpressionStmt(std::move(var));
        }
    }

    ASTPtr expr = parse_expression(0);

    Token next = peek();

    if (next.kind == "SEM") {
        advance();
    } else {
        MissingTerminatorError("Missing statement terminator after expression", current().lineNo);
    }

    return ExpressionStmt(std::move(expr));
}

ASTPtr Parser::parse_expression(int min_bp) {
    Token token = current();

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
            SyntaxError("Somehow an integer with an invalid radix has slipped through the cracks..."
                    "this message shouldn't ever appear at all, really."
                    "Please report this in the Issue Tracker.", token.lineNo);

        left = std::make_unique<IntLiteral>(std::strtoll(token.text.c_str(), NULL, base));
    } else if (token.kind == "FLOAT") {
        left = std::make_unique<FloatLiteral>(std::strtof(token.text.c_str(), NULL));
    } else if (token.kind == "STR") {
        left = std::make_unique<StrLiteral>(token.text);
    } else if (token.kind == "BOOL") {
        left = std::make_unique<BoolLiteral>(token.text == "true");
    } else if (token.kind == "OPEN_BRACKET") {
        advance();

        std::vector<ASTPtr> elems;
        
        while (current().kind != "CLOSE_BRACKET") {
            if (current().kind == "EOF") {
                SyntaxError("unterminated vector literal", current().lineNo);
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

        ASTPtr right = parse_expression(bp+1);
        left = std::make_unique<BinaryOp>(op.kind, std::move(left), std::move(right));
    }

    return left;
}
