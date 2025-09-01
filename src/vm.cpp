#include <cmath>
#include "vm.hpp"

EvalExpr VM::applyBinaryOp(const EvalExpr& a, const EvalExpr& b, Opcode op) {
	auto visitor = [&op](auto&& l, auto&& r) -> EvalExpr {
		using L = std::decay_t<decltype(l)>;
		using R = std::decay_t<decltype(r)>;

		if constexpr (std::is_same_v<L, std::string> && std::is_same_v<R, std::string>) {
			if (op == Opcode::ADD) return EvalExpr(l + r);
			if (op == Opcode::EQEQ) return EvalExpr(l == r);
			if (op == Opcode::NOTEQ) return EvalExpr(l != r);

			throw std::runtime_error("invalid operator for string type");
		} else if constexpr (std::is_same_v<L, std::string> && std::is_integral_v<R>) {
			if (op == Opcode::INDEX) return std::string(1, l[static_cast<nl_int_t>(r)]);
		} else if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
			using ResultType = std::conditional_t<
					std::is_integral_v<L> && std::is_integral_v<R>, nl_int_t, nl_dec_t
			>;

			ResultType a = static_cast<ResultType>(l);
			ResultType b = static_cast<ResultType>(r);

			if (op == Opcode::ADD) return a + b;
			if (op == Opcode::SUB) return a - b;
			if (op == Opcode::MUL) return a * b;
			if (op == Opcode::DIV) {
				if (b == 0) throw std::runtime_error("Division by zero");

				return a / b;
			}

			if constexpr (std::is_integral_v<ResultType>) {
				using IntegralResultType = std::conditional_t<
						std::is_same_v<L, nl_bool_t>, nl_bool_t, nl_int_t
				>;
                if (op == Opcode::POW) return static_cast<IntegralResultType>(ipow(a, b));
				if (op == Opcode::MOD) return static_cast<IntegralResultType>(a % b);
				if (op == Opcode::BIT_AND) return static_cast<IntegralResultType>(a & b);
				if (op == Opcode::BIT_XOR) return static_cast<IntegralResultType>(a ^ b);
				if (op == Opcode::BIT_OR) return static_cast<IntegralResultType>(a | b);
				if (op == Opcode::LSHIFT) return static_cast<IntegralResultType>(a << b);
				if (op == Opcode::RSHIFT) return static_cast<IntegralResultType>(a >> b);
                if (op == Opcode::FLOOR_DIV) return static_cast<IntegralResultType>(a / b);
			}
			if (op == Opcode::BIT_AND ||
				op == Opcode::BIT_XOR ||
				op == Opcode::BIT_OR ||
				op == Opcode::LSHIFT ||
				op == Opcode::RSHIFT ||
                op == Opcode::FLOOR_DIV) TypeError("failed to apply operator to non-integral operand(s)", -1);
			if (op == Opcode::MOD) return std::fmodf(a,b);
			if (op == Opcode::POW) return std::powf(a, b);

			if (op == Opcode::EQEQ) return a == b;
			if (op == Opcode::NOTEQ) return a != b;
			if (op == Opcode::LESS) return a < b;
			if (op == Opcode::LESSEQ) return a <= b;
			if (op == Opcode::GREATER) return a > b;
			if (op == Opcode::GREATEREQ) return a >= b;

			if (op == Opcode::AND) return a && b;
			if (op == Opcode::OR) return a || b;
		} else if constexpr (std::is_same_v<L, std::vector<NonVecEvalExpr>> && std::is_integral_v<R>) {
			const std::vector<NonVecEvalExpr> a = static_cast<std::vector<NonVecEvalExpr>>(l);
			nl_int_t b = static_cast<nl_int_t>(r);
			
			if (op == Opcode::INDEX) {
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

	return std::visit(visitor, a, b);
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
		} else if (bytecode[i].op == Opcode::PUSH_STRING) {
			uint64_t len;
			fileHandle.read(reinterpret_cast<char*>(&len), sizeof(len));
			std::string s;
			s.resize(len);
			if (len) fileHandle.read(&s[0], static_cast<std::streamsize>(len));
			bytecode[i].operand = s;
		} else if (bytecode[i].op == Opcode::PUSH_BOOL) {
			uint8_t b;
			fileHandle.read(reinterpret_cast<char*>(&b), sizeof(b));
			bytecode[i].operand = nl_bool_t(b != 0);
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
			} case Opcode::PUSH_FLOAT: {
				nl_dec_t val = std::get<nl_dec_t>(instr.operand);
				stack.push_back(val);
				break;
			} case Opcode::PUSH_STRING: {
				std::string val = std::get<std::string>(instr.operand);
				stack.push_back(val);
				break;
			} case Opcode::PUSH_BOOL: {
				bool val = std::get<bool>(instr.operand);
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
			}
			case Opcode::PRINTLN:
			case Opcode::PRINT: {
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

				if (instr.op == Opcode::PRINTLN)
					std::cout << std::endl;

				std::cout.flush();
				break;
			} default:
				std::cerr << "Unknown opcode\n";
				break;
		}
	}
}
