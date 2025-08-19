#include "token.hpp"

Token::Token(std::string tokenText, std::string tokenKind, int line) : text(tokenText), kind(tokenKind), lineNo(line) {}

void Token::print() {
    std::cout << "TOKEN(" << text << ", " << kind << ", " << lineNo << ")" << std::endl;
}