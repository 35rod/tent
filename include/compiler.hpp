#pragma once

#include "ast.hpp"
#include "evaluator.hpp"
#include "opcodes.hpp"

TokenType opcodeForBinaryOp(const std::string& opName);
TokenType opcodeForUnaryOp(const std::string& opName);

class Instruction {
	public:
		TokenType op;
		Value operand;

		Instruction() = default;
		Instruction(TokenType opcode) : op(opcode) {}
		Instruction(TokenType opcode, nl_int_t val) : op(opcode), operand(val) {}
		Instruction(TokenType opcode, nl_dec_t val) : op(opcode), operand(val) {}
		Instruction(TokenType opcode, std::string val) : op(opcode), operand(val) {}
		Instruction(TokenType opcode, bool val) : op(opcode), operand(val) {}
};

class Compiler {
	void compileStmt(ASTNode* node, std::vector<Instruction>& bytecode);
	void compileExpr(ASTNode* node, std::vector<Instruction>& bytecode);

	public:
		std::vector<Instruction> compileProgram(ASTPtr program);
		void saveToFile(std::vector<Instruction>& bytecode, const std::string& filename);
};
