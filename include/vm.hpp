#pragma once

#include "compiler.hpp"
#include "evaluator.hpp"

class VM {
	std::vector<EvalExpr> stack;

	public:
		VM() {}
		
		std::vector<Instruction> loadFile(const std::string& filename);
		void run(const std::vector<Instruction>& bytecode);
};
