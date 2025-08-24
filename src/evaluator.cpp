#include "evaluator.hpp"
#include "errors.hpp"
#include "ast.hpp"
#include <cmath>
#include <cstdio>

Evaluator::Evaluator() {
    nativeFunctions["print"] = [](const std::vector<EvalExpr>& args) {
        std::string total;

        for (size_t i = 0; i < args.size(); i++) {
            std::visit([&total](auto&& v) {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, nl_int_t>)
                    total += IntLiteral::to_str(v);
                else if constexpr (std::is_same_v<T, nl_dec_t>)
                    total += FloatLiteral::to_str(v);
                else if constexpr (std::is_same_v<T, nl_bool_t>)
                    total += BoolLiteral::to_str(v);
                else if constexpr (std::is_same_v<T, std::string>)
                    total += v;
                else if constexpr (std::is_same_v<T, std::vector<NonVecEvalExpr>>)
                    total += VecValue::to_str(v);
                else
                    total += "(null)";
            }, args[i]);
        }

        std::cout << total;

        return EvalExpr((nl_int_t) total.size());
    };

    nativeFunctions["println"] = [this](const std::vector<EvalExpr>& args) {
        EvalExpr temp = this->nativeFunctions["print"](args);
        std::cout << std::endl;
        return EvalExpr(std::get<nl_int_t>(temp) + 1);
    };

	nativeFunctions["input"] = [](const std::vector<EvalExpr>& args) {
		if (!std::holds_alternative<std::string>(args[0])) {
			TypeError("passed non-string argument to first parameter of input", -1);
		}

		std::string prompt = std::get<std::string>(args[0]);
		std::string input;

		std::cout << prompt;
		std::cin >> input;

		return EvalExpr(input);
	};

	nativeFunctions["log"] = [](const std::vector<EvalExpr>& args) {
		if (std::holds_alternative<nl_int_t>(args[0])) {
			return EvalExpr(nl_int_t(std::log10(std::get<nl_int_t>(args[0]))));
		} else if (std::holds_alternative<nl_dec_t>(args[0])) {
			return EvalExpr(nl_dec_t(std::log10(std::get<nl_dec_t>(args[0]))));
		} else {
			TypeError("passed non-numeric argument to first parameter of log", -1);
		}

		return EvalExpr(NoOp());
	};

    nativeFunctions["exit"] = [this](const std::vector<EvalExpr>&) {
        program_should_terminate = true;

        return EvalExpr(NoOp());
    };

    nativeFunctions["stoll"] = [](const std::vector<EvalExpr>& args) {
        if (!std::holds_alternative<std::string>(args[0]))
            TypeError("passed non-string argument to first parameter of `stoll`", -1);

        size_t base = 10;

        if (args.size() > 1) {
            if (!std::holds_alternative<nl_int_t>(args[1]))
                TypeError("passed non-integer argument to first parameter of `stoll`", -1);
			
            base = (size_t) std::get<nl_int_t>(args[1]);
        }

        return std::stoll(std::get<std::string>(args[0]), nullptr, base);
    };

    nativeFunctions["veclen"] = [](const std::vector<EvalExpr>& args) {
        if (args.size() != 1)
            Error("veclen takes exactly 1 argument, but " + std::to_string(args.size()) + " were given.", -1);

        if (!std::holds_alternative<std::vector<NonVecEvalExpr>>(args[0]))
            TypeError("passed non-vector value to first parameter of `veclen`", -1);

        return EvalExpr((nl_int_t) std::get<std::vector<NonVecEvalExpr>>(args[0]).size());
    };
}

EvalExpr Evaluator::evalProgram(Program& program, const std::vector<std::string> args) {
    EvalExpr last;
    int counter = 0;

    std::vector<NonVecEvalExpr> tmp_vec;
    for (std::string s : args) {
        tmp_vec.push_back(s);
    }
    variables["ARGS"] = tmp_vec;

    variables["ARG_COUNT"] = EvalExpr((nl_int_t) args.size());

    for (ExpressionStmt& stmt : program.statements) {
        EvalExpr expr = evalStmt(stmt);

        if (!std::holds_alternative<NoOp>(expr)) {
            counter++;
            last = std::move(expr);
        }

        if (program_should_terminate)
            break;
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
    } else if (auto sl = dynamic_cast<StrLiteral*>(node)) {
        return EvalExpr(sl->value);
    } else if (auto bl = dynamic_cast<BoolLiteral*>(node)) {
        return EvalExpr(bl->value);
    } else if (auto vl = dynamic_cast<VecLiteral*>(node)) {
        std::vector<NonVecEvalExpr> ret;
        const std::vector<ASTPtr>& vec = vl->elems;
        for (auto& node : vec) {
            EvalExpr val = evalExpr(std::move(node).get(), local_vars);
            if (std::holds_alternative<nl_int_t>(val))
                ret.push_back(std::get<nl_int_t>(val));
            if (std::holds_alternative<nl_dec_t>(val))
                ret.push_back(std::get<nl_dec_t>(val));
            if (std::holds_alternative<nl_bool_t>(val))
                ret.push_back(std::get<nl_bool_t>(val));
            if (std::holds_alternative<std::string>(val))
                ret.push_back(std::get<std::string>(val));
        }
        return ret;
    } else if (auto vv = dynamic_cast<VecValue*>(node)) {
        return EvalExpr(vv->elems);
    } else if (auto ifl = dynamic_cast<IfLiteral*>(node)) {
        if (std::get<nl_bool_t>(evalExpr(ifl->condition.get(), local_vars)) == true) {
            for (ExpressionStmt& stmt : ifl->stmts) {
                if (stmt.isBreak) {
                    break;
                }

                EvalExpr res = evalStmt(stmt, local_vars);

                if (returning) return res;
            }
        }
    } else if (auto wl = dynamic_cast<WhileLiteral*>(node)) {
        while (std::get<nl_bool_t>(evalExpr(wl->condition.get(), local_vars)) == true) {
            for (ExpressionStmt& stmt : wl->stmts) {
                if (stmt.isBreak) {
                    break;
                }

                EvalExpr res = evalStmt(stmt, local_vars);

                if (returning) return res;
            }
        }

        return EvalExpr(NoOp());
    } else if (auto fnl = dynamic_cast<FunctionLiteral*>(node)) {
        functions.push_back(fnl);

        return EvalExpr(NoOp());
    } else if (auto rl = dynamic_cast<ReturnLiteral*>(node)) {
        returning = true;

        return evalExpr(rl->value.get(), local_vars);
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

                if (std::holds_alternative<nl_int_t>(evalValue)) {
                    paramNodeEval = std::make_unique<IntLiteral>(std::get<nl_int_t>(evalValue));
                } else if (std::holds_alternative<nl_dec_t>(evalValue)) {
                    paramNodeEval = std::make_unique<FloatLiteral>(std::get<nl_dec_t>(evalValue));
                } else if (std::holds_alternative<nl_bool_t>(evalValue)) {
                    paramNodeEval = std::make_unique<BoolLiteral>(std::get<nl_bool_t>(evalValue));
                } else if (std::holds_alternative<std::string>(evalValue)) {
                    paramNodeEval = std::make_unique<StrLiteral>(std::get<std::string>(evalValue));
                } else if (std::holds_alternative<std::vector<NonVecEvalExpr>>(evalValue)) {
                    paramNodeEval = std::make_unique<VecValue>(std::get<std::vector<NonVecEvalExpr>>(evalValue));

                } else {
                    throw std::runtime_error("Unsupported parameter type");
                }
                
                Variable newVar(formalParam->name, std::move(paramNodeEval));
                lvars.push_back(std::move(newVar));
            }

            for (ExpressionStmt& stmt : func->stmts) {
                EvalExpr res = evalStmt(stmt, lvars);

                if (returning) {
                    returning = false;

                    return res;
                }
            }

            return EvalExpr(NoOp());
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
        
        return evalBinaryOp(bin->op, std::move(left), std::move(right));
    } else {
        throw std::runtime_error("Unknown AST node type in evaluator");
    }

    return EvalExpr(NoOp());
}

EvalExpr Evaluator::evalBinaryOp(std::string op, EvalExpr left, EvalExpr right) {
    auto visitor = [&op](auto&& l, auto&& r) -> EvalExpr {
        using L = std::decay_t<decltype(l)>;
        using R = std::decay_t<decltype(r)>;

        if constexpr (std::is_same_v<L, NoOp> || std::is_same_v<R, NoOp>) {
            return EvalExpr(NoOp());
        } else if constexpr (std::is_same_v<L, std::string> && std::is_same_v<R, std::string>) {
            if (op == "ADD") return EvalExpr(l + r);
            if (op == "EQEQ") return EvalExpr(l == r);
            if (op == "NOTEQ") return EvalExpr(l != r);
            throw std::runtime_error("invalid operator for string type: " + op);
        } else if constexpr (std::is_same_v<L, std::string> && std::is_integral_v<R>) {
            if (op == "INDEX") return std::string(1, l[static_cast<nl_int_t>(r)]);
        } else if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
            using ResultType = std::conditional_t<
                    std::is_integral_v<L> && std::is_integral_v<R>, nl_int_t, nl_dec_t
            >;

            ResultType a = static_cast<ResultType>(l);
            ResultType b = static_cast<ResultType>(r);

            if (op == "ADD") return a + b;
            if (op == "SUB") return a - b;
            if (op == "MUL") return a * b;
            if (op == "DIV") {
                if (b == 0) throw std::runtime_error("Division by zero");

                return a / b;
            }
            if constexpr (std::is_integral_v<ResultType>) {
                using IntegralResultType = std::conditional_t<
                        std::is_same_v<L, nl_bool_t>, nl_bool_t, nl_int_t
                >;
                if (op == "MOD") return static_cast<IntegralResultType>(a % b);
                if (op == "BIN_AND") return static_cast<IntegralResultType>(a & b);
                if (op == "BIN_XOR") return static_cast<IntegralResultType>(a ^ b);
                if (op == "BIN_OR") return static_cast<IntegralResultType>(a | b);
                if (op == "LSHIFT") return static_cast<IntegralResultType>(a << b);
                if (op == "RSHIFT") return static_cast<IntegralResultType>(a >> b);
            }
            if (op == "BIN_AND" ||
                op == "BIN_XOR" ||
                op == "BIN_OR" ||
                op == "LSHIFT" ||
                op == "RSHIFT") TypeError("failed to apply bitwise operator " + op + " to non-integral operand(s)", -1);
            if (op == "MOD") return std::fmodf(a,b);
            if (op == "POW") return std::powf(a, b);

            if (op == "EQEQ") return a == b;
            if (op == "NOTEQ") return a != b;
            if (op == "LESS") return a < b;
            if (op == "LESSEQ") return a <= b;
            if (op == "GREATER") return a > b;
            if (op == "GREATEREQ") return a >= b;

            if (op == "AND") return a && b;
            if (op == "OR") return a || b;
        } else if constexpr (std::is_same_v<L, std::vector<NonVecEvalExpr>> && std::is_integral_v<R>) {
            const std::vector<NonVecEvalExpr> a = static_cast<std::vector<NonVecEvalExpr>>(l);
            nl_int_t b = static_cast<nl_int_t>(r);
            
            if (op == "INDEX") {
                if ((std::vector<NonVecEvalExpr>::size_type) b >= a.size())
                    Error("index " + std::to_string(b) + " is out of bounds for vector of size "
                            + std::to_string(a.size()) + ".", -1);

                if (const auto* s = std::get_if<nl_int_t>(&a.at(b)))
                    return *s;
                if (const auto* s = std::get_if<nl_dec_t>(&a.at(b)))
                    return *s;
                if (const auto* s = std::get_if<nl_bool_t>(&a.at(b)))
                    return *s;
                if (const auto* s = std::get_if<std::string>(&a.at(b)))
                    return *s;
            }
        }

        return EvalExpr(NoOp());
    };

    return std::visit(visitor, left, right);
}
