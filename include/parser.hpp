#pragma once

#include <iostream>
#include <map>
#include "token.hpp"
#include "ast.hpp"

class Parser {
    std::map<std::string, int> precedence = {
	    {"POW", 16},
        {"INDEX", 14},
	    {"MUL", 12}, {"DIV", 12}, {"MOD", 12},
	    {"ADD", 11}, {"SUB", 11},

	    {"LSHIFT", 10}, {"RSHIFT", 10},

	    {"LESS", 9}, {"LESSEQ", 9}, {"GREATER", 9}, {"GREATEREQ", 9},
	    {"EQEQ", 8}, {"NOTEQ", 8},

	    {"BIN_AND", 7}, {"BIN_XOR", 6}, {"BIN_OR", 5},
	    {"AND", 4}, {"OR", 3},
    };
    std::vector<Token> tokens;
    std::vector<Token>::size_type pos = 0;

    Token current();
    Token peek(int num=1);
    Token advance(int num=1);
    Token expect(std::string ttype);
    ExpressionStmt parse_statement();
    ASTPtr parse_expression(int minBp);

    public:
        Program parse_program();

        Parser(std::vector<Token> parserTokens);
};
