#include "evaluator.hpp"

EvalExpr::EvalExpr(int exprRes, bool exprNoOp=false) : res(exprRes), noOp(exprNoOp) {}

int Evaluator::evalProgram(Program& program) {
    int last = 0;

    for (ExpressionStmt& stmt : program.statements) {
        EvalExpr expr = evalStmt(stmt);

        if (!expr.noOp) {
            last = expr.res;
        }
    }

    return last;
}

EvalExpr Evaluator::evalStmt(ExpressionStmt& stmt) {
    ASTNode* expr = stmt.expr.get();
    
    if (!expr) return 0;

    return evalExpr(expr);
}

EvalExpr Evaluator::evalExpr(ASTNode* node) {
    if (!node) throw std::runtime_error("Null AST node in evaluator");

    if (auto il = dynamic_cast<IntLiteral*>(node)) {
        return EvalExpr(il->value, false);
    } else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
        int left = evalExpr(bin->left.get()).res;
        int right = evalExpr(bin->right.get()).res;

        if (bin->op == "ADD") return EvalExpr(left + right);
        if (bin->op == "SUB") return EvalExpr(left - right);
        if (bin->op == "MUL") return EvalExpr(left * right);
        if (bin->op == "DIV") return EvalExpr(left / right);
        if (bin->op == "EQEQ") return EvalExpr(left == right ? 1 : 0);

        throw std::runtime_error("Unknown binary operator: " + bin->op);
    } else if (auto il = dynamic_cast<NoOp*>(node)) {
        return EvalExpr(0, true);
    } else {
        throw std::runtime_error("Unknown AST node type in evaluator");
    }
}