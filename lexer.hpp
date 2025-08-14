#include <string>
#include <list>
#include "token.hpp"

class Lexer {
    std::string source;
    char curChar;
    int curPos = -1;

    void nextChar();
    char peek();
    void skipWhitespace();
    void skipComment();
    
    Token getToken();
    void getTokens();

    public:
        std::list<Token> tokens;
        
        Lexer(std::string input);
};