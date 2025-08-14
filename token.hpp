#include <string>

class Token {
    public:
        std::string text;
        std::string kind;
        
        Token(std::string tokenText, std::string tokenKind);
};