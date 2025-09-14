#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <memory>
#include "opcodes.hpp"
#include "types.hpp"

enum class NodeType : uint8_t {
	Program = 0xC0,
	ExpressionStmt,
	IntLiteral,
	FloatLiteral,
	StrLiteral,
	BoolLiteral,
	VecLiteral,
	Variable,
	UnaryOp,
	BinaryOp,
	IfLiteral,
	WhileLiteral,
	ForLiteral,
	FunctionCall,
	ReturnLiteral,
	FunctionLiteral,
	NoOp,
	ASTNode
};

using ASTPtr = std::unique_ptr<ASTNode>;

class IntLiteral : public ASTNode {
	public:
		nl_int_t value;

		void print(int indent) override;

		IntLiteral(nl_int_t literalValue);
};

class FloatLiteral : public ASTNode {
	public:
		nl_dec_t value;

		void print(int indent) override;

		FloatLiteral(nl_dec_t literalValue);
};

class StrLiteral : public ASTNode {
	public:
		std::string value;
		
		void print(int indent) override;

		StrLiteral(std::string literalValue);
};

class BoolLiteral : public ASTNode {
	public:
		nl_bool_t value;
		
		void print(int indent) override;

		BoolLiteral(nl_bool_t literalValue);
};

class VecLiteral : public ASTNode {
	public:
		std::vector<ASTPtr> elems;

		void print(int indent) override;

		VecLiteral(std::vector<ASTPtr> literalValue);
};

class Variable : public ASTNode {
	public:
		std::string name;
		ASTPtr value;

		void print(int indent) override;

		Variable(std::string varName, ASTPtr varValue=nullptr);
};

class UnaryOp : public ASTNode {
	public:
		TokenType op;
		ASTPtr operand;

		void print(int indent) override;

		UnaryOp(TokenType opOp, ASTPtr opOperand);
};

class BinaryOp : public ASTNode {
	public:
		TokenType op;
		ASTPtr left;
		ASTPtr right;

		void print(int indent) override;

		BinaryOp(TokenType opOp, ASTPtr opLeft, ASTPtr opRight);
};

class ExpressionStmt : public ASTNode {
	public:
		ASTPtr expr;
		bool noOp;
		bool isBreak;
		bool isContinue;

		void print(int indent) override;

		ExpressionStmt(
                ASTPtr expr,
                bool exprNoOp=false,
                bool exprIsBreak=false,
                bool exprIsContinue=false);
};

class IfLiteral : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> thenClauseStmts;
		std::vector<ExpressionStmt> elseClauseStmts;

		void print(int indent) override;

		IfLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> thenStmts, std::vector<ExpressionStmt> elseStmts={});
};

class WhileLiteral : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		WhileLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts);
};

class ForLiteral : public ASTNode {
	public:
		ASTPtr var;
		ASTPtr iter;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		ForLiteral(ASTPtr literalVar, ASTPtr literalIter, std::vector<ExpressionStmt> literalStmts);
};

class FunctionCall : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;

		void print(int indent) override;

		FunctionCall(std::string callName, std::vector<ASTPtr> callParams);
};

class ReturnLiteral : public ASTNode {
	public:
		ASTPtr value;

		void print(int indent) override;

		ReturnLiteral(ASTPtr literalValue);
};

class FunctionLiteral : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;
		ASTPtr returnValue;

		void print(int indent) override;

		FunctionLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts, ASTPtr literalReturnValue=nullptr);
};

class ClassLiteral : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		ClassLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts);
};

class Program : public ASTNode {
	public:
		std::vector<ExpressionStmt> statements;

		void print(int indent) override;

		Program(std::vector<ExpressionStmt>&& programStatements);
};
