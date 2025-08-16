#include "token.hpp"

Token::Token(std::string tokenText, std::string tokenKind) : text(tokenText), kind(tokenKind) {}

void Token::print() {
    std::cout << "TOKEN(" << text << ", " << kind << ")" << std::endl;
}