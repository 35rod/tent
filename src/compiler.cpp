#include "compiler.hpp"

#include <cstdlib>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>

#include "args.hpp"

std::string SYSTEM_COMPILER = "clang";

void Compiler::compile(Program* program, const std::string& outputExe, const std::string& moduleName) {
	llvm::LLVMContext ctx;
	llvm::IRBuilder<> builder(ctx);
	llvm::Module module(moduleName, ctx);

	program->codegen(ctx, builder, module);
	
	std::error_code EC;
	const std::string LlFileName = defaultOutputExeName + ".ll";
	llvm::raw_fd_ostream irFile(LlFileName, EC, llvm::sys::fs::OF_None);
	module.print(irFile, nullptr);
	irFile.close();

	const std::string compileCommand = SYSTEM_COMPILER + " " + LlFileName + " -o " + outputExe;
	if (IS_FLAG_SET(DEBUG))
		std::cerr << "compile command: " << compileCommand << std::endl;

	int code = std::system(compileCommand.c_str());
	if (code == 0 && !IS_FLAG_SET(SAVE_TEMPS))
		std::system(("rm " + LlFileName).c_str());
	
	if (code == 0)
		std::cout << "Executable generated: " << outputExe << "\n";
	else
		std::cerr << "Compile command failed with return code " << code << std::endl;
}
