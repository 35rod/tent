#include "evaluator.hpp"

Evaluator::Evaluator() {
    nativeFunctions["print"] = [](const std::vector<EvalExpr>& args) {
        std::string total = "";

        for (size_t i = 0; i < args.size(); i++) {
            if (std::holds_alternative<int>(args[i])) {
                total += std::to_string(std::get<int>(args[i]));
            } else if (std::holds_alternative<float>(args[i])) {
                total += std::to_string(std::get<float>(args[i]));
            } else {
                total += "null";
            }
        }

        std::cout << total << std::endl;

        return EvalExpr(NoOp());
    };
}

EvalExpr Evaluator::evalProgram(Program& program) {
    EvalExpr last;
    int counter = 0;

    for (ExpressionStmt& stmt : program.statements) {
        EvalExpr expr = evalStmt(stmt);

        if (!std::holds_alternative<NoOp>(expr)) {
            counter++;
            last = expr;
        }
    }

    if (counter == 0) {
        return EvalExpr(NoOp());
    }

    return last;
}

EvalExpr Evaluator::evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars) {
    ASTNode* expr = stmt.expr.get();
    
    if (!expr) throw std::runtime_error("Invalid expression");

    return evalExpr(expr, local_vars);
}

EvalExpr Evaluator::evalExpr(ASTNode* node, const std::vector<Variable>& local_vars) {
    if (!node) throw std::runtime_error("Null AST node in evaluator");

    if (auto il = dynamic_cast<IntLiteral*>(node)) {
        return EvalExpr(il->value);
    } else if (auto fl = dynamic_cast<FloatLiteral*>(node)) {
        return EvalExpr(fl->value);
    } else if (auto fnl = dynamic_cast<FunctionLiteral*>(node)) {
        functions.push_back(fnl);

        return EvalExpr(NoOp());
    } else if (auto fc = dynamic_cast<FunctionCall*>(node)) {
        std::vector<EvalExpr> evalArgs;
        evalArgs.reserve(fc->params.size());

        for (const ASTPtr& paramPtr : fc->params) {
            if (!paramPtr) throw std::runtime_error("Null parameter AST Node");
            evalArgs.push_back(evalExpr(paramPtr.get(), local_vars));
        }

        auto nit = nativeFunctions.find(fc->name);

        if (nit != nativeFunctions.end()) {
            return nit->second(evalArgs);
        }

        FunctionLiteral* func = nullptr;

        for (FunctionLiteral* form : functions) {
            if (form->name == fc->name) {
                func = form;
                break;
            }
        }

        if (func) {
            if (fc->params.size() > func->params.size()) {
                throw std::runtime_error("Too many parameters in function call");
            } else if (fc->params.size() < func->params.size()) {
                throw std::runtime_error("Too few parameters in function call");
            }

            std::vector<Variable> lvars;
            std::vector<EvalExpr> evalParams;

            for (size_t i = 0; i < func->params.size(); i++) {
                Variable* formalParam = dynamic_cast<Variable*>(func->params[i].get());
                ASTNode* paramNode = fc->params[i].get();

                EvalExpr evalValue = evalExpr(std::move(paramNode), local_vars);

                ASTPtr paramNodeEval;

                if (std::holds_alternative<int>(evalValue)) {
                    paramNodeEval = std::make_unique<IntLiteral>(std::get<int>(evalValue));
                } else if (std::holds_alternative<float>(evalValue)) {
                    paramNodeEval = std::make_unique<FloatLiteral>(std::get<float>(evalValue));
                } else {
                    throw std::runtime_error("Unsupported parameter type");
                }
                
                Variable newVar(formalParam->name, std::move(paramNodeEval));
                lvars.push_back(std::move(newVar));
            }

            EvalExpr result = EvalExpr(NoOp());

            for (ExpressionStmt& stmt : func->stmts) {
                EvalExpr res = evalStmt(stmt, lvars);

                if (!std::holds_alternative<NoOp>(res)) {
                    result = res;
                }
            }

            return result;
        }
    } else if (auto v = dynamic_cast<Variable*>(node)) {
        if (v->context == "ASSIGN") {
            variables[v->name] = evalExpr(v->value.get(), local_vars);
        } else if (v->context == "EXPR") {
            for (const Variable& var : local_vars) {
                if (var.name == v->name) {
                    return evalExpr(var.value.get(), local_vars);
                }
            }

            if (variables.count(v->name) == 1) {
                return variables[v->name];
            } else {
                throw std::runtime_error("Unknown identifier " + v->name);
            }
        } else {
            if (variables.count(v->name) == 1) {
                variables[v->name] = evalExpr(v->value.get(), local_vars);
            } else {
                throw std::runtime_error("Cannot reassign " + v->name + ", because it does not exist");
            }
        }

        return EvalExpr(NoOp());
    } else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
        EvalExpr left = evalExpr(bin->left.get(), local_vars);
        EvalExpr right = evalExpr(bin->right.get(), local_vars);
        
        return evalBinaryOp(bin->op, left, right);

        // if (bin->op == "EQEQ") return EvalExpr(left == right ? 1 : 0);

        // throw std::runtime_error("Unknown binary operator: " + bin->op);
    } else {
        throw std::runtime_error("Unknown AST node type in evaluator");
    }

    return EvalExpr(NoOp());
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