#pragma once

#include <iostream>
#include <map>
#include "token.hpp"
#include "ast.hpp"

class Parser {
    std::map<std::string, int> precedence = {
	    {"POW", 16},
        {"NOT", 15}, {"BIT_NOT", 15}, {"SUB", 15},
        {"INCREMENT", 15}, {"DECREMENT", 15},
        {"INDEX", 14},
	    {"MUL", 12}, {"DIV", 12}, {"FLOOR_DIV", 12}, {"MOD", 12},
	    {"ADD", 11}, {"SUB", 11},

	    {"LSHIFT", 10}, {"RSHIFT", 10},

	    {"LESS", 9}, {"LESSEQ", 9}, {"GREATER", 9}, {"GREATEREQ", 9},
	    {"EQEQ", 8}, {"NOTEQ", 8},

	    {"BIT_AND", 7}, {"BIT_XOR", 6}, {"BIT_OR", 5},
	    {"AND", 4}, {"OR", 3},

        {"ASSIGN", 1}, {"MOD_ASSIGN", 1}, {"POW_ASSIGN", 1},
        {"ADD_ASSIGN", 1}, {"SUB_ASSIGN", 1}, {"MUL_ASSIGN", 1}, {"DIV_ASSIGN", 1},
        {"FLOOR_DIV_ASSIGN", 1}, {"AND_ASSIGN", 1}, {"OR_ASSIGN", 1},
        {"BIT_AND_ASSIGN", 1}, {"BIT_XOR_ASSIGN", 1}, {"BIT_OR_ASSIGN", 1},
        {"LSHIFT_ASSIGN", 1}, {"RSHIFT_ASSIGN", 1},
    };
    std::vector<Token> tokens;
    std::vector<Token>::size_type pos = 0;

    const std::vector<std::string> file_search_dirs;

    Token current();
    Token peek(int num=1);
    /* returns the current token, then advances (like i++ vs ++i) */
    Token advance(int num=1);
    Token expect(std::string ttype);
    std::vector<ExpressionStmt> parse_block();
    ExpressionStmt parse_statement();
    ASTPtr parse_expression(int minBp);

    public:
        ASTPtr parse_program();

        Parser(std::vector<Token> parserTokens, std::vector<std::string> search_dirs);
};
