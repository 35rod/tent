#pragma once

#include <string>
#include <variant>
#include <functional>
#include <map>
#include <unordered_map>
#include "ast.hpp"
#include "misc.hpp"
#include "opcodes.hpp"

using EvalExpr = std::variant<
	nl_int_t, nl_dec_t, nl_bool_t, std::string,
	std::vector<NonVecEvalExpr>, NoOp
>;

class Evaluator {
	bool returning = false;
	bool program_should_terminate = false;

	std::map<std::string, EvalExpr> variables;
	std::vector<FunctionLiteral*> functions;
	std::unordered_map<std::string, std::function<EvalExpr(const std::vector<EvalExpr>&)>> nativeFunctions;
	std::unordered_map<std::string, std::unordered_map<std::string, std::function<EvalExpr(const EvalExpr&, const std::vector<EvalExpr>&)>>> nativeMethods;

	static EvalExpr evalBinaryOp(const EvalExpr& left, const EvalExpr& right, TokenType op);
	static EvalExpr evalUnaryOp(const EvalExpr& operand, TokenType op);
	EvalExpr evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars={});
	EvalExpr evalExpr(ASTNode* node, const std::vector<Variable>& local_vars={});

	friend class VM;

	public:
		EvalExpr evalProgram(ASTPtr program, const std::vector<std::string> args={});

		Evaluator();
};
