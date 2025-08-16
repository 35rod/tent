#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

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
        int value;

        void print(int indent) override;

        IntLiteral(int literalValue);
};

class FloatLiteral : public ASTNode {
    public:
        float value;

        void print(int indent) override;

        FloatLiteral(float literalValue);
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

        void print(int indent) override;

        ExpressionStmt(ASTPtr expr, bool exprNoOp=false);
};

class Program : public ASTNode {
    public:
        std::vector<ExpressionStmt> statements;

        void print(int indent) override;

        Program(std::vector<ExpressionStmt>&& programStatements);
};