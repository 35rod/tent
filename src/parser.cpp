#include "parser.hpp"

Parser::Parser(std::vector<Token> parserTokens) : tokens(parserTokens) {}

Token Parser::current() {
    if (pos >= tokens.size()) {
        return Token("\0", "EOF");
    }

    return tokens[pos];
}

Token Parser::peek() {
    if ((pos+1) >= tokens.size()) {
        return Token("\0", "EOF");
    }

    return tokens[pos+1];
}

Token Parser::advance() {
    if (pos >= tokens.size()) {
        return Token("\0", "EOF");
    }

    Token token = tokens[pos];
    pos += 1;

    return token;
}

Token Parser::expect(std::string ttype) {
    if (current().kind == ttype) {
        return current();
    }

    throw std::runtime_error("Expected " + ttype + ", got " + current().kind);
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

    if (token.kind == "SEM" || token.kind == "NEWLINE") {
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

        if (peek().kind == "SEM" || peek().kind == "NEWLINE") {
            advance();
        } else {
            throw std::runtime_error("Missing statement terminator after variable assignment");
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
                throw std::runtime_error("Closing parentheses required for function definition");
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
            ExpressionStmt&& stmt = parse_statement();

            if (!stmt.noOp) {
                stmts.push_back(std::move(stmt));
            }
        }

        advance();

        ASTPtr form = std::make_unique<FunctionLiteral>(name.text, std::move(params), std::move(stmts));

        return ExpressionStmt(std::move(form));
    } else if (token.kind == "IDENT") {
        if (peek().kind == "OPEN_PAREN") {
            advance();
            advance();

            std::vector<ASTPtr> params;

            while (current().kind != "CLOSE_PAREN") {
                if (current().kind == "EOF") {
                    throw std::runtime_error("Closing parentheses required for function call");
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

            advance();

            ASTPtr call = std::make_unique<FunctionCall>(token.text, std::move(params));

            if (current().kind == "SEM" || current().kind == "NEWLINE") {
                advance();
            } else {
                throw std::runtime_error("Missing statement terminator after function call");
            }

            return ExpressionStmt(std::move(call));
        } else if (peek().kind == "EQ") {
            advance();
            advance();

            ASTPtr value = parse_expression(0);

            if (peek().kind == "SEM" || peek().kind == "NEWLINE") {
                advance();
            } else {
                throw std::runtime_error("Missing statement terminator after variable assignment");
            }

            ASTPtr var = std::make_unique<Variable>(token.text, std::move(value), "REASSIGN");

            return ExpressionStmt(std::move(var));
        }
    }

    ASTPtr expr = parse_expression(0);

    Token next = peek();

    if (next.kind == "SEM" || next.kind == "NEWLINE") {
        advance();
    } else {
        throw std::runtime_error("Missing statement terminator after expression");
    }

    return ExpressionStmt(std::move(expr));
}

ASTPtr Parser::parse_expression(int min_bp) {
    Token token = current();

    ASTPtr left;

    if (token.kind == "INT") {
        left = std::make_unique<IntLiteral>(std::stol(token.text));
    } else if (token.kind == "FLOAT") {
        left = std::make_unique<FloatLiteral>(std::stof(token.text));
    } else if (token.kind == "IDENT") {
        left = std::make_unique<Variable>(token.text);
    } else if (token.kind == "OPEN_PAREN") {
        advance();
        left = parse_expression(0);
        advance();
        expect("CLOSE_PAREN");
    } else {
        throw std::runtime_error("Unexpected token in expression: " + token.kind + 
            (token.text.empty() ? "" : (", " + token.text)));
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
