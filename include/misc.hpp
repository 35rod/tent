#pragma once

#include <iostream>
#include "span.hpp"

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
		Span span;
	public:
		ASTNode(Span s) : span(s) {}

		virtual llvm::Value* codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) { return nullptr; }

		virtual void print(int indent) {
			printIndent(indent);
			std::cout << "ASTNode()" << std::endl;
		}

		friend class Evaluator;

		virtual ~ASTNode() = default;
};

class NullLiteral : public ASTNode {
    public:
		NullLiteral() : ASTNode(Span()) {}

		llvm::Value* codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) override { return nullptr; }

        void print(int indent) override {
            printIndent(indent);
            std::cout << "NoOp()\n";
        }
};

class NoOp : public ASTNode {
	public:
		NoOp() : ASTNode(Span()) {}

		llvm::Value* codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) override { return nullptr; }

		void print(int indent) override {
			printIndent(indent);
			std::cout << "NoOp()\n";
		}
};
