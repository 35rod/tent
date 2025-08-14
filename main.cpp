#include <iostream>
#include <fstream>
#include "lexer.hpp"

int main() {
    std::ifstream fileHandle("main.nl");

    std::string output;
    std::string line;

    while (std::getline(fileHandle, line)) {
        output += line;
        output.push_back('\n');
    }

    Lexer lexer(output);

    lexer.getTokens();

    for (Token token : lexer.tokens) {
        std::cout << "TOKEN(" << token.kind << ", " << token.text << ")" << std::endl;
    }

    return 0;
}