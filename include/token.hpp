#pragma once

#include <iostream>
#include <string>

class Token {
    public:
        std::string text;
        std::string kind;

        void print();
        
        Token(std::string tokenText, std::string tokenKind);
};