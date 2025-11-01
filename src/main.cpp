#include <iostream>
#include <fstream>

#include <cstdint>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "compiler.hpp"
#include "errors.hpp"
#include "args.hpp"

const std::string RESET = "\033[0m";
const std::string BOLD  = "\033[1m";
const std::string CYAN  = "\033[36m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string RED   = "\033[31m";
const std::string MAGENTA = "\033[35m";
const std::string GRAY  = "\033[90m";

uint64_t runtime_flags = 0;

std::string SRC_FILENAME, PROG_NAME;
std::vector<std::string> prog_args, search_dirs;

void start_repl(const std::vector<std::string>& search_dirs) {
	std::cout << BOLD << CYAN << "Tent Interactive REPL\n"
	<< GRAY << "Type 'exit' or press Ctrl+C to quit.\n\n" << RESET;

	std::string buffer;
	int openBraces = 0, openParens = 0;

	while (true) {
		std::cout << (buffer.empty() ? (BOLD + GREEN + ">> " + RESET)
		: (BOLD + YELLOW + ".. " + RESET));

		std::string line;

		if (!std::getline(std::cin, line)) {
			std::cout << "\n" << GRAY << "Exiting REPL..." << RESET << std::endl;
			break;
		}

		if (line == "exit") {
			break;
		}

		buffer += line + "\n";

		openBraces = openParens = 0;

		for (char c : buffer) {
			if (c == '{') openBraces++;
			if (c == '}') openBraces--;
			if (c == '(') openParens++;
			if (c == ')') openParens--;
		}

		if (openBraces >0 || openParens > 0)
			continue;
		
		try {
			Lexer lexer(buffer);
			lexer.nextChar();
			lexer.getTokens();

			Parser parser(lexer.tokens, buffer, "<stdin>", search_dirs);
			ASTPtr program = parser.parse_program();

			Evaluator evaluator(buffer);
			evaluator.evalProgram(std::move(program), {});
		} catch (const std::exception& e) {
			std::cerr << RED << "ERror: " << e.what() << RESET << "\n";
		}

		buffer.clear();
	}
}

int32_t main(int32_t argc, char **argv) {
	parse_args(argc, argv);

	if (IS_FLAG_SET(REPL)) {
		start_repl(search_dirs);
		return 0;
	}

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

	Lexer lexer(output, SRC_FILENAME);

	lexer.nextChar();
	lexer.getTokens();

	Parser parser(lexer.tokens, output, SRC_FILENAME, search_dirs);
	program = parser.parse_program();

	if (IS_FLAG_SET(DEBUG))
		program->print(0);

	if (IS_FLAG_SET(COMPILE)) {
		Compiler compiler;
		compiler.compile(static_cast<Program*>(program.get()));

		return 0;
	} else {
	    if (!IS_FLAG_SET(DEBUG_STOP)) {
			try {
				Evaluator evaluator(output);
				evaluator.evalProgram(std::move(program), prog_args);
			} catch (const Error& e) {
				e.print();
				return 1;
			} catch (const std::exception& e) {
				std::cerr << "Unexpected error: " << e.what() << std::endl;
				return 1;
			}
        }
	}

	return 0;
}
