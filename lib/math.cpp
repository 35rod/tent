#include "native.hpp"
#include <iostream>
#include <cmath>

Value nlog(const std::vector<Value>& args) {
	if (std::holds_alternative<nl_int_t>(args[0].v)) {
		return Value(nl_int_t(std::log10(std::get<nl_int_t>(args[0].v))));
	} else if (std::holds_alternative<nl_dec_t>(args[0].v)) {
		return Value(nl_dec_t(std::log10(std::get<nl_dec_t>(args[0].v))));
	} else {
		std::cerr << "Passed non-numeric argument to first parameter of log" << std::endl;
	}

	return Value();
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["log"] = nlog;
}