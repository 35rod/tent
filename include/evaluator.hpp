#pragma once

#include <string>
#include <variant>
#include <functional>
#include <map>
#include <unordered_map>
#include "ast.hpp"
#include "misc.hpp"

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

    EvalExpr evalBinaryOp(std::string op, EvalExpr left, EvalExpr right);
    EvalExpr evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars={});
    EvalExpr evalExpr(ASTNode* node, const std::vector<Variable>& local_vars={});

    public:
        EvalExpr evalProgram(ASTPtr program, const std::vector<std::string> args={});

        Evaluator();
};
