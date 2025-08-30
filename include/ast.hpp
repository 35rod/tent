#pragma once

#include <string>
#include <vector>
#include <memory>
#include "misc.hpp"

class ASTNode {
	public:
		virtual void print(int indent);
		virtual void serialize(std::ostream& out, int indent);
		virtual ~ASTNode() {}
};

using ASTPtr = std::unique_ptr<ASTNode>;

class NoOp : public ASTNode {
	public:
		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;
};

class IntLiteral : public ASTNode {
	public:
		nl_int_t value;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		IntLiteral(nl_int_t literalValue);
};

class FloatLiteral : public ASTNode {
	public:
		nl_dec_t value;

		static std::string to_str(nl_dec_t val, int prec=6);
		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		FloatLiteral(nl_dec_t literalValue);
};

class StrLiteral : public ASTNode {
	public:
		std::string value;

		static std::string to_str(std::string val);
		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		StrLiteral(std::string literalValue);
};

class BoolLiteral : public ASTNode {
	public:
		nl_bool_t value;

		static std::string to_str(nl_bool_t val);
		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		BoolLiteral(bool literalValue);
};

class VecLiteral : public ASTNode {
	public:
		std::vector<ASTPtr> elems;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		VecLiteral(std::vector<ASTPtr> literalValue);
};

class VecValue: public ASTNode {
	public:
		std::vector<NonVecEvalExpr> elems;

		static std::string to_str(std::vector<NonVecEvalExpr> val);
		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		VecValue(std::vector<NonVecEvalExpr> literalValue);
};

class Variable : public ASTNode {
	public:
		std::string name;
		ASTPtr value;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		Variable(std::string varName, ASTPtr varValue=nullptr);
};

class UnaryOp : public ASTNode {
	public:
		std::string op;
		ASTPtr operand;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		UnaryOp(std::string opOp, ASTPtr opOperand);
};

class BinaryOp : public ASTNode {
	public:
		std::string op;
		ASTPtr left;
		ASTPtr right;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight);
};

class ExpressionStmt : public ASTNode {
	public:
		ASTPtr expr;
		bool noOp;
		bool isBreak;
		bool isContinue;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

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
		void serialize(std::ostream& out, int indent) override;

		IfLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> thenStmts, std::vector<ExpressionStmt> elseStmts={});
};

class WhileLiteral : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		WhileLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts);
};

class FunctionCall : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		FunctionCall(std::string callName, std::vector<ASTPtr> callParams);
};

class ReturnLiteral : public ASTNode {
	public:
		ASTPtr value;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		ReturnLiteral(ASTPtr literalValue);
};

class FunctionLiteral : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;
		ASTPtr returnValue;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		FunctionLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts, ASTPtr literalReturnValue=nullptr);
};

class Program : public ASTNode {
	public:
		std::vector<ExpressionStmt> statements;

		void print(int indent) override;
		void serialize(std::ostream& out, int indent) override;

		Program(std::vector<ExpressionStmt>&& programStatements);
};
