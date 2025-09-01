#pragma once

#include <fstream>
#include <stdexcept>
#include "ast.hpp"
#include "evaluator.hpp"

enum class Opcode : uint8_t {
	PUSH_INT,
	PUSH_FLOAT,
	PUSH_STRING,
	PUSH_BOOL,
	POW,
	NOT, BIT_NOT,
	INCREMENT, DECREMENT,
	INDEX,
	MUL, DIV, FLOOR_DIV, MOD,
	ADD, SUB,
	LSHIFT, RSHIFT,
	LESS, LESSEQ, GREATER, GREATEREQ,
	EQEQ, NOTEQ,
	BIT_AND, BIT_XOR, BIT_OR,
	AND, OR,
	ASSIGN, MOD_ASSIGN, POW_ASSIGN,
	ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN,
	FLOOR_DIV_ASSIGN, AND_ASSIGN, OR_ASSIGN,
	BIT_AND_ASSIGN, BIT_XOR_ASSIGN, BIT_OR_ASSIGN,
	LSHIFT_ASSIGN, RSHIFT_ASSIGN,
	PRINT,
	PRINTLN,
};

class Instruction {
	public:
		Opcode op;
		EvalExpr operand;

		Instruction() = default;
		Instruction(Opcode opcode) : op(opcode) {}
		Instruction(Opcode opcode, nl_int_t val) : op(opcode), operand(val) {}
		Instruction(Opcode opcode, nl_dec_t val) : op(opcode), operand(val) {}
		Instruction(Opcode opcode, std::string val) : op(opcode), operand(val) {}
		Instruction(Opcode opcode, bool val) : op(opcode), operand(val) {}
};

class Compiler {
	void compileStmt(ASTNode* node, std::vector<Instruction>& bytecode);
	void compileExpr(ASTNode* node, std::vector<Instruction>& bytecode);

	public:
		std::vector<Instruction> compileProgram(ASTPtr program);
		void saveToFile(std::vector<Instruction>& bytecode, const std::string& filename);
};
