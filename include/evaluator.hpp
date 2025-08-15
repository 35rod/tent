#include <iostream>
#include <string>
#include <variant>
#include "ast.hpp"

using EvalExpr = std::variant<int, float>;

class Evaluator {
    EvalExpr evalBinaryOp(std::string op, EvalExpr left, EvalExpr right);
    EvalExpr evalStmt(ExpressionStmt& stmt);
    EvalExpr evalExpr(ASTNode* node);

    public:
        EvalExpr evalProgram(Program& program);
};