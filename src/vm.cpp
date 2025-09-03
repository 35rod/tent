#include <iostream>
#include <fstream>
#include "vm.hpp"

std::vector<Instruction> VM::loadFile(const std::string& filename) {
	std::ifstream fileHandle(filename, std::ios::binary);

	uint64_t count64 = 0;
	fileHandle.read(reinterpret_cast<char*>(&count64), sizeof(count64));
	
	std::vector<Instruction> bytecode(count64);

	for (size_t i = 0; i < count64; i++) {
		TokenTypeSize op;
		fileHandle.read(reinterpret_cast<char*>(&op), sizeof(op));
		bytecode[i].op = static_cast<TokenType>(op);

		switch(bytecode[i].op) {
			case TokenType::PUSH_INT: {
				nl_int_t val;
				fileHandle.read(reinterpret_cast<char*>(&val), sizeof(val));
				bytecode[i].operand = val;
				break;
			} case TokenType::PUSH_FLOAT: {
				nl_dec_t val;
				fileHandle.read(reinterpret_cast<char*>(&val), sizeof(val));
				bytecode[i].operand = val;
				break;
			} case TokenType::PUSH_STRING:
			case TokenType::VAR:
			case TokenType::ASSIGN: {
				uint64_t len;
				fileHandle.read(reinterpret_cast<char*>(&len), sizeof(len));
				std::string s(len, '\0');
				if (len) fileHandle.read(&s[0], static_cast<std::streamsize>(len));
				bytecode[i].operand = s;
				break;
			} case TokenType::PUSH_BOOL: {
				uint8_t b;
				fileHandle.read(reinterpret_cast<char*>(&b), sizeof(b));
				bytecode[i].operand = nl_bool_t(b != 0);
				break;
			} default:
				break;
		}
	}

	return bytecode;
}

enum WillSkipReason {
	WillNotSkip,
	WillSkipThen,
	WillSkipElse,
};

void VM::run(const std::vector<Instruction>& bytecode) {
	std::unordered_map<std::string, EvalExpr> variables;
	int nested = 0;
	bool skip = false;
	WillSkipReason will_skip = WillNotSkip;

	for (const auto& instr : bytecode) {
		if (will_skip != WillNotSkip) {
			if (instr.op == TokenType::IF) nested++;
			else if (instr.op == TokenType::END_IF) {
				if (nested == 0) {
					skip = false;
					will_skip = WillNotSkip;
				} else nested--;
			} else if (instr.op == TokenType::ELSE && nested == 0 && will_skip == WillSkipThen) {
				skip = false;
				will_skip = WillNotSkip;
			} else if (instr.op == TokenType::ELSE && nested == 0 && will_skip == WillSkipElse) {
				skip = true;
			} else if (will_skip == WillSkipThen) {
				skip = true;
			}
		}

		if (skip)
			continue;

		if (instr.op == TokenType::ASSIGN) {
			if (stack.empty()) throw std::runtime_error("Stack overflow on ASSIGN");
			EvalExpr val = stack.back(); stack.pop_back();

			std::string name = std::get<std::string>(instr.operand);
			variables[name] = val;
			stack.push_back(val);
			continue;
		}

		if (instr.op >= TokenType::ADD && instr.op <= TokenType::NOTEQ) {
			if (stack.size() < 2) throw std::runtime_error("Stack underflow for binary op");
			EvalExpr b = stack.back(); stack.pop_back();
			//std::cout << "popped element 1 for binary operator\n";
			EvalExpr a = stack.back(); stack.pop_back();
			//std::cout << "popped element 2 for binary operator\n";
			stack.push_back(Evaluator::evalBinaryOp(a, b, instr.op));
			//std::cout << "pushed element for binary operator\n";
			continue;
		} else if (instr.op >= TokenType::NOT && instr.op <= TokenType::NEGATE) {
			if (stack.size() < 1) throw std::runtime_error("Stack underflow for unary op");
			EvalExpr a = stack.back(); stack.pop_back();
			//std::cout << "popped element for unary operator\n";
			stack.push_back(Evaluator::evalUnaryOp(a, instr.op));
			//std::cout << "pushed element for unary operator\n";
			continue;
		}

		switch(instr.op) {
			case TokenType::PUSH_INT: {
				nl_int_t val = std::get<nl_int_t>(instr.operand);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_FLOAT: {
				nl_dec_t val = std::get<nl_dec_t>(instr.operand);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_STRING: {
				std::string val = std::get<std::string>(instr.operand);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_BOOL: {
				bool val = std::get<bool>(instr.operand);
				stack.push_back(val);
				break;
			} case TokenType::VAR: {
				std::string name = std::get<std::string>(instr.operand);
				
				if (variables.find(name) == variables.end()) {
					throw std::runtime_error("Undefined variable: " + name);
				}

				stack.push_back(variables[name]);
				break;
			} case TokenType::IF: {
				if (stack.empty()) throw std::runtime_error("Stack underflow on IF condition");
				
				EvalExpr cond = stack.back(); stack.pop_back();

				bool condTrue = std::visit([](auto&& v) -> bool {
					using T = std::decay_t<decltype(v)>;
					if constexpr (std::is_same_v<T, nl_bool_t>) return v;
					if constexpr (std::is_same_v<T, nl_int_t>) return v != 0;
					if constexpr (std::is_same_v<T, nl_dec_t>) return v != 0.0;
					return true;
				}, cond);

				if (!condTrue) will_skip = WillSkipThen;
				else           will_skip = WillSkipElse;
				break;
			} case TokenType::ELSE:
			case TokenType::END_IF: {
				break;
			} case TokenType::PRINTLN:
			case TokenType::PRINT: {
				if (stack.empty()) {
					std::cerr << "Error: Stack is empty on PRINT\n";
					break;
				}

				EvalExpr val = stack.back();
				stack.pop_back();
				//std::cout << "popped element for PRINT/PRINTLN\n";

				std::visit([](auto&& v){
					using T = std::decay_t<decltype(v)>;
					if constexpr (std::is_same_v<T, nl_int_t>)
						std::cout << v;
					else if constexpr (std::is_same_v<T, nl_dec_t>)
						std::cout << FloatLiteral::to_str(v);
					else if constexpr (std::is_same_v<T, nl_bool_t>)
						std::cout << BoolLiteral::to_str(v);
					else if constexpr (std::is_same_v<T, std::string>)
						std::cout << v;
					else if constexpr (std::is_same_v<T, std::vector<NonVecEvalExpr>>)
						std::cout << VecValue::to_str(v);
					else
						std::cout << "(null)";
				}, val);

				if (instr.op == TokenType::PRINTLN)
					std::cout << std::endl;

				std::cout.flush();
				break;
			} default:
				std::cerr << "Unknown opcode\n";
				break;
		}
	}
}
