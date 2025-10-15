#include <fstream>
#include "compiler.hpp"

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

	uint64_t lib_count = static_cast<uint64_t>(nativeLibs.size());
	out.write(reinterpret_cast<const char*>(&lib_count), sizeof(lib_count));

	for (const std::string& nativeLib : nativeLibs) {
		uint64_t len = static_cast<uint64_t>(nativeLib.size());
		out.write(reinterpret_cast<const char*>(&len), sizeof(len));
		if (len) out.write(nativeLib.data(), static_cast<std::streamsize>(len));
	}

	uint64_t count = static_cast<uint64_t>(bytecode.size());
	out.write(reinterpret_cast<const char*>(&count), sizeof(count));

	for (const Instruction& instr : bytecode) {
		TokenTypeSize op = static_cast<TokenTypeSize>(instr.op);
		out.write(reinterpret_cast<const char*>(&op), sizeof(op));

		switch(instr.op) {
			case TokenType::PUSH_INT: {
				tn_int_t val = std::get<tn_int_t>(instr.operand.v);
				out.write(reinterpret_cast<const char*>(&val), sizeof(val));
				break;
			} case TokenType::PUSH_FLOAT: {
				tn_dec_t val = std::get<tn_dec_t>(instr.operand.v);
				out.write(reinterpret_cast<const char*>(&val), sizeof(val));
				break;
			} case TokenType::PUSH_STRING:
			case TokenType::FORM:
			case TokenType::CALL:
			case TokenType::VAR:
			case TokenType::ASSIGN:
			case TokenType::INCREMENT:
			case TokenType::DECREMENT: {
				std::string val = std::get<std::string>(instr.operand.v);
				uint64_t len = static_cast<uint64_t>(val.size());
				out.write(reinterpret_cast<const char*>(&len), sizeof(len));
				if (len) out.write(val.data(), static_cast<std::streamsize>(len));
				break;
			} case TokenType::PUSH_BOOL: {
				uint8_t b = std::get<bool>(instr.operand.v) ? 1 : 0;
				out.write(reinterpret_cast<const char*>(&b), sizeof(b));
				break;
			} case TokenType::JUMP_IF_FALSE:
			case TokenType::JUMP:
			case TokenType::CALL_INLINE: {
				tn_int_t addr = std::get<tn_int_t>(instr.operand.v);
				out.write(reinterpret_cast<const char*>(&addr), sizeof(addr));
				break;
			} default:
				break;
		}
	}

	out.close();
}

void Compiler::compileStmt(ASTNode* node, std::vector<Instruction>& bytecode, bool isInline) {
	if (!node) return;

	if (dynamic_cast<NoOp*>(node)) return;

	compileExpr(node, bytecode, isInline);
}

