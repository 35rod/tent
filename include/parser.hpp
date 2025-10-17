#pragma once

#include <iostream>
#include <map>
#include "opcodes.hpp"
#include "token.hpp"
#include "ast.hpp"

extern std::vector<std::string> nativeLibs;

class Parser {
    std::map<TokenType, int> precedence = {
        {TokenType::DOT, 90},
	    {TokenType::POW, 16},
        {TokenType::NOT, 15}, {TokenType::BIT_NOT, 15},
        {TokenType::INCREMENT, 15}, {TokenType::DECREMENT, 15},
        {TokenType::INDEX, 14},
	    {TokenType::MUL, 12}, {TokenType::DIV, 12}, {TokenType::FLOOR_DIV, 12}, {TokenType::MOD, 12},
	    {TokenType::ADD, 11}, {TokenType::SUB, 11},

	    {TokenType::LSHIFT, 10}, {TokenType::RSHIFT, 10},

	    {TokenType::LESS, 9}, {TokenType::LESSEQ, 9}, {TokenType::GREATER, 9}, {TokenType::GREATEREQ, 9},
	    {TokenType::EQEQ, 8}, {TokenType::NOTEQ, 8},

	    {TokenType::BIT_AND, 7}, {TokenType::BIT_XOR, 6}, {TokenType::BIT_OR, 5},
	    {TokenType::AND, 4}, {TokenType::OR, 3},

        {TokenType::ASSIGN, 1}, {TokenType::MOD_ASSIGN, 1}, {TokenType::POW_ASSIGN, 1},
        {TokenType::ADD_ASSIGN, 1}, {TokenType::SUB_ASSIGN, 1}, {TokenType::MUL_ASSIGN, 1},
        {TokenType::DIV_ASSIGN, 1},
        {TokenType::FLOOR_DIV_ASSIGN, 1}, {TokenType::AND_ASSIGN, 1}, {TokenType::OR_ASSIGN, 1},
        {TokenType::BIT_AND_ASSIGN, 1}, {TokenType::BIT_XOR_ASSIGN, 1}, {TokenType::BIT_OR_ASSIGN, 1},
        {TokenType::LSHIFT_ASSIGN, 1}, {TokenType::RSHIFT_ASSIGN, 1},
    };
    std::vector<Token> tokens;
    std::vector<Token>::size_type pos = 0;

    std::string source;
    std::string filename;

    const std::vector<std::string> file_search_dirs;

    Token current();
    Token peek(int num=1);
    /* returns the current token, then advances (like i++ vs ++i) */
    Token advance(int num=1);
    Token expect(TokenType ttype);
    std::vector<ExpressionStmt> parse_block();
    ExpressionStmt parse_statement();
    ASTPtr parse_expression(int minBp);

    public:
        ASTPtr parse_program();

        Parser(std::vector<Token> parserTokens, std::string input, std::string file, std::vector<std::string> search_dirs);
};
