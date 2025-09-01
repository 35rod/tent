#include <iostream>
#include "ast.hpp"
#include "misc.hpp"

static void printIndent(int indent) {
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

template<typename T>
T readBinary(std::istream& in) {
	T value;
	in.read(reinterpret_cast<char*>(&value), sizeof(T));
	
	return value;
}

std::string readText(std::istream& in) {
	uint32_t length = readBinary<uint32_t>(in);
	std::string s(length, '\0');
	in.read(&s[0], length);

	return s;
}

ASTPtr deserializeAST(std::istream& in) {
	NodeType type = readBinary<NodeType>(in);

	switch (type) {
		case NodeType::ASTNode: return std::make_unique<ASTNode>();
		case NodeType::NoOp: return std::make_unique<NoOp>();
		case NodeType::IntLiteral: return IntLiteral::deserialize(in);
		case NodeType::FloatLiteral: return FloatLiteral::deserialize(in);
		case NodeType::StrLiteral: return StrLiteral::deserialize(in);
		case NodeType::BoolLiteral: return BoolLiteral::deserialize(in);
		case NodeType::VecLiteral: return VecLiteral::deserialize(in);
		case NodeType::Variable: return Variable::deserialize(in);
		case NodeType::UnaryOp: return UnaryOp::deserialize(in);
		case NodeType::BinaryOp: return BinaryOp::deserialize(in);
		case NodeType::IfLiteral: return IfLiteral::deserialize(in);
		case NodeType::WhileLiteral: return WhileLiteral::deserialize(in);
		case NodeType::FunctionCall: return FunctionCall::deserialize(in);
		case NodeType::ReturnLiteral: return ReturnLiteral::deserialize(in);
		case NodeType::FunctionLiteral: return FunctionLiteral::deserialize(in);
		case NodeType::ExpressionStmt: return ExpressionStmt::deserialize(in);
		case NodeType::Program: return Program::deserialize(in);
		default:
			throw std::runtime_error("Unknown NodeType during deserialization");
	}
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

ASTPtr IntLiteral::deserialize(std::istream& in) {
	return std::make_unique<IntLiteral>(readBinary<nl_int_t>(in));
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

ASTPtr FloatLiteral::deserialize(std::istream& in) {
	return std::make_unique<FloatLiteral>(readBinary<nl_dec_t>(in));
}

StrLiteral::StrLiteral(std::string literalValue) : ASTNode(), value(literalValue) {}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << value << ")\n";
}

void StrLiteral::serialize(std::ostream& out) {
	writeBinary<NodeType>(out, NodeType::StrLiteral);
	writeText(out, value);
}

ASTPtr StrLiteral::deserialize(std::istream& in) {
	return std::make_unique<StrLiteral>(readText(in));
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

ASTPtr BoolLiteral::deserialize(std::istream& in) {
	return std::make_unique<BoolLiteral>(readBinary<uint8_t>(in) != 0);
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

ASTPtr VecLiteral::deserialize(std::istream& in) {
	uint32_t size = readBinary<uint32_t>(in);
	std::vector<ASTPtr> elems;
	elems.reserve(size);

	for (uint32_t i = 0; i < size; i++) {
		elems.push_back(deserializeAST(in));
	}

	return std::make_unique<VecLiteral>(std::move(elems));
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
			buf += std::get<std::string>(val[i]);
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

ASTPtr Variable::deserialize(std::istream& in) {
	std::string name = readText(in);
	bool hasValue = readBinary<bool>(in);
	ASTPtr val = nullptr;

	if (hasValue) {
		val = deserializeAST(in);
	}

	return std::make_unique<Variable>(name, std::move(val));
}

UnaryOp::UnaryOp(TokenType opOp, ASTPtr opOperand)
: ASTNode(), op(opOp), operand(std::move(opOperand)) {}

void UnaryOp::print(int indent) {
	printIndent(indent);
	std::cout << "UnaryOp(op=\"" << (uint16_t)op << "\")\n";
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
	writeBinary<TokenType>(out, op);
	operand->serialize(out);
}

ASTPtr UnaryOp::deserialize(std::istream& in) {
	TokenType op = readBinary<TokenType>(in);
	ASTPtr operand = deserializeAST(in);

	return std::make_unique<UnaryOp>(op, std::move(operand));
}

BinaryOp::BinaryOp(TokenType opOp, ASTPtr opLeft, ASTPtr opRight) :
ASTNode(), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {}

void BinaryOp::print(int indent) {
	printIndent(indent);
	std::cout << "BinaryOp(op=\"" << (uint16_t)op << "\")" << std::endl;
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
	writeBinary<TokenType>(out, op);
	left->serialize(out);
	right->serialize(out);
}

ASTPtr BinaryOp::deserialize(std::istream& in) {
	TokenType op = readBinary<TokenType>(in);
	ASTPtr left = deserializeAST(in);
	ASTPtr right = deserializeAST(in);

	return std::make_unique<BinaryOp>(op, std::move(left), std::move(right));
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

ASTPtr IfLiteral::deserialize(std::istream& in) {
	ASTPtr cond = deserializeAST(in);

	uint32_t then_count = readBinary<uint32_t>(in);
	std::vector<ExpressionStmt> thenStmts;
	thenStmts.reserve(then_count);

	for (uint32_t i = 0; i < then_count; i++) {
		thenStmts.push_back(std::move(*dynamic_cast<ExpressionStmt*>(deserializeAST(in).release())));
	}

	uint32_t else_count = readBinary<uint32_t>(in);
	std::vector<ExpressionStmt> elseStmts;
	elseStmts.reserve(else_count);

	for (uint32_t i = 0; i < else_count; i++) {
		elseStmts.push_back(std::move(*dynamic_cast<ExpressionStmt*>(deserializeAST(in).release())));
	}

	return std::make_unique<IfLiteral>(std::move(cond), std::move(thenStmts), std::move(elseStmts));
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

ASTPtr WhileLiteral::deserialize(std::istream& in) {
	ASTPtr cond = deserializeAST(in);

	uint32_t stmt_count = readBinary<uint32_t>(in);
	std::vector<ExpressionStmt> statements;
	statements.reserve(stmt_count);

	for (uint32_t i = 0; i < stmt_count; i++) {
		statements.push_back(std::move(*dynamic_cast<ExpressionStmt*>(deserializeAST(in).release())));
	}

	return std::make_unique<WhileLiteral>(std::move(cond), std::move(statements));
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

ASTPtr FunctionCall::deserialize(std::istream& in) {
	std::string name = readText(in);

	uint32_t param_count = readBinary<uint32_t>(in);
	std::vector<ASTPtr> parameters;
	parameters.reserve(param_count);

	for (uint32_t i = 0; i < param_count; i++) {
		parameters.push_back(deserializeAST(in));
	}

	return std::make_unique<FunctionCall>(name, std::move(parameters));
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

ASTPtr ReturnLiteral::deserialize(std::istream& in) {
	bool hasVal = readBinary<bool>(in);
	ASTPtr val = nullptr;

	if (hasVal) {
		val = deserializeAST(in);
	}

	return std::make_unique<ReturnLiteral>(std::move(val));
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

ASTPtr FunctionLiteral::deserialize(std::istream& in) {
	std::string name = readText(in);

	uint32_t param_count = readBinary<uint32_t>(in);
	std::vector<ASTPtr> parameters;
	parameters.reserve(param_count);

	for (uint32_t i = 0; i < param_count; i++) {
		parameters.push_back(deserializeAST(in));
	}

	uint32_t stmt_count = readBinary<uint32_t>(in);
	std::vector<ExpressionStmt> statements;
	statements.reserve(stmt_count);

	for (uint32_t i = 0; i < stmt_count; i++) {
		statements.push_back(std::move(*dynamic_cast<ExpressionStmt*>(deserializeAST(in).release())));
	}

	bool hasReturn = readBinary<bool>(in);
	ASTPtr ret = nullptr;

	if (hasReturn) {
		ret = deserializeAST(in);
	}

	return std::make_unique<FunctionLiteral>(name, std::move(parameters), std::move(statements), std::move(ret));
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

ASTPtr ExpressionStmt::deserialize(std::istream& in) {
	bool noOp = readBinary<bool>(in);
	bool isBreak = readBinary<bool>(in);
	bool isContinue = readBinary<bool>(in);

	bool hasExpr = readBinary<bool>(in);
	ASTPtr expr = nullptr;

	if (hasExpr) {
		expr = deserializeAST(in);
	}

	return std::make_unique<ExpressionStmt>(std::move(expr), noOp, isBreak, isContinue);
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

ASTPtr Program::deserialize(std::istream& in) {
	uint32_t num_stmts = readBinary<uint32_t>(in);
	std::vector<ExpressionStmt> stmts;
	stmts.reserve(num_stmts);

	for (uint32_t i = 0; i < num_stmts; i++) {
		stmts.push_back(std::move(*dynamic_cast<ExpressionStmt*>(deserializeAST(in).release())));
	}

	return std::make_unique<Program>(std::move(stmts));
}
