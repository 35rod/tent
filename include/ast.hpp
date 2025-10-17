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
		tn_int_t value;

		void print(int indent) override;

		IntLiteral(tn_int_t literalValue, int line, int col, std::string file);
};

class FloatLiteral : public ASTNode {
	public:
		tn_dec_t value;

		void print(int indent) override;

		FloatLiteral(tn_dec_t literalValue, int line, int col, std::string file);
};

class StrLiteral : public ASTNode {
	public:
		std::string value;
		
		void print(int indent) override;

		StrLiteral(std::string literalValue, int line, int col, std::string file);
};

class BoolLiteral : public ASTNode {
	public:
		tn_bool_t value;
		
		void print(int indent) override;

		BoolLiteral(tn_bool_t literalValue, int line, int col, std::string file);
};

class VecLiteral : public ASTNode {
	public:
		std::vector<ASTPtr> elems;

		void print(int indent) override;

		VecLiteral(std::vector<ASTPtr> literalValue, int line, int col, std::string file);
};

class DicLiteral : public ASTNode {
    public:
        std::map<ASTPtr, ASTPtr> dic;

        void print(int indent) override;

        DicLiteral(std::map<ASTPtr, ASTPtr> literalDic, int line, int col, std::string file);
};

class TypeInt : public ASTNode {
	public:
		void print(int indent) override;

		TypeInt(int line, int col, std::string file);
};

class TypeFloat : public ASTNode {
	public:
		void print(int indent) override;

		TypeFloat(int line, int col, std::string file);
};

class TypeStr : public ASTNode {
	public:
		void print(int indent) override;

		TypeStr(int line, int col, std::string file);
};

class TypeBool : public ASTNode {
	public:
		void print(int indent) override;

		TypeBool(int line, int col, std::string file);
};

class TypeVec : public ASTNode {
	public:
		void print(int indent) override;

		TypeVec(int line, int col, std::string file);
};

class TypeDic : public ASTNode {
    public:
        void print(int indent) override;

        TypeDic(int line, int col, std::string file);
};

class Variable : public ASTNode {
	public:
		std::string name;
		ASTPtr value;

		void print(int indent) override;

		Variable(std::string varName, ASTPtr varValue=nullptr, int line=-1, int col=-1, std::string file="");
};

class UnaryOp : public ASTNode {
	public:
		TokenType op;
		ASTPtr operand;

		void print(int indent) override;

		UnaryOp(TokenType opOp, ASTPtr opOperand, int line, int col, std::string file);
};

class BinaryOp : public ASTNode {
	public:
		TokenType op;
		ASTPtr left;
		ASTPtr right;

		void print(int indent) override;

		BinaryOp(TokenType opOp, ASTPtr opLeft, ASTPtr opRight, int line, int col, std::string file);
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
			bool exprIsContinue=false,
			int line = -1,
			int col = -1,
			std::string file = ""
		);
};

class IfStmt : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> thenClauseStmts;
		std::vector<ExpressionStmt> elseClauseStmts;

		void print(int indent) override;

		IfStmt(
			ASTPtr stmtCondition, 
			std::vector<ExpressionStmt> thenStmts, 
			std::vector<ExpressionStmt> elseStmts={},
			int line=-1,
			int col=-1,
			std::string file=""
		);
};

class WhileStmt : public ASTNode {
	public:
		ASTPtr condition;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		WhileStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> stmtStmts, int line, int col, std::string file);
};

class ForStmt : public ASTNode {
	public:
		std::string var;
		ASTPtr iter;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		ForStmt(
			std::string stmtVar, 
			ASTPtr stmtIter, 
			std::vector<ExpressionStmt> stmtStmts,
			int line,
			int col,
			std::string file
		);
};

class FunctionCall : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;

		void print(int indent) override;

		FunctionCall(std::string callName, std::vector<ASTPtr> callParams, int line, int col, std::string file);
};

class ReturnStmt : public ASTNode {
	public:
		ASTPtr value;

		void print(int indent) override;

		ReturnStmt(ASTPtr stmtValue, int line, int col, std::string file);
};

class FunctionStmt : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;
		ASTPtr returnValue;

		void print(int indent) override;

		FunctionStmt(
			std::string stmtName, 
			std::vector<ASTPtr> stmtParams, 
			std::vector<ExpressionStmt> stmtStmts, 
			ASTPtr stmtReturnValue=nullptr,
			int line=-1,
			int col=-1,
			std::string file=""
		);
};

class InlineStmt : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;
		ASTPtr returnValue;

		void print(int indent) override;

		InlineStmt(
			std::string stmtName, 
			std::vector<ASTPtr> stmtParams, 
			std::vector<ExpressionStmt> stmtStmts, 
			ASTPtr stmtReturnValue=nullptr,
			int line=-1,
			int col=-1,
			std::string file=""
		);
};

class ClassStmt : public ASTNode {
	public:
		std::string name;
		std::vector<ASTPtr> params;
		std::vector<ExpressionStmt> stmts;

		void print(int indent) override;

		ClassStmt(
			std::string literalName, 
			std::vector<ASTPtr> literalParams, 
			std::vector<ExpressionStmt> literalStmts,
			int line,
			int col,
			std::string file
		);
};

class Program : public ASTNode {
	public:
		std::vector<ExpressionStmt> statements;

		void print(int indent) override;

		Program(std::vector<ExpressionStmt>&& programStatements, int line, int col, std::string file);
};
