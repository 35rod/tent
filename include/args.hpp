#pragma once

#include <cstdint>

#define BIT(n) (1<<n)
enum {
	DEBUG = BIT(0),
	DEBUG_STOP = BIT(1),
	COMPILE = BIT(2)
};

#define IS_FLAG_SET(f) ((runtime_flags & f) != 0)
#define SET_FLAG(f) (runtime_flags |= f)

void parse_args(int32_t argc, char **argv);
void print_usage(void);
