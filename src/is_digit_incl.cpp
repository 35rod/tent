#ifndef IS_DIGIT_INCL
#define IS_DIGIT_INCL

#include <cctype>

static inline bool is_dec_digit(char c)
{
	return isdigit(c);
}

static inline bool is_hex_digit(char c)
{
	return isxdigit(c);
}

static inline bool is_oct_digit(char c)
{
	return (c >= '0' && c <= '7');
}

static inline bool is_bin_digit(char c)
{
	return (c >= '0' && c <= '1');
}

#endif /* IS_DIGIT_INCL */