void Compiler::compileExpr(ASTNode* node, std::vector<Instruction>& bytecode, bool isInline) {
	if (!node) return;

	if (auto il = dynamic_cast<IntLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_INT, il->value));
	} else if (auto fl = dynamic_cast<FloatLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_FLOAT, fl->value));
	} else if (auto sl = dynamic_cast<StrLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_STRING, sl->value));
	} else if (auto bl = dynamic_cast<BoolLiteral*>(node)) {
		bytecode.push_back(Instruction(TokenType::PUSH_BOOL, bl->value));
	} else if (dynamic_cast<TypeInt*>(node)) {
		bytecode.push_back(Instruction(TokenType::TYPE_INT));
	} else if (dynamic_cast<TypeFloat*>(node)) {
		bytecode.push_back(Instruction(TokenType::TYPE_FLOAT));
	} else if (dynamic_cast<TypeStr*>(node)) {
		bytecode.push_back(Instruction(TokenType::TYPE_STR));
	} else if (dynamic_cast<TypeBool*>(node)) {
		bytecode.push_back(Instruction(TokenType::TYPE_BOOL));
	} else if (dynamic_cast<TypeVec*>(node)) {
		bytecode.push_back(Instruction(TokenType::TYPE_VEC));
	} else if (auto ifl = dynamic_cast<IfStmt*>(node)) {
		compileExpr(ifl->condition.get(), bytecode);
		
		bytecode.push_back(Instruction(TokenType::JUMP_IF_FALSE, tn_int_t(-1)));
		size_t jumpIfFalseIndex = bytecode.size() - 1;

		for (auto& stmt : ifl->thenClauseStmts) {
			compileStmt(stmt.expr.get(), bytecode);
		}

		if (!ifl->elseClauseStmts.empty()) {
			bytecode.push_back(Instruction(TokenType::JUMP));
			size_t jumpOverElseIndex = bytecode.size() - 1;

			bytecode[jumpIfFalseIndex].operand = static_cast<tn_int_t>(bytecode.size());

			for (auto& stmt : ifl->elseClauseStmts) {
				compileStmt(stmt.expr.get(), bytecode);
			}

			bytecode[jumpOverElseIndex].operand = static_cast<tn_int_t>(bytecode.size());
		} else {
			bytecode[jumpIfFalseIndex].operand = static_cast<tn_int_t>(bytecode.size());
		}
	} else if (auto wl = dynamic_cast<WhileStmt*>(node)) {
		size_t condStart = bytecode.size();
		compileExpr(wl->condition.get(), bytecode);

		bytecode.push_back(Instruction(TokenType::JUMP_IF_FALSE, tn_int_t(-1)));
		size_t jumpIfFalseIndex = bytecode.size() - 1;

		for (auto& stmt : wl->stmts) {
			if (stmt.isBreak) {
				bytecode.push_back(Instruction(TokenType::JUMP, tn_int_t(-1)));
			} else if (stmt.isContinue) {
				bytecode.push_back(Instruction(TokenType::JUMP, static_cast<tn_int_t>(condStart)));
			} else {
				compileStmt(stmt.expr.get(), bytecode);
			}
		}

		bytecode.push_back(Instruction(TokenType::JUMP, static_cast<tn_int_t>(condStart)));
		
		size_t loopEnd = bytecode.size();
		bytecode[jumpIfFalseIndex].operand = static_cast<tn_int_t>(loopEnd);

		for (size_t i = jumpIfFalseIndex + 1; i < loopEnd; i++) {
			if (bytecode[i].op == TokenType::JUMP) {
				if (std::holds_alternative<tn_int_t>(bytecode[i].operand.v) && std::get<tn_int_t>(bytecode[i].operand.v) == -1) {
					bytecode[i].operand = static_cast<tn_int_t>(loopEnd);
				}
			}
		}
	} else if (auto fnl = dynamic_cast<FunctionStmt*>(node)) {
		bytecode.push_back(Instruction(TokenType::FORM, fnl->name));

		bytecode.push_back(Instruction(TokenType::PUSH_INT, static_cast<tn_int_t>(fnl->params.size())));

		for (auto& param : fnl->params) {
			auto v = dynamic_cast<Variable*>(param.get());
			if (!v) throw std::runtime_error("Function parameter is not a variable");
			bytecode.push_back(Instruction(TokenType::PUSH_STRING, v->name));
		}

		size_t lengthIndex = bytecode.size();
		bytecode.push_back(Instruction(TokenType::PUSH_INT, tn_int_t(-1)));

		size_t bodyStart = bytecode.size();

		for (auto& stmt : fnl->stmts) {
			compileStmt(stmt.expr.get(), bytecode);
		}

		bytecode[lengthIndex].operand = static_cast<tn_int_t>(bytecode.size() - bodyStart);
	} else if (auto inl = dynamic_cast<InlineStmt*>(node)) {
		bytecode.push_back(Instruction(TokenType::INLINE, inl->name));
		tn_int_t startIndex = bytecode.size();

		bytecode.push_back(Instruction(TokenType::PUSH_INT, static_cast<tn_int_t>(inl->params.size())));

		for (auto& param : inl->params) {
			auto v = dynamic_cast<Variable*>(param.get());
			if (!v) throw std::runtime_error("Inline parameter is not a variable");
			bytecode.push_back(Instruction(TokenType::PUSH_STRING, v->name));
		}

		size_t lengthIndex = bytecode.size();
		bytecode.push_back(Instruction(TokenType::PUSH_INT, tn_int_t(-1)));

		size_t bodyStart = bytecode.size();

		for (auto& stmt : inl->stmts) {
			compileStmt(stmt.expr.get(), bytecode, true);
		}

		bytecode[lengthIndex].operand = static_cast<tn_int_t>(bytecode.size() - bodyStart);

		inlines[inl->name] = startIndex;
	} else if (auto rl = dynamic_cast<ReturnStmt*>(node)) {
		compileExpr(rl->value.get(), bytecode);

		if (isInline) {
			bytecode.push_back(Instruction(TokenType::RETURN_INLINE));
		} else {
			bytecode.push_back(Instruction(TokenType::RETURN));
		}
	} else if (auto fc = dynamic_cast<FunctionCall*>(node)) {
		for (auto& param : fc->params) {
			compileExpr(param.get(), bytecode);
		}

		if (inlines.find(fc->name) != inlines.end()) {
			bytecode.push_back(Instruction(TokenType::CALL_INLINE, inlines[fc->name]));
		} else {
			bytecode.push_back(Instruction(TokenType::CALL, fc->name));
		}
	} else if (auto v = dynamic_cast<Variable*>(node)) {
		if (v->value) {
			compileExpr(v->value.get(), bytecode);
			bytecode.push_back(Instruction(TokenType::ASSIGN, v->name));
		} else {
			bytecode.push_back(Instruction(TokenType::VAR, v->name));
		}
	} else if (auto un = dynamic_cast<UnaryOp*>(node)) {
		if (un->op == TokenType::INCREMENT || un->op == TokenType::DECREMENT) {
			if (auto var = dynamic_cast<Variable*>(un->operand.get())) {
				bytecode.push_back(Instruction(un->op, var->name));
			} else {
				throw std::runtime_error("Increment/decrement operator only valid for integer or float types: " + var->name);
			}
		} else {
			compileExpr(un->operand.get(), bytecode);
			bytecode.push_back(Instruction(un->op));
		}
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
