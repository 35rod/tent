#include <fstream>
#include "compiler.hpp"
#include "ast.hpp"

std::vector<Instruction> Compiler::compileProgram(ASTPtr program) {
	std::vector<Instruction> bytecode;

	if (!program) throw std::runtime_error("Null program AST");

	Program* p = dynamic_cast<Program*>(program.get());
	if (!p) throw std::runtime_error("AST is not a Program node");

	for (ExpressionStmt& stmt : p->statements) {
		compileStmt(stmt.expr.get(), bytecode);
	}

	return bytecode;
}

void Compiler::saveToFile(std::vector<Instruction>& bytecode, const std::string& filename) {
	std::ofstream out(filename, std::ios::binary);
	if (!out) throw std::runtime_error("Failed to open file");

	uint64_t count = static_cast<uint64_t>(bytecode.size());
	out.write(reinterpret_cast<const char*>(&count), sizeof(count));

	for (const Instruction& instr : bytecode) {
		TokenTypeSize op = static_cast<TokenTypeSize>(instr.op);
		out.write(reinterpret_cast<const char*>(&op), sizeof(op));

		switch(instr.op) {
			case TokenType::PUSH_INT: {
				nl_int_t val = std::get<nl_int_t>(instr.operand);
				out.write(reinterpret_cast<const char*>(&val), sizeof(val));
				break;
			} case TokenType::PUSH_FLOAT: {
				nl_dec_t val = std::get<nl_dec_t>(instr.operand);
				out.write(reinterpret_cast<const char*>(&val), sizeof(val));
				break;
			} case TokenType::PUSH_STRING:
			case TokenType::VAR:
			case TokenType::ASSIGN: {
				std::string val = std::get<std::string>(instr.operand);
				uint64_t len = static_cast<uint64_t>(val.size());
				out.write(reinterpret_cast<const char*>(&len), sizeof(len));
				if (len) out.write(val.data(), static_cast<std::streamsize>(len));
				break;
			} case TokenType::PUSH_BOOL: {
				uint8_t b = std::get<bool>(instr.operand) ? 1 : 0;
				out.write(reinterpret_cast<const char*>(&b), sizeof(b));
				break;
			} default:
				break;
		}
	}

	out.close();
}

void Compiler::compileStmt(ASTNode* node, std::vector<Instruction>& bytecode) {
	if (!node) return;

	if (auto fc = dynamic_cast<FunctionCall*>(node)) {
		if (fc->name == "print") {
			for (auto& param : fc->params) {
				compileExpr(param.get(), bytecode);
			}

			bytecode.push_back(Instruction(TokenType::PRINT));
		} else if (fc->name == "println") {
			for (auto& param : fc->params) {
				compileExpr(param.get(), bytecode);
			}

			bytecode.push_back(Instruction(TokenType::PRINTLN));
		} else {
			throw std::runtime_error("Only 'print' or 'println' function is supported");
		}
	} else {
		compileExpr(node, bytecode);
	}
}

void Compiler::compileExpr(ASTNode* node, std::vector<Instruction>& bytecode) {
	if (!node) return;

	if (auto il = dynamic_cast<IntLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_INT, il->value));
	} else if (auto fl = dynamic_cast<FloatLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_FLOAT, fl->value));
	} else if (auto sl = dynamic_cast<StrLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_STRING, sl->value));
	} else if (auto bl = dynamic_cast<BoolLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_BOOL, bl->value));
	} else if (auto v = dynamic_cast<Variable*>(node)) {
		if (v->value) {
			compileExpr(v->value.get(), bytecode);
			bytecode.push_back(Instruction(TokenType::ASSIGN, v->name));
		} else {
			bytecode.push_back(Instruction(TokenType::VAR, v->name));
		}
	} else if (auto un = dynamic_cast<UnaryOp*>(node)) {
		compileExpr(un->operand.get(), bytecode);
		bytecode.push_back(Instruction(un->op));
	} else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
		if (isRightAssoc(bin->op)) {
			if (auto* varNode = dynamic_cast<Variable*>(bin->left.get())) {
				compileExpr(bin->right.get(), bytecode);
				bytecode.push_back(Instruction(bin->op, varNode->name));
				return;
			}
		}

		compileExpr(bin->left.get(), bytecode);
		compileExpr(bin->right.get(), bytecode);
		bytecode.push_back(Instruction(bin->op));
	} else {
		throw std::runtime_error("Unsupported AST node type in compiler");
	}
}
