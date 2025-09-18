#include "native.hpp"
#include <memory>
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

Value stdnl__vec_from_size(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<nl_int_t>(args[0].v))
	{
		std::cerr << "`vec_from_size` takes exactly one int argument" << std::endl;
		return Value();
	}
	Value::VecT ret = std::make_shared<std::vector<Value>>((std::vector<Value>::size_type) std::get<nl_int_t>(args[0].v));

	return Value(ret);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["exit"] = stdnl__exit;
	table["stoi"] = stdnl__stoi;
	table["stof"] = stdnl__stof;
	table["vec_from_size"] = stdnl__vec_from_size;
}
