#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"

int main() {
    std::ifstream fileHandle("main.nl");

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

    // for loop prints AST tree
    for (ExpressionStmt& stmt : ast.statements) {
        ASTNode* expr = std::move(stmt.expr.get());

        expr->print();
    }

    Evaluator evaluator;
    int res = evaluator.evalProgram(ast);
    std::cout << res << std::endl;

    return 0;
}