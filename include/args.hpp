#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#define BIT(n) (1<<n)
enum {
	DEBUG = BIT(0),
	DRY_RUN = BIT(1),
	COMPILE = BIT(2),
	REPL = BIT(3),
	SAVE_TEMPS = BIT(4),
	PRINT_LIB_PATH = BIT(5),
};

#ifndef TENT_MAIN_CPP_FILE
extern uint64_t runtime_flags;

extern std::string finalLibraryPath;
#endif

#define IS_FLAG_SET(f) ((runtime_flags & f) != 0)
#define SET_FLAG(f) (runtime_flags |= f)

void parseArgs(int32_t argc, char **argv);
void printUsage(void);
// returns a pair of the search dir in which suffix was found, and the actual suffix
// (which might be different from 'suffix' if is_dylib_prefix = true)
std::optional<std::pair<std::string, std::string>> checkSearchPathsFor(std::string suffix, const std::vector<std::string>& search_paths);
