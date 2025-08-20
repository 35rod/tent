#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "misc.hpp"

class ASTNode {
    public:
        virtual void print(int indent);
        virtual ~ASTNode() {}
};

using ASTPtr = std::unique_ptr<ASTNode>;

class NoOp : public ASTNode {
    public:
        void print(int indent) override;
};

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
        bool value;

        void print(int indent) override;

        BoolLiteral(bool literalValue);
};

class Variable : public ASTNode {
    public:
        std::string name;
        ASTPtr value;
        std::string context;

        void print(int indent) override;

        Variable(std::string varName, ASTPtr varValue=nullptr, std::string varContext="EXPR");
};

class BinaryOp : public ASTNode {
    public:
        std::string op;
        ASTPtr left;
        ASTPtr right;

        void print(int indent) override;

        BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight);
};

class ExpressionStmt : public ASTNode {
    public:
        ASTPtr expr;
        bool noOp;
        bool isBreak;

        void print(int indent) override;

        ExpressionStmt(ASTPtr expr, bool exprNoOp=false, bool exprIsBreak=false);
};

class IfLiteral : public ASTNode {
    public:
        ASTPtr condition;
        std::vector<ExpressionStmt> stmts;

        void print(int indent) override;

        IfLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts);
};

class WhileLiteral : public ASTNode {
    public:
        ASTPtr condition;
        std::vector<ExpressionStmt> stmts;

        void print(int indent) override;

        WhileLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts);
};

class FunctionCall : public ASTNode {
    public:
        std::string name;
        std::vector<ASTPtr> params;

        void print(int indent) override;

        FunctionCall(std::string callName, std::vector<ASTPtr> callParams);
};

class FunctionLiteral : public ASTNode {
    public:
        std::string name;
        std::vector<ASTPtr> params;
        std::vector<ExpressionStmt> stmts;

        void print(int indent) override;

        FunctionLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts);
};

class Program : public ASTNode {
    public:
        std::vector<ExpressionStmt> statements;

        void print(int indent) override;

        Program(std::vector<ExpressionStmt>&& programStatements);
};
