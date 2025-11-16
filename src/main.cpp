#include <iostream>
#include <fstream>

#include <cstdint>

#define TENT_MAIN_CPP_FILE
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "compiler.hpp"
#include "errors.hpp"
#include "args.hpp"
#include "config.hpp"

const std::string RESET = "\033[0m";
const std::string BOLD  = "\033[1m";
const std::string CYAN  = "\033[36m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string RED   = "\033[31m";
const std::string MAGENTA = "\033[35m";
const std::string GRAY  = "\033[90m";

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
	Config config;
	parse_args(argc, argv, config);

	if (config.is_flag_set(REPL)) {
		start_repl(config.search_dirs);
		return 0;
	}

	std::ifstream fileHandle(config.src_filename);

	if (!fileHandle.is_open())
		std::cerr << "File error: could not open file '" << config.src_filename << "'." << std::endl;

	std::string output;
	std::string line;

	while (std::getline(fileHandle, line)) {
		output += line;
		output.push_back('\n');
	}

	fileHandle.close();

	ASTPtr program = nullptr;

	Lexer lexer(output, config.src_filename);

	lexer.nextChar();
	lexer.getTokens();

	Parser parser(lexer.tokens, output, config.src_filename, config.search_dirs);
	program = parser.parse_program();

	if (config.is_flag_set(DEBUG))
		program->print(0);

	if (config.is_flag_set(COMPILE)) {
		Compiler::compile(config, static_cast<Program*>(program.get()), config.out_filename);

		return 0;
	} else {
		if (!config.is_flag_set(DRY_RUN)) {
			try {
				Evaluator evaluator(output);
				evaluator.evalProgram(std::move(program), config.prog_args);
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
