#include "compiler.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>

void Compiler::compile(Program* program, const std::string& moduleName, const std::string& outputExe) {
	llvm::LLVMContext ctx;
	llvm::IRBuilder<> builder(ctx);
	llvm::Module module(moduleName, ctx);

	program->codegen(ctx, builder, module);

	std::error_code EC;
	llvm::raw_fd_ostream irFile("../program.ll", EC, llvm::sys::fs::OF_None);
	module.print(irFile, nullptr);
	irFile.close();

	system(("cd .. && clang program.ll -o " + outputExe).c_str());
	system("cd .. && rm program.ll");
	
	std::cout << "Executable generated: " << outputExe << "\n";
}