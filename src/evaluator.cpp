#include "evaluator.hpp"
#include <cmath>
#include <cstdint>

Evaluator::Evaluator() {
    nativeFunctions["print"] = [](const std::vector<EvalExpr>& args) {
        std::string concat_buf = "";

        for (size_t i = 0; i < args.size(); i++) {
            if (std::holds_alternative<nl_int_type>(args[i])) {
                concat_buf += std::to_string(std::get<nl_int_type>(args[i]));
            } else if (std::holds_alternative<nl_float_type>(args[i])) {
                concat_buf += std::to_string(std::get<nl_float_type>(args[i]));
            } else {
                concat_buf += "null";
            }
        }

        std::cout << concat_buf << std::endl;

        return EvalExpr(NoOp());
    };
    // this function does not work because the functionality 
    // does not exist for returned values from functions.
    /*nativeFunctions["sqrt"] = [](const std::vector<EvalExpr>& args) {
        if (std::holds_alternative<nl_int_type>(args[0]))
            return EvalExpr(sqrtf(std::get<nl_int_type>(args[0])));
        else if (std::holds_alternative<nl_float_type>(args[0]))
            return EvalExpr(sqrtf(std::get<nl_float_type>(args[0])));

        return EvalExpr(0);
    };*/
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

                if (std::holds_alternative<nl_int_type>(evalValue)) {
                    paramNodeEval = std::make_unique<IntLiteral>(std::get<nl_int_type>(evalValue));
                } else if (std::holds_alternative<nl_float_type>(evalValue)) {
                    paramNodeEval = std::make_unique<FloatLiteral>(std::get<nl_float_type>(evalValue));
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

int64_t ipow(int64_t base, uint8_t exp);

EvalExpr Evaluator::evalBinaryOp(std::string op, EvalExpr left, EvalExpr right) {
    if (std::holds_alternative<nl_int_type>(left) && std::holds_alternative<nl_int_type>(right)) {
        nl_int_type leftVal = std::get<nl_int_type>(left);
        nl_int_type rightVal = std::get<nl_int_type>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        } else if (op == "POW") {
            return EvalExpr(ipow(leftVal, rightVal));
        } else if (op == "MOD") {
            return EvalExpr(leftVal % rightVal);
        } else if (op == "BIN_AND") {
            return EvalExpr(leftVal & rightVal);
        } else if (op == "BIN_XOR") {
            return EvalExpr(leftVal ^ rightVal);
        } else if (op == "BIN_OR") {
            return EvalExpr(leftVal | rightVal);
        } else if (op == "LSHIFT") {
            return EvalExpr(leftVal << rightVal);
        } else if (op == "RSHIFT") {
            return EvalExpr(leftVal >> rightVal);
        }
    } else if (std::holds_alternative<nl_int_type>(left) && std::holds_alternative<nl_float_type>(right)) {
        nl_int_type leftVal = std::get<nl_int_type>(left);
        nl_float_type rightVal = std::get<nl_float_type>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        } else if (op == "POW") {
            return EvalExpr((nl_float_type) pow(leftVal, rightVal));
        } else if (op == "MOD") {
            return EvalExpr(std::fmod((nl_float_type) leftVal, rightVal));
        } else if (op == "BIN_AND" || op == "BIN_XOR" || op == "BIN_OR"
                || op == "LSHIFT" || op == "RSHIFT") {
            throw std::runtime_error("Unable to apply binary operator " + op + " to decimal typed operands.");
        }
    } else if (std::holds_alternative<nl_float_type>(left) && std::holds_alternative<nl_int_type>(right)) {
        nl_float_type leftVal = std::get<nl_float_type>(left);
        nl_int_type rightVal = std::get<nl_int_type>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        } else if (op == "POW") {
            return EvalExpr((nl_float_type) pow(leftVal, rightVal));
        } else if (op == "MOD") {
            return EvalExpr(std::fmod(leftVal, (nl_float_type) rightVal));
        } else if (op == "BIN_AND" || op == "BIN_XOR" || op == "BIN_OR"
                || op == "LSHIFT" || op == "RSHIFT") {
            throw std::runtime_error("Unable to apply binary operator " + op + " to decimal typed operands.");
        }
    } else {
        nl_float_type leftVal = std::get<nl_float_type>(left);
        nl_float_type rightVal = std::get<nl_float_type>(right);

        if (op == "ADD") {
            return EvalExpr(leftVal + rightVal);
        } else if (op == "SUB") {
            return EvalExpr(leftVal - rightVal);
        } else if (op == "MUL") {
            return EvalExpr(leftVal * rightVal);
        } else if (op == "DIV") {
            return EvalExpr(leftVal / rightVal);
        } else if (op == "POW") {
            return EvalExpr((nl_float_type) pow(leftVal, rightVal));
        } else if (op == "MOD") {
            return EvalExpr(std::fmod(leftVal, rightVal));
        } else if (op == "BIN_AND" || op == "BIN_XOR" || op == "BIN_OR"
                || op == "LSHIFT" || op == "RSHIFT") {
            throw std::runtime_error("Unable to apply binary operator " + op + " to decimal typed operands.");
        }
    }

    return EvalExpr(0);
}

/* https://gist.github.com/orlp/3551590 
 * --> https://en.wikipedia.org/wiki/Exponentiation_by_squaring
 * */ 
int64_t ipow(int64_t base, uint8_t exp) {
    static const uint8_t highest_bit_set[] = {
        0, 1, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 255, // anything past 63 is a guaranteed overflow with base > 1
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
    };

    int64_t result = 1;

    switch (highest_bit_set[exp]) {
    case 255: // we use 255 as an overflow marker and return 0 on overflow/underflow
        if (base == 1) {
            return 1;
        }
        
        if (base == -1) {
            return 1 - 2 * (exp & 1);
        }
        
        return 0;
    case 6:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        [[fallthrough]];
    case 5:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        [[fallthrough]];
    case 4:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        [[fallthrough]];
    case 3:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        [[fallthrough]];
    case 2:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        [[fallthrough]];
    case 1:
        if (exp & 1) result *= base;
        [[fallthrough]];
    default:
        return result;
    }

    return 0;
}
