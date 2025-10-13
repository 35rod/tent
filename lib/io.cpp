#include "native.hpp"
#include <fstream>
#include <iostream>
#include <variant>

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
			total += "null";
	}

	std::cout << total;

	return Value();
}

Value println(const std::vector<Value>& args) {
	Value res = print(args);
	std::cout << std::endl;

	return Value();
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

Value io__getc(const std::vector<Value>&) {
	return Value((nl_int_t)fgetc(stdin));
}

static std::vector<std::ifstream> file_handles;

Value io__file__open_file(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].v)) {
		return Value((nl_int_t)-1);
	}
	const std::string& filename = std::get<std::string>(args[0].v);
	std::ifstream temp_file(filename);
	if (!temp_file.is_open())
	{
		return Value((nl_int_t)-1);
	}

	const std::vector<std::ifstream>::size_type file_index = file_handles.size();
	file_handles.push_back(std::move(temp_file));

	return Value((nl_int_t)file_index);
}

Value io__file__read_line(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<nl_int_t>(args[0].v)) {
		std::cerr << "`File::readLine` takes a file index (int)" << std::endl;
		return Value((nl_int_t)-1);
	}

	const nl_int_t& fd = std::get<nl_int_t>(args[0].v);
	if (fd == -1)
		std::cerr << "`File::readLine`: invalid file index passed" << std::endl;
	std::ifstream& file = file_handles[fd];
	
	std::string output;
	std::getline(file, output);

	return Value(output);
}

Value io__file__read_file(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<nl_int_t>(args[0].v)) {
		std::cerr << "`File::readFile` takes a file index (int)" << std::endl;
		return Value((nl_int_t)-1);
	}

	const nl_int_t& fd = std::get<nl_int_t>(args[0].v);
	if (fd == -1)
		std::cerr << "`File::readFile`: invalid file index passed" << std::endl;

	std::ifstream& file = file_handles[fd];
	
	std::string output, line;
	while (std::getline(file, line)) {
		output += line;
		output.push_back('\n');
	}

	return Value(output);
}

Value io__file__close_file(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<nl_int_t>(args[0].v)) {
		std::cerr << "`File::closeFile` takes a file index (int)" << std::endl;
		return Value((nl_int_t)0);
	}

	const nl_int_t& fd = std::get<nl_int_t>(args[0].v);
	if (fd == -1)
		std::cerr << "`File::closeFile`: invalid file index passed" << std::endl;

	std::ifstream& file = file_handles[fd];
	
	file.close();

	return Value((nl_int_t)1);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["print"] = print;
	table["println"] = println;
	table["input"] = input;

	table["getc"] = io__getc;
	table["file__openFile"] = io__file__open_file;
	table["file__readLine"] = io__file__read_line;
	table["file__readFile"] = io__file__read_file;
	table["file__closeFile"] = io__file__close_file;
}
