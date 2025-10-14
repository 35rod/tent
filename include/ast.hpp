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
	TypeInt,
	TypeFloat,
	TypeStr,
	TypeBool,
	TypeVec,
	Variable,
	UnaryOp,
	BinaryOp,
	IfStmt,
	WhileStmt,
	ForStmt,
	FunctionCall,
	ReturnStmt,
	FunctionStmt,
	InlineStmt,
	ClassStmt,
	NullLiteral,
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

class DicLiteral : public ASTNode {
    public:
        std::map<ASTPtr, ASTPtr> dic;

        void print(int indent) override;

        DicLiteral(std::map<ASTPtr, ASTPtr> literalDic);
};

class TypeInt : public ASTNode {
	public:
		void print(int indent) override;

		TypeInt();
};

class TypeFloat : public ASTNode {
	public:
		void print(int indent) override;

		TypeFloat();
};

class TypeStr : public ASTNode {
	public:
		void print(int indent) override;

		TypeStr();
};

class TypeBool : public ASTNode {
	public:
		void print(int indent) override;

		TypeBool();
};

class TypeVec : public ASTNode {
	public:
		void print(int indent) override;

		TypeVec();
};

class TypeDic : public ASTNode {
    public:
        void print(int indent) override;

        TypeDic();
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

class IfStmt : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> thenClauseStmts;
		std::vector<ExpressionStmt> elseClauseStmts;

		void print(int indent) override;

		IfStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> thenStmts, std::vector<ExpressionStmt> elseStmts={});
};

class WhileStmt : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		WhileStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> stmtStmts);
};

class ForStmt : public ASTNode {
	public:
		std::string var;
		ASTPtr iter;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		ForStmt(std::string stmtVar, ASTPtr stmtIter, std::vector<ExpressionStmt> stmtStmts);
};

class FunctionCall : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;

		void print(int indent) override;

		FunctionCall(std::string callName, std::vector<ASTPtr> callParams);
};

class ReturnStmt : public ASTNode {
	public:
		ASTPtr value;

		void print(int indent) override;

		ReturnStmt(ASTPtr stmtValue);
};

class FunctionStmt : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;
		ASTPtr returnValue;

		void print(int indent) override;

		FunctionStmt(std::string stmtName, std::vector<ASTPtr> stmtParams, std::vector<ExpressionStmt> stmtStmts, ASTPtr stmtReturnValue=nullptr);
};

class InlineStmt : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;
		ASTPtr returnValue;

		void print(int indent) override;

		InlineStmt(std::string stmtName, std::vector<ASTPtr> stmtParams, std::vector<ExpressionStmt> stmtStmts, ASTPtr stmtReturnValue=nullptr);
};

class ClassStmt : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		ClassStmt(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts);
};

class Program : public ASTNode {
	public:
		std::vector<ExpressionStmt> statements;

		void print(int indent) override;

		Program(std::vector<ExpressionStmt>&& programStatements);
};
