#include "ast.hpp"

class EvalExpr {
    public:
        int res;
        bool noOp;

        EvalExpr(int exprRes, bool exprNoOp);
};

class Evaluator {
    EvalExpr evalStmt(ExpressionStmt& stmt);
    EvalExpr evalExpr(ASTNode* node);

    public:
        int evalProgram(Program& program);
};