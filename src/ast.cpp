#include "ast.hpp"

#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

#include "misc.hpp"

llvm::StructType* DynamicValueTy = nullptr;

llvm::StructType* getDynamicValueType(llvm::LLVMContext& ctx) {
	if (DynamicValueTy) return DynamicValueTy;

	std::vector<llvm::Type*> fields = {
		llvm::Type::getInt32Ty(ctx),
		llvm::PointerType::get(ctx, 0)
	};

	DynamicValueTy = llvm::StructType::create(ctx, fields, "DynamicValue");

	return DynamicValueTy;
}

llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* mainFunc, const std::string& varName) {
	llvm::BasicBlock& entryBlock = mainFunc->getEntryBlock();
	llvm::IRBuilder<> TmpBuilder(&entryBlock, entryBlock.begin());

	llvm::StructType* dynamicType = getDynamicValueType(mainFunc->getContext());

	return TmpBuilder.CreateAlloca(dynamicType, nullptr, varName.c_str());
}

llvm::Value* boxPrimitive(llvm::Value* val, llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
	llvm::StructType* dynamicType = getDynamicValueType(ctx);
	
	llvm::FunctionCallee boxFunc;
	llvm::Value* arg = val;

	if (val->getType()->isIntegerTy(64)) {
		boxFunc = module.getOrInsertFunction(
			"box_int",
			llvm::FunctionType::get(
				dynamicType,
				{llvm::Type::getInt64Ty(ctx)},
				false
			)
		);
	} else if (val->getType()->isDoubleTy()) {
		boxFunc = module.getOrInsertFunction(
			"box_float",
			llvm::FunctionType::get(
				dynamicType,
				{llvm::Type::getDoubleTy(ctx)},
				false
			)
		);
	} else if (val->getType()->isPointerTy()) {
		boxFunc = module.getOrInsertFunction(
			"box_string",
			llvm::FunctionType::get(
				dynamicType,
				{llvm::Type::getInt8Ty(ctx)->getPointerTo()},
				false
			)
		);
	} else if (val->getType()->isIntegerTy(1)) {
		boxFunc = module.getOrInsertFunction(
			"box_bool",
			llvm::FunctionType::get(
				dynamicType,
				{llvm::Type::getInt1Ty(ctx)},
				false
			)
		);
	}

	return builder.CreateCall(boxFunc, {arg});
}

enum TypeTag {
	INT,
	FLOAT,
	STRING,
	BOOL
};

std::map<std::string, llvm::AllocaInst*> NamedValues;
std::vector<llvm::BasicBlock*> BreakBlockStack;
std::vector<llvm::BasicBlock*> ContinueBlockStack;

IntLiteral::IntLiteral(tn_int_t literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

llvm::Value* IntLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module&) {
	return llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), value);
}

void IntLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "IntLiteral(value=" << value << ")\n";
}

FloatLiteral::FloatLiteral(tn_dec_t literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

llvm::Value* FloatLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase&, llvm::Module&) {
	return llvm::ConstantFP::get(ctx, llvm::APFloat(value));
}

void FloatLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "FloatLiteral(value=" << value << ")\n";
}

StrLiteral::StrLiteral(std::string literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

llvm::Value* StrLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	auto& builder = static_cast<llvm::IRBuilder<>&>(builderBase);
	return builder.CreateGlobalString(value, "str");
}

void StrLiteral::print(int indent) {
	printIndent(indent);
	std::cout << "StringLiteral(value=" << value << ")\n";
}

BoolLiteral::BoolLiteral(tn_bool_t literalValue, int line, int col, std::string file)
: ASTNode(line, col, file), value(literalValue) {}

llvm::Value* BoolLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase&, llvm::Module&) {
	return llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), value, false);
}

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

llvm::Value* Variable::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	llvm::IRBuilder<>& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	llvm::AllocaInst* allocaInst = NamedValues.count(name) ? NamedValues.at(name) : nullptr;

	if (!allocaInst) {
		std::cerr << "Error: Undefined variable: " << name << std::endl;
		return nullptr;
	}

	llvm::StructType* dynamicType = getDynamicValueType(ctx);

	return builder.CreateLoad(dynamicType, allocaInst, name + ".loaded_dynamic_value");
}

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

llvm::Value* BinaryOp::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
    llvm::IRBuilder<>& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	if (op == TokenType::ASSIGN) {
		if (Variable* varNode = dynamic_cast<Variable*>(left.get())) {
			llvm::Value* RHS_Value = right->codegen(ctx, builder, module);
			if (!RHS_Value) return nullptr;

			llvm::AllocaInst* allocaInst = NamedValues.count(varNode->name) ? NamedValues.at(varNode->name) : nullptr;

			if (!allocaInst) {
				llvm::Function* mainFunc = builder.GetInsertBlock()->getParent();
				allocaInst = CreateEntryBlockAlloca(mainFunc, varNode->name);
				NamedValues[varNode->name] = allocaInst;
			}

			llvm::StructType* dynamicType = getDynamicValueType(ctx);

			llvm::Value* boxedRHS = (RHS_Value->getType() == dynamicType) ? RHS_Value : boxPrimitive(RHS_Value, ctx, builder, module);
			builder.CreateStore(boxedRHS, allocaInst);

			return boxedRHS;
		}
	}

    llvm::Value* L = left->codegen(ctx, builder, module);
    llvm::Value* R = right->codegen(ctx, builder, module);

    if (!L || !R) return nullptr;

	if (L->getType()->isIntegerTy(64) && R->getType()->isIntegerTy(64)) {
		switch (op) {
			case TokenType::ADD: return builder.CreateAdd(L, R, "add_i64");
			case TokenType::SUB: return builder.CreateSub(L, R, "sub_i64");
			case TokenType::MUL: return builder.CreateMul(L, R, "mul_i64");
			case TokenType::DIV: return builder.CreateSDiv(L, R, "div_i64");
			default: break;
		}
	} else if ((L->getType()->isDoubleTy() || L->getType()->isIntegerTy(64)) &&
		(R->getType()->isDoubleTy() || R->getType()->isIntegerTy(64))) {
			llvm::Value* Lf = L->getType()->isDoubleTy() ? L : builder.CreateSIToFP(L, llvm::Type::getDoubleTy(ctx));
			llvm::Value* Rf = R->getType()->isDoubleTy() ? R : builder.CreateSIToFP(R, llvm::Type::getDoubleTy(ctx));

			switch (op) {
				case TokenType::ADD: return builder.CreateFAdd(Lf, Rf, "add_f64");
				case TokenType::SUB: return builder.CreateFSub(Lf, Rf, "sub_f64");
				case TokenType::MUL: return builder.CreateFMul(Lf, Rf, "mul_f64");
				case TokenType::DIV: return builder.CreateFDiv(Lf, Rf, "div_f64");
				default: break;
			}
		}

    llvm::StructType* dynamicType = getDynamicValueType(ctx);
	llvm::FunctionType* binOpTy = llvm::FunctionType::get(
		dynamicType,
		{dynamicType, dynamicType},
		false
	);

	llvm::FunctionCallee binOpFunc;
	std::string funcName;

	switch (op) {
		case TokenType::ADD: funcName = "dynamic_add"; break;
		case TokenType::SUB: funcName = "dynamic_sub"; break;
		case TokenType::MUL: funcName = "dynamic_mul"; break;
		case TokenType::DIV: funcName = "dynamic_div"; break;
		case TokenType::EQEQ: funcName = "dynamic_eqeq"; break;
		case TokenType::NOTEQ: funcName = "dynamic_noteq"; break;
		case TokenType::LESS: funcName = "dynamic_less"; break;
		case TokenType::GREATER: funcName = "dynamic_greater"; break;
		case TokenType::LESSEQ: funcName = "dynamic_lesseq"; break;
		case TokenType::GREATEREQ: funcName = "dynamic_greatereq"; break;
		default:
			std::cerr << "Error: Dynamic operation " << tokenTypeToString(op) << " not implemented" << std::endl;
			return nullptr;
	}

	binOpFunc = module.getOrInsertFunction(funcName, binOpTy);

	llvm::Value* LB = L->getType() == dynamicType ? L : boxPrimitive(L, ctx, builder, module);
	llvm::Value* RB = R->getType() == dynamicType ? R : boxPrimitive(R, ctx, builder, module);

	llvm::Value* res = builder.CreateCall(binOpFunc, {LB, RB}, funcName + "_result");

	llvm::FunctionType* freeTy = llvm::FunctionType::get(
		llvm::Type::getVoidTy(ctx),
		{dynamicType},
		false
	);

	llvm::FunctionCallee freeFunc = module.getOrInsertFunction("free_dynamic_value", freeTy);

	if (L->getType() != dynamicType) {
		builder.CreateCall(freeFunc, {LB});
	} if (R->getType() != dynamicType) {
		builder.CreateCall(freeFunc, {RB});
	}

	return res;
}

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

llvm::Value* IfStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	llvm::IRBuilder<>& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	llvm::Value* condResult = condition->codegen(ctx, builder, module);
	if (!condResult) return nullptr;

	llvm::Value* Final_Cond_Value = condResult;
	llvm::Type* i1Type = llvm::Type::getInt1Ty(ctx);
	llvm::StructType* dynamicType = getDynamicValueType(ctx);

	if (condResult->getType() == dynamicType) {
		llvm::FunctionType* unboxTy = llvm::FunctionType::get(
			i1Type,
			{dynamicType},
			false
		);

		llvm::FunctionCallee unboxFunc = module.getOrInsertFunction("unbox_bool", unboxTy);

		Final_Cond_Value = builder.CreateCall(unboxFunc, {condResult}, "final_cond_bool");
	} else if (condResult->getType() == i1Type) {
		Final_Cond_Value = condResult;
	} else {
		std::cerr << "Error: Conditional expression must resolve to a boolean type" << std::endl;
		return nullptr;
	}

	llvm::Function* mainFunc = builder.GetInsertBlock()->getParent();

	llvm::BasicBlock* ThenBB = llvm::BasicBlock::Create(ctx, "then", mainFunc);
	llvm::BasicBlock* ElseBB = llvm::BasicBlock::Create(ctx, "else", mainFunc);
	llvm::BasicBlock* IfContBB = llvm::BasicBlock::Create(ctx, "ifcont");

	builder.CreateCondBr(Final_Cond_Value, ThenBB, ElseBB);
	builder.SetInsertPoint(ThenBB);

	for (auto& stmt : thenClauseStmts) {
		stmt.codegen(ctx, builder, module);
	}

	if (!builder.GetInsertBlock()->getTerminator()) {
		builder.CreateBr(IfContBB);
	}

	builder.SetInsertPoint(ElseBB);

	for (auto& stmt : elseClauseStmts) {
		stmt.codegen(ctx, builder, module);
	}

	if (!builder.GetInsertBlock()->getTerminator()) {
		builder.CreateBr(IfContBB);
	}

	mainFunc->insert(mainFunc->end(), IfContBB);
	builder.SetInsertPoint(IfContBB);

	return nullptr;
}

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

llvm::Value* WhileStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
	llvm::IRBuilder<>& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	llvm::Function* mainFunc = builder.GetInsertBlock()->getParent();

	llvm::BasicBlock* LoopCondBB = llvm::BasicBlock::Create(ctx, "loop_cond", mainFunc);
	llvm::BasicBlock* LoopBodyBB = llvm::BasicBlock::Create(ctx, "loop_body", mainFunc);
	llvm::BasicBlock* LoopExitBB = llvm::BasicBlock::Create(ctx, "loop_exit");

	builder.CreateBr(LoopCondBB);
	builder.SetInsertPoint(LoopCondBB);

	llvm::Value* condResult = condition->codegen(ctx, builder, module);
	if (!condResult) return nullptr;

	llvm::Value* Final_Cond_Value = condResult;
	llvm::Type* i1Type = llvm::Type::getInt1Ty(ctx);
	llvm::StructType* dynamicType = getDynamicValueType(ctx);

	if (condResult->getType() == dynamicType) {
		llvm::FunctionType* unboxTy = llvm::FunctionType::get(
			i1Type,
			{dynamicType},
			false
		);

		llvm::FunctionCallee unboxFunc = module.getOrInsertFunction("unbox_bool", unboxTy);

		Final_Cond_Value = builder.CreateCall(unboxFunc, {condResult}, "final_cond_bool");
	} else if (condResult->getType() == i1Type) {
		Final_Cond_Value = condResult;
	} else {
		std::cerr << "Error: Conditional expression must resolve to a boolean type" << std::endl;
		return nullptr;
	}

	builder.CreateCondBr(Final_Cond_Value, LoopBodyBB, LoopExitBB);

	BreakBlockStack.push_back(LoopExitBB);
	ContinueBlockStack.push_back(LoopCondBB);

	builder.SetInsertPoint(LoopBodyBB);

	for (auto& stmt : stmts) {
		stmt.codegen(ctx, builder, module);
	}

	if (!builder.GetInsertBlock()->getTerminator()) {
		builder.CreateBr(LoopCondBB);
	}

	BreakBlockStack.pop_back();
	ContinueBlockStack.pop_back();

	mainFunc->insert(mainFunc->end(), LoopExitBB);
	builder.SetInsertPoint(LoopExitBB);

	return nullptr;
}

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
		if (!argVal) return nullptr;

		llvm::StructType* dynamicType = getDynamicValueType(ctx);

		if (argVal->getType() != dynamicType) {
			llvm::FunctionCallee printfFunc = module.getOrInsertFunction(
				"printf",
				llvm::FunctionType::get(
					llvm::Type::getInt32Ty(ctx),
					llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
					true
				)
			);

			llvm::Value* formatStr = nullptr;
			llvm::Value* argToPrint = argVal;

			if (argVal->getType()->isIntegerTy(64)) {
				formatStr = builder.CreateGlobalString("%lld\n", "intFormat");
			} else if (argVal->getType()->isDoubleTy()) {
				formatStr = builder.CreateGlobalString("%g\n", "floatFormat");
			} else if (argVal->getType()->isPointerTy()) {
				formatStr = builder.CreateGlobalString("%s\n", "strFormat");
			} else if (argVal->getType()->isIntegerTy(1)) {
				llvm::Value* trueStr = builder.CreateGlobalString("true");
				llvm::Value* falseStr = builder.CreateGlobalString("false");
				argToPrint = builder.CreateSelect(argVal, trueStr, falseStr);
				formatStr = builder.CreateGlobalString("%s\n", "boolFormat");
			}

			return builder.CreateCall(printfFunc, {formatStr, argToPrint});
		}

		llvm::FunctionType* printTy = llvm::FunctionType::get(
			llvm::Type::getVoidTy(ctx),
			{dynamicType},
			false
		);

		llvm::FunctionCallee printFunc = module.getOrInsertFunction("print_dynamic_value", printTy);

		return builder.CreateCall(printFunc, {argVal});
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

ContractStmt::ContractStmt(std::string stmtName, std::map<ASTPtr, ASTPtr> literalDic, int line, int col, std::string file)
: ASTNode(line, col, file), name(stmtName), dic(std::move(literalDic)) {}

void ContractStmt::print(int indent) {
	printIndent(indent);
	std::cout << "ContractStmt(name=" << name << ", " << "size=" << dic.size() << ")\n";
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
	llvm::IRBuilder<>& builder = static_cast<llvm::IRBuilder<>&>(builderBase);

	llvm::Function* mainFunc = builder.GetInsertBlock()->getParent();

	if (isBreak) {
		if (BreakBlockStack.empty()) {
			std::cerr << "Error: 'break' statement outside of loop" << std::endl;
			return nullptr;
		}

		builder.CreateBr(BreakBlockStack.back());

		llvm::BasicBlock* nextBB = llvm::BasicBlock::Create(ctx, "unreachable_break", mainFunc);
		builder.SetInsertPoint(nextBB);

		return nullptr;
	}

	if (isContinue) {
		if (ContinueBlockStack.empty()) {
			std::cerr << "Error: 'continue' statement outside of loop" << std::endl;
			return nullptr;
		}

		builder.CreateBr(ContinueBlockStack.back());

		llvm::BasicBlock* nextBB = llvm::BasicBlock::Create(ctx, "unreachable_continue", mainFunc);
		builder.SetInsertPoint(nextBB);

		return nullptr;
	}

	if (expr) {
		return expr->codegen(ctx, builderBase, module);
	}

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
