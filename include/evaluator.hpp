#pragma once

#include <string>
#include <variant>
#include <functional>
#include <map>
#include <unordered_map>
#include "ast.hpp"
#include "types.hpp"
#include "opcodes.hpp"

class Evaluator {
	bool returning = false;
	bool program_should_terminate = false;

	std::map<std::string, Value> variables;
	std::vector<FunctionLiteral*> functions;
	std::unordered_map<std::string, std::unordered_map<std::string, std::function<Value(const Value&, const std::vector<Value>&)>>> nativeMethods;

	static Value evalBinaryOp(const Value& left, const Value& right, TokenType op);
	static Value evalUnaryOp(const Value& operand, TokenType op);
	Value evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars={});
	Value evalExpr(ASTNode* node, const std::vector<Variable>& local_vars={});

	friend class VM;

	public:
		bool load(const std::string& path);
		Value evalProgram(ASTPtr program, const std::vector<std::string> args={});

		Evaluator();
};
