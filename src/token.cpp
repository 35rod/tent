#include "token.hpp"

Token::Token(std::string tokenText, TokenType tokenKind, Span s) : text(tokenText), kind(tokenKind), span(s) {}

void Token::print() {
    std::cout << "TOKEN(" << text << ", " << (uint16_t) kind << ", " << span.getLineNum() << ")" << std::endl;
}
