#pragma once

#include <iostream>

namespace llvm {
	class LLVMContext;
	class IRBuilderBase;
	class Module;
	class Value;
}

inline void printIndent(int indent) {
	printf("%*s", indent, " ");
}

class ASTNode {
	protected:
		int lineNo;
		int colNo;
		std::string filename;
	public:
		ASTNode(int line = -1, int col = -1, std::string file = "")
		: lineNo(line), colNo(col), filename(file) {}

		virtual llvm::Value* codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) = 0;

		virtual void print(int indent) {
			printIndent(indent);
			std::cout << "ASTNode()" << std::endl;
		}

		friend class Evaluator;
		
		virtual ~ASTNode() = default;
};

class NullLiteral : public ASTNode {
    public:
		llvm::Value* codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) override { return nullptr; }

        void print(int indent) override {
            printIndent(indent);
            std::cout << "NoOp()\n";
        }
};

class NoOp : public ASTNode {
	public:
		llvm::Value* codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) override { return nullptr; }

		void print(int indent) override {
			printIndent(indent);
			std::cout << "NoOp()\n";
		}
};