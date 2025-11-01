#include "ast.hpp"

#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

#include "misc.hpp"

IntLiteral::IntLiteral(tn_int_t literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

llvm::Value* IntLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module&) {
	auto& builder = static_cast<llvm::IRBuilder<>&>(builderBase);
	return llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), value);
}

void IntLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IntLiteral(value=" << value << ")\n";
}

FloatLiteral::FloatLiteral(tn_dec_t literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

void FloatLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "FloatLiteral(value=" << value << ")\n";
}

StrLiteral::StrLiteral(std::string literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

llvm::Value* StrLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	auto& builder = static_cast<llvm::IRBuilder<>&>(builderBase);
	return builder.CreateGlobalStringPtr(value, "str");
}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << value << ")\n";
}

BoolLiteral::BoolLiteral(tn_bool_t literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

void BoolLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "BoolLiteral(value=" << (value ? "true" : "false") << ")\n";
}

VecLiteral::VecLiteral(std::vector<ASTPtr> literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), elems(std::move(literalValue)) {}

void VecLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "VecLiteral(size=" << elems.size() << ")\n";
}

DicLiteral::DicLiteral(std::map<ASTPtr, ASTPtr> literalDic, int line, int col, std::string file)
: ASTNode(line, col, file), dic(std::move(literalDic)) {}

void DicLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "DicLiteral(size=" << dic.size() << ")\n";
}

TypeInt::TypeInt(int line, int col, std::string file)
: ASTNode(line, col, file) {}

void TypeInt::print(int indent) {
	printIndent(indent);
	std::cout << "TypeInt()" << std::endl;
}

TypeFloat::TypeFloat(int line, int col, std::string file)
: ASTNode(line, col, file) {}

void TypeFloat::print(int indent) {
	printIndent(indent);
	std::cout << "TypeFloat()" << std::endl;
}

TypeStr::TypeStr(int line, int col, std::string file)
: ASTNode(line, col, file) {}

void TypeStr::print(int indent) {
	printIndent(indent);
	std::cout << "TypeStr()" << std::endl;
}

TypeBool::TypeBool(int line, int col, std::string file)
: ASTNode(line, col, file) {}

void TypeBool::print(int indent) {
	printIndent(indent);
	std::cout << "TypeBool()" << std::endl;
}

TypeVec::TypeVec(int line, int col, std::string file)
: ASTNode(line, col, file) {}

void TypeVec::print(int indent) {
	printIndent(indent);
	std::cout << "TypeVec()" << std::endl;
}

TypeDic::TypeDic(int line, int col, std::string file)
: ASTNode(line, col, file) {}

void TypeDic::print(int indent) {
    printIndent(indent);
    std::cout << "TypeDic()" << std::endl;
}

Variable::Variable(std::string varName, ASTPtr varValue, int line, int col, std::string file)
: ASTNode(line, col, file), name(varName), value(std::move(varValue)) {}

void Variable::print(int indent) {
	printIndent(indent);
	std::cout << "Variable(name=" << name << ")\n";
	printIndent(indent+2);
	std::cout << "Value:\n";

	if (value) {
		value->print(indent+4);
	}
}

UnaryOp::UnaryOp(TokenType opOp, ASTPtr opOperand, int line, int col, std::string file)
: ASTNode(line, col, file), op(opOp), operand(std::move(opOperand)) {}

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

BinaryOp::BinaryOp(TokenType opOp, ASTPtr opLeft, ASTPtr opRight, int line, int col, std::string file)
: ASTNode(line, col, file), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {}

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

IfStmt::IfStmt(
	ASTPtr stmtCondition,
	std::vector<ExpressionStmt> thenStmts,
	std::vector<ExpressionStmt> elseStmts,
	int line,
	int col,
	std::string file
) : ASTNode(line, col, file), condition(std::move(stmtCondition)), thenClauseStmts(std::move(thenStmts)), elseClauseStmts(std::move(elseStmts)) {}

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

WhileStmt::WhileStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> stmtStmts, int line, int col, std::string file) :
ASTNode(line, col, file), condition(std::move(stmtCondition)), stmts(std::move(stmtStmts)) {}

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

ForStmt::ForStmt(std::string stmtVar, ASTPtr stmtIter, std::vector<ExpressionStmt> stmtStmts, int line, int col, std::string file)  :
ASTNode(line, col, file), var(std::move(stmtVar)), iter(std::move(stmtIter)), stmts(std::move(stmtStmts)) {}

void ForStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ForStmt(statements=" << stmts.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Var:\n";
	printIndent(indent+4);
	printf("%s", (var + '\n').c_str());
	printIndent(indent+2);
	std::cout << "Iter:\n";
	iter->print(indent+4);
	printIndent(indent+2);
	std::cout << "Statements:\n";

	for (ExpressionStmt& stmt : stmts) {
		stmt.print(indent+4);
	}
}

FunctionCall::FunctionCall(std::string callName, std::vector<ASTPtr> callParams, int line, int col, std::string file)
: ASTNode(line, col, file), name(callName), params(std::move(callParams)) {}

llvm::Value* FunctionCall::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	auto& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	if (name == "print" && !params.empty()) {
		llvm::Value* argVal = params[0]->codegen(ctx, builder, module);

		llvm::FunctionType* printfType = llvm::FunctionType::get(
			llvm::Type::getInt32Ty(ctx),
			llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
			true
		);

		llvm::FunctionCallee printfFunc = module.getOrInsertFunction("printf", printfType);

		llvm::Value* fmt = nullptr;

		if (argVal->getType()->isIntegerTy())
			fmt = builder.CreateGlobalStringPtr("%lld\n", "fmt_int");
		else
			fmt = builder.CreateGlobalStringPtr("%s\n", "fmt_str");
		
		return builder.CreateCall(printfFunc, {fmt, argVal});
	}

	return nullptr;
}

void FunctionCall::print(int indent) {
	printIndent(indent);
	std::cout << "FunctionCall(name=" << name << ", parameters=" << params.size() << ")\n";
	printIndent(indent+2);
	std::cout << "Parameters:\n";

	for (const auto& param : params) {
		param->print(indent+4);
	}
}

ReturnStmt::ReturnStmt(ASTPtr stmtValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(std::move(stmtValue)) {}

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

FunctionStmt::FunctionStmt(
	std::string stmtName, 
	std::vector<ASTPtr> stmtParams, 
	std::vector<ExpressionStmt> stmtStmts, 
	ASTPtr stmtReturnValue,
	int line,
	int col,
	std::string file
) : ASTNode(line, col, file), name(stmtName), params(std::move(stmtParams)), stmts(std::move(stmtStmts)), returnValue(std::move(stmtReturnValue)) {}

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

InlineStmt::InlineStmt(
	std::string stmtName, 
	std::vector<ASTPtr> stmtParams, 
	std::vector<ExpressionStmt> stmtStmts, 
	ASTPtr stmtReturnValue,
	int line,
	int col,
	std::string file
) : ASTNode(line, col, file), name(stmtName), params(std::move(stmtParams)), stmts(std::move(stmtStmts)), returnValue(std::move(stmtReturnValue)) {}

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

ClassStmt::ClassStmt(
	std::string stmtName, 
	std::vector<ASTPtr> stmtParams, 
	std::vector<ExpressionStmt> stmtStmts,
	int line,
	int col,
	std::string file
) : ASTNode(line, col, file), name(stmtName), params(std::move(stmtParams)), stmts(std::move(stmtStmts)) {}

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
	bool exprIsContinue,
	int line,
	int col,
	std::string file
) : ASTNode(line, col, file), expr(std::move(stmtExpr)), noOp(stmtNoOp), isBreak(exprIsBreak), isContinue(exprIsContinue) {}

llvm::Value* ExpressionStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	if (expr)
		return expr->codegen(ctx, builderBase, module);
	
	return nullptr;
}

void ExpressionStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ExpressionStmt(break=" << (isBreak ? "true" : "false") << ")" << std::endl;
	
	if (expr) {
		expr->print(indent+2);
	} else {
		printIndent(indent+2);
		std::cout << "nullptr\n";
	}
}

Program::Program(std::vector<ExpressionStmt>&& programStatements, int line, int col, std::string file)
: ASTNode(line, col, file), statements(std::move(programStatements)) {}

llvm::Value* Program::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	auto& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	llvm::FunctionType* mainType = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
	llvm::Function* mainFunc = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", &module);

	llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", mainFunc);
	builder.SetInsertPoint(entry);

	for (auto& stmt : statements)
		stmt.codegen(ctx, builder, module);
	
	builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0));

	llvm::verifyFunction(*mainFunc);
	llvm::verifyModule(module);

	return mainFunc;
}

void Program::print(int indent) {
	printIndent(indent);
	std::cout << "Program(statements=" << statements.size() << ")" << std::endl;

	for (ExpressionStmt& stmt : statements) {
		stmt.print(indent+2);
	}
}
