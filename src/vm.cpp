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
			case TokenType::ASSIGN: 
			case TokenType::INCREMENT:
			case TokenType::DECREMENT: {
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
			} case TokenType::JUMP_IF_FALSE:
			case TokenType::JUMP: {
				nl_int_t addr;
				fileHandle.read(reinterpret_cast<char*>(&addr), sizeof(addr));
				bytecode[i].operand = addr;
				break;
			} default:
				break;
		}
	}

	return bytecode;
}

void VM::run(const std::vector<Instruction>& bytecode) {
	std::unordered_map<std::string, Value> variables;
	size_t ip = 0;

	while (ip < bytecode.size()) {
		const Instruction& instr = bytecode[ip];

		switch(instr.op) {
			case TokenType::PUSH_INT: {
				nl_int_t val = std::get<nl_int_t>(instr.operand.v);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_FLOAT: {
				nl_dec_t val = std::get<nl_dec_t>(instr.operand.v);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_STRING: {
				std::string val = std::get<std::string>(instr.operand.v);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_BOOL: {
				bool val = std::get<bool>(instr.operand.v);
				stack.push_back(val);
				break;
			} case TokenType::VAR: {
				std::string name = std::get<std::string>(instr.operand.v);
				
				if (variables.find(name) == variables.end()) {
					throw std::runtime_error("Undefined variable: " + name);
				}

				stack.push_back(variables[name]);
				break;
			} case TokenType::ASSIGN: {
				if (stack.empty()) throw std::runtime_error("Stack underflow on ASSIGN");
				
				Value val = stack.back(); stack.pop_back();
				std::string name = std::get<std::string>(instr.operand.v);
				variables[name] = val;
				stack.push_back(val);
				break;
			} case TokenType::INCREMENT:
			case TokenType::DECREMENT: {
				std::string name = std::get<std::string>(instr.operand.v);

				if (variables.find(name) == variables.end()) {
					throw std::runtime_error("Undefined variable: " + name);
				}

				if (std::holds_alternative<nl_int_t>(variables[name].v)) {
					variables[name] = std::get<nl_int_t>(variables[name].v) + (instr.op == TokenType::INCREMENT ? 1 : -1);
				} else if (std::holds_alternative<nl_dec_t>(variables[name].v)) {
					variables[name] = std::get<nl_dec_t>(variables[name].v) + (instr.op == TokenType::INCREMENT ? 1 : -1);
				}

				stack.push_back(variables[name]);
				break;
			} case TokenType::JUMP_IF_FALSE: {
				if (stack.empty()) throw std::runtime_error("Stack underflow on JUMP_IF_FALSE");

				Value cond = stack.back(); stack.pop_back();

				bool condTrue = std::visit([&cond]() -> nl_bool_t {
					using T = std::decay_t<decltype(cond.v)>;
					if constexpr (std::is_same_v<T, nl_bool_t>) return std::get<bool>(cond.v);
					if constexpr (std::is_same_v<T, nl_int_t>) return std::get<nl_bool_t>(cond.v) != 0;
					if constexpr (std::is_same_v<T, nl_dec_t>) return std::get<nl_bool_t>(cond.v) != 0.0;
					return true;
				});

				if (!condTrue) ip = static_cast<size_t>(std::get<nl_int_t>(instr.operand.v)) - 1;
				break;
			} case TokenType::JUMP: {
				ip = static_cast<size_t>(std::get<nl_int_t>(instr.operand.v)) - 1;
				break;
			} case TokenType::PRINTLN:
			case TokenType::PRINT: {
				if (stack.empty()) {
					std::cerr << "Error: Stack is empty on PRINT\n";
					break;
				}

				Value val = stack.back();
				stack.pop_back();
				//std::cout << "popped element for PRINT/PRINTLN\n";

				// std::visit([&val]() {
				// 	using T = std::decay_t<decltype(val.v)>;
				// 	if constexpr (std::is_same_v<T, nl_int_t>)
				// 		std::cout << std::get<nl_int_t>(val.v);
				// 	else if constexpr (std::is_same_v<T, nl_dec_t>)
				// 		std::cout << FloatLiteral::to_str(std::get<nl_dec_t>(val.v));
				// 	else if constexpr (std::is_same_v<T, nl_bool_t>)
				// 		std::cout << BoolLiteral::to_str(std::get<nl_bool_t>(val.v));
				// 	else if constexpr (std::is_same_v<T, std::string>)
				// 		std::cout << std::get<std::string>(val.v);
				// 	else
				// 		std::cout << "(null)";
				// });

				if (instr.op == TokenType::PRINTLN)
					std::cout << std::endl;

				std::cout.flush();
				break;
			} default:
				if (instr.op >= TokenType::ADD && instr.op <= TokenType::NOTEQ) {
					if (stack.size() < 2) throw std::runtime_error("Stack underflow for binary op");
					Value b = stack.back(); stack.pop_back();
					Value a = stack.back(); stack.pop_back();
					stack.push_back(Evaluator::evalBinaryOp(a, b, instr.op));
				} else if (instr.op >= TokenType::NOT && instr.op <= TokenType::NEGATE) {
					if (stack.empty()) throw std::runtime_error("Stack underflow for unary op");
					Value a = stack.back(); stack.pop_back();
					stack.push_back(Evaluator::evalUnaryOp(a, instr.op));
				} else {
					std::cerr << "Unknown opcode\n";
				}
				
				break;
		}

		ip++;
	}
}
