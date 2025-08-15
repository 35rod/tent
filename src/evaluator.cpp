#include "evaluator.hpp"

EvalExpr Evaluator::evalProgram(Program& program) {
    EvalExpr last;

    for (ExpressionStmt& stmt : program.statements) {
        EvalExpr expr = evalStmt(stmt);

        last = expr;
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
        return EvalExpr(il->value);
    } else if (auto fl = dynamic_cast<FloatLiteral*>(node)) {
        return EvalExpr(fl->value);
    } else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
        EvalExpr left = evalExpr(bin->left.get());
        EvalExpr right = evalExpr(bin->right.get());
        
        return evalBinaryOp(bin->op, left, right);

        // if (bin->op == "EQEQ") return EvalExpr(left == right ? 1 : 0);

        // throw std::runtime_error("Unknown binary operator: " + bin->op);
    } else if (auto il = dynamic_cast<NoOp*>(node)) {
        return EvalExpr(0);
    } else {
        throw std::runtime_error("Unknown AST node type in evaluator");
    }
}

EvalExpr Evaluator::evalBinaryOp(std::string op, EvalExpr left, EvalExpr right) {
    if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
        int leftVal = std::get<int>(left);
        int rightVal = std::get<int>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        }
    } else if (std::holds_alternative<int>(left) && std::holds_alternative<float>(right)) {
        int leftVal = std::get<int>(left);
        float rightVal = std::get<float>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        }
    } else if (std::holds_alternative<float>(left) && std::holds_alternative<int>(right)) {
        float leftVal = std::get<float>(left);
        int rightVal = std::get<int>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        }
    } else {
        float leftVal = std::get<float>(left);
        float rightVal = std::get<float>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        }
    }

    return EvalExpr(0);
}