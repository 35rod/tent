#include <iostream>
#include <fstream>

#include <cstdint>
#include <optional>
#include <string>

#define TENT_MAIN_CPP_FILE
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "compiler.hpp"
#include "diagnostics.hpp"
#include "errors.hpp"
#include "args.hpp"

uint64_t runtime_flags = 0;

std::string SRC_FILENAME, OUT_FILENAME, PROG_NAME;
std::vector<std::string> prog_args, search_dirs;
std::string finalLibraryPath;

void start_repl(const std::vector<std::string>& search_dirs) {
	std::cout << BOLD << CYAN << "Tent Interactive REPL\n"
	<< GRAY << "Type 'exit' or press Ctrl+C to quit.\n\n" << RESET;

	std::string buffer;
	int openBraces = 0, openParens = 0;

	while (true) {
		// std::cout << (buffer.empty() ? (BOLD + GREEN + ">> " + RESET)
		// : (BOLD + YELLOW + ".. " + RESET));

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
			Diagnostics diags;

			Lexer lexer(buffer, diags);
			lexer.nextChar();
			lexer.getTokens();

			if (diags.has_errors()) {
				diags.print_errors();
				buffer.clear();
				continue;
			}

			Parser parser(lexer.tokens, diags, "<stdin>", search_dirs);
			ASTPtr program = parser.parse_program();

			if (diags.has_errors()) {
				diags.print_errors();
				buffer.clear();
				continue;
			}

			Evaluator evaluator(buffer, diags, "<stdin>");
			evaluator.evalProgram(std::move(program), {});

			if (diags.has_errors()) {
				diags.print_errors();
				buffer.clear();
				continue;
			}
		} catch (const std::exception& e) {
			std::cerr << RED << "Error: " << e.what() << RESET << "\n";
		}

		buffer.clear();
	}
}

int32_t main(int32_t argc, char **argv) {
	parseArgs(argc, argv);
	const auto libPathOpt = checkSearchPathsFor("libTentRuntime.a", search_dirs);
	if (!libPathOpt.has_value()) {
		std::cerr << "could not find library path with the provided search paths "
				 "(use '-S <path>' to specify the path)";
		return 1;
	}
	finalLibraryPath = libPathOpt.value().first;
	if (IS_FLAG_SET(PRINT_LIB_PATH)) {
		std::cout << finalLibraryPath;
		return 0;
	}

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

	Diagnostics diags;

	Lexer lexer(output, diags, SRC_FILENAME);

	lexer.nextChar();
	lexer.getTokens();

	if (diags.has_errors()) {
		diags.print_errors();
		return 1;
	}

	Parser parser(lexer.tokens, diags, SRC_FILENAME, search_dirs);
	program = parser.parse_program();

	if (diags.has_errors()) {
		diags.print_errors();
		return 1;
	}

	if (IS_FLAG_SET(DEBUG))
		program->print(0);

	if (IS_FLAG_SET(COMPILE)) {
		Compiler::compile(static_cast<Program*>(program.get()), OUT_FILENAME);

		return 0;
	} else {
		if (!IS_FLAG_SET(DRY_RUN)) {
			try {
				Evaluator evaluator(output, diags, SRC_FILENAME);
				evaluator.evalProgram(std::move(program), prog_args);
			} catch (const std::exception& e) {
				if (diags.has_errors()) {
					diags.print_errors();
				}

				return 1;
			}
		}
	}

	return 0;
}
