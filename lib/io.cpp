#include "../include/native.hpp"
#include <iostream>

#define MAX_DEC_LEN 50

static std::string vec_to_string(const Value::VecT& vecPtr) {
	std::string out = "[";

	if (vecPtr) {
		for (size_t i = 0; i < vecPtr->size(); i++) {
			const Value& elem = (*vecPtr)[i];

			if (std::holds_alternative<nl_int_t>(elem.v))
				out += std::to_string(std::get<nl_int_t>(elem.v));
			else if (std::holds_alternative<nl_dec_t>(elem.v)) {
				static char str_buf[MAX_DEC_LEN+1];
				std::snprintf(str_buf, MAX_DEC_LEN, "%.*f", 6, std::get<nl_dec_t>(elem.v));
				out += str_buf;
			} else if (std::holds_alternative<nl_bool_t>(elem.v))
				out += std::get<nl_bool_t>(elem.v) ? "true" : "false";
			else if (std::holds_alternative<std::string>(elem.v))
				out += std::get<std::string>(elem.v);
			else if (std::holds_alternative<Value::VecT>(elem.v))
				out += vec_to_string(std::get<Value::VecT>(elem.v));
			else
				out += "(null)";
			
			if ((i + 1) < vecPtr->size()) out += ", ";
		}
	}

	out += "]";

	return out;
}

Value print(const std::vector<Value>& args) {
	std::string total;

	for (const Value& e : args) {
		if (std::holds_alternative<nl_int_t>(e.v))
			total += std::to_string(std::get<nl_int_t>(e.v));
		else if (std::holds_alternative<nl_dec_t>(e.v)) {
			static char str_buf[MAX_DEC_LEN+1];
			std::snprintf(str_buf, MAX_DEC_LEN, "%.*f", 6, std::get<nl_dec_t>(e.v));
			total += str_buf;
		} else if (std::holds_alternative<nl_bool_t>(e.v))
			total += std::get<nl_bool_t>(e.v) ? "true" : "false";
		else if (std::holds_alternative<std::string>(e.v))
			total += std::get<std::string>(e.v);
		else if (std::holds_alternative<Value::VecT>(e.v))
			total += vec_to_string(std::get<Value::VecT>(e.v));
		else
			total += "(null)";
	}

	std::cout << total;

	return Value((nl_int_t)total.size());
}

Value println(const std::vector<Value>& args) {
	Value res = print(args);
	std::cout << std::endl;

	if (std::holds_alternative<nl_int_t>(res.v)) {
		return Value(std::get<nl_int_t>(res.v) + 1);
	}

	return Value(nl_int_t(1));
}

Value input(const std::vector<Value>& args) {
	if (!std::holds_alternative<std::string>(args[0].v)) {
		std::cerr << "Passed non-string argument to first parameter of input" << std::endl;
	}

	std::string prompt = std::get<std::string>(args[0].v);
	std::string input;

	std::cout << prompt;
	std::cin >> input;

	return Value(input);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["print"] = print;
	table["println"] = println;
	table["input"] = input;
}