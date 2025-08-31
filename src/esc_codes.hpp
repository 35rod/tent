#pragma once

#include <cstdint>
#include <string>

uint8_t get_escape(const std::string s, char *out_c);
std::string read_escape(const std::string s);
