#include <iostream>
#include <cstdio>
#include "ast.hpp"
#include "misc.hpp"

static void printIndent(int indent) {
	printf("%*s", indent, " ");
}

void ASTNode::print(int indent) {
	printIndent(indent);
	std::cout << "ASTNode()" << std::endl;
}

void NoOp::print(int ident) {
	printIndent(ident);
	std::cout << "NoOp()" << std::endl;
}


IntLiteral::IntLiteral(nl_int_t literalValue) : ASTNode(), value(literalValue) {}

std::string IntLiteral::to_str(nl_int_t val) {
	return std::to_string(val);
}

void IntLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IntLiteral(value=" << to_str(value) << ")\n";
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


StrLiteral::StrLiteral(std::string literalValue) : ASTNode(), value(literalValue) {}

std::string StrLiteral::to_str(std::string val) {
	return "\"" + val + "\"";
}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << to_str(value) << ")\n";
}


BoolLiteral::BoolLiteral(nl_bool_t literalValue) : ASTNode(), value(literalValue) {}

std::string BoolLiteral::to_str(nl_bool_t val) {
	return (val) ? "true" : "false";
}

void BoolLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "BoolLiteral(value=" << to_str(value) << ")\n";
}


VecLiteral::VecLiteral(std::vector<ASTPtr> literalValue) : ASTNode(), elems(std::move(literalValue)) {}

void VecLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "VecLiteral(size=" + std::to_string(elems.size()) + ")\n";
}

VecValue::VecValue(std::vector<NonVecEvalExpr> literalValue) : ASTNode(), elems(literalValue) {}

std::string VecValue::to_str(std::vector<NonVecEvalExpr> val) {
	std::string buf = "[";
	const size_t val_len = val.size();
	for (size_t i = 0; i < val_len; ++i) {
		if (std::holds_alternative<nl_int_t>(val[i]))
			buf += IntLiteral::to_str(std::get<nl_int_t>(val[i]));
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

	if (value) {
		value->print(indent);
	}
}

UnaryOp::UnaryOp(std::string opOp, ASTPtr opOperand)
: ASTNode(), op(opOp), operand(std::move(opOperand)) {}

void UnaryOp::print(int indent) {
	printIndent(indent);
	std::cout << "UnaryOp(op=\"" << op << "\")" << std::endl;
	printIndent(indent);
	std::cout << " Operand:" << std::endl;

	if (operand) {
		operand->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr" << std::endl;
	}
}

BinaryOp::BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight) :
ASTNode(), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {}

void BinaryOp::print(int indent) {
	printIndent(indent);
	std::cout << "BinaryOp(op=\"" << op << "\")" << std::endl;
	printIndent(indent);
	std::cout << " Left:" << std::endl;
	
	if (left) {
		left->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr" << std::endl;
	}

	printIndent(indent);
	std::cout << " Right:" << std::endl;
	
	if (right) {
		right->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr" << std::endl;
	}
}

IfLiteral::IfLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> thenStmts, std::vector<ExpressionStmt> elseStmts) : condition(std::move(literalCondition)), thenClauseStmts(std::move(thenStmts)), elseClauseStmts(std::move(elseStmts)) {}

void IfLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IfLiteral(thenStmts=" << thenClauseStmts.size()
			  << ", elseStmts=" << elseClauseStmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << " Condition:\n";
	condition->print(indent+4);

	printIndent(indent+2);
	std::cout << " ThenClauseStatements:\n";
	for (ExpressionStmt& stmt : thenClauseStmts) {
		stmt.print(indent+4);
	}

	printIndent(indent+2);
	std::cout << "  ElseClauseStatements:\n";
	for (ExpressionStmt& stmt : elseClauseStmts) {
		stmt.print(indent+4);
	}
}

WhileLiteral::WhileLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts) : condition(std::move(literalCondition)), stmts(std::move(literalStmts)) {}

void WhileLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "WhileLiteral(statements=" << stmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << " Condition:\n";
	condition->print(indent+4);
	printIndent(indent+2);
	std::cout << " Statements:\n";

	for (ExpressionStmt& stmt : stmts) {
		stmt.print(indent+4);
	}
}

FunctionCall::FunctionCall(std::string callName, std::vector<ASTPtr> callParams)
: name(callName), params(std::move(callParams)) {}

void FunctionCall::print(int indent) {
	printIndent(indent);
	std::cout << "FunctionCall(name=" << name << ", parameters=" << params.size() << ")\n";
	printIndent(indent+2);
	std::cout << " Parameters:\n";

	for (const auto& param : params) {
		param->print(indent+4);
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
		std::cout << "nullptr" << std::endl;
	}
}

FunctionLiteral::FunctionLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts, ASTPtr literalReturnValue)
: name(literalName), params(std::move(literalParams)), stmts(std::move(literalStmts)), returnValue(std::move(literalReturnValue)) {}

void FunctionLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "FunctionLiteral(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
	printIndent(indent+2);
	std::cout << " Parameters:\n";

	for (const auto& param : params) {
		param->print(indent+4);
	}

	printIndent(indent+2);
	std::cout << " Statements:\n";

	for (ExpressionStmt& stmt: stmts) {
		stmt.print(indent+4);
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
		std::cout << "nullptr" << std::endl;
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
