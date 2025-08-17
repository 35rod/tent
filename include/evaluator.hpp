#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <variant>
#include <functional>
#include <map>
#include <unordered_map>
#include "ast.hpp"

using EvalExpr = std::variant<int, float, std::string, NoOp>;

class Evaluator {
    std::map<std::string, EvalExpr> variables;
    std::vector<FunctionLiteral*> functions;
    std::unordered_map<std::string, std::function<EvalExpr(const std::vector<EvalExpr>&)>> nativeFunctions;

    std::string floatToString(float v, int prec);

    EvalExpr evalBinaryOp(std::string op, EvalExpr left, EvalExpr right);
    EvalExpr evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars={});
    EvalExpr evalExpr(ASTNode* node, const std::vector<Variable>& local_vars={});

    public:
        EvalExpr evalProgram(Program& program);

        Evaluator();
};
