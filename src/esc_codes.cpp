#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include "errors.hpp"

/* subtract '\\' before indexing */
static const char ESC_CHARS[0x76-0x5c + 1] = {
	'\\',	// 0x5c
	0,	// 0x5d
	0,	// 0x5e
	0,	// 0x5f
	0,	// 0x60
	'\a',	// 0x61
	'\b',	// 0x62
	0,	// 0x63
	0,	// 0x64
	0,	// 0x65
	'\f',	// 0x66
	0,	// 0x67
	0,	// 0x68
	0,	// 0x69
	0,	// 0x6a
	0,	// 0x6b
	0,	// 0x6c
	0,	// 0x6d
	'\n',	// 0x6e
	0,	// 0x6f
	0,	// 0x70
	0,	// 0x71
	'\r',	// 0x72
	0,	// 0x73
	'\t',	// 0x74
	0,	// 0x75
	'\v',	// 0x76
};

#define is_oct_dig(c) ((c) >= '0' && (c) < '8')
#define MAX_OCT_LEN 3

/* Returns the escape sequence length. */
uint8_t get_escape(const std::string s, char *out_c)
{
	if (s[0] == '\0')
		return 0;
	static char temp_buf[MAX_OCT_LEN+1] = {0};
	static size_t temp_size = 0;
	if (s[0] == '\\' && s[1] != '\0')
	{
		if (s[1] == '\'' || s[1] == '"')
		{
			if (out_c == NULL) return 2;
			*out_c = s[1];
			return 2;
		}
		if (s[1] >= '\\' && s[1] <= 'v' && s[1] != 'x')
		{
			if (out_c == NULL) return 2;
			*out_c = ESC_CHARS[s[1]-'\\'];
			return 2;
		}
		if (is_oct_dig(s[1]))
		{
			temp_buf[0] = s[1];
			temp_size = 2;
			while (temp_size < MAX_OCT_LEN+1 && is_oct_dig(s[temp_size]))
			{
				temp_buf[temp_size-1] = s[temp_size];
				temp_size++;
			}
			temp_buf[temp_size-1] = '\0';

			if (out_c == NULL) return temp_size-1;
			*out_c = (char) strtoul(temp_buf, NULL, 8);
			return temp_size;
		}
		if (s[1] == 'x')
		{
			if ((temp_size = s.length()) < 3)
				Error("invalid escape sequence starts around '" + s.substr(0, 5) + "'\n", -1);
			if (temp_size < 4)
			{
				if (out_c == NULL) return 3;
				temp_size = 3;
				temp_buf[1] = '\0';
			} else
			{
				if (out_c == NULL) return 4;
				temp_size = 4;
				temp_buf[1] = s[3];
				temp_buf[2] = '\0';
			}
			temp_buf[0] = s[2];
			temp_buf[1] = (s.length() < 4) ? '\0' : s[3];
			temp_buf[2] = '\0';
			*out_c = (char) strtoul(temp_buf, NULL, 16);
			return temp_size;
		}
	} 

	if (out_c == NULL) return 1;
	*out_c = s[0];
	return 1;
}

std::string read_escape(const std::string s)
{
	std::string out;
	size_t s_pos = 0;
	while (s_pos < s.length())
		s_pos += get_escape(s.substr(s_pos, s.length()-s_pos), NULL);

	s_pos = 0;
	char c = 0;
	while (s_pos < s.length())
	{
		s_pos += get_escape(s.substr(s_pos, s.length()-s_pos), &c);
		out.push_back(c);
	}

	return out;
}
