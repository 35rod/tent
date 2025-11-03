#pragma once

#include "ast.hpp"

static const std::string defaultOutputExeName = "t.out";

class Compiler {
	public:
		Compiler() = delete;

		static void compile(Program* program, const std::string& outputExe = defaultOutputExeName, const std::string& moduleName = "TentProgram");
};
