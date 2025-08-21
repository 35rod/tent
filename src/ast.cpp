#include <cstdio>
#include "ast.hpp"
#include "misc.hpp"

static void printIndent(int indent) {
    printf("%*s", indent, " ");
}

void ASTNode::print(int indent) {
    printIndent(indent);
    std::cout << "ASTNode()" << std::endl;
}

void NoOp::print(int ident) {
    printIndent(ident);
    std::cout << "NoOp()" << std::endl;
}

IntLiteral::IntLiteral(nl_int_t literalValue) : ASTNode(), value(literalValue) {}

void IntLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "IntLiteral(value=" << value << ")\n";
}

FloatLiteral::FloatLiteral(nl_dec_t literalValue) : ASTNode(), value(literalValue) {}

void FloatLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "FloatLiteral(value=" << value << ")\n";
}

StrLiteral::StrLiteral(std::string literalValue) : ASTNode(), value(literalValue) {}

void StrLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "StringLiteral(value='" << value << "')\n";
}

BoolLiteral::BoolLiteral(bool literalValue) : ASTNode(), value(literalValue) {}

void BoolLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "BoolLiteral(value=" << value << ")\n";
}

Variable::Variable(std::string varName, ASTPtr varValue, std::string varContext) : ASTNode(), name(varName), value(std::move(varValue)), context(varContext) {}

void Variable::print(int indent) {
    printIndent(indent);
    std::cout << "Variable(name=" << name << ", context=" << context << ")\n";

    if (value) {
        value->print(indent);
    }
}

BinaryOp::BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight) :
ASTNode(), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {}

void BinaryOp::print(int indent) {
    printIndent(indent);
    std::cout << "BinaryOp(op=\"" << op << "\")" << std::endl;
    printIndent(indent);
    std::cout << " Left:" << std::endl;
    
    if (left) {
        left->print(indent+2);
    } else {
        printIndent(indent+2);
        std::cout << "nullptr" << std::endl;
    }

    printIndent(indent);
    std::cout << " Right:" << std::endl;
    
    if (right) {
        right->print(indent+2);
    } else {
        printIndent(indent+2);
        std::cout << "nullptr" << std::endl;
    }
}

IfLiteral::IfLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts) : condition(std::move(literalCondition)), stmts(std::move(literalStmts)) {}

void IfLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "IfLiteral(statements=" << stmts.size() << ")\n";
    printIndent(indent+2);
    std::cout << "Condition:\n";
    condition->print(indent+4);
    printIndent(indent+2);
    std::cout << "Statements:\n";

    for (ExpressionStmt& stmt : stmts) {
        stmt.print(indent+4);
    }
}

WhileLiteral::WhileLiteral(ASTPtr literalCondition, std::vector<ExpressionStmt> literalStmts) : condition(std::move(literalCondition)), stmts(std::move(literalStmts)) {}

void WhileLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "WhileLiteral(statements=" << stmts.size() << ")\n";
    printIndent(indent+2);
    std::cout << "Condition:\n";
    condition->print(indent+4);
    printIndent(indent+2);
    std::cout << "Statements:\n";

    for (ExpressionStmt& stmt : stmts) {
        stmt.print(indent+4);
    }
}

FunctionCall::FunctionCall(std::string callName, std::vector<ASTPtr> callParams)
: name(callName), params(std::move(callParams)) {}

void FunctionCall::print(int indent) {
    printIndent(indent);
    std::cout << "FunctionCall(name=" << name << ", parameters=" << params.size() << ")\n";
    printIndent(indent+2);
    std::cout << "Parameters:\n";

    for (const auto& param : params) {
        param->print(indent+4);
    }
}

FunctionLiteral::FunctionLiteral(std::string literalName, std::vector<ASTPtr> literalParams, std::vector<ExpressionStmt> literalStmts)
: name(literalName), params(std::move(literalParams)), stmts(std::move(literalStmts)) {}

void FunctionLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "FunctionLiteral(name=" << name << ", statements=" << stmts.size() << ", parameters=" << params.size() << ")\n";
    printIndent(indent+2);
    std::cout << "Parameters:\n";

    for (const auto& param : params) {
        param->print(indent+4);
    }

    printIndent(indent+2);
    std::cout << "Statements:\n";

    for (ExpressionStmt& stmt: stmts) {
        stmt.print(indent+4);
    }
}

ExpressionStmt::ExpressionStmt(ASTPtr stmtExpr, bool stmtNoOp, bool exprIsBreak) : ASTNode(), expr(std::move(stmtExpr)), noOp(stmtNoOp), isBreak(exprIsBreak) {}

void ExpressionStmt::print(int indent) {
    printIndent(indent);
    std::cout << "ExpressionStmt()" << std::endl;
    
    if (expr) {
        expr->print(indent+2);
    } else {
        printIndent(indent+2);
        std::cout << "nullptr" << std::endl;
    }
}

Program::Program(std::vector<ExpressionStmt>&& programStatements) : ASTNode(), statements(std::move(programStatements)) {}

void Program::print(int indent) {
    printIndent(indent);
    std::cout << "Program(statements=" << statements.size() << ")" << std::endl;

    for (ExpressionStmt& stmt : statements) {
        stmt.print(indent+2);
    }
}
