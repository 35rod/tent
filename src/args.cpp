#include "args.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include <cstring>
#include <vector>
#include <cstdio>

extern std::string SRC_FILENAME, PROG_NAME;
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
	const char *home;
	if ((home = std::getenv("HOME")) != NULL)
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
				if (argv[arg_n][2] == '\0') {
					doing_prog_args = true;
					if (SRC_FILENAME.empty()) {
						std::cerr
							<< "Argument error: cannot pass arguments to program before the"
							<< "program filename is resolved." << std::endl;
						print_usage();
					}
				} else if (strcmp(argv[arg_n]+2, "debug") == 0)
					SET_FLAG(DEBUG);
				else if (strcmp(argv[arg_n]+2, "debug_stop") == 0)
					SET_FLAG(DEBUG_STOP);
				else if (strcmp(argv[arg_n]+2, "compile") == 0)
					SET_FLAG(COMPILE);
				else if (strcmp(argv[arg_n]+2, "repl") == 0)
					SET_FLAG(REPL);
				else if (strcmp(argv[arg_n]+2, "help") == 0)
					print_usage();
				else if (strncmp(argv[arg_n]+2, "file=", 5) == 0)
				{
					if (SRC_FILENAME.empty())
					{
						SRC_FILENAME = std::string(argv[arg_n]+2+5);
					} else
					{
						std::cerr << "Argument error: found filename indicator ('" << argv[arg_n]
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
						if (*(pos+1) == 's') {
							SET_FLAG(DEBUG_STOP);
							skip_next = true;
						}

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
					case 'S':
						if (*(pos+1) == '\0' && argv[arg_n+1] != NULL)
							search_dirs.push_back(std::string(argv[++arg_n]));
						else if (*(pos+1) != '\0')
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
							std::cerr << "Argument error: found filename indicator ('" << argv[arg_n]
								<< "') after filename was already resolved." << std::endl;
							print_usage();
						}
						if (*(pos+1) == '\0' && argv[arg_n+1] != NULL)
							SRC_FILENAME = std::string(argv[++arg_n]);
						else if (*(pos+1) != '\0')
							SRC_FILENAME = std::string(pos+1);
						else {
							std::cerr << "Argument error: found filename indicator ('" << argv[arg_n]
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
}

void print_usage(void)
{
	std::cerr << "usage: " << PROG_NAME << " [options] <FILENAME>" << std::endl << std::endl

			  << "options:" << std::endl
			  << "   -d, --debug                        Enable debug output" << std::endl
			  << "   -ds, --debug_stop                  Enable debug output and stop program before evaluator" << std::endl
			  << "   -c, --compile                      Compile program" << std::endl
			  << "   -r, --repl                         Start the interactive REPL" << std::endl
			  << "   -h, --help                         Display this help message" << std::endl
			  << "   -f FILENAME, --file=FILENAME       Read program from FILENAME" << std::endl
		  << "   -S SEARCHDIR                           Permit the `load` statement to also search for files in SEARCHDIR" << std::endl;
	exit(1);
}
