#pragma once

#include <iostream>
#include "span.hpp"

inline void printIndent(int indent) {
	printf("%*s", indent, " ");
}

class ASTNode {
	protected:
		Span span;
	public:
		ASTNode(Span s) : span(s) {}

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

        void print(int indent) override {
            printIndent(indent);
            std::cout << "NoOp()\n";
        }
};

class NoOp : public ASTNode {
	public:
		NoOp() : ASTNode(Span()) {}

		void print(int indent) override {
			printIndent(indent);
			std::cout << "NoOp()\n";
		}
};
