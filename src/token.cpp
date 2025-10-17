#include "token.hpp"

Token::Token(std::string tokenText, TokenType tokenKind, int line, int col) : text(tokenText), kind(tokenKind), lineNo(line), colNo(col) {}

void Token::print() {
    std::cout << "TOKEN(" << text << ", " << (uint16_t) kind << ", " << lineNo << ")" << std::endl;
}
