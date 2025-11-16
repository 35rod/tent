#pragma once

#include "ast.hpp"
#include "config.hpp"

static const std::string defaultOutputExeName = "t.out";

class Compiler {
	public:
		Compiler() = delete;

		static void compile(const Config& config, Program* program, const std::string& outputExe = defaultOutputExeName, const std::string& moduleName = "TentProgram");
};
