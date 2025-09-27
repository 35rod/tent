#pragma once

#include <iostream>
#include <fstream>
#include "compiler.hpp"
#include "native.hpp"
#include "evaluator.hpp"

struct VMFunc {
	std::string name;
	std::vector<std::string> params;
	std::vector<Instruction> bytecode;
};

class VM {
	std::vector<Value> stack;
	std::unordered_map<std::string, Value> variables;
	std::vector<CallFrame> callStack;
	std::unordered_map<std::string, NativeFn> vmNativeFunctions;
	std::unordered_map<std::string, VMFunc> functions;

	public:
		VM() {}
		
		std::vector<Instruction> loadFile(const std::string& filename, const std::vector<std::string>& search_dirs);
		void run(const std::vector<Instruction>& bytecode);
};
