#include "evaluator.hpp"
#include "errors.hpp"
#include "ast.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>

static std::string vec_to_string(const Value::VecT& vecPtr) {
	std::string out = "[";

	if (vecPtr) {
		for (size_t i = 0; i < vecPtr->size(); i++) {
			const Value& elem = (*vecPtr)[i];

			if (std::holds_alternative<nl_int_t>(elem.v))
				out += std::to_string(std::get<nl_int_t>(elem.v));
			else if (std::holds_alternative<nl_dec_t>(elem.v))
				out += FloatLiteral::to_str(std::get<nl_dec_t>(elem.v));
			else if (std::holds_alternative<nl_bool_t>(elem.v))
				out += BoolLiteral::to_str(std::get<nl_bool_t>(elem.v));
			else if (std::holds_alternative<std::string>(elem.v))
				out += std::get<std::string>(elem.v);
			else if (std::holds_alternative<Value::VecT>(elem.v))
				out += vec_to_string(std::get<Value::VecT>(elem.v));
			else
				out += "(null)";
			
			if ((i + 1) < vecPtr->size()) out += ", ";
		}
	}

	out += "]";

	return out;
}

static ASTPtr convertValue(const Value& v) {
	if (std::holds_alternative<nl_int_t>(v.v)) {
		return std::make_unique<IntLiteral>(std::get<nl_int_t>(v.v));
	} else if (std::holds_alternative<nl_dec_t>(v.v)) {
		return std::make_unique<FloatLiteral>(std::get<nl_dec_t>(v.v));
	} else if (std::holds_alternative<nl_bool_t>(v.v)) {
		return std::make_unique<BoolLiteral>(std::get<nl_bool_t>(v.v));
	} else if (std::holds_alternative<std::string>(v.v)) {
		return std::make_unique<StrLiteral>(std::get<std::string>(v.v));
	} else if (std::holds_alternative<Value::VecT>(v.v)) {
		auto vecPtr = std::get<Value::VecT>(v.v);
		std::vector<ASTPtr> elemsAst;

		if (vecPtr && !vecPtr->empty()) {
			elemsAst.reserve(vecPtr->size());
			
			for (const auto& elem : *vecPtr) {
				elemsAst.push_back(convertValue(elem));
			}
		}

		return std::make_unique<VecLiteral>(std::move(elemsAst));
	} else {
		return std::make_unique<NoOp>();
	}
}

Evaluator::Evaluator() {
	nativeFunctions["print"] = [](const std::vector<Value>& args) {
		std::string total;

		for (const Value& e : args) {
			if (std::holds_alternative<nl_int_t>(e.v))
				total += std::to_string(std::get<nl_int_t>(e.v));
			else if (std::holds_alternative<nl_dec_t>(e.v))
				total += FloatLiteral::to_str(std::get<nl_dec_t>(e.v));
			else if (std::holds_alternative<nl_bool_t>(e.v))
				total += BoolLiteral::to_str(std::get<nl_bool_t>(e.v));
			else if (std::holds_alternative<std::string>(e.v))
				total += std::get<std::string>(e.v);
			else if (std::holds_alternative<Value::VecT>(e.v))
				total += vec_to_string(std::get<Value::VecT>(e.v));
			else
				total += "(null)";
		}

		std::cout << total;

		return Value((nl_int_t)total.size());
	};

	nativeFunctions["println"] = [this](const std::vector<Value>& args) {
		Value res = this->nativeFunctions["print"](args);
		std::cout << std::endl;
		
		if (std::holds_alternative<nl_int_t>(res.v)) {
			return Value(std::get<nl_int_t>(res.v) + 1);
		}

		return Value(nl_int_t(1));
	};

	nativeFunctions["input"] = [](const std::vector<Value>& args) {
		if (!std::holds_alternative<std::string>(args[0].v)) {
			TypeError("Passed non-string argument to first parameter of input", -1);
		}

		std::string prompt = std::get<std::string>(args[0].v);
		std::string input;

		std::cout << prompt;
		std::cin >> input;

		return Value(input);
	};

	nativeFunctions["log"] = [](const std::vector<Value>& args) {
		if (std::holds_alternative<nl_int_t>(args[0].v)) {
			return Value(nl_int_t(std::log10(std::get<nl_int_t>(args[0].v))));
		} else if (std::holds_alternative<nl_dec_t>(args[0].v)) {
			return Value(nl_dec_t(std::log10(std::get<nl_dec_t>(args[0].v))));
		} else {
			TypeError("Passed non-numeric argument to first parameter of log", -1);
		}

		return Value();
	};

	nativeFunctions["exit"] = [this](const std::vector<Value>&) {
		program_should_terminate = true;

		return Value();
	};

	nativeFunctions["stoll"] = [](const std::vector<Value>& args) {
		if (!std::holds_alternative<std::string>(args[0].v))
			TypeError("passed non-string argument to first parameter of `stoll`", -1);

		size_t base = 10;

		if (args.size() > 1) {
			if (!std::holds_alternative<nl_int_t>(args[1].v))
				TypeError("passed non-integer argument to first parameter of `stoll`", -1);
			
			base = (size_t) std::get<nl_int_t>(args[1].v);
		}

		try {
			nl_int_t val = (nl_int_t) std::stoll(std::get<std::string>(args[0].v), nullptr, base);
			return Value(val);
		} catch (std::exception& e) {
			Error("stoll: failed to convert string to integer", -1);
			return Value((nl_int_t(0)));
		}
	};

	nativeFunctions["vec_from_size"] = [](const std::vector<Value>& args) {
		if (args.empty() || !std::holds_alternative<nl_int_t>(args[0].v)) {
			TypeError("passed non-integer argument to vec_from_size(size)", -1);
		}

		size_t size = (size_t)std::get<nl_int_t>(args[0].v);
		auto vec = std::make_shared<std::vector<Value>>(size, Value(nl_int_t(0)));

		return Value(vec);
	};

	nativeFunctions["len"] = [](const std::vector<Value>& args) {
		if (args.size() != 1) {
			Error("len takes exactly 1 argument, but " + std::to_string(args.size()) + " were given", -1);
		}

		if (std::holds_alternative<Value::VecT>(args[0].v)) {
			return Value(nl_int_t(std::get<Value::VecT>(args[0].v)->size()));
		} else if (std::holds_alternative<std::string>(args[0].v)) {
			return Value(nl_int_t(std::get<std::string>(args[0].v).length()));
		}

		return Value(nl_int_t(-1));
	};

	nativeFunctions["ord"] = [](const std::vector<Value>& args) {
		if (args.size() != 1)
			Error("`ord` takes exactly 1 argument, but " + std::to_string(args.size()) + " were given.", -1);

		if (const auto *s = std::get_if<std::string>(&args[0].v))
			return Value(nl_int_t((*s)[0]));
		else
			TypeError("passed non-string value to first parameter of `ord`", -1);

		return Value(nl_int_t(-1));
	};

	nativeFunctions["chr"] = [](const std::vector<Value>& args) {
		if (args.size() != 1)
			Error("`chr` takes exactly 1 argument, but " + std::to_string(args.size()) + " were given.", -1);

		if (const auto *i = std::get_if<nl_int_t>(&args[0].v))
			return Value(std::string(1, *i));
		else
			TypeError("passed non-integer value to first parameter of `chr`", -1);

		return Value("");
	};

	// nativeFunctions["vecpush"] = [this](const std::vector<EvalExpr>& args) {
	// 	if (args.size() != 2)
	// 		Error("`vecpush` takes exactly 2 arguments, but " + std::to_string(args.size()) + " were given.", -1);

	// 	if (!std::holds_alternative<std::string>(args[0]))
	// 		TypeError("passed non-string value to first parameter of `vecpush`", -1);
	// 	if (auto* vec = std::get_if<std::vector<NonVecEvalExpr>>(&variables[std::get<std::string>(args[0])])) {
	// 		if (const auto *s = std::get_if<nl_int_t>(&args[1]))
	// 			vec->push_back(*s);
	// 		else if (const auto *s = std::get_if<nl_dec_t>(&args[1]))
	// 			vec->push_back(*s);
	// 		else if (const auto *s = std::get_if<nl_bool_t>(&args[1]))
	// 			vec->push_back(*s);
	// 		else if (const auto *s = std::get_if<std::string>(&args[1]))
	// 			vec->push_back(*s);
	// 		else
	// 			TypeError("Invalid type of argument 1 of `vecpush`", -1);
	// 	} else
	// 		TypeError("Couldn't find vector '" + std::get<std::string>(args[0]) + "'", -1);

	// 	return EvalExpr(NoOp());
	// };

	// nativeFunctions["vecassign"] = [this](const std::vector<EvalExpr>& args) {
	// 	if (!std::holds_alternative<nl_int_t>(args[0])) {
	// 		TypeError("passed non-integer value to first parameter of `vecassign`", -1);
	// 	}

	// 	if (auto* vec = std::get_if<std::vector<NonVecEvalExpr>>(&variables[std::get<std::string>(args[1])])) {
	// 		nl_int_t index = std::get<nl_int_t>(args[0]);

	// 		if (index < 0 || (std::vector<NonVecEvalExpr>::size_type) index >= vec->size()) {
	// 			Error("index " + std::to_string(index) + " is out of bounds for vector of size " + std::to_string(vec->size()) + ".", -1);
	// 		}

	// 		if (const auto *s = std::get_if<nl_int_t>(&args[2]))
	// 			(*vec)[index] = *s;
	// 		else if (const auto *s = std::get_if<nl_dec_t>(&args[2]))
	// 			(*vec)[index] = *s;
	// 		else if (const auto *s = std::get_if<nl_bool_t>(&args[2]))
	// 			(*vec)[index] = *s;
	// 		else if (const auto *s = std::get_if<std::string>(&args[2]))
	// 			(*vec)[index] = *s;
	// 		else
	// 			TypeError("Invalid type of argument 2 of `vecassign`", -1);
	// 	} else {
	// 		TypeError("Couldn't find vector '" + std::get<std::string>(args[1]) + "'", -1);
	// 	}

	// 	return EvalExpr(NoOp());
	// };

	nativeFunctions["getc"] = [](const std::vector<Value>&) {
		return Value((nl_int_t)fgetc(stdin));
	};
	nativeFunctions["read_in"] = [this](const std::vector<Value>& args) {
		nl_int_t n = 1;
		if (args.size() < 1 || args.size() > 2)
			TypeError("builtin functino `read_in` takes 1 or 2 arguments, but "
					+ std::to_string(args.size()) + " were provided", -1);
		if (!std::holds_alternative<std::string>(args[0].v))
			TypeError("passed non-string value to first parameter of `read_in`, 'buf_name'", -1);
		if (args.size() == 2 && std::holds_alternative<nl_int_t>(args[1].v))
			n = std::get<nl_int_t>(args[1].v);
		else if (args.size() == 2)
			TypeError("passed non-integer value to second parameter of `read_in`, 'n'", -1);

		static uint8_t *buf;
		buf = (uint8_t *)calloc(n+1, 1);
		if (auto *s = std::get_if<std::string>(&variables[std::get<std::string>(args[0].v)].v)) {
			nl_int_t nread = fread(buf, 1, n, stdin);
			*s = std::string((char *)buf);
			return Value(nread);
		}
			
		return Value((nl_int_t)-1);
	};

	nativeFunctions["read_file"] = [](const std::vector<Value>& args) {
		if (!std::holds_alternative<std::string>(args[0].v))
			TypeError("passed non-string value to first parameter of `read_file`, `file_name`", -1);

		const std::string& FILENAME = std::get<std::string>(args[0].v);
		std::ifstream fileHandle(FILENAME);
		if (!fileHandle.is_open())
			Error("failed to open file '" + FILENAME + "'.", -1);

		std::string buf, line;
		while (std::getline(fileHandle, line)) {
			buf += line;
			buf.push_back('\n');
		}

		return Value(buf);
	};

	nativeMethods["str"]["toUpperCase"] = [](const Value& lhs, const std::vector<Value>&) {
		std::string str = std::get<std::string>(lhs.v);
		for (char& c : str) c = toupper(c);

		return Value(str);
	};

	nativeMethods["str"]["toLowerCase"] = [](const Value& lhs, const std::vector<Value>&) {
		std::string str = std::get<std::string>(lhs.v);
		for (char& c : str) c = tolower(c);

		return Value(str);
	};
}

Value Evaluator::evalProgram(ASTPtr program, const std::vector<std::string> args) {
	Value last;

	{
		auto vecPtr = std::make_shared<std::vector<Value>>();
		vecPtr->reserve(args.size());
		for (const std::string& s : args) vecPtr->push_back(Value(s));
		variables["ARGS"] = Value(vecPtr);
	}

	variables["ARG_COUNT"] = Value(nl_int_t(args.size()));
	variables["EOF"] = Value(nl_int_t(EOF));

	Program* p = dynamic_cast<Program*>(program.get());

	for (ExpressionStmt& stmt : p->statements) {
		last = evalStmt(stmt);
		if (program_should_terminate) break;
	}

	return last;
}

Value Evaluator::evalStmt(ExpressionStmt& stmt, const std::vector<Variable>& local_vars) {
	ASTNode* expr = stmt.expr.get();
	
	if (!expr) throw std::runtime_error("Invalid expression");

	return evalExpr(expr, local_vars);
}

Value Evaluator::evalExpr(ASTNode* node, const std::vector<Variable>& local_vars) {
	if (!node) throw std::runtime_error("Null AST node in evaluator");

	if (auto il = dynamic_cast<IntLiteral*>(node)) {
		return Value(il->value);
	} else if (auto fl = dynamic_cast<FloatLiteral*>(node)) {
		return Value(fl->value);
	} else if (auto sl = dynamic_cast<StrLiteral*>(node)) {
		return Value(sl->value);
	} else if (auto bl = dynamic_cast<BoolLiteral*>(node)) {
		return Value(bl->value);
	} else if (auto vl = dynamic_cast<VecLiteral*>(node)) {
		std::vector<Value> elems;
		elems.reserve(vl->elems.size());

		for (auto& elem : vl->elems) {
			elems.push_back(evalExpr(elem.get(), local_vars));
		}

		return Value(std::make_shared<std::vector<Value>>(elems));
	} else if (auto ifl = dynamic_cast<IfLiteral*>(node)) {
		const bool condition = std::get<nl_bool_t>(evalExpr(ifl->condition.get(), local_vars).v);
		Value cur_res;

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

		while (std::get<nl_bool_t>(evalExpr(wl->condition.get(), local_vars).v) == true && !break_while_loop) {
			for (ExpressionStmt& stmt : wl->stmts) {
				if (stmt.isBreak) {
					break_while_loop = true;
					break;
				} else if (stmt.isContinue) {
					break;
				}

				Value res = evalStmt(stmt, local_vars);

				if (returning) return res;
			}
		}

		return Value();
	} else if (auto fnl = dynamic_cast<FunctionLiteral*>(node)) {
		functions.push_back(fnl);

		return Value();
	} else if (auto rl = dynamic_cast<ReturnLiteral*>(node)) {
		returning = true;

		return evalExpr(rl->value.get(), local_vars);
	} else if (auto fc = dynamic_cast<FunctionCall*>(node)) {
		std::vector<Value> evalArgs;
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
			std::vector<Value> evalParams;

			for (size_t i = 0; i < func->params.size(); i++) {
				Variable* formalParam = dynamic_cast<Variable*>(func->params[i].get());
				ASTNode* paramNode = fc->params[i].get();

				Value evalValue = evalExpr(std::move(paramNode), local_vars);

				ASTPtr paramNodeEval;

				if (std::holds_alternative<nl_int_t>(evalValue.v)) {
					paramNodeEval = std::make_unique<IntLiteral>(std::get<nl_int_t>(evalValue.v));
				} else if (std::holds_alternative<nl_dec_t>(evalValue.v)) {
					paramNodeEval = std::make_unique<FloatLiteral>(std::get<nl_dec_t>(evalValue.v));
				} else if (std::holds_alternative<nl_bool_t>(evalValue.v)) {
					paramNodeEval = std::make_unique<BoolLiteral>(std::get<nl_bool_t>(evalValue.v));
				} else if (std::holds_alternative<std::string>(evalValue.v)) {
					paramNodeEval = std::make_unique<StrLiteral>(std::get<std::string>(evalValue.v));
				} else if (std::holds_alternative<Value::VecT>(evalValue.v)) {
					ASTPtr vecAst = convertValue(evalValue);
					paramNodeEval = std::move(vecAst);
				} else {
					throw std::runtime_error("Unsupported parameter type");
				}
				
				Variable newVar(formalParam->name, std::move(paramNodeEval));
				lvars.push_back(std::move(newVar));
			}

			for (ExpressionStmt& stmt : func->stmts) {
				Value res = evalStmt(stmt, lvars);

				if (returning) {
					returning = false;

					return res;
				}
			}

			return Value();
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
			
			if (std::holds_alternative<nl_int_t>(variables[var->name].v)) {
				if (un->op == TokenType::INCREMENT) {
					return variables[var->name] = std::get<nl_int_t>(variables[var->name].v) + 1;
				} else {
					return variables[var->name] = std::get<nl_int_t>(variables[var->name].v) - 1;
				}
			}
		} else {
			TypeError("Increment/decrement operator applied to non-variable", -1);
		}
	} else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
		if (isRightAssoc(bin->op)) {
			if (auto* leftIndex = dynamic_cast<BinaryOp*>(bin->left.get())) {
				if (leftIndex->op == TokenType::INDEX && bin->op == TokenType::ASSIGN) {
					if (auto* vecVar = dynamic_cast<Variable*>(leftIndex->left.get())) {
						if (variables.count(vecVar->name) != 1) SyntaxError("Undefined variable: " + vecVar->name, -1);
						Value& holder = variables[vecVar->name];
						if (!std::holds_alternative<Value::VecT>(holder.v)) TypeError("indexing non-vector", -1);
						auto vecPtr = std::get<Value::VecT>(holder.v);
						if (!vecPtr) Error("null vector", -1);

						Value idxVal = evalExpr(leftIndex->right.get(), local_vars);
						if (!std::holds_alternative<nl_int_t>(idxVal.v)) TypeError("index must be integer", -1);
						nl_int_t idx = std::get<nl_int_t>(idxVal.v);

						if (idx < 0 || (size_t)idx >= vecPtr->size())
							Error("index " + std::to_string(idx) + " is out of bounds for vector of size " + std::to_string(vecPtr->size()) + ".", -1);

						Value rhs = evalExpr(bin->right.get(), local_vars);
						(*vecPtr)[(size_t)idx] = rhs;

						return rhs;
					} else {
						TypeError("Left-hand side of indexed assignment must be a variable", -1);
					}
				}
			} else if (auto* varNode = dynamic_cast<Variable*>(bin->left.get())) {
				Value right = evalExpr(bin->right.get(), local_vars);

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
			Value lhs = evalExpr(bin->left.get(), local_vars);

			if (auto fc = dynamic_cast<FunctionCall*>(bin->right.get())) {
				std::string methodName = fc->name;
				std::vector<Value> args;

				for (const ASTPtr& param : fc->params) {
					args.push_back(evalExpr(param.get(), local_vars));
				}

				if (auto strPtr = std::get_if<std::string>(&lhs.v)) {
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

				if (auto strPtr = std::get_if<std::string>(&lhs.v)) {
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

		Value left = evalExpr(bin->left.get(), local_vars);
		Value right = evalExpr(bin->right.get(), local_vars);
		
		return evalBinaryOp(std::move(left), std::move(right), bin->op);
	} else {
		throw std::runtime_error("Unknown AST node type in evaluator");
	}

	return Value();
}

Value Evaluator::evalBinaryOp(const Value& left, const Value& right, TokenType op) {
	auto visitor = [&op](auto l, auto r) -> Value {
		using L = std::decay_t<decltype(l)>;
		using R = std::decay_t<decltype(r)>;

		if constexpr (std::is_same_v<L, NoOp> || std::is_same_v<R, NoOp>) {
			return Value();
		} else if constexpr (std::is_same_v<L, std::string> && std::is_same_v<R, std::string>) {
			switch (op) {
				case TokenType::ADD:  return Value(l + r); break;
				case TokenType::EQEQ: return Value(l == r); break;
				case TokenType::NOTEQ: return Value(l != r); break;
				default:
					throw std::runtime_error("invalid operator for string type: "
							+ std::to_string((uint16_t)op));
			}
		} else if constexpr (std::is_same_v<L, std::string> && std::is_integral_v<R>) {
			if (op == TokenType::INDEX) {
				nl_int_t idx = static_cast<nl_int_t>(r);
				if (idx < 0 || (size_t)idx >= l.size()) Error("string index out of bounds", -1);

				return Value(std::string(1, l[(size_t)idx]));
			}
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
		} else if constexpr (std::is_same_v<L, Value::VecT> && std::is_integral_v<R>) {
			auto vecPtr = l;
			if (!vecPtr) Error("null vector", -1);
			nl_int_t idx = static_cast<nl_int_t>(r);

			if (idx < 0 || (size_t)idx >= vecPtr->size()) {
				Error("index " + std::to_string(idx) + " is out of bounds for vector of size " + std::to_string(vecPtr->size()), -1);
			}

			return (*vecPtr)[(size_t)idx];
		}

		return Value();
	};

	return Value(std::visit(visitor, left.v, right.v));
}

Value Evaluator::evalUnaryOp(const Value& operand, TokenType op) {
	if (op == TokenType::NOT) {
		return Value(!std::get<nl_bool_t>(operand.v));
	} else if (op == TokenType::BIT_NOT) {
		if (std::holds_alternative<nl_int_t>(operand.v))
			return Value(~std::get<nl_int_t>(operand.v));
		else if (std::holds_alternative<nl_bool_t>(operand.v))
			return Value(~std::get<nl_int_t>(operand.v));
		else
			TypeError("failed to apply operator BIT_NOT to non-integral operand", -1);
	} else if (op == TokenType::NEGATE) {
		if (std::holds_alternative<nl_int_t>(operand.v)) {
			return Value(-std::get<nl_int_t>(operand.v));
		} else if (std::holds_alternative<nl_dec_t>(operand.v)) {
			return Value(-std::get<nl_dec_t>(operand.v));
		} else {
			TypeError("Unary minus operator applied to non-numeric type", -1);
		}
	}

	return Value();
}
