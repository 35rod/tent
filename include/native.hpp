#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "types.hpp"

using NativeFn = Value(*)(const std::vector<Value>&);

extern std::unordered_map<std::string, NativeFn> nativeFunctions;