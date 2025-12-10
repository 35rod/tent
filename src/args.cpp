#include "args.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "compiler.hpp"

#define strlit(s) s, (sizeof(s)-1)

extern std::string SRC_FILENAME, OUT_FILENAME, PROG_NAME, SYSTEM_COMPILER;
extern std::vector<std::string> prog_args, search_dirs;
extern uint64_t runtime_flags;

void parseArgs(int32_t argc, char **argv) {
	PROG_NAME = std::string(argv[0]);

	// add some sensible defaults (the '..' ones are for 35rod)
	search_dirs.push_back(".");
	search_dirs.push_back("lib");
	search_dirs.push_back("..");
	search_dirs.push_back("../lib");
	search_dirs.push_back("runtime");
	search_dirs.push_back("../runtime");

	search_dirs.push_back("/usr/lib/tent");
	search_dirs.push_back("/usr/local/lib/tent");
	const char *home = std::getenv("HOME");
	if (home != nullptr)
		search_dirs.push_back(std::string(home) + "/.local/lib/tent");

	bool doing_prog_args = false;
	std::string command;
	int32_t arg_i = 1;

	if (argc > 1 && argv[1][0] != '-') {
		std::string first = argv[1];

		if (first == "compile" || first == "repl" || first == "help") {
			command = first;
			arg_i = 2;
		}
	}

	if (command == "compile")
		SET_FLAG(COMPILE);
	else if (command == "repl")
		SET_FLAG(REPL);
	else if (command == "help")
		printUsage();

	for (; arg_i < argc; arg_i++) {
		std::string arg = argv[arg_i];

		if (doing_prog_args) {
			prog_args.push_back(arg);
			continue;
		}

		if (arg == "--") {
			doing_prog_args = true;
			continue;
		}

		if (arg == "--help")
			printUsage();
		else if (arg == "--lib-path")
			SET_FLAG(PRINT_LIB_PATH);
		else if (arg == "-d" || arg == "--debug")
			SET_FLAG(DEBUG);
		else if (arg == "--dry") {
			SET_FLAG(DRY_RUN);
			SET_FLAG(DEBUG);
		} else if (arg == "-s" || arg == "--save-temps")
			SET_FLAG(SAVE_TEMPS);
		else if (arg.rfind("--system-compiler=", 0) == 0)
			SYSTEM_COMPILER = arg.substr(18);
		else if (arg == "-o") {
			if (arg_i + 1 >= argc) {
				std::cerr << "Missing output filename after '-o'\n";
				printUsage();
			}

			OUT_FILENAME = argv[arg_i++];
		} else if (arg.rfind("--out=", 0) == 0)
			OUT_FILENAME = arg.substr(6);
		else if (arg.rfind("-S", 0) == 0) {
			if (arg.size() > 2)
				search_dirs.push_back(arg.substr(2));
			else if (arg_i + 1 < argc)
				search_dirs.push_back(argv[arg_i++]);
			else {
				std::cerr << "Missing path after '-S'\n";
				printUsage();
			}
		} else if (arg[0] == '-') {
			std::cerr << "Unknown option: " << arg << "\n";
			printUsage();
		} else if (SRC_FILENAME.empty())
			SRC_FILENAME = arg;
		else
			prog_args.push_back(arg);
	}

	if (command.empty() && SRC_FILENAME.empty())
		SET_FLAG(REPL);

	if (IS_FLAG_SET(COMPILE)) {
		if (SRC_FILENAME.empty()) {
			std::cerr << "Compile mode requires a source file\n";
			printUsage();
		}

		if (OUT_FILENAME.empty())
			OUT_FILENAME = defaultOutputExeName;
	} else if (IS_FLAG_SET(REPL)) {
		if (!SRC_FILENAME.empty()) {
			std::cerr << "REPL mode does not take a file argument\n";
			printUsage();
		}
	} else {
		if (SRC_FILENAME.empty()) {
			std::cerr << "No input file specified\n";
			printUsage();
		}
	}
}

void printUsage(void) {
	std::cerr
        << "Usage:\n"
        << "  " << PROG_NAME << " <file> [options]          Run a Tent source file\n"
        << "  " << PROG_NAME << " compile <file> [options]  Compile Tent source file\n"
        << "  " << PROG_NAME << " repl                      Start interactive REPL\n"
        << "  " << PROG_NAME << " help                      Show this help message\n\n"
        << "Options:\n"
        << "  -d, --debug                 Enable debug output\n"
        << "  --dry                       Dry run (implies debug)\n"
        << "  --lib-path                  Show the library path (found from the provided search paths)\n"
        << "  -s, --save-temps            Keep temporary files (.ll, etc.)\n"
        << "  -o <file>, --out=<file>     Output filename (default: t.out)\n"
        << "  -S <path>                   Add library search path\n"
        << "  --system-compiler=<prog>    Override system compiler (default: clang)\n"
        << "  --help                      Show this help message"
        << std::endl;

    exit(1);
}

std::optional<std::pair<std::string, std::string>> checkSearchPathsFor(std::string suffix, const std::vector<std::string>& search_paths) {
	for (const std::string& dir : search_paths) {
		const std::string full_path = dir + "/" + suffix;
		if (std::filesystem::exists(full_path)) {
			return std::make_pair(dir, suffix);
		}
	}
	return std::nullopt;
}
