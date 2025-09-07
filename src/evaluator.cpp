#include "evaluator.hpp"
#include "errors.hpp"
#include "ast.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>

Evaluator::Evaluator() {
	nativeFunctions["print"] = [](const std::vector<EvalExpr>& args) {
		std::string total;

		for (size_t i = 0; i < args.size(); i++) {
			std::visit([&total](auto&& v) {
				using T = std::decay_t<decltype(v)>;

				if constexpr (std::is_same_v<T, nl_int_t>)
					total += std::to_string(v);
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
			TypeError("Passed non-string argument to first parameter of input", -1);
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
			TypeError("Passed non-numeric argument to first parameter of log", -1);
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

	nativeFunctions["vec_from_size"] = [](const std::vector<EvalExpr>& args) {
		if (!std::holds_alternative<nl_int_t>(args[0]))
			TypeError("passed non-integer argument to first parameter of `vec_from_size`, `size`", -1);
		size_t size = std::get<nl_int_t>(args[0]);

		return std::vector<NonVecEvalExpr>(size, 0);
	};

	nativeFunctions["len"] = [](const std::vector<EvalExpr>& args) {
		if (args.size() != 1)
			Error("veclen takes exactly 1 argument, but " + std::to_string(args.size()) + " were given.", -1);

		if (std::holds_alternative<std::vector<NonVecEvalExpr>>(args[0]))
			return (nl_int_t) std::get<std::vector<NonVecEvalExpr>>(args[0]).size();
		if (std::holds_alternative<std::string>(args[0]))
			return (nl_int_t) std::get<std::string>(args[0]).length();
		return (nl_int_t) -1;
	};

	nativeFunctions["ord"] = [](const std::vector<EvalExpr>& args) {
		if (args.size() != 1)
			Error("`ord` takes exactly 1 argument, but " + std::to_string(args.size()) + " were given.", -1);

		if (const auto *s = std::get_if<std::string>(&args[0]))
			return (nl_int_t) (*s)[0];
		else
			TypeError("passed non-string value to first parameter of `ord`", -1);

		return (nl_int_t) -1;
	};

	nativeFunctions["chr"] = [](const std::vector<EvalExpr>& args) {
		if (args.size() != 1)
			Error("`chr` takes exactly 1 argument, but " + std::to_string(args.size()) + " were given.", -1);

		if (const auto *i = std::get_if<nl_int_t>(&args[0]))
			return std::string(1, *i);
		else
			TypeError("passed non-integer value to first parameter of `chr`", -1);

		return std::string("");
	};

	nativeFunctions["vecpush"] = [this](const std::vector<EvalExpr>& args) {
		if (args.size() != 2)
			Error("`vecpush` takes exactly 2 arguments, but " + std::to_string(args.size()) + " were given.", -1);

		if (!std::holds_alternative<std::string>(args[0]))
			TypeError("passed non-string value to first parameter of `vecpush`", -1);
		if (auto* vec = std::get_if<std::vector<NonVecEvalExpr>>(&variables[std::get<std::string>(args[0])])) {
			if (const auto *s = std::get_if<nl_int_t>(&args[1]))
				vec->push_back(*s);
			else if (const auto *s = std::get_if<nl_dec_t>(&args[1]))
				vec->push_back(*s);
			else if (const auto *s = std::get_if<nl_bool_t>(&args[1]))
				vec->push_back(*s);
			else if (const auto *s = std::get_if<std::string>(&args[1]))
				vec->push_back(*s);
			else
				TypeError("Invalid type of argument 1 of `vecpush`", -1);
		} else
			TypeError("Couldn't find vector '" + std::get<std::string>(args[0]) + "'", -1);

		return EvalExpr(NoOp());
	};

	nativeFunctions["vecassign"] = [this](const std::vector<EvalExpr>& args) {
		if (!std::holds_alternative<nl_int_t>(args[0])) {
			TypeError("passed non-integer value to first parameter of `vecassign`", -1);
		}

		if (auto* vec = std::get_if<std::vector<NonVecEvalExpr>>(&variables[std::get<std::string>(args[1])])) {
			nl_int_t index = std::get<nl_int_t>(args[0]);

			if (index < 0 || (std::vector<NonVecEvalExpr>::size_type) index >= vec->size()) {
				Error("index " + std::to_string(index) + " is out of bounds for vector of size " + std::to_string(vec->size()) + ".", -1);
			}

			if (const auto *s = std::get_if<nl_int_t>(&args[2]))
				(*vec)[index] = *s;
			else if (const auto *s = std::get_if<nl_dec_t>(&args[2]))
				(*vec)[index] = *s;
			else if (const auto *s = std::get_if<nl_bool_t>(&args[2]))
				(*vec)[index] = *s;
			else if (const auto *s = std::get_if<std::string>(&args[2]))
				(*vec)[index] = *s;
			else
				TypeError("Invalid type of argument 2 of `vecassign`", -1);
		} else {
			TypeError("Couldn't find vector '" + std::get<std::string>(args[1]) + "'", -1);
		}

		return EvalExpr(NoOp());
	};

	nativeFunctions["getc"] = [](const std::vector<EvalExpr>&) {
		return fgetc(stdin);
	};
	nativeFunctions["read_in"] = [this](const std::vector<EvalExpr>& args) {
		nl_int_t n = 1;
		if (args.size() < 1 || args.size() > 2)
			TypeError("builtin functino `read_in` takes 1 or 2 arguments, but "
					+ std::to_string(args.size()) + " were provided", -1);
		if (!std::holds_alternative<std::string>(args[0]))
			TypeError("passed non-string value to first parameter of `read_in`, 'buf_name'", -1);
		if (args.size() == 2 && std::holds_alternative<nl_int_t>(args[1]))
			n = std::get<nl_int_t>(args[1]);
		else if (args.size() == 2)
			TypeError("passed non-integer value to second parameter of `read_in`, 'n'", -1);

		static uint8_t *buf;
		buf = (uint8_t *)calloc(n+1, 1);
		if (auto *s = std::get_if<std::string>(&variables[std::get<std::string>(args[0])])) {
			nl_int_t nread = fread(buf, 1, n, stdin);
			*s = std::string((char *)buf);
			return nread; 
		}
			
		return (nl_int_t) -1;
	};

	nativeFunctions["read_file"] = [](const std::vector<EvalExpr>& args) {
		if (!std::holds_alternative<std::string>(args[0]))
			TypeError("passed non-string value to first parameter of `read_file`, `file_name`", -1);

		const std::string& FILENAME = std::get<std::string>(args[0]);
		std::ifstream fileHandle(FILENAME);
		if (!fileHandle.is_open())
			Error("failed to open file '" + FILENAME + "'.", -1);

		std::string buf, line;
		while (std::getline(fileHandle, line)) {
			buf += line;
			buf.push_back('\n');
		}

		return buf;
	};

	nativeMethods["str"]["toUpperCase"] = [](const EvalExpr& lhs, const std::vector<EvalExpr>&) {
		std::string str = std::get<std::string>(lhs);
		for (char& c : str) c = toupper(c);

		return EvalExpr(str);
	};

	nativeMethods["str"]["toLowerCase"] = [](const EvalExpr& lhs, const std::vector<EvalExpr>&) {
		std::string str = std::get<std::string>(lhs);
		for (char& c : str) c = tolower(c);

		return EvalExpr(str);
	};
}

EvalExpr Evaluator::evalProgram(ASTPtr program, const std::vector<std::string> args) {
	EvalExpr last;
	int counter = 0;

	std::vector<NonVecEvalExpr> tmp_vec;
	for (std::string s : args) {
		tmp_vec.push_back(s);
	}
	variables["ARGS"] = tmp_vec;

	variables["ARG_COUNT"] = EvalExpr((nl_int_t) args.size());
	variables["EOF"] = EvalExpr(EOF);

	Program* p = dynamic_cast<Program*>(program.get());

	for (ExpressionStmt& stmt : p->statements) {
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
		const bool condition = std::get<nl_bool_t>(evalExpr(ifl->condition.get(), local_vars));
		EvalExpr cur_res = EvalExpr(NoOp());
		if (condition) {
			for (ExpressionStmt& stmt : ifl->thenClauseStmts) {
				if (stmt.isBreak || stmt.isContinue) {
					break;
				} 

				cur_res = evalStmt(stmt, local_vars);

				if (returning) return cur_res;
			}
		} else {
			for (ExpressionStmt& stmt : ifl->elseClauseStmts) {
				if (stmt.isBreak || stmt.isContinue) {
					break;
				} 

				cur_res = evalStmt(stmt, local_vars);

				if (returning) return cur_res;
			}
		}
		return cur_res;
	} else if (auto wl = dynamic_cast<WhileLiteral*>(node)) {
		bool break_while_loop = false;
		while (std::get<nl_bool_t>(evalExpr(wl->condition.get(), local_vars)) == true && !break_while_loop) {
			for (ExpressionStmt& stmt : wl->stmts) {
				if (stmt.isBreak) {
					break_while_loop = true;
					break;
				} else if (stmt.isBreak) {
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
		for (const Variable& var : local_vars) {
			if (var.name == v->name) {
				return evalExpr(var.value.get(), local_vars);
			}
		}

		if (variables.count(v->name) == 1) {
			return variables[v->name];
		} else {
			SyntaxError("Undefined variable: " + v->name, -1);
		}
	} else if (auto un = dynamic_cast<UnaryOp*>(node)) {
		if (un->op != TokenType::INCREMENT && un->op != TokenType::DECREMENT) {
			return evalUnaryOp(evalExpr(un->operand.get(), local_vars), un->op);
		}
		if (auto var = dynamic_cast<Variable*>(un->operand.get())) {
			if (variables.count(var->name) != 1) {
				SyntaxError("Undefined variable: " + var->name, -1);
			}
			
			if (std::holds_alternative<nl_int_t>(variables[var->name])) {
				if (un->op == TokenType::INCREMENT) {
					return variables[var->name] = std::get<nl_int_t>(variables[var->name]) + 1;
				} else {
					return variables[var->name] = std::get<nl_int_t>(variables[var->name]) - 1;
				}
			}
		} else {
			TypeError("Increment/decrement operator applied to non-variable", -1);
		}
	} else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
		if (isRightAssoc(bin->op)) {
			if (auto* varNode = dynamic_cast<Variable*>(bin->left.get())) {
				EvalExpr right = evalExpr(bin->right.get(), local_vars);

				if (bin->op == TokenType::ASSIGN)
					return variables[varNode->name] = right;
				else
					return variables[varNode->name] = evalBinaryOp(
						variables[varNode->name], right,
						(TokenType)((uint16_t)bin->op -
						((uint16_t)TokenType::MOD - (uint16_t)TokenType::MOD_ASSIGN))
					);
			}
		} else if (bin->op == TokenType::DOT) {
			EvalExpr lhs = evalExpr(bin->left.get(), local_vars);

			if (auto fc = dynamic_cast<FunctionCall*>(bin->right.get())) {
				std::string methodName = fc->name;
				std::vector<EvalExpr> args;

				for (const ASTPtr& param : fc->params) {
					args.push_back(evalExpr(param.get(), local_vars));
				}

				if (auto strPtr = std::get_if<std::string>(&lhs)) {
					if (nativeMethods["str"].count(methodName)) {
						return nativeMethods["str"][methodName](*strPtr, args);
					} else {
						TypeError("Unknown string method: " + methodName, -1);
					}
				} else {
					TypeError("Method call not supported on this type", -1);
				}
			} else if (auto var = dynamic_cast<Variable*>(bin->right.get())) {
				std::string propName = var->name;

				if (auto strPtr = std::get_if<std::string>(&lhs)) {
					if (propName == "length") {
						return nl_int_t(strPtr->length());
					} else {
						TypeError("Unknown string property: " + propName, -1);
					}
				} else {
					TypeError("Property access not supported on this type", -1);
				}
			}
		}

		EvalExpr left = evalExpr(bin->left.get(), local_vars);
		EvalExpr right = evalExpr(bin->right.get(), local_vars);
		
		return evalBinaryOp(std::move(left), std::move(right), bin->op);
	} else {
		throw std::runtime_error("Unknown AST node type in evaluator");
	}

	return EvalExpr(NoOp());
}

EvalExpr Evaluator::evalBinaryOp(const EvalExpr& left, const EvalExpr& right, TokenType op) {
	auto visitor = [&op](auto&& l, auto&& r) -> EvalExpr {
		using L = std::decay_t<decltype(l)>;
		using R = std::decay_t<decltype(r)>;

		if constexpr (std::is_same_v<L, NoOp> || std::is_same_v<R, NoOp>) {
			return EvalExpr(NoOp());
		} else if constexpr (std::is_same_v<L, std::string> && std::is_same_v<R, std::string>) {
			switch (op) {
			case TokenType::ADD:  return EvalExpr(l + r); break;
			case TokenType::EQEQ: return EvalExpr(l == r); break;
			case TokenType::NOTEQ: return EvalExpr(l != r); break;
			default:
				throw std::runtime_error("invalid operator for string type: "
						+ std::to_string((uint16_t)op));
			}
		} else if constexpr (std::is_same_v<L, std::string> && std::is_integral_v<R>) {
			if (op == TokenType::INDEX) return std::string(1, l[static_cast<nl_int_t>(r)]);
		} else if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
			using ResultType = std::conditional_t<
					std::is_integral_v<L> && std::is_integral_v<R>, nl_int_t, nl_dec_t
			>;

			ResultType a = static_cast<ResultType>(l);
			ResultType b = static_cast<ResultType>(r);

			if constexpr (std::is_integral_v<ResultType>) {
				using IntegralResultType = std::conditional_t<
						std::is_same_v<L, nl_bool_t>, nl_bool_t, nl_int_t
				>;
				switch (op) {
				case TokenType::POW: return static_cast<IntegralResultType>(ipow(a, b));
				case TokenType::MOD: return static_cast<IntegralResultType>(a % b);
				case TokenType::FLOOR_DIV: return static_cast<IntegralResultType>(a / b);
				case TokenType::BIT_AND: return static_cast<IntegralResultType>(a & b);
				case TokenType::BIT_XOR: return static_cast<IntegralResultType>(a ^ b);
				case TokenType::BIT_OR: return static_cast<IntegralResultType>(a | b);
				case TokenType::LSHIFT: return static_cast<IntegralResultType>(a << b);
				case TokenType::RSHIFT: return static_cast<IntegralResultType>(a >> b);
				default:
					break;
				}
			}
			
			if (op >= TokenType::FLOOR_DIV
			 && op <= TokenType::RSHIFT)
				TypeError("failed to apply operator "
						+ std::to_string((uint16_t)op) + " to non-integral operand(s)", -1);
			switch (op) {
			case TokenType::ADD: return a + b;
			case TokenType::SUB: return a - b;
			case TokenType::MUL: return a * b;
			case TokenType::DIV:
				if (b == 0) throw std::runtime_error("Division by zero");
				return a / b;
			case TokenType::MOD: return std::fmodf(a,b);
			case TokenType::POW: return std::powf(a, b);

			case TokenType::EQEQ: return a == b;
			case TokenType::NOTEQ: return a != b;
			case TokenType::LESS: return a < b;
			case TokenType::LESSEQ: return a <= b;
			case TokenType::GREATER: return a > b;
			case TokenType::GREATEREQ: return a >= b;

			case TokenType::AND: return a && b;
			case TokenType::OR: return a || b;
			default:
				Error("unknown operator for arithmetic operands: "
						+ std::to_string((uint16_t)op), -1);
			}
		} else if constexpr (std::is_same_v<L, std::vector<NonVecEvalExpr>> && std::is_integral_v<R>) {
			const std::vector<NonVecEvalExpr> a = static_cast<std::vector<NonVecEvalExpr>>(l);
			nl_int_t b = static_cast<nl_int_t>(r);
			
			if (op == TokenType::INDEX) {
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

EvalExpr Evaluator::evalUnaryOp(const EvalExpr& operand, TokenType op) {
	if (op == TokenType::NOT) {
		return EvalExpr(!std::get<nl_bool_t>(operand));
	} else if (op == TokenType::BIT_NOT) {
		if (std::holds_alternative<nl_int_t>(operand))
			return EvalExpr(~std::get<nl_int_t>(operand));
		else if (std::holds_alternative<nl_bool_t>(operand))
			return EvalExpr(~std::get<nl_int_t>(operand));
		else
			TypeError("failed to apply operator BIT_NOT to non-integral operand", -1);
	} else if (op == TokenType::NEGATE) {
		if (std::holds_alternative<nl_int_t>(operand)) {
			return EvalExpr(-std::get<nl_int_t>(operand));
		} else if (std::holds_alternative<nl_dec_t>(operand)) {
			return EvalExpr(-std::get<nl_dec_t>(operand));
		} else {
			TypeError("Unary minus operator applied to non-numeric type", -1);
		}
	}

	return EvalExpr(NoOp());
}
