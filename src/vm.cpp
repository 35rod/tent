#include "vm.hpp"

#include <stdexcept>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

std::vector<Instruction> VM::loadFile(const std::string& filename, const std::vector<std::string>& search_dirs) {
	std::ifstream fileHandle(filename, std::ios::binary);

	uint64_t lib_count = 0;
	fileHandle.read(reinterpret_cast<char*>(&lib_count), sizeof(lib_count));

	using RegisterFn = void(*)(std::unordered_map<std::string, NativeFn>&);

	for (uint64_t i = 0; i < lib_count; i++) {
		uint64_t len = 0;
		fileHandle.read(reinterpret_cast<char*>(&len), sizeof(len));
		std::string libName(len, '\0');
		if (len) fileHandle.read(&libName[0], static_cast<std::streamsize>(len));

		#if defined(_WIN32) || defined(_WIN64)
		HMODULE handle = LoadLibraryA(("lib" + libName).c_str());
		if (!handle) handle = LoadLibraryA(("lib" + libName + ".dll").c_str());

		if (!handle) {
			for (const std::string& dir : search_dirs) {
				if (handle) break;
				handle = LoadLibraryA((dir + "/lib" + libName).c_str());
			}
		}

		if (!handle) {
			std::cerr << "Failed to load library: " << libName << std::endl;
			exit(1);
		}

		RegisterFn reg = reinterpret_cast<RegisterFn>(GetProcAddress(handle, "registerFunctions"));

		if (!reg) {
			std::cerr << "Library missing registerFunctions symbol" << std::endl;
			FreeLibrary(handle);
			exit(1);
		}

		reg(vmNativeFunctions);
#else
		void* handle = NULL;
		for (const std::string& dir : search_dirs) {
			if (handle) break;
			handle = dlopen((dir + "/lib" + libName).c_str(), RTLD_LAZY);
			if (!handle) handle = dlopen((dir + "/lib" + libName + ".dylib").c_str(), RTLD_LAZY);
			if (!handle) handle = dlopen((dir + "/lib" + libName + ".so").c_str(), RTLD_LAZY);
		}
		
		if (!handle) {
			char *err_str = dlerror();
			std::cerr << "Failed to load library: " << libName <<
					(err_str ? (": \n" + std::string(err_str)) : "") << std::endl;
			exit(1);
		}

		RegisterFn reg = reinterpret_cast<RegisterFn>(dlsym(handle, "registerFunctions"));

		if (!reg) {
			std::cerr << "Library missing registerFunctions symbol" << std::endl;
			dlclose(handle);
			exit(1);
		}

		reg(vmNativeFunctions);
#endif
	}

	uint64_t count = 0;
	fileHandle.read(reinterpret_cast<char*>(&count), sizeof(count));
	
	std::vector<Instruction> bytecode(count);

	for (size_t i = 0; i < count; i++) {
		TokenTypeSize op;
		fileHandle.read(reinterpret_cast<char*>(&op), sizeof(op));
		bytecode[i].op = static_cast<TokenType>(op);

		switch(bytecode[i].op) {
			case TokenType::PUSH_INT: {
				tn_int_t val;
				fileHandle.read(reinterpret_cast<char*>(&val), sizeof(val));
				bytecode[i].operand = val;
				break;
			} case TokenType::PUSH_FLOAT: {
				tn_dec_t val;
				fileHandle.read(reinterpret_cast<char*>(&val), sizeof(val));
				bytecode[i].operand = val;
				break;
			} case TokenType::PUSH_STRING:
			case TokenType::FORM:
			case TokenType::CALL:
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
				bytecode[i].operand = tn_bool_t(b != 0);
				break;
			} case TokenType::JUMP_IF_FALSE:
			case TokenType::JUMP:
			case TokenType::CALL_INLINE: {
				tn_int_t addr;
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
	size_t ip = 0;

	while (ip < bytecode.size()) {
		const Instruction& instr = bytecode[ip];

		switch(instr.op) {
			case TokenType::PUSH_INT: {
				tn_int_t val = std::get<tn_int_t>(instr.operand.v);
				stack.push_back(val);
				break;
			} case TokenType::PUSH_FLOAT: {
				tn_dec_t val = std::get<tn_dec_t>(instr.operand.v);
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
			} case TokenType::FORM: {
				std::string funcName = std::get<std::string>(instr.operand.v);

				ip++;
				tn_int_t paramCount = std::get<tn_int_t>(bytecode[ip].operand.v);

				std::vector<std::string> params;

				for (tn_int_t j = 0; j < paramCount; j++) {
					ip++;
					params.push_back(std::get<std::string>(bytecode[ip].operand.v));
				}

				ip++;
				tn_int_t funcLength = std::get<tn_int_t>(bytecode[ip].operand.v);

				std::vector<Instruction> funcBytecode;
				size_t funcBodyGlobalStart = ip + 1;

				for (tn_int_t j = 0; j < funcLength; j++) {
					ip++;
					funcBytecode.push_back(bytecode[ip]);
				}

				for (Instruction& finstr : funcBytecode) {
					if (finstr.op == TokenType::JUMP_IF_FALSE || finstr.op == TokenType::JUMP) {
						tn_int_t globalAddr = std::get<tn_int_t>(finstr.operand.v);
						finstr.operand = globalAddr - static_cast<tn_int_t>(funcBodyGlobalStart);
					}
				}

				functions[funcName] = VMFunc{funcName, params, funcBytecode};

				break;
			} case TokenType::INLINE: {
				tn_int_t paramCount = std::get<tn_int_t>(instr.operand.v);
				ip += paramCount;
				tn_int_t bodyLen = std::get<tn_int_t>(bytecode[ip+1].operand.v);
				ip += bodyLen + 1;
				break;
			} case TokenType::RETURN: {
				Value retVal = stack.back();
				stack.pop_back();
				callStack.pop_back();
				stack.push_back(retVal);
				return;
			} case TokenType::RETURN_INLINE: {
				if (stack.empty()) throw std::runtime_error("Stack underflow on RETURN_INLINE");
				Value retVal = stack.back(); stack.pop_back();
				
				if (!callStack.empty()) callStack.pop_back();
				if (returnAddrs.empty()) throw std::runtime_error("Inline RETURN without return address");

				size_t ret = returnAddrs.back(); returnAddrs.pop_back();
				stack.push_back(retVal);
				ip = ret;

				continue;
			} case TokenType::CALL: {
				std::string funcName = std::get<std::string>(instr.operand.v);

				auto nit = vmNativeFunctions.find(funcName);
				
				if (nit != vmNativeFunctions.end()) {
					NativeFn fn = nit->second;
					std::vector<Value> args;

					while (!stack.empty()) {
						args.push_back(stack.back());
						stack.pop_back();
					}

					std::reverse(args.begin(), args.end());

					Value result = fn(args);
					stack.push_back(result);

					break;
				}

				auto fit = functions.find(funcName);
				if (fit == functions.end()) throw std::runtime_error("Undefined function: " + funcName);

				VMFunc& func = fit->second;
				size_t numArgs = func.params.size();

				if (stack.size() < numArgs) {
					throw std::runtime_error("Not enough arguments for function: " + funcName);
				}

				CallFrame frame;

				for (size_t i = 0; i < numArgs; i++) {
					Value arg = stack.back(); stack.pop_back();
					frame.locals[func.params[numArgs - i - 1]] = arg;
				}

				const std::vector<CallFrame>::size_type callstack_top_index = callStack.size();
				callStack.push_back(frame);

				size_t ret_ip = ip;
				run(func.bytecode);

				if (callStack.size() != callstack_top_index) {
					throw std::runtime_error("return statement is required prior to the termination of a function ('" + funcName + "').\n");
				}

				ip = ret_ip;

				break;
			} case TokenType::CALL_INLINE: {
				tn_int_t targetAddr = std::get<tn_int_t>(instr.operand.v);
				returnAddrs.push_back(ip+1);
				
				tn_int_t paramCount = std::get<tn_int_t>(bytecode[targetAddr+1].operand.v);
				std::vector<std::string> params;
				
				for (tn_int_t j = 0; j < paramCount; j++) {
					params.push_back(std::get<std::string>(bytecode[targetAddr+2+j].operand.v));
				}

				CallFrame frame;

				for (tn_int_t j = paramCount-1; j >= 0; j--) {
					if (stack.empty()) throw std::runtime_error("Not enough args for inline call");
					Value arg = stack.back(); stack.pop_back();
					frame.locals[params[j]] = arg;
				}

				callStack.push_back(frame);
				ip = targetAddr+2+paramCount+1;

				continue;
			} case TokenType::VAR: {
				std::string name = std::get<std::string>(instr.operand.v);
				bool found = false;

				for (auto it = callStack.rbegin(); it != callStack.rend(); it++) {
					auto fit = it->locals.find(name);

					if (fit != it->locals.end()) {
						stack.push_back(fit->second);
						found = true;
						break;
					}
				}

				if (!found) {
					auto gfit = variables.find(name);

					if (gfit != variables.end()) {
						stack.push_back(gfit->second);
					} else {
						throw std::runtime_error("Undefined variable: " + name);
					}
				}

				break;
			} case TokenType::ASSIGN: {
				if (stack.empty()) throw std::runtime_error("Stack underflow on ASSIGN");
				
				Value val = stack.back(); stack.pop_back();
				std::string name = std::get<std::string>(instr.operand.v);

				if (!callStack.empty()) {
					callStack.back().locals[name] = val;
				} else {
					variables[name] = val;
				}

				stack.push_back(val);

				break;
			} case TokenType::INCREMENT:
			case TokenType::DECREMENT: {
				std::string name = std::get<std::string>(instr.operand.v);
				Value* target = nullptr;

				if (!callStack.empty()) {
					auto& frame = callStack.back();
					auto it = frame.locals.find(name);
					if (it != frame.locals.end()) target = &it->second;
				}

				if (!target) {
					auto it = variables.find(name);
					if (it != variables.end()) target = &it->second;
				}

				if (!target) throw std::runtime_error("Undefined variable: " + name);

				if (std::holds_alternative<tn_int_t>(target->v)) {
					*target = std::get<tn_int_t>(target->v) + (instr.op == TokenType::INCREMENT ? 1 : -1);
				} else if (std::holds_alternative<tn_dec_t>(target->v)) {
					*target = std::get<tn_dec_t>(target->v) + (instr.op == TokenType::INCREMENT ? 1 : -1);
				} else {
					throw std::runtime_error("Increment/decrement applied to non-numeric value");
				}

				stack.push_back(*target);

				break;
			} case TokenType::JUMP_IF_FALSE: {
				if (stack.empty()) throw std::runtime_error("Stack underflow on JUMP_IF_FALSE");

				Value cond = stack.back(); stack.pop_back();

				bool condTrue = std::visit([&]() -> tn_bool_t {
					if (std::holds_alternative<tn_bool_t>(cond.v)) return std::get<tn_bool_t>(cond.v);
					if (std::holds_alternative<tn_int_t>(cond.v)) return std::get<tn_bool_t>(cond.v) != 0;
					if (std::holds_alternative<tn_dec_t>(cond.v)) return std::get<tn_bool_t>(cond.v) != 0.0;
					return true;
				});

				if (!condTrue) ip = static_cast<size_t>(std::get<tn_int_t>(instr.operand.v)) - 1;
				break;
			} case TokenType::JUMP: {
				ip = static_cast<size_t>(std::get<tn_int_t>(instr.operand.v)) - 1;
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
					std::cout << (long long)instr.op << std::endl;
					std::cerr << "Unknown opcode\n";
				}
				
				break;
		}

		ip++;
	}
}
