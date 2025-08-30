#include <iostream>
#include <cstdio>
#include "ast.hpp"
#include "misc.hpp"

static void printIndent(int indent) {
	printf("%*s", indent, " ");
}

static void writeIndent(std::ostream& out, int indent) {
	out << std::string(indent, ' ');
}

void ASTNode::print(int indent) {
	printIndent(indent);
	std::cout << "ASTNode()" << std::endl;
}

void ASTNode::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "ASTNode()" << std::endl;
}

void NoOp::print(int ident) {
	printIndent(ident);
	std::cout << "NoOp()\n";
}

void NoOp::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "NoOp()\n";
}

IntLiteral::IntLiteral(nl_int_t literalValue) : ASTNode(), value(literalValue) {}

void IntLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IntLiteral(value=" << value << ")\n";
}

void IntLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "IntLiteral(value=" << value << ")\n";
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

void FloatLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "FloatLiteral(value=" << to_str(value) << ")\n";
}

StrLiteral::StrLiteral(std::string literalValue) : ASTNode(), value(literalValue) {}

std::string StrLiteral::to_str(std::string val) {
	return "\"" + val + "\"";
}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << to_str(value) << ")\n";
}

void StrLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "StringLiteral(value=" << to_str(value) << ")\n";
}


BoolLiteral::BoolLiteral(nl_bool_t literalValue) : ASTNode(), value(literalValue) {}

std::string BoolLiteral::to_str(nl_bool_t val) {
	return (val) ? "true" : "false";
}

void BoolLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "BoolLiteral(value=" << to_str(value) << ")\n";
}


void BoolLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "BoolLiteral(value=" << to_str(value) << ")\n";
}

VecLiteral::VecLiteral(std::vector<ASTPtr> literalValue) : ASTNode(), elems(std::move(literalValue)) {}

void VecLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "VecLiteral(size=" << elems.size() << ")\n";
}

void VecLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "VecLiteral(size=" << elems.size() << ")\n";
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

void VecValue::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "VecValue(value=" << to_str(elems) << ")\n";
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

void Variable::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "Variable(name=" << name << ")\n";
	writeIndent(out, indent+2);
	out << "Value:\n";

	if (value) {
		value->serialize(out, indent+4);
	} else {
		writeIndent(out, indent+4);
		out << "nullptr\n";
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

void UnaryOp::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "UnaryOp(op=\"" << op << "\")\n";
	writeIndent(out, indent);
	out << "Operand:\n";

	if (operand) {
		operand->serialize(out, indent+2);
	} else {
		writeIndent(out, indent+2);
		out << "nullptr\n";
	}
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

void BinaryOp::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "BinaryOp(op=\"" << op << "\")" << std::endl;
	writeIndent(out, indent+2);
	out << "Left:\n";
	
	if (left) {
		left->print(indent+4);
	} else {
		writeIndent(out, indent+4);
		out << "nullptr\n";
	}

	writeIndent(out, indent+2);
	out << "Right:\n";
	
	if (right) {
		right->serialize(out, indent+4);
	} else {
		writeIndent(out, indent+4);
		out << "nullptr\n";
	}
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

void IfLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "IfLiteral(thenStmts=" << thenClauseStmts.size()
			  << ", elseStmts=" << elseClauseStmts.size() << ")\n";
	writeIndent(out, indent+2);
	out << "Condition:\n";
	condition->serialize(out, indent+4);

	printIndent(indent+2);
	out << "ThenClauseStatements:\n";
	for (ExpressionStmt& stmt : thenClauseStmts) {
		stmt.serialize(out, indent+4);
	}

	printIndent(indent+2);
	out << "ElseClauseStatements:\n";
	for (ExpressionStmt& stmt : elseClauseStmts) {
		stmt.serialize(out, indent+4);
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

void WhileLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "WhileLiteral(statements=" << stmts.size() << ")\n";
	writeIndent(out, indent+2);
	out << "Condition:\n";
	condition->serialize(out, indent+4);
	writeIndent(out, indent+2);
	out << "Statements:\n";

	for (ExpressionStmt& stmt : stmts) {
		stmt.serialize(out, indent+4);
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

void FunctionCall::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "FunctionCall(name=" << name << ", parameters=" << params.size() << ")\n";
	writeIndent(out, indent+2);
	out << "Parameters:\n";

	for (const auto& param : params) {
		param->serialize(out, indent+4);
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

void ReturnLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "ReturnLiteral()\n";

	if (value) {
		value->serialize(out, indent+2);
	} else {
		writeIndent(out, indent+2);
		out << "nullptr\n";
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

void FunctionLiteral::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "FunctionLiteral(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
	writeIndent(out, indent+2);
	out << "Parameters:\n";

	for (const auto& param : params) {
		param->serialize(out, indent+4);
	}

	writeIndent(out, indent+2);
	out << "Statements:\n";

	for (ExpressionStmt& stmt: stmts) {
		stmt.serialize(out, indent+4);
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

void ExpressionStmt::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "ExpressionStmt()" << std::endl;
	
	if (expr) {
		expr->serialize(out, indent+2);
	} else {
		writeIndent(out, indent+2);
		out << "nullptr\n";
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

void Program::serialize(std::ostream& out, int indent) {
	writeIndent(out, indent);
	out << "Program(statements=" << statements.size() << ")" << std::endl;

	for (ExpressionStmt& stmt : statements) {
		stmt.serialize(out, indent+2);
	}
}
