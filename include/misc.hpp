#pragma once

#include <iostream>

namespace llvm {
	class LLVMContext;
	class IRBuilderBase;
	class Module;
	class Value;
	class Type;
}

inline void printIndent(int indent) {
	printf("%*s", indent, " ");
}

struct CValue {
	enum class Kind {
		Int,
		Float,
		Bool,
		String,
		Dynamic,
		Void
	};

	llvm::Value* value;
	llvm::Type* type;
	Kind kind;

	CValue() : value(nullptr), type(nullptr), kind(Kind::Void) {}
	CValue(llvm::Value* v, llvm::Type* t, Kind k) : value(v), type(t), kind(k) {}

	bool isDynamic() const { return kind == Kind::Dynamic; }
	bool isPrimitive() const { return kind == Kind::Int || kind == Kind::Float || kind == Kind::Bool; }
};

class ASTNode {
	protected:
		int lineNo;
		int colNo;
		std::string filename;
	public:
		ASTNode(int line = -1, int col = -1, std::string file = "")
		: lineNo(line), colNo(col), filename(file) {}

		virtual CValue codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) = 0;

		virtual void print(int indent) {
			printIndent(indent);
			std::cout << "ASTNode()" << std::endl;
		}

		friend class Evaluator;
		
		virtual ~ASTNode() = default;
};

class NullLiteral : public ASTNode {
    public:
		CValue codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) override { return CValue(); }

        void print(int indent) override {
            printIndent(indent);
            std::cout << "NoOp()\n";
        }
};

class NoOp : public ASTNode {
	public:
		CValue codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) override { return CValue(); }

		void print(int indent) override {
			printIndent(indent);
			std::cout << "NoOp()\n";
		}
};