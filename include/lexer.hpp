#include <string>
#include <vector>
#include "token.hpp"

class Lexer {
    std::string source;
    int curPos;
    char curChar;

    char peek();
    void skipWhitespace();
    void skipComment();
    
    Token getToken();

    public:
        std::vector<Token> tokens;

        void nextChar();
        void getTokens();

        Lexer(std::string input);
};