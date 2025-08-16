#include <iostream>
#include <string>
#include <variant>
#include <map>
#include "ast.hpp"

using EvalExpr = std::variant<int, float, NoOp>;

class Evaluator {
    std::map<std::string, EvalExpr> variables;

    EvalExpr evalBinaryOp(std::string op, EvalExpr left, EvalExpr right);
    EvalExpr evalStmt(ExpressionStmt& stmt);
    EvalExpr evalExpr(ASTNode* node);

    public:
        EvalExpr evalProgram(Program& program);
};