#pragma once

#include <fstream>
#include <stdexcept>
#include "ast.hpp"

enum class Opcode : uint8_t {
	PUSH_INT,
	PUSH_FLOAT,
	ADD,
	SUB,
	MUL,
	DIV,
	PRINT,
	PRINTLN,
};

class Instruction {
	public:
		Opcode op;
		std::variant<nl_int_t, nl_dec_t> operand;

		Instruction() = default;
		Instruction(Opcode opcode) : op(opcode) {}
		Instruction(Opcode opcode, nl_int_t val) : op(opcode), operand(val) {}
		Instruction(Opcode opcode, nl_dec_t val) : op(opcode), operand(val) {}
};

class Compiler {
	void compileStmt(ASTNode* node, std::vector<Instruction>& bytecode);
	void compileExpr(ASTNode* node, std::vector<Instruction>& bytecode);

	public:
		std::vector<Instruction> compileProgram(ASTPtr program);
		void saveToFile(std::vector<Instruction>& bytecode, const std::string& filename);
};
