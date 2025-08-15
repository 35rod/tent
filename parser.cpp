#include "parser.hpp"

Parser::Parser(std::vector<Token> parserTokens) : tokens(parserTokens) {}

Token Parser::peek() {
    if (pos >= tokens.size()) {
        return Token("EOF", "");
    }

    return tokens[pos+1];
}

Token Parser::advance() {
    if (pos >= tokens.size()) {
        return Token("EOF", "");
    }

    Token token = tokens[pos];
    pos += 1;

    return token;
}

Token Parser::expect(std::string ttype) {
    if (peek().kind == ttype) {
        return advance();
    }

    printf("Expected a certain type, got another");
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
    if (tokens[pos].kind == "SEM" || tokens[pos].kind == "NEWLINE") {
        advance();

        ASTPtr noOp = std::make_unique<NoOp>(NoOp());
        ExpressionStmt expressionStmt = ExpressionStmt(std::move(noOp), true);

        return expressionStmt;
    }

    ASTPtr expr = parse_expression(0);

    Token next = peek();

    if (next.kind == "SEM" || next.kind == "NEWLINE") {
        advance();
    } else {
        throw std::runtime_error("missing statement terminator after expression");
    }

    return ExpressionStmt(std::move(expr));
}

ASTPtr Parser::parse_expression(int min_bp) {
    Token token = tokens[pos];

    if (token.kind == "INT") {
        ASTPtr left = std::make_unique<IntLiteral>(std::stoi(token.text));

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

            Token op = tokens[pos];

            advance();

            ASTPtr right = parse_expression(bp+1);
            left = std::make_unique<BinaryOp>(op.kind, std::move(left), std::move(right));
        }

        return left;
    } else {
        throw std::runtime_error("Unexpected token in expression: " + token.kind + 
            (token.text.empty() ? "" : (", " + token.text)));
    }
}