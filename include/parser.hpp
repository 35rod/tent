#pragma once

#include <iostream>
#include <map>
#include "token.hpp"
#include "ast.hpp"

class Parser {
    std::map<std::string, int> precedence = {
        {"EQEQ", 5}, {"ADD", 10}, {"SUB", 10},
        {"MUL", 20}, {"DIV", 20}
    };
    std::vector<Token> tokens;
    std::vector<Token>::size_type pos = 0;

    Token current();
    Token peek();
    Token advance();
    Token expect(std::string ttype);
    ExpressionStmt parse_statement();
    ASTPtr parse_expression(int minBp);

    public:
        Program parse_program();

        Parser(std::vector<Token> parserTokens);
};
