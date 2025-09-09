#pragma once

class ASTNode {
	public:
		virtual void print(int indent);
		
		virtual ~ASTNode() {}
};

class NoOp : public ASTNode {
	public:
		void print(int indent) override;
};