#include <iostream>
#include "ast.hpp"
#include "misc.hpp"

void printIndent(int indent) {
	printf("%*s", indent, " ");
}

template<typename T>
void writeBinary(std::ostream& out, const T& value) {
	out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

void writeText(std::ostream& out, std::string& s) {
	uint32_t length = s.length();
	writeBinary(out, length);
	out.write(s.data(), length);
}

void ASTNode::print(int indent) {
	printIndent(indent);
	std::cout << "ASTNode()" << std::endl;
}

void ASTNode::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::ASTNode);
}

void NoOp::print(int ident) {
	printIndent(ident);
	std::cout << "NoOp()\n";
}

void NoOp::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::NoOp);
}

IntLiteral::IntLiteral(nl_int_t literalValue) : ASTNode(), value(literalValue) {}

void IntLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IntLiteral(value=" << value << ")\n";
}

void IntLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::IntLiteral);
	writeBinary<nl_int_t>(out, value);
}

FloatLiteral::FloatLiteral(nl_dec_t literalValue) : ASTNode(), value(literalValue) {}

#define MAX_DEC_LEN 50
std::string FloatLiteral::to_str(nl_dec_t val, int prec) {
	static char str_buf[MAX_DEC_LEN + 1];
	std::snprintf(str_buf, MAX_DEC_LEN, "%.*f", prec, val); 

	return std::string(str_buf);
}

void FloatLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "FloatLiteral(value=" << to_str(value) << ")\n";
}

void FloatLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::FloatLiteral);
	writeBinary<nl_dec_t>(out, value);
}

StrLiteral::StrLiteral(std::string literalValue) : ASTNode(), value(literalValue) {}

std::string StrLiteral::to_str(std::string val) {
	return "\"" + val + "\"";
}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << to_str(value) << ")\n";
}

void StrLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::StringLiteral);
	writeText(out, value);
}

BoolLiteral::BoolLiteral(nl_bool_t literalValue) : ASTNode(), value(literalValue) {}

std::string BoolLiteral::to_str(nl_bool_t val) {
	return (val) ? "true" : "false";
}

void BoolLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "BoolLiteral(value=" << to_str(value) << ")\n";
}


void BoolLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::BoolLiteral);
	uint8_t bool_val = value ? 1 : 0;
	writeBinary<uint8_t>(out, bool_val);
}

VecLiteral::VecLiteral(std::vector<ASTPtr> literalValue) : ASTNode(), elems(std::move(literalValue)) {}

void VecLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "VecLiteral(size=" << elems.size() << ")\n";
}

void VecLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::VecLiteral);
	uint32_t size = elems.size();
	writeBinary<uint32_t>(out, size);

	for (const auto& elem : elems) {
		elem->serialize(out);
	}
}

VecValue::VecValue(std::vector<NonVecEvalExpr> literalValue) : ASTNode(), elems(literalValue) {}

std::string VecValue::to_str(std::vector<NonVecEvalExpr> val) {
	std::string buf = "[";
	const size_t val_len = val.size();
	for (size_t i = 0; i < val_len; ++i) {
		if (std::holds_alternative<nl_int_t>(val[i]))
			buf += std::to_string(std::get<nl_int_t>(val[i]));
		else if (std::holds_alternative<nl_dec_t>(val[i]))
			buf += FloatLiteral::to_str(std::get<nl_dec_t>(val[i]));
		else if (std::holds_alternative<nl_bool_t>(val[i]))
			buf += BoolLiteral::to_str(std::get<nl_bool_t>(val[i]));
		else if (std::holds_alternative<std::string>(val[i]))
			buf += StrLiteral::to_str(std::get<std::string>(val[i]));
		if (i < val_len - 1)
			buf += ", ";
	}
	return buf + "]";
}

void VecValue::print(int indent) {
	printIndent(indent);
	std::cout << "VecValue(value=" + to_str(elems) + ")\n";
}

Variable::Variable(std::string varName, ASTPtr varValue) : ASTNode(), name(varName), value(std::move(varValue)) {}

void Variable::print(int indent) {
	printIndent(indent);
	std::cout << "Variable(name=" << name << ")\n";
	printIndent(indent+2);
	std::cout << "Value:\n";

	if (value) {
		value->print(indent+4);
	}
}

void Variable::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::Variable);
	writeText(out, name);

	if (value) {
		writeBinary<bool>(out, true);
		value->serialize(out);
	} else {
		writeBinary<bool>(out, false);
	}
}

UnaryOp::UnaryOp(std::string opOp, ASTPtr opOperand)
: ASTNode(), op(opOp), operand(std::move(opOperand)) {}

void UnaryOp::print(int indent) {
	printIndent(indent);
	std::cout << "UnaryOp(op=\"" << op << "\")\n";
	printIndent(indent);
	std::cout << "Operand:\n";

	if (operand) {
		operand->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr\n";
	}
}

void UnaryOp::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::UnaryOp);
	writeText(out, op);
	operand->serialize(out);
}

BinaryOp::BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight) :
ASTNode(), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {}

void BinaryOp::print(int indent) {
	printIndent(indent);
	std::cout << "BinaryOp(op=\"" << op << "\")" << std::endl;
	printIndent(indent+2);
	std::cout << "Left:\n";
	
	if (left) {
		left->print(indent+4);
	} else {
		printIndent(indent+4);
		std::cout << "nullptr\n";
	}

	printIndent(indent+2);
	std::cout << "Right:\n";
	
	if (right) {
		right->print(indent+4);
	} else {
		printIndent(indent+4);
		std::cout << "nullptr\n";
	}
}

void BinaryOp::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::BinaryOp);
	writeText(out, op);
	left->serialize(out);
	right->serialize(out);
}

IfLiteral::IfLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> thenStmts, std::vector<ExpressionStmt> elseStmts) : condition(std::move(literalCondition)), thenClauseStmts(std::move(thenStmts)), elseClauseStmts(std::move(elseStmts)) {}

void IfLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IfLiteral(thenStmts=" << thenClauseStmts.size()
			  << ", elseStmts=" << elseClauseStmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Condition:\n";
	condition->print(indent+4);

	printIndent(indent+2);
	std::cout << "ThenClauseStatements:\n";
	for (ExpressionStmt& stmt : thenClauseStmts) {
		stmt.print(indent+4);
	}

	printIndent(indent+2);
	std::cout << "ElseClauseStatements:\n";
	for (ExpressionStmt& stmt : elseClauseStmts) {
		stmt.print(indent+4);
	}
}

void IfLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::IfLiteral);
	condition->serialize(out);

	uint32_t then_count = thenClauseStmts.size();
	writeBinary<uint32_t>(out, then_count);

	for (ExpressionStmt& stmt : thenClauseStmts) {
		stmt.serialize(out);
	}

	uint32_t else_count = elseClauseStmts.size();
	writeBinary<uint32_t>(out, else_count);

	for (ExpressionStmt& stmt : elseClauseStmts) {
		stmt.serialize(out);
	}
}

WhileLiteral::WhileLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts) : condition(std::move(literalCondition)), stmts(std::move(literalStmts)) {}

void WhileLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "WhileLiteral(statements=" << stmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Condition:\n";
	condition->print(indent+4);
	printIndent(indent+2);
	std::cout << "Statements:\n";

	for (ExpressionStmt& stmt : stmts) {
		stmt.print(indent+4);
	}
}

void WhileLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::WhileLiteral);
	condition->serialize(out);

	uint32_t stmt_count = stmts.size();
	writeBinary<uint32_t>(out, stmt_count);

	for (ExpressionStmt& stmt : stmts) {
		stmt.serialize(out);
	}
}

FunctionCall::FunctionCall(std::string callName, std::vector<ASTPtr> callParams)
: name(callName), params(std::move(callParams)) {}

void FunctionCall::print(int indent) {
	printIndent(indent);
	std::cout << "FunctionCall(name=" << name << ", parameters=" << params.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Parameters:\n";

	for (const auto& param : params) {
		param->print(indent+4);
	}
}

void FunctionCall::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::FunctionCall);
	writeText(out, name);

	uint32_t param_count = params.size();
	writeBinary<uint32_t>(out, param_count);

	for (const auto& param : params) {
		param->serialize(out);
	}
}

ReturnLiteral::ReturnLiteral(ASTPtr literalValue) : value(std::move(literalValue)) {}

void ReturnLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "ReturnLiteral()\n";

	if (value) {
		value->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr\n";
	}
}

void ReturnLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::ReturnLiteral);

	if (value) {
		writeBinary<bool>(out, true);
		value->serialize(out);
	} else {
		writeBinary<bool>(out, false);
	}
}

FunctionLiteral::FunctionLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts, ASTPtr literalReturnValue)
: name(literalName), params(std::move(literalParams)), stmts(std::move(literalStmts)), returnValue(std::move(literalReturnValue)) {}

void FunctionLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "FunctionLiteral(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Parameters:\n";

	for (const auto& param : params) {
		param->print(indent+4);
	}

	printIndent(indent+2);
	std::cout << "Statements:\n";

	for (ExpressionStmt& stmt: stmts) {
		stmt.print(indent+4);
	}
}

void FunctionLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::FunctionLiteral);
	writeText(out, name);

	uint32_t param_count = params.size();
	writeBinary<uint32_t>(out, param_count);

	for (const auto& param : params) {
		param->serialize(out);
	}

	uint32_t stmt_count = stmts.size();
	writeBinary<uint32_t>(out, stmt_count);

	for (ExpressionStmt& stmt : stmts) {
		stmt.serialize(out);
	}

	if (returnValue) {
		writeBinary<bool>(out, true);
		returnValue->serialize(out);
	} else {
		writeBinary<bool>(out, false);
	}
}

ExpressionStmt::ExpressionStmt(
		ASTPtr stmtExpr,
		bool stmtNoOp,
		bool exprIsBreak,
		bool exprIsContinue)
	: ASTNode(), expr(std::move(stmtExpr)), noOp(stmtNoOp), isBreak(exprIsBreak), isContinue(exprIsContinue) {}

void ExpressionStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ExpressionStmt()" << std::endl;
	
	if (expr) {
		expr->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr\n";
	}
}

void ExpressionStmt::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::ExpressionStmt);
	writeBinary<bool>(out, noOp);
	writeBinary<bool>(out, isBreak);
	writeBinary<bool>(out, isContinue);

	if (expr) {
		writeBinary<bool>(out, true);
		expr->serialize(out);
	} else {
		writeBinary<bool>(out, false);
	}
}


Program::Program(std::vector<ExpressionStmt>&& programStatements) : ASTNode(), statements(std::move(programStatements)) {}

void Program::print(int indent) {
	printIndent(indent);
	std::cout << "Program(statements=" << statements.size() << ")" << std::endl;

	for (ExpressionStmt& stmt : statements) {
		stmt.print(indent+2);
	}
}

void Program::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::Program);
	uint32_t num_stmts = statements.size();
	writeBinary<uint32_t>(out, num_stmts);

	for (ExpressionStmt& stmt : statements) {
		stmt.serialize(out);
	}
}
