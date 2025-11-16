#include "args.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "compiler.hpp"

#define strlit(s) s, (sizeof(s)-1)

void parse_args(int32_t argc, char **argv, Config& config) {
	config.prog_name = std::string(argv[0]);

	// add some sensible defaults (the '..' ones are for 35rod)
	config.search_dirs.push_back(".");
	config.search_dirs.push_back("lib");
	config.search_dirs.push_back("..");
	config.search_dirs.push_back("../lib");

	config.search_dirs.push_back("/usr/lib/tent");
	config.search_dirs.push_back("/usr/local/lib/tent");
	const char *home = std::getenv("HOME");
	if (home != nullptr)
		config.search_dirs.push_back(std::string(home) + "/.local/lib/tent");

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
		config.set_flag(COMPILE);
	else if (command == "repl")
		config.set_flag(REPL);
	else if (command == "help")
		print_usage();
	
	for (; arg_i < argc; arg_i++) {
		std::string arg = argv[arg_i];

		if (doing_prog_args) {
			config.prog_args.push_back(arg);
			continue;
		}

		if (arg == "--") {
			doing_prog_args = true;
			continue;
		}

		if (arg == "-d" || arg == "--debug")
			config.set_flag(DEBUG);
		else if (arg == "--dry") {
			config.set_flag(DRY_RUN);
			config.set_flag(DEBUG);
		} else if (arg == "-s" || arg == "--save-temps")
			config.set_flag(SAVE_TEMPS);
		else if (arg.rfind("--system-compiler=", 0) == 0)
			config.system_compiler = arg.substr(18);
		else if (arg == "-o") {
			if (arg_i + 1 >= argc) {
				std::cerr << "Missing output filename after '-o'\n";
				print_usage();
			}

			config.out_filename = argv[arg_i++];
		} else if (arg.rfind("--out=", 0) == 0)
			config.out_filename = arg.substr(6);
		else if (arg.rfind("-S", 0) == 0) {
			if (arg.size() > 2)
				config.search_dirs.push_back(arg.substr(2));
			else if (arg_i + 1 < argc)
				config.search_dirs.push_back(argv[arg_i++]);
			else {
				std::cerr << "Missing path after '-S'\n";
				print_usage();
			}
		} else if (arg[0] == '-') {
			std::cerr << "Unknown option: " << arg << "\n";
			print_usage();
		} else if (config.src_filename.empty())
			config.src_filename = arg;
		else
			config.prog_args.push_back(arg);
	}

	if (command.empty() && config.src_filename.empty())
		config.set_flag(REPL);
	
	if (config.is_flag_set(COMPILE)) {
		if (config.src_filename.empty()) {
			std::cerr << "Compile mode requires a source file\n";
			print_usage();
		}

		if (config.out_filename.empty())
			config.out_filename = defaultOutputExeName;
	} else if (config.is_flag_set(REPL)) {
		if (!config.src_filename.empty()) {
			std::cerr << "REPL mode does not take a file argument\n";
			print_usage();
		}
	} else {
		if (config.src_filename.empty()) {
			std::cerr << "No input file specified\n";
			print_usage();
		}
	}
}

void print_usage(void) {
	std::cerr
        << "Usage:\n"
        << "  " << "tent" << " <file> [options]         Run a Tent source file\n"
        << "  " << "tent" << " compile <file> [options]  Compile Tent source file\n"
        << "  " << "tent" << " repl                      Start interactive REPL\n"
        << "  " << "tent" << " help                      Show this help message\n\n"
        << "Options:\n"
        << "  -d, --debug                 Enable debug output\n"
        << "  --dry                       Dry run (implies debug)\n"
        << "  -s, --save-temps            Keep temporary files (.ll, etc.)\n"
        << "  -o <file>, --out=<file>     Output filename (default: t.out)\n"
        << "  -S <path>                   Add library search path\n"
        << "  --system-compiler=<prog>    Override system compiler (default: clang)\n"
        << std::endl;
	
    exit(1);
}
