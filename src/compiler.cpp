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
		uint8_t op = static_cast<uint8_t>(instr.op);
		out.write(reinterpret_cast<const char*>(&op), sizeof(op));

		switch(instr.op) {
			case Opcode::PUSH_INT: {
				nl_int_t val = std::get<nl_int_t>(instr.operand);
				out.write(reinterpret_cast<const char*>(&val), sizeof(val));
				break;
			} case Opcode::PUSH_FLOAT: {
				nl_dec_t val = std::get<nl_dec_t>(instr.operand);
				out.write(reinterpret_cast<const char*>(&val), sizeof(val));
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

			bytecode.push_back(Instruction(Opcode::PRINT));
		} else {
			throw std::runtime_error("Only 'print' function is supported");
		}
	}
}

void Compiler::compileExpr(ASTNode* node, std::vector<Instruction>& bytecode) {
	if (!node) return;

	if (auto il = dynamic_cast<IntLiteral*>(node)) {
		bytecode.push_back(Instruction(Opcode::PUSH_INT, il->value));
	} else if (auto fl = dynamic_cast<FloatLiteral*>(node)) {
		bytecode.push_back(Instruction(Opcode::PUSH_FLOAT, fl->value));
	} else if (auto bin = dynamic_cast<BinaryOp*>(node)) {
		compileExpr(bin->left.get(), bytecode);
		compileExpr(bin->right.get(), bytecode);

		if (bin->op == "ADD") bytecode.push_back(Instruction(Opcode::ADD));
        else if (bin->op == "SUB") bytecode.push_back(Instruction(Opcode::SUB));
        else if (bin->op == "MUL") bytecode.push_back(Instruction(Opcode::MUL));
        else if (bin->op == "DIV") bytecode.push_back(Instruction(Opcode::DIV));
	} else {
		throw std::runtime_error("Unsupported AST node type in compiler");
	}
}