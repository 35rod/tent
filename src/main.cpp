#include <iostream>
#include <fstream>
#include <cstdint>
#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "evaluator.hpp"
#include "vm.hpp"
#include "args.hpp"

uint64_t runtime_flags = 0;

std::string SRC_FILENAME, PROG_NAME;
std::vector<std::string> prog_args, search_dirs;

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

	fileHandle.close();

	ASTPtr program = nullptr;

	if (SRC_FILENAME.find(".tnc") != std::string::npos) {
		VM vm;
		auto bytecode = vm.loadFile(SRC_FILENAME, search_dirs);
		vm.run(bytecode);
		exit(0);
	} else {
		Lexer lexer(output);

		lexer.nextChar();
		lexer.getTokens();

		Parser parser(lexer.tokens, search_dirs);
		program = parser.parse_program();
	}

	if (IS_FLAG_SET(DEBUG))
		program->print(0);

	if (IS_FLAG_SET(COMPILE)) {
		Compiler compiler;
		std::vector<Instruction> bytecode = compiler.compileProgram(std::move(program));
		compiler.saveToFile(bytecode, SRC_FILENAME.substr(0, SRC_FILENAME.rfind(".")) + ".tnc");
	} else {
	    if (!IS_FLAG_SET(DEBUG_STOP)) {
            Evaluator evaluator;
            evaluator.evalProgram(std::move(program), prog_args);
        }
	}

	return 0;
}
