#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"

int main() {
    std::ifstream fileHandle("../main.nl");

    std::string output;
    std::string line;

    while (std::getline(fileHandle, line)) {
        output += line;
        output.push_back('\n');
    }

    Lexer lexer(output);

    lexer.nextChar();
    lexer.getTokens();

    Parser parser(lexer.tokens);
    Program ast = parser.parse_program();

    ast.print(0);

    Evaluator evaluator;
    EvalExpr res = evaluator.evalProgram(ast);
    
    if (std::holds_alternative<int>(res)) {
        std::cout << std::get<int>(res) << std::endl;
    } else {
        std::cout << std::get<float>(res) << std::endl;
    }

    return 0;
}