#include <iostream>
#include <string>

#include <cstring>
#include <cstdio>

#include "args.hpp"

extern std::string SRC_FILENAME, PROG_NAME;
extern uint64_t runtime_flags;

void parse_args(int32_t argc, char **argv)
{
    PROG_NAME = std::string(argv[0]);
    for (int32_t arg_n = 1; arg_n < argc; ++arg_n)
    {
        if (argv[arg_n][0] == '-')
        {
            if (argv[arg_n][1] == '-')
            {
                if (strcmp(argv[arg_n]+2, "debug") == 0)
                    SET_FLAG(DEBUG);
                else if (strcmp(argv[arg_n]+2, "help") == 0)
                    print_usage();
                else if (strncmp(argv[arg_n]+2, "file=", 5) == 0)
                {
                    if (SRC_FILENAME.empty())
                        SRC_FILENAME = std::string(argv[arg_n]+2+5);
                    else
                    {
                        std::cout << "Argument error: found filename indicator ('" << argv[arg_n]
                            << "') after filename was already resolved." << std::endl;
                        print_usage();
                    }
                } else
                {
                    std::cout << "Argument error: unknown argument type '" << argv[arg_n] << "'." << std::endl;
                    print_usage();
                }
            } else {
                bool end_loop = false;
                for (char *pos = argv[arg_n]+1; *pos && !end_loop; pos++) 
                {
                    switch (*pos) {
                    case 'd':
                        SET_FLAG(DEBUG);
                        break;
                    case 'h':
                        print_usage();
                        break;
                    case 'f':
                        if (!SRC_FILENAME.empty())
                        {
                            std::cout << "Argument error: found filename indicator ('" << argv[arg_n]
                                << "') after filename was already resolved." << std::endl;
                            print_usage();
                        }
                        if (*(pos+1) == '\0' && argv[arg_n+1] != NULL)
                            SRC_FILENAME = std::string(argv[++arg_n]);
                        else if (*(pos+1) != '\0')
                            SRC_FILENAME = std::string(pos+1);
                        else
                        {
                            std::cout << "Argument error: found filename indicator ('" << argv[arg_n]
                                << "') without following a filename." << std::endl;
                            print_usage();
                        }
                        
                        arg_n++;
                        end_loop = true;
                        break;
                    default:
                        std::cout << "Argument error: unknown argument type '-" << *pos << "'." << std::endl;
                        print_usage();
                        break;
                    }
                }
            }
        } else if (SRC_FILENAME.empty())
            SRC_FILENAME = std::string(argv[arg_n]);
    }
    
    if (SRC_FILENAME.empty())
        print_usage();
}

void print_usage(void)
{
    std::cout << "usage: " << PROG_NAME << " [options] <FILENAME>" << std::endl << std::endl

              << "options:" << std::endl
              << "   -d, --debug                       Enable debug output" << std::endl
              << "   -h, --help                        Display this help message" << std::endl
              << "   -f FILENAME, --file=FILENAME      Read data from FILENAME" << std::endl;
    exit(1);
}


