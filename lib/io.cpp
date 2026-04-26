#include "native.hpp"
#include "value_string.hpp"
#include <fstream>
#include <iostream>

Value print(const std::vector<Value>& args) {
	std::string total;
	for (const Value& e : args)
		total += value_to_string(e);
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
	std::getline(std::cin, input);

	return Value(input);
}

Value io__getc(const std::vector<Value>&) {
	return Value((tn_int_t)fgetc(stdin));
}

static std::vector<std::ifstream> file_handles;

Value io__file__open_file(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].v)) {
		return Value((tn_int_t)-1);
	}
	const std::string& filename = std::get<std::string>(args[0].v);
	std::ifstream temp_file(filename);
	if (!temp_file.is_open())
	{
		return Value((tn_int_t)-1);
	}

	const std::vector<std::ifstream>::size_type file_index = file_handles.size();
	file_handles.push_back(std::move(temp_file));

	return Value((tn_int_t)file_index);
}

Value io__file__read_line(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<tn_int_t>(args[0].v)) {
		std::cerr << "`File::readLine` takes a file index (int)" << std::endl;
		return Value((tn_int_t)-1);
	}

	const tn_int_t& fd = std::get<tn_int_t>(args[0].v);
	if (fd < 0 || (size_t)fd >= file_handles.size()) {
		std::cerr << "`File::readLine`: invalid file index passed" << std::endl;
		return Value((tn_int_t)-1);
	}
	std::ifstream& file = file_handles[fd];
	
	std::string output;
	std::getline(file, output);

	return Value(output);
}

Value io__file__read_file(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<tn_int_t>(args[0].v)) {
		std::cerr << "`File::readFile` takes a file index (int)" << std::endl;
		return Value((tn_int_t)-1);
	}

	const tn_int_t& fd = std::get<tn_int_t>(args[0].v);
	if (fd < 0 || (size_t)fd >= file_handles.size()) {
		std::cerr << "`File::readFile`: invalid file index passed" << std::endl;
		return Value((tn_int_t)-1);
	}

	std::ifstream& file = file_handles[fd];
	
	std::string output, line;
	while (std::getline(file, line)) {
		output += line;
		output.push_back('\n');
	}

	return Value(output);
}

Value io__file__close_file(const std::vector<Value>& args) {
	if (args.size() != 1 || !std::holds_alternative<tn_int_t>(args[0].v)) {
		std::cerr << "`File::closeFile` takes a file index (int)" << std::endl;
		return Value((tn_int_t)0);
	}

	const tn_int_t& fd = std::get<tn_int_t>(args[0].v);
	if (fd < 0 || (size_t)fd >= file_handles.size()) {
		std::cerr << "`File::closeFile`: invalid file index passed" << std::endl;
		return Value((tn_int_t)0);
	}

	std::ifstream& file = file_handles[fd];
	
	file.close();

	return Value((tn_int_t)1);
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
