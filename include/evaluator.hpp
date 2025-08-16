#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <map>
#include "ast.hpp"

using EvalExpr = std::variant<int, float, NoOp>;

class Evaluator {
    std::map<std::string, EvalExpr> variables;
    std::vector<FunctionLiteral*> functions;

    EvalExpr evalBinaryOp(std::string op, EvalExpr left, EvalExpr right);
    EvalExpr evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars={});
    EvalExpr evalExpr(ASTNode* node, const std::vector<Variable>& local_vars={});

    public:
        EvalExpr evalProgram(Program& program);
};
