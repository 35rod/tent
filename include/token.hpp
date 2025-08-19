#pragma once

#include <iostream>
#include <string>

class Token {
    public:
        std::string text;
        std::string kind;
        int lineNo;

        void print();
        
        Token(std::string tokenText, std::string tokenKind, int line);
};