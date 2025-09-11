#pragma once

#include <iostream>

inline void printIndent(int indent) {
	printf("%*s", indent, " ");
}

class ASTNode {
	public:
		virtual void print(int indent) {
			printIndent(indent);
			std::cout << "ASTNode()" << std::endl;
		}
		
		virtual ~ASTNode() {}
};

class NoOp : public ASTNode {
	public:
		void print(int indent) override {
			printIndent(indent);
			std::cout << "NoOp()\n";
		}
};