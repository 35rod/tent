#pragma once

#include "ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

class Compiler {
	public:
		llvm::LLVMContext context;
		std::unique_ptr<llvm::Module> module;
		llvm::IRBuilder<> builder;

		Compiler(std::string moduleName);

		void generate(Program* program);
		llvm::Value* genExpr(ASTNode* node);
};