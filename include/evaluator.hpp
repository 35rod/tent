#pragma once

#include <string>
#include <functional>
#include <map>
#include <unordered_map>
#include "ast.hpp"
#include "types.hpp"
#include "opcodes.hpp"

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

	int lineNo = -1;
	int colNo = -1;
	std::string filename = "";

	static Value evalBinaryOp(const Value& left, const Value& right, TokenType op);
	static Value evalUnaryOp(const Value& operand, TokenType op);
	Value evalStmt(ExpressionStmt& stmt);
	Value evalExpr(ASTNode* node);

	friend class VM;

	public:
		bool load(const std::string& path);
		Value evalProgram(ASTPtr program, const std::vector<std::string> args={});

		Evaluator(std::string input);
};
