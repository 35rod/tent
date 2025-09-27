#include "native.hpp"

#include <string>
#include <iostream>
#include <variant>
#include <unordered_map>

Value stdnl__exit(const std::vector<Value>& args) {
	int exit_code = 0;
	if (args.size() == 1 && std::holds_alternative<nl_int_t>(args[0].v))
		exit_code = (int)std::get<nl_int_t>(args[0].v);

	Value ret = Value((nl_int_t)exit_code);
	ret.isExit = true;
	return ret;
}

Value stdnl__stoi(const std::vector<Value>& args) {
	if (args.size() < 1 || args.size() > 2 || !std::holds_alternative<std::string>(args[0].v))
	{
		std::cerr << "`stoi` takes 1. one string argument or 2. one string argument and an int base" << std::endl;
		return Value();
	}
	nl_int_t radix = 10;
	const std::string& s = std::get<std::string>(args[0].v);
	if (args.size() == 2 && std::holds_alternative<nl_int_t>(args[1].v))
		radix = std::get<nl_int_t>(args[1].v);

	return Value((nl_int_t)std::stoll(s, nullptr, radix));
}

Value stdnl__stof(const std::vector<Value>& args) {
	if (args.size() < 1 || args.size() > 2 || !std::holds_alternative<std::string>(args[0].v))
	{
		std::cerr << "`stof` takes exactly one string argument" << std::endl;;
		return Value();
	}
	const std::string& s = std::get<std::string>(args[0].v);

	return Value((nl_dec_t)std::stof(s, nullptr));
}

Value stdnl__isErr(const std::vector<Value>& args) {
	if (args.size() != 1)
		std::cerr << "isErr(v: any): incorrect number of arguments passed: isErr() takes one argument" << std::endl;
	return std::holds_alternative<NoOp>(args[0].v);
}

Value stdnl__chr(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<nl_int_t>(args[0].v))
		std::cerr << "chr(n: int): incorrect number of arguments passed: takes one 'int'" << std::endl;
	return Value(std::string(1, (char)std::get<nl_int_t>(args[0].v)));
}

Value stdnl__ord(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<nl_int_t>(args[0].v))
		std::cerr << "ord(c: str): incorrect number of arguments passed: takes one 'str'" << std::endl;
	return Value((nl_int_t)std::get<std::string>(args[0].v)[0]);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["exit"] = stdnl__exit;
	table["stoi"] = stdnl__stoi;
	table["stof"] = stdnl__stof;
	table["isErr"] = stdnl__isErr;
	table["chr"] = stdnl__chr;
	table["ord"] = stdnl__ord;
}
