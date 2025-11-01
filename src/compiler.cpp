#include "compiler.hpp"

Compiler::Compiler(std::string moduleName)
: module(std::make_unique<llvm::Module>(moduleName, context)), builder(context) {}

void Compiler::generate(Program* program) {
	llvm::FunctionType* mainType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false);
	llvm::Function* mainFunc = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());
	llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
	builder.SetInsertPoint(entry);

	for (auto& stmt : program->statements) {
		if (stmt.expr)
			genExpr(stmt.expr.get());
	}

	builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));

	llvm::verifyFunction(*mainFunc);
	llvm::verifyModule(*module);
}

llvm::Value* Compiler::genExpr(ASTNode* node) {
	if (auto call = dynamic_cast<FunctionCall*>(node)) {
		if (call->name == "print") {
			if (call->params.empty())
				return nullptr;
			
			llvm::Value* arg = genExpr(call->params[0].get());

			llvm::FunctionType* printfType = llvm::FunctionType::get(
				llvm::Type::getInt32Ty(context),
				llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
				true
			);

			llvm::FunctionCallee printfFunc = module->getOrInsertFunction("printf", printfType);

			llvm::Value* fmt = nullptr;

			if (arg->getType()->isIntegerTy())
				fmt = builder.CreateGlobalStringPtr("%d\n");
			else
				fmt = builder.CreateGlobalStringPtr("%s\n");

			return builder.CreateCall(printfFunc, {fmt, arg});
		}
	}

	if (auto str = dynamic_cast<StrLiteral*>(node)) {
		return builder.CreateGlobalStringPtr(str->value);
	}

	if (auto i = dynamic_cast<IntLiteral*>(node)) {
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i->value);
	}

	return nullptr;
}