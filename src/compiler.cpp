#include "compiler.hpp"

#include <cstdlib>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>

#include "args.hpp"

void Compiler::compile(const Config& config, Program* program, const std::string& outputExe, const std::string& moduleName) {
	llvm::LLVMContext ctx;
	llvm::IRBuilder<> builder(ctx);
	llvm::Module module(moduleName, ctx);

	{
		llvm::FunctionType* printfType = llvm::FunctionType::get(
			llvm::Type::getInt32Ty(ctx),
			{ llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0) },
			true
		);

		llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
	}

	program->codegen(ctx, builder, module);

	std::error_code EC;
	const std::string LlFileName = defaultOutputExeName + ".ll";
	llvm::raw_fd_ostream irFile(LlFileName, EC, llvm::sys::fs::OF_None);
	module.print(irFile, nullptr);
	irFile.close();

	const std::string compileCommand = config.system_compiler + " " + LlFileName + " -o " + outputExe;
	if (config.is_flag_set(DEBUG))
		std::cerr << "compile command: " << compileCommand << std::endl;

	int code = std::system(compileCommand.c_str());
	if (code == 0 && !config.is_flag_set(SAVE_TEMPS))
		std::system(("rm " + LlFileName).c_str());
	
	if (code == 0)
		std::cout << "Executable generated: " << outputExe << "\n";
	else
		std::cerr << "Compile command failed with return code " << code << std::endl;
}
