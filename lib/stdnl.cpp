#include "native.hpp"

#include <string>
#include <iostream>
#include <variant>
#include <unordered_map>

Value stdtn__exit(const std::vector<Value>& args) {
	int exit_code = 0;
	if (args.size() == 1 && std::holds_alternative<tn_int_t>(args[0].v))
		exit_code = (int)std::get<tn_int_t>(args[0].v);

	Value ret = Value((tn_int_t)exit_code);
	ret.isExit = true;
	return ret;
}

Value stdtn__stoi(const std::vector<Value>& args) {
	if (args.size() < 1 || args.size() > 2 || !std::holds_alternative<std::string>(args[0].v)) {
		std::cerr << "`stoi` takes 1. one string argument or 2. one string argument and an int base" << std::endl;
		return Value();
	}

	tn_int_t radix = 10;
	const std::string& s = std::get<std::string>(args[0].v);

	if (args.size() == 2 && std::holds_alternative<tn_int_t>(args[1].v))
		radix = std::get<tn_int_t>(args[1].v);

	return Value((tn_int_t)std::stoll(s, nullptr, radix));
}

Value stdtn__stof(const std::vector<Value>& args) {
	if (args.size() < 1 || args.size() > 2 || !std::holds_alternative<std::string>(args[0].v))
	{
		std::cerr << "`stof` takes exactly one string argument" << std::endl;;
		return Value();
	}
	const std::string& s = std::get<std::string>(args[0].v);

	return Value((tn_dec_t)std::stof(s, nullptr));
}

Value stdtn__isErr(const std::vector<Value>& args) {
	if (args.size() != 1)
		std::cerr << "isErr(v: any): incorrect number of arguments passed: isErr() takes one argument" << std::endl;
	return std::holds_alternative<NullLiteral>(args[0].v);
}

Value stdtn__chr(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<tn_int_t>(args[0].v))
		std::cerr << "chr(n: int): incorrect number of arguments passed: takes one 'int'" << std::endl;
	return Value(std::string(1, (char)std::get<tn_int_t>(args[0].v)));
}

Value stdtn__ord(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<tn_int_t>(args[0].v))
		std::cerr << "ord(c: str): incorrect number of arguments passed: takes one 'str'" << std::endl;

	return Value((tn_int_t)std::get<std::string>(args[0].v)[0]);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["exit"] = stdtn__exit;
	table["stoi"] = stdtn__stoi;
	table["stof"] = stdtn__stof;
	table["isErr"] = stdtn__isErr;
	table["chr"] = stdtn__chr;
	table["ord"] = stdtn__ord;
}
