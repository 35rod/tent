#include <iostream>
#include "ast.hpp"
#include "misc.hpp"

IntLiteral::IntLiteral(nl_int_t literalValue) : ASTNode(), value(literalValue) {}

void IntLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IntLiteral(value=" << value << ")\n";
}

FloatLiteral::FloatLiteral(nl_dec_t literalValue) : ASTNode(), value(literalValue) {}

void FloatLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "FloatLiteral(value=" << value << ")\n";
}

StrLiteral::StrLiteral(std::string literalValue) : ASTNode(), value(literalValue) {}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << value << ")\n";
}

BoolLiteral::BoolLiteral(nl_bool_t literalValue) : ASTNode(), value(literalValue) {}

void BoolLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "BoolLiteral(value=" << (value ? "true" : "false") << ")\n";
}

VecLiteral::VecLiteral(std::vector<ASTPtr> literalValue) : ASTNode(), elems(std::move(literalValue)) {}

void VecLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "VecLiteral(size=" << elems.size() << ")\n";
}

TypeInt::TypeInt() : ASTNode() {}

void TypeInt::print(int indent) {
	printIndent(indent);
	std::cout << "TypeInt()" << std::endl;
}

TypeFloat::TypeFloat() : ASTNode() {}

void TypeFloat::print(int indent) {
	printIndent(indent);
	std::cout << "TypeFloat()" << std::endl;
}

TypeStr::TypeStr() : ASTNode() {}

void TypeStr::print(int indent) {
	printIndent(indent);
	std::cout << "TypeStr()" << std::endl;
}

TypeBool::TypeBool() : ASTNode() {}

void TypeBool::print(int indent) {
	printIndent(indent);
	std::cout << "TypeBool()" << std::endl;
}

TypeVec::TypeVec() : ASTNode() {}

void TypeVec::print(int indent) {
	printIndent(indent);
	std::cout << "TypeVec()" << std::endl;
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

IfStmt::IfStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> thenStmts, std::vector<ExpressionStmt> elseStmts) : condition(std::move(stmtCondition)), thenClauseStmts(std::move(thenStmts)), elseClauseStmts(std::move(elseStmts)) {}

void IfStmt::print(int indent) {
	printIndent(indent);
	std::cout << "IfStmt(thenStmts=" << thenClauseStmts.size() << ", elseStmts=" << elseClauseStmts.size() << ")\n";
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

WhileStmt::WhileStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> stmtStmts) :
ASTNode(), condition(std::move(stmtCondition)), stmts(std::move(stmtStmts)) {}

void WhileStmt::print(int indent) {
	printIndent(indent);
	std::cout << "WhileStmt(statements=" << stmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Condition:\n";
	condition->print(indent+4);
	printIndent(indent+2);
	std::cout << "Statements:\n";

	for (ExpressionStmt& stmt : stmts) {
		stmt.print(indent+4);
	}
}

ForStmt::ForStmt(ASTPtr stmtVar, ASTPtr stmtIter, std::vector<ExpressionStmt> stmtStmts)  :
ASTNode(), var(std::move(stmtVar)), iter(std::move(stmtIter)), stmts(std::move(stmtStmts)) {}

void ForStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ForStmt(statements=" << stmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Var:\n";
	var->print(indent+4);
	printIndent(indent+2);
	std::cout << "Iter:\n";
	iter->print(indent+4);
	printIndent(indent+2);
	std::cout << "Statements:\n";

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
	std::cout << "Parameters:\n";

	for (const auto& param : params) {
		param->print(indent+4);
	}
}

ReturnStmt::ReturnStmt(ASTPtr stmtValue) : value(std::move(stmtValue)) {}

void ReturnStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ReturnStmt()\n";

	if (value) {
		value->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr\n";
	}
}

FunctionStmt::FunctionStmt(std::string stmtName, std::vector<ASTPtr> stmtParams, std::vector<ExpressionStmt> stmtStmts, ASTPtr stmtReturnValue)
: name(stmtName), params(std::move(stmtParams)), stmts(std::move(stmtStmts)), returnValue(std::move(stmtReturnValue)) {}

void FunctionStmt::print(int indent) {
	printIndent(indent);
	std::cout << "FunctionStmt(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
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

InlineStmt::InlineStmt(std::string stmtName, std::vector<ASTPtr> stmtParams, std::vector<ExpressionStmt> stmtStmts, ASTPtr stmtReturnValue)
: name(stmtName), params(std::move(stmtParams)), stmts(std::move(stmtStmts)), returnValue(std::move(stmtReturnValue)) {}

void InlineStmt::print(int indent) {
	printIndent(indent);
	std::cout << "InlineStmt(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
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

ClassStmt::ClassStmt(std::string stmtName, std::vector<ASTPtr> stmtParams, std::vector<ExpressionStmt> stmtStmts)
: name(stmtName), params(std::move(stmtParams)), stmts(std::move(stmtStmts)) {}

void ClassStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ClassStmt(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
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

ExpressionStmt::ExpressionStmt(
		ASTPtr stmtExpr,
		bool stmtNoOp,
		bool exprIsBreak,
		bool exprIsContinue
) : ASTNode(), expr(std::move(stmtExpr)), noOp(stmtNoOp), isBreak(exprIsBreak), isContinue(exprIsContinue) {}

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

Program::Program(std::vector<ExpressionStmt>&& programStatements) : ASTNode(), statements(std::move(programStatements)) {}

void Program::print(int indent) {
	printIndent(indent);
	std::cout << "Program(statements=" << statements.size() << ")" << std::endl;

	for (ExpressionStmt& stmt : statements) {
		stmt.print(indent+2);
	}
}