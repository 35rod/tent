#include "args.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "compiler.hpp"

#define strlit(s) s, (sizeof(s)-1)

extern std::string SRC_FILENAME, OUT_FILENAME, PROG_NAME, SYSTEM_COMPILER;
extern std::vector<std::string> prog_args, search_dirs;
extern uint64_t runtime_flags;

void parse_args(int32_t argc, char **argv) {
	PROG_NAME = std::string(argv[0]);

	// add some sensible defaults (the '..' ones are for 35rod)
	search_dirs.push_back(".");
	search_dirs.push_back("lib");
	search_dirs.push_back("..");
	search_dirs.push_back("../lib");

	search_dirs.push_back("/usr/lib/tent");
	search_dirs.push_back("/usr/local/lib/tent");
	const char *home = std::getenv("HOME");
	if (home != NULL)
		search_dirs.push_back(std::string(home) + "/.local/lib/tent");

	bool doing_prog_args = false;

	for (int32_t arg_n = 1; arg_n < argc; ++arg_n)
	{
		if (doing_prog_args) {
			prog_args.push_back(std::string(argv[arg_n]));
			continue;
		}

		if (argv[arg_n][0] == '-') {
			if (argv[arg_n][1] == '-') {
				if (argv[arg_n][2] == '\0') { // '--' option
					doing_prog_args = true;
					if (SRC_FILENAME.empty()) {
						std::cerr
							<< "Argument error: cannot pass arguments to program before the"
							<< "program filename is resolved." << std::endl;
						print_usage();
					}
				} else if (strcmp(argv[arg_n]+2, "debug") == 0)
					SET_FLAG(DEBUG);
				else if (strcmp(argv[arg_n]+2, "dry") == 0) {
					SET_FLAG(DRY_RUN);
					SET_FLAG(DEBUG);
				} else if (strcmp(argv[arg_n]+2, "compile") == 0)
					SET_FLAG(COMPILE);
				else if (strcmp(argv[arg_n]+2, "repl") == 0)
					SET_FLAG(REPL);
				else if (strcmp(argv[arg_n]+2, "save-temps") == 0)
					SET_FLAG(SAVE_TEMPS);
				else if (strcmp(argv[arg_n]+2, "help") == 0)
					print_usage();
				else if (strncmp(argv[arg_n]+2, strlit("system-compiler=")) == 0)
					SYSTEM_COMPILER = std::string(argv[arg_n]+2+sizeof("system-compiler=")-1);
				else if (strncmp(argv[arg_n]+2, strlit("file=")) == 0) {
					if (SRC_FILENAME.empty()) {
						SRC_FILENAME = std::string(argv[arg_n]+2 + sizeof("file=")-1);
					} else {
						std::cerr << "Argument error: found filename indicator ('" << argv[arg_n]
							<< "') after filename was already resolved." << std::endl;
						print_usage();
					}
				} else if (strncmp(argv[arg_n]+2, strlit("out=")) == 0) {
					if (OUT_FILENAME.empty()) {
						OUT_FILENAME = std::string(argv[arg_n]+2 + sizeof("out=")-1);
					} else {
						std::cerr << "Argument error: found source filename indicator ('" << argv[arg_n]
							<< "') after filename was already resolved." << std::endl;
						print_usage();
					}
				} else
				{
					std::cerr << "Argument error: unknown argument type '" << argv[arg_n] << "'." << std::endl;
					print_usage();
				}
			} else {
				bool end_loop = false;
				bool skip_next = false;

				for (char *pos = argv[arg_n]+1; *pos && !end_loop; pos++)
				{
					if (skip_next) {
						skip_next = false;
						continue;
					}

					switch (*pos) {
					case 'd':
						SET_FLAG(DEBUG);
						break;
					case 'c':
						SET_FLAG(COMPILE);
						break;
					case 'r':
						SET_FLAG(REPL);
						return;
					case 'h':
						print_usage();
						break;
					case 's':
						SET_FLAG(SAVE_TEMPS);
						break;
					case 'S':
						if (pos[1] == '\0' && argv[arg_n+1] != NULL)
							search_dirs.push_back(std::string(argv[++arg_n]));
						else if (pos[1] != '\0')
							search_dirs.push_back(std::string(pos+1));
						else
						{
							std::cerr << "Argument error: found search directory indicator ('" << argv[arg_n] << "') without a following search directory name." << std::endl;
							print_usage();
						}

						end_loop = true;
						break;
					case 'f':
						if (!SRC_FILENAME.empty()) {
							std::cerr << "Argument error: found source filename indicator ('" << argv[arg_n]
								<< "') after source filename was already resolved." << std::endl;
							print_usage();
						}
						if (pos[1] == '\0' && argv[arg_n+1] != NULL)
							SRC_FILENAME = std::string(argv[++arg_n]);
						else if (pos[1] != '\0')
							SRC_FILENAME = std::string(pos+1);
						else {
							std::cerr << "Argument error: found source filename indicator ('" << argv[arg_n]
								<< "') without a following filename." << std::endl;
							print_usage();
						}
						
						end_loop = true;
						break;
					case 'o':
						if (!OUT_FILENAME.empty()) {
							std::cerr << "Argument error: found output filename indicator ('" << argv[arg_n]
								<< "') after output filename was already resolved." << std::endl;
							print_usage();
						}
						if (pos[1] == '\0' && argv[arg_n+1] != NULL)
							OUT_FILENAME = std::string(argv[++arg_n]);
						else if (pos[1] != '\0')
							OUT_FILENAME = std::string(pos+1);
						else {
							std::cerr << "Argument error: found output filename indicator ('" << argv[arg_n]
								<< "') without a following filename." << std::endl;
							print_usage();
						}
						
						end_loop = true;
						break;
					default:
						std::cerr << "Argument error: unknown argument type '-" << *pos << "'." << std::endl;
						print_usage();
						break;
					}
				}
			}
		} else if (SRC_FILENAME.empty()) {
			SRC_FILENAME = std::string(argv[arg_n]);
		} else {
			std::cerr
				<< "Argument error: found (assumed) filename ('" << argv[arg_n]
				<< "') after filename was already resolved." << std::endl;
			print_usage();
		}
	}
	
	if (SRC_FILENAME.empty())
		print_usage();
	if (OUT_FILENAME.empty())
		OUT_FILENAME = defaultOutputExeName;
}

void print_usage(void)
{
	std::cerr << "usage: " << PROG_NAME << " [options] <FILENAME>" << std::endl << std::endl
	  << "options:" << std::endl
	  << "   -d, --debug                        Enable debug output" << std::endl
	  << "   --dry                              Dry run; Enable debug output and stop program before evaluator" << std::endl
	  << "   -c, --compile                      Compile program" << std::endl
	  << "   -o [string], --out=[string]        Save compiled executable to specified file (default: 't.out')" << std::endl
	  << "   --system-compiler=[string]         Compile '.ll' file with specified compiler (use with '-c') (default: 'clang')" << std::endl
	  << "   -s, --save-temps                   Save temporary files (like '.ll' from '-c' option) (default: no)" << std::endl
	  << "   -r, --repl                         Start the interactive REPL" << std::endl
	  << "   -h, --help                         Display this help message" << std::endl
	  << "   -f [string], --file=[string]       Read program from specified file" << std::endl
	  << "   -S [string]                        Permit the `load` statement to also search for files in specified path" << std::endl;
	exit(1);
}
