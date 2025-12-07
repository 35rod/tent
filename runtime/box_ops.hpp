#pragma once
#include <cstdint>
#include "dynamic_value.hpp"

extern "C" DynamicValue box_int(int64_t val);
extern "C" DynamicValue box_float(double val);
extern "C" DynamicValue box_string(const char* val);
extern "C" DynamicValue box_bool(bool val);