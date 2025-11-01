#pragma once

#include "ast.hpp"

class Compiler {
	public:
		Compiler() = default;

		void compile(Program* program, const std::string& moduleName = "TentProgram", const std::string& outputExe = "a.out");
};