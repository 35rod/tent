#pragma once

#include <iostream>

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

		virtual void print(int indent) {
			printIndent(indent);
			std::cout << "ASTNode()" << std::endl;
		}

		friend class Evaluator;
		
		virtual ~ASTNode() {}
};

class NullLiteral : public ASTNode {
    public:
        void print(int indent) override {
            printIndent(indent);
            std::cout << "NoOp()\n";
        }
};

class NoOp : public ASTNode {
	public:
		void print(int indent) override {
			printIndent(indent);
			std::cout << "NoOp()\n";
		}
};