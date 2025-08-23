#include <iostream>
#include <fstream>

#include <cstdint>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "args.hpp"

uint64_t runtime_flags = 0;

std::string SRC_FILENAME, PROG_NAME;
std::vector<std::string> prog_args;

int32_t main(int32_t argc, char **argv) {
    parse_args(argc, argv);

    std::ifstream fileHandle(SRC_FILENAME);
    if (!fileHandle.is_open())
        std::cerr << "File error: could not open file '" << SRC_FILENAME << "'." << std::endl;

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
    Program ast = parser.parse_program();\

    if (IS_FLAG_SET(DEBUG))
        ast.print(0);

    Evaluator evaluator;
    evaluator.evalProgram(ast, prog_args);

    return 0;
}
