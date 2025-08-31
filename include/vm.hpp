#pragma once

#include <iostream>
#include <fstream>
#include "compiler.hpp"
#include "evaluator.hpp"

class VM {
	std::vector<EvalExpr> stack;

	EvalExpr applyBinaryOp(const EvalExpr& a, const EvalExpr& b, Opcode op);

	public:
		VM() {}
		
		std::vector<Instruction> loadFile(const std::string& filename);
		void run(const std::vector<Instruction>& bytecode);
};