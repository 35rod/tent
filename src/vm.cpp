#include "vm.hpp"

EvalExpr VM::applyBinaryOp(const EvalExpr& a, const EvalExpr& b, Opcode op) {
	return std::visit([op](auto&& lhs, auto&& rhs) -> EvalExpr {
		using L = std::decay_t<decltype(lhs)>;
		using R = std::decay_t<decltype(rhs)>;

		if constexpr (std::is_same_v<L, nl_int_t> && std::is_same_v<R, nl_int_t>) {
			switch(op) {
				case Opcode::ADD: return lhs + rhs;
				case Opcode::SUB: return lhs - rhs;
				case Opcode::MUL: return lhs * rhs;
				case Opcode::DIV: return lhs / rhs;
				default: throw std::runtime_error("Unknown opcode");
			}
		} else if constexpr (std::is_same_v<L, nl_dec_t> && std::is_same_v<R, nl_dec_t>) {
			switch(op) {
				case Opcode::ADD: return lhs + rhs;
				case Opcode::SUB: return lhs - rhs;
				case Opcode::MUL: return lhs * rhs;
				case Opcode::DIV: return lhs / rhs;
				default: throw std::runtime_error("Unknown opcode");
			}
		} else {
			throw std::runtime_error("Invalid operand types for binary op");
		}
	}, a, b);

}

std::vector<Instruction> VM::loadFile(const std::string& filename) {
	std::ifstream fileHandle(filename, std::ios::binary);

	uint64_t count64 = 0;
	fileHandle.read(reinterpret_cast<char*>(&count64), sizeof(count64));

	std::vector<Instruction> bytecode(count64);

	for (size_t i = 0; i < count64; i++) {
		uint8_t op;
		fileHandle.read(reinterpret_cast<char*>(&op), sizeof(op));
		bytecode[i].op = static_cast<Opcode>(op);

		if (bytecode[i].op == Opcode::PUSH_INT) {
			nl_int_t val;
			fileHandle.read(reinterpret_cast<char*>(&val), sizeof(val));
			bytecode[i].operand = val;
		} else if (bytecode[i].op == Opcode::PUSH_FLOAT) {
			nl_dec_t val;
			fileHandle.read(reinterpret_cast<char*>(&val), sizeof(val));
			bytecode[i].operand = val;
		}
	}

	return bytecode;
}

void VM::run(const std::vector<Instruction>& bytecode) {
	for (const auto& instr : bytecode) {
		switch(instr.op) {
			case Opcode::PUSH_INT: {
				nl_int_t val = std::get<nl_int_t>(instr.operand);
				stack.push_back(val);
				break;
			} case Opcode::PUSH_FLOAT : {
				nl_dec_t val = std::get<nl_dec_t>(instr.operand);
				stack.push_back(val);
				break;
			} case Opcode::ADD:
			case Opcode::SUB:
			case Opcode::MUL:
			case Opcode::DIV: {
				if (stack.size() < 2) throw std::runtime_error("Stack underflow for binary op");
				EvalExpr b = stack.back(); stack.pop_back();
				EvalExpr a = stack.back(); stack.pop_back();
				stack.push_back(applyBinaryOp(a, b, instr.op));
				break;
			} case Opcode::PRINT: {
				if (stack.empty()) {
					std::cerr << "Error: Stack is empty on PRINT\n";
					break;
				}

				EvalExpr val = stack.back();
				stack.pop_back();

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

				std::cout << std::endl; // <-- flush and newline
				break;
			} default:
				std::cerr << "Unknown opcode\n";
				break;
		}
	}
}