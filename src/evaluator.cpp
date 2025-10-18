#include "evaluator.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <dlfcn.h>
#include <exception>
#include <stdexcept>
#include <string>
#include <variant>

#include "native.hpp"
#include "types.hpp"
#include "opcodes.hpp"
#include "errors.hpp"
#include "ast.hpp"

Evaluator::Evaluator(std::string input) : source(input) {
	nativeMethods["type_int"]["parse"] = [&](const Value&, const std::vector<Value>& rhs) {
		if (!std::holds_alternative<std::string>(rhs[0].v)) {
			TypeError(
				"int.parse(s: str[, b: int]): invalid argument(s) passed: first argument must be 'string'",
				lineNo,
				colNo,
				filename,
				"",
				getLineText(source, lineNo)
			).print();
			exit(1);
		}

		int base = 0; // special value that allows for 0x10 or 0b010 or the like
		if (rhs.size() == 2 && std::holds_alternative<tn_int_t>(rhs[1].v))
			base = std::get<tn_int_t>(rhs[1].v);
		else if (rhs.size() == 2) {
			TypeError(
				"int.parse(s: str[, b: int]): invalid argument(s) passed: second argument must be 'int', if present",
				lineNo,
				colNo,
				filename,
				"",
				getLineText(source, lineNo)
			).print();
			exit(1);
		}

		try {
			return Value(tn_int_t(std::stoi(std::get<std::string>(rhs[0].v), nullptr, base)));
		} catch (std::exception&) {
			// should probably return an exception or error value but we don't have those yet, so this is what I've got
			return Value();
		}
	};

	nativeMethods["type_vec"]["fill"] = [&](const Value&, const std::vector<Value>& rhs) {
		// vec.fill(n: int[, v: any]): return a vector of size 'n', optionally filled with 'v'.
		Value::VecT ret = std::make_shared<std::vector<Value>>((std::vector<Value>::size_type) std::get<tn_int_t>(rhs[0].v));

		if (rhs.size() > 1) {
			if (!is_primitive_val(rhs[1])) {
				TypeError(
					"vec.fill(n: int[, v: any\\class_instance]): invalid argument(s) passed: second argument is of an invalid type for this function",
					lineNo,
					colNo,
					filename,
					"",
					getLineText(source, lineNo)
				).print();
				exit(1);
			}

			std::fill(ret->begin(), ret->end(), rhs[1]);
		}

		return Value(ret);
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

	nativeMethods["str"]["len"] = [](const Value& lhs, const std::vector<Value>&) {
		const std::string& str = std::get<std::string>(lhs.v);
		return Value((tn_int_t)str.length());
	};

	nativeMethods["vec"]["len"] = [](const Value& lhs, const std::vector<Value>&) {
		Value::VecT vec = std::get<Value::VecT>(lhs.v);
		return Value((tn_int_t)vec->size());
	};
	nativeMethods["vec"]["push"] = [](const Value& lhs, const std::vector<Value>& rhs) {
		Value::VecT vec = std::get<Value::VecT>(lhs.v);

		if (auto ip = std::get_if<tn_int_t>(&rhs[0].v)) {
			vec->push_back(*ip);
		}

		return Value();
	};
	nativeMethods["vec"]["pop"] = [&](const Value& lhs, const std::vector<Value>&) {
		Value::VecT vec = std::get<Value::VecT>(lhs.v);

		if (vec->size() < 1) {
			Error(
				"attempted to pop an element back from an empty vector",
				lineNo,
				colNo,
				filename,
				"",
				getLineText(source, lineNo)
			).print();
			exit(1);
		}

		Value ret = vec->back();
		vec->pop_back();
		return ret;
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

	variables["ARG_COUNT"] = Value(tn_int_t(args.size()));
	variables["EOF"] = Value(tn_int_t(EOF));

	Program* p = dynamic_cast<Program*>(program.get());

	for (ExpressionStmt& stmt : p->statements) {
		last = evalStmt(stmt);
		if (last.isExit) break;
	}

	return last;
}

Value Evaluator::evalStmt(ExpressionStmt& stmt) {
	ASTNode* expr = stmt.expr.get();
	
	if (!expr) throw std::runtime_error("Invalid expression");

	return evalExpr(expr);
}

Value Evaluator::evalExpr(ASTNode* node) {
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
			elems.push_back(evalExpr(elem.get()));
		}

		return Value(std::make_shared<std::vector<Value>>(elems));
	} else if (auto dl = dynamic_cast<DicLiteral*>(node)) {
		std::map<std::string, Value> dic;
		
		for (auto& pair : dl->dic) {
			const Value first_in_pair = evalExpr(pair.first.get());
			if (!std::holds_alternative<std::string>(first_in_pair.v)) {
				Error(
					"dictionary key must be a string",
					dl->lineNo,
					dl->colNo,
					dl->filename,
					"",
					getLineText(source, dl->lineNo)
				).print();
				exit(1);
			}

			dic[std::get<std::string>(first_in_pair.v)] = evalExpr(pair.second.get());
		}

		return Value(std::make_shared<std::map<std::string, Value>>(dic));
	} else if (dynamic_cast<TypeInt*>(node)) {
        Value res;
        res.typeInt = true;
        return res;
	} else if (dynamic_cast<TypeFloat*>(node)) {
		Value res;
		res.typeFloat = true;
		return res;
	} else if (dynamic_cast<TypeStr*>(node)) {
		Value res;
		res.typeStr = true;
		return res;
	} else if (dynamic_cast<TypeBool*>(node)) {
		Value res;
		res.typeBool = true;
		return res;
	} else if (dynamic_cast<TypeVec*>(node)) {
		Value res;
		res.typeVec = true;
		return res;
	} else if (dynamic_cast<TypeDic*>(node)) {
		Value res;
		res.typeDic = true;
		return res;
	} else if (auto ifl = dynamic_cast<IfStmt*>(node)) {
		const bool condition = std::get<tn_bool_t>(evalExpr(ifl->condition.get()).v);
		Value cur_res;

		if (condition) {
			for (ExpressionStmt& stmt : ifl->thenClauseStmts) {
				if (stmt.isBreak || stmt.isContinue) {
					break;
				} 

				cur_res = evalStmt(stmt);

				if (cur_res.isReturn || cur_res.isExit) {
					return cur_res;
				}
			}
		} else {
			for (ExpressionStmt& stmt : ifl->elseClauseStmts) {
				if (stmt.isBreak || stmt.isContinue) {
					break;
				} 

				cur_res = evalStmt(stmt);

				if (cur_res.isReturn || cur_res.isExit) {
					return cur_res;
				}
			}
		}

		return cur_res;
	} else if (auto wl = dynamic_cast<WhileStmt*>(node)) {
		bool break_while_loop = false;

		while (std::get<tn_bool_t>(evalExpr(wl->condition.get()).v) == true && !break_while_loop) {
			for (ExpressionStmt& stmt : wl->stmts) {
				if (stmt.isBreak) {
					break_while_loop = true;
					break;
				} else if (stmt.isContinue) {
					break;
				}

				Value res = evalStmt(stmt);

				if (res.isReturn || res.isExit) {
					return res;
				}
			}
		}

		return Value();
	} else if (auto fl = dynamic_cast<ForStmt*>(node)) {
	    bool break_for_loop = false;
	    int index = 0;
	    int length = 0;
		Value::DicT::element_type::iterator dic_it;
		bool is_dic = false;
		
	    Value iter = evalExpr(fl->iter.get());

	    if (std::holds_alternative<tn_int_t>(iter.v)) {
	        length = std::get<tn_int_t>(iter.v);
	    } else if (std::holds_alternative<std::string>(iter.v)) {
	        length = std::get<std::string>(iter.v).size();
	    } else if (std::holds_alternative<Value::VecT>(iter.v)) {
	        length = std::get<Value::VecT>(iter.v)->size();
	    } else if (std::holds_alternative<Value::DicT>(iter.v)) {
			dic_it = std::get<Value::DicT>(iter.v)->begin();
			is_dic = true;
		}

        while (((is_dic && dic_it != std::get<Value::DicT>(iter.v)->end()) || (index < length))
				&& !break_for_loop) {
            if (std::holds_alternative<tn_int_t>(iter.v)) {
                variables[fl->var] = Value((tn_int_t)index);
            } else if (std::holds_alternative<std::string>(iter.v)) {
                variables[fl->var] = Value(std::string(1, std::get<std::string>(iter.v)[index]));
            } else if (std::holds_alternative<Value::VecT>(iter.v)) {
                variables[fl->var] = (*std::get<Value::VecT>(iter.v))[index];
            } else if (std::holds_alternative<Value::DicT>(iter.v)) {
				std::vector<Value> single = {Value(dic_it->first), dic_it->second};
				variables[fl->var] = Value(std::make_shared<Value::VecT::element_type>(single));

				dic_it++;
			}

            for (ExpressionStmt& stmt : fl->stmts) {
                if (stmt.isBreak) {
                    break_for_loop = true;
                    break;
                } else if (stmt.isContinue) {
                    break;
                }

                Value res = evalStmt(stmt);

                if (res.isReturn || res.isExit) {
                    return res;
                }
            }

            index++;
        }
	} else if (auto fnl = dynamic_cast<FunctionStmt*>(node)) {
		functions.push_back(fnl);

		return Value();
	} else if (auto rl = dynamic_cast<ReturnStmt*>(node)) {
		Value v = evalExpr(rl->value.get());
		v.isReturn = true;

		return v;
	} else if (auto cl = dynamic_cast<ClassStmt*>(node)) {
		classes[cl->name] = cl;

		return Value();
	} else if (auto fc = dynamic_cast<FunctionCall*>(node)) {
		if (classes.count(fc->name)) {
			// class constructor call
			ClassStmt* classDef = classes[fc->name];
			
			Value::ClassInstance instance(classDef->name);

			CallFrame frame;

			for (size_t i = 0; i < fc->params.size(); i++) {
				Variable* paramVar = dynamic_cast<Variable*>(classDef->params[i].get());
				Value argVal = evalExpr(fc->params[i].get());
				instance.fields[paramVar->name] = argVal;
				frame.locals[paramVar->name] = argVal;
			}

			callStack.push_back(std::move(frame));

			for (ExpressionStmt& stmt : classDef->stmts) {
				if (auto fn = dynamic_cast<FunctionStmt*>(stmt.expr.get())) {
					instance.methods[fn->name] = fn;
				} else if (auto bin = dynamic_cast<BinaryOp*>(stmt.expr.get())) {
					if (auto var = dynamic_cast<Variable*>(bin->left.get())) {
						instance.fields[var->name] = evalExpr(bin->right.get());
					}
				} else if (auto varStmt = dynamic_cast<Variable*>(stmt.expr.get())) {
					instance.fields[varStmt->name] = Value();
				} else {
					evalStmt(stmt);
				}
			}

			callStack.pop_back();

			return Value(instance);
		}

		FunctionStmt* func = nullptr;

		for (FunctionStmt* f : functions) {
			if (f->name == fc->name) {
				func = f;
				break;
			}
		}

		if (!func) {
			auto nit = nativeFunctions.find(fc->name);

			if (nit != nativeFunctions.end()) {
				std::vector<Value> evalArgs;
				evalArgs.reserve(fc->params.size());

				for (const auto& param : fc->params) {
					if (!param)  {
						Error(
							"Null parameter AST Node",
							param->lineNo,
							param->colNo,
							param->filename,
							"",
							getLineText(source, fc->lineNo)
						).print();
						exit(1);
					}

					evalArgs.push_back(evalExpr(param.get()));
				}

				return nit->second(evalArgs);
			} else {
				Error(
					"Undefined function: " + fc->name,
					fc->lineNo,
					fc->colNo,
					fc->filename,
					"",
					getLineText(source, fc->lineNo)
				).print();
				exit(1);
			}
		}

		if (fc->params.size() != func->params.size()) {
			Error(
				"Parameter count mismatch in function call to " + fc->name,
				fc->lineNo,
				fc->colNo,
				fc->filename,
				getLineText(source, fc->lineNo)
			).print();
			exit(1);
		}

		CallFrame frame;

		for (size_t i = 0; i < func->params.size(); i++) {
			Variable* formalParam = dynamic_cast<Variable*>(func->params[i].get());
			if (!formalParam) {
				Error(
					"Function parameter is not a variable",
					formalParam->lineNo,
					formalParam->colNo,
					formalParam->filename,
					"",
					getLineText(source, formalParam->lineNo)
				).print();
				exit(1);
			}
			
			Value evalValue = evalExpr(fc->params[i].get());
			frame.locals[formalParam->name] = evalValue;
		}

		callStack.push_back(std::move(frame));

		Value result;

		for (ExpressionStmt& stmt : func->stmts) {
			result = evalStmt(stmt);

			if (result.isReturn) {
				result.isReturn = false;
				callStack.pop_back();

				return result;
			}
			if (result.isExit)
				return result;
		}

		callStack.pop_back();

		return result;
	} else if (auto v = dynamic_cast<Variable*>(node)) {
		for (auto it = callStack.rbegin(); it != callStack.rend(); it++) {
			auto found = it->locals.find(v->name);

			if (found != it->locals.end()) {
				return found->second;
			}
		}

		if (variables.count(v->name)) {
			return variables[v->name];
		} else {
			SyntaxError(
				"Undefined variable: " + v->name,
				v->lineNo,
				v->colNo,
				v->filename,
				"",
				getLineText(source, v->lineNo)
			).print();
			exit(1);
		}
	} else if (auto un = dynamic_cast<UnaryOp*>(node)) {
		if (un->op != TokenType::INCREMENT && un->op != TokenType::DECREMENT) {
			return evalUnaryOp(evalExpr(un->operand.get()), un->op);
		}

		if (auto var = dynamic_cast<Variable*>(un->operand.get())) {
			if (variables.count(var->name) != 1) {
				SyntaxError(
					"Undefined variable: " + var->name,
					var->lineNo,
					var->colNo,
					var->filename,
					"",
					getLineText(source, var->lineNo)
				).print();
				exit(1);
			}
			
			if (std::holds_alternative<tn_int_t>(variables[var->name].v)) {
				if (un->op == TokenType::INCREMENT) {
					return variables[var->name] = std::get<tn_int_t>(variables[var->name].v) + 1;
				} else {
					return variables[var->name] = std::get<tn_int_t>(variables[var->name].v) - 1;
				}
			}
		} else {
			TypeError(
				"Increment/decrement operator applied to non-variable",
				var->lineNo,
				var->colNo,
				var->filename,
				"",
				getLineText(source, var->lineNo)
			).print();
			exit(1);
		}
	} else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
		if (isRightAssoc(bin->op)) {
			if (auto* leftIndex = dynamic_cast<BinaryOp*>(bin->left.get())) {
				if (leftIndex->op == TokenType::INDEX && bin->op == TokenType::ASSIGN) {
					if (auto* vecVar = dynamic_cast<Variable*>(leftIndex->left.get())) {
						if (variables.count(vecVar->name) != 1) SyntaxError("Undefined variable: " + vecVar->name, -1);
						Value& holder = variables[vecVar->name];

						if (std::holds_alternative<Value::VecT>(holder.v)) {
							auto vecPtr = std::get<Value::VecT>(holder.v);

							if (!vecPtr) {
								Error(
									"null vector",
									vecVar->lineNo,
									vecVar->colNo,
									vecVar->filename,
									"",
									getLineText(source, vecVar->lineNo)
								).print();
								exit(1);
							}

							Value idxVal = evalExpr(leftIndex->right.get());
							if (!std::holds_alternative<tn_int_t>(idxVal.v)) TypeError("index must be integer", -1);
							tn_int_t idx = std::get<tn_int_t>(idxVal.v);

							if (idx < 0 || (size_t)idx >= vecPtr->size()) {
								Error(
									"index " + std::to_string(idx) + " is out of bounds for vector of size " + std::to_string(vecPtr->size()) + ".",
									vecVar->lineNo,
									vecVar->colNo,
									vecVar->filename,
									"",
									getLineText(source, vecVar->lineNo)
								).print();
								exit(1);
							}

							Value rhs = evalExpr(bin->right.get());
							(*vecPtr)[(size_t)idx] = rhs;

							return rhs;
						} else if (std::holds_alternative<Value::DicT>(holder.v)) {
							auto dictPtr = std::get<Value::DicT>(holder.v);

							if (!dictPtr) {
								Error(
									"null dictionary",
									vecVar->lineNo,
									vecVar->colNo,
									vecVar->filename,
									"",
									getLineText(source, vecVar->lineNo)
								).print();
								exit(1);
							}

							Value idxVal = evalExpr(leftIndex->right.get());
							if (!std::holds_alternative<std::string>(idxVal.v)) TypeError("dictionary key must be a string", -1);
							std::string idx = std::get<std::string>(idxVal.v);

							Value rhs = evalExpr(bin->right.get());
							(*dictPtr)[idx] = rhs;
						}
					} else {
						TypeError(
							"Left-hand side of indexed assignment must be a variable",
							leftIndex->left->lineNo,
							leftIndex->left->colNo,
							leftIndex->left->filename,
							"",
							getLineText(source, leftIndex->left->lineNo)
						).print();
						exit(1);
					}
				}
			} else if (auto* varNode = dynamic_cast<Variable*>(bin->left.get())) {
				Value right = evalExpr(bin->right.get());

				if (bin->op == TokenType::ASSIGN) {
					if (!callStack.empty()) {
						auto& frame = callStack.back();
						frame.locals[varNode->name] = right;

						return right;
					} else {
						return variables[varNode->name] = right;
					}
				} else {
					Value& target = (!callStack.empty() && callStack.back().locals.count(varNode->name))
					? callStack.back().locals[varNode->name]
					: variables[varNode->name];

					target = evalBinaryOp(target, right, (TokenType)((uint16_t)bin->op - ((uint16_t)TokenType::MOD - (uint16_t)TokenType::MOD_ASSIGN)));

					return target;
				}
			}
		} else if (bin->op == TokenType::DOT) {
			Value lhs = evalExpr(bin->left.get());

			if (auto fc = dynamic_cast<FunctionCall*>(bin->right.get())) {
				std::string name = fc->name;

				if (auto inst = std::get_if<Value::ClassInstance>(&lhs.v)) {
					auto it = inst->methods.find(name);

					if (it != inst->methods.end()) {
						FunctionStmt* method = it->second;

						CallFrame frame;

						for (auto&[fieldName, fieldVal] : inst->fields) {
							frame.locals[fieldName] = fieldVal;
						}

						for (size_t i = 0; i < method->params.size(); i++) {
							Variable* formalParam = dynamic_cast<Variable*>(method->params[i].get());
							frame.locals[formalParam->name] = evalExpr(fc->params[i].get());
						}

						callStack.push_back(std::move(frame));
						Value result;

						for (ExpressionStmt& stmt : method->stmts) {
							result = evalStmt(stmt);

							if (result.isReturn) {
								result.isReturn = false;

								for (auto&[k, v] : callStack.back().locals) {
									inst->fields[k] = v;
								}

								callStack.pop_back();

								return result;
							}

							if (result.isExit)
								return result;
						}

						for (auto&[k, v] : callStack.back().locals) {
							inst->fields[k] = v;
						}

						callStack.pop_back();

						return result;
					} else {
						TypeError(
							"Unknown method '" + name + "' for class '" + inst->name + "'",
							fc->lineNo,
							fc->colNo,
							fc->filename,
							"",
							getLineText(source, fc->lineNo)
						).print();
						exit(1);
					}
				} else if (auto strPtr = std::get_if<std::string>(&lhs.v)) {
					if (nativeMethods["str"].count(name)) {
						std::vector<Value> args;
						for (auto& param : fc->params) args.push_back(evalExpr(param.get()));

						return nativeMethods["str"][name](*strPtr, args);
					} else {
						TypeError(
							"Unknown string method: " + name,
							fc->lineNo,
							fc->colNo,
							fc->filename,
							"",
							getLineText(source, fc->lineNo)
						).print();
						exit(1);
					}
				} else if (auto vecPtr = std::get_if<Value::VecT>(&lhs.v)) {
					if (nativeMethods["vec"].count(name)) {
						std::vector<Value> args;
						for (auto& param : fc->params) args.push_back(evalExpr(param.get()));

						return nativeMethods["vec"][name](*vecPtr, args);
					} else {
						TypeError(
							"Unknown vector method: " + name,
							fc->lineNo,
							fc->colNo,
							fc->filename,
							"",
							getLineText(source, fc->lineNo)
						).print();
						exit(1);
					}
				} else if (std::get_if<NullLiteral>(&lhs.v)) {
					if (lhs.typeInt) {
						if (nativeMethods["type_int"].count(name)) {
							std::vector<Value> args;
							for (auto& param : fc->params) args.push_back(evalExpr(param.get()));

							return nativeMethods["type_int"][name](Value(), args);
						}
					} else if (lhs.typeVec) {
						if (nativeMethods["type_vec"].count(name)) {
							std::vector<Value> args;
							for (auto& param : fc->params) args.push_back(evalExpr(param.get()));

							return nativeMethods["type_vec"][name](Value(), args);
						}
					}
				} else {
					TypeError(
						"Method call not supported on this type",
						fc->lineNo,
						fc->colNo,
						fc->filename,
						"",
						getLineText(source, fc->lineNo)
					).print();
					exit(1);
				}
			} else if (auto var = dynamic_cast<Variable*>(bin->right.get())) {
				std::string propName = var->name;

				if (auto inst = std::get_if<Value::ClassInstance>(&lhs.v)) {
					auto fieldIt = inst->fields.find(propName);

					if (fieldIt != inst->fields.end()) {
						return fieldIt->second;
					}

					TypeError("Unknown property '" + propName + "' for class '" + inst->name + "'", -1);
				} else if (auto strPtr = std::get_if<std::string>(&lhs.v)) {
					if (propName == "length") return tn_int_t(strPtr->length());

					TypeError("Unknown string poperty: " + propName, -1);
				} else {
					TypeError("Property access not supported on this type", -1);
				}
			}
		}

		Value left = evalExpr(bin->left.get());
		Value right = evalExpr(bin->right.get());
		
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

		if constexpr (std::is_same_v<L, NullLiteral> || std::is_same_v<R, NullLiteral>) {
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
				tn_int_t idx = static_cast<tn_int_t>(r);
				if (idx < 0 || (size_t)idx >= l.size()) Error("string index out of bounds", -1);

				return Value(std::string(1, l[(size_t)idx]));
			}
		} else if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
			using ResultType = std::conditional_t<
					std::is_integral_v<L> && std::is_integral_v<R>, tn_int_t, tn_dec_t
			>;

			ResultType a = static_cast<ResultType>(l);
			ResultType b = static_cast<ResultType>(r);

			if constexpr (std::is_integral_v<ResultType>) {
				using IntegralResultType = std::conditional_t<
						std::is_same_v<L, tn_bool_t>, tn_bool_t, tn_int_t
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
			// op is vector index it just doesn't say so
			assert(op == TokenType::INDEX);
			auto vecPtr = l;
			if (!vecPtr) Error("null vector", -1);
			tn_int_t idx = static_cast<tn_int_t>(r);

			if (idx < 0 || (size_t)idx >= vecPtr->size()) {
				Error("index " + std::to_string(idx) + " is out of bounds for vector of size " + std::to_string(vecPtr->size()), -1);
			}

			return (*vecPtr)[(size_t)idx];
		} else if constexpr (std::is_same_v<L, Value::DicT> && std::is_same_v<R, std::string>) {
			assert(op == TokenType::INDEX);
			Value::DicT dictPtr = l;
			if (!dictPtr) Error("null dictionary", -1);
			std::string idx = static_cast<std::string>(r);
			
			try {
				return dictPtr->at(idx);
			} catch (std::exception&) {
				Error("key '" + idx + "' was not found in dictionary", -1);
			}
		}

		return Value();
	};

	return Value(std::visit(visitor, left.v, right.v));
}

Value Evaluator::evalUnaryOp(const Value& operand, TokenType op) {
	if (op == TokenType::NOT) {
		return Value(!std::get<tn_bool_t>(operand.v));
	} else if (op == TokenType::BIT_NOT) {
		if (std::holds_alternative<tn_int_t>(operand.v))
			return Value(~std::get<tn_int_t>(operand.v));
		else if (std::holds_alternative<tn_bool_t>(operand.v))
			return Value(~std::get<tn_int_t>(operand.v));
		else
			TypeError("failed to apply operator BIT_NOT to non-integral operand", -1);
	} else if (op == TokenType::NEGATE) {
		if (std::holds_alternative<tn_int_t>(operand.v)) {
			return Value(-std::get<tn_int_t>(operand.v));
		} else if (std::holds_alternative<tn_dec_t>(operand.v)) {
			return Value(-std::get<tn_dec_t>(operand.v));
		} else {
			TypeError("Unary minus operator applied to non-numeric type", -1);
		}
	}

	return Value();
}
