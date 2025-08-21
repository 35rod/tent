#include <cctype>

bool is_dec_digit(char c)
{
	return isdigit(c);
}

bool is_hex_digit(char c)
{
	return isxdigit(c);
}

bool is_oct_digit(char c)
{
	return (c >= '0' && c <= '7');
}

bool is_bin_digit(char c)
{
	return (c >= '0' && c <= '1');
}
