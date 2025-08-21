#pragma once

#include <string>
#include <vector>
#include "token.hpp"

class Lexer {
    std::string source;
    std::string::size_type curPos;
    char curChar;
    int lineNo;

    char peek();
    void skipWhitespace();
    void skipComment();
    
    Token getToken();

    public:
        std::vector<Token> tokens;

        void nextChar(int num=1);
        void getTokens();

        Lexer(std::string input);
};
