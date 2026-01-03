#pragma once

#include <string>
#include <functional>
#include <map>
#include <unordered_map>
#include "ast.hpp"
#include "types.hpp"
#include "opcodes.hpp"
#include "diagnostics.hpp"

struct CallFrame {
	std::unordered_map<std::string, Value> locals;
};

class Evaluator {
	std::string source;

	bool program_should_terminate = false;

	std::vector<CallFrame> callStack;
	std::map<std::string, Value> variables;
	std::vector<FunctionStmt*> functions;
	std::unordered_map<std::string, ClassStmt*> classes;
	std::unordered_map<std::string, std::unordered_map<std::string, std::function<Value(const Value&, const std::vector<Value>&)>>> nativeMethods;

	Diagnostics& diags;
	std::string filename;

	Value evalBinaryOp(const Value& left, const Value& right, TokenType op);
	Value evalUnaryOp(const Value& operand, TokenType op);
	Value evalStmt(ExpressionStmt& stmt);
	Value evalExpr(ASTNode* node);

	void exitErrors();

	friend class VM;

	public:
		Value evalProgram(ASTPtr program, const std::vector<std::string> args={});

		Evaluator(std::string input, Diagnostics& diagnostics, std::string fname);
};
