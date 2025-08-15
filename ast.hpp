#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class ASTNode {
    public:
        virtual void print();
};

using ASTPtr = std::unique_ptr<ASTNode>;

class NoOp : public ASTNode {
    public:
        void print() override;
};

class IntLiteral : public ASTNode {
    public:
        int value;

        void print() override;

        IntLiteral(int literalValue);
};

class BinaryOp : public ASTNode {
    public:
        std::string op;
        ASTPtr left;
        ASTPtr right;

        void print() override;

        BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight);
};

class ExpressionStmt : public ASTNode {
    public:
        ASTPtr expr;
        bool noOp;

        void print() override;

        ExpressionStmt(ASTPtr expr, bool exprNoOp=false);
};

class Program : public ASTNode {
    public:
        std::vector<ExpressionStmt> statements;

        void print() override;

        Program(std::vector<ExpressionStmt>&& programStatements);
};
