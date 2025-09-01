#include "token.hpp"

Token::Token(std::string tokenText, TokenType tokenKind, int line) : text(tokenText), kind(tokenKind), lineNo(line) {}

void Token::print() {
    std::cout << "TOKEN(" << text << ", " << (uint16_t) kind << ", " << lineNo << ")" << std::endl;
}
