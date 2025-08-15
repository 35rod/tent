#include "ast.hpp"

static void printIndent(int indent) {
    for (int i = 0; i < indent; i++) std::cout << ' ';
}

void ASTNode::print(int indent) {
    printIndent(indent);
    std::cout << "ASTNode()" << std::endl;
}

void NoOp::print(int ident) {
    printIndent(ident);
    std::cout << "NoOp()" << std::endl;
}

IntLiteral::IntLiteral(int literalValue) : ASTNode(), value(literalValue) {}

void IntLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "IntLiteral(value=" << value << ")\n";
}

FloatLiteral::FloatLiteral(float literalValue) : ASTNode(), value(literalValue) {}

void FloatLiteral::print(int indent) {
    printIndent(indent);
    std::cout << "FloatLiteral(value=" << value << ")\n";
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

ExpressionStmt::ExpressionStmt(ASTPtr stmtExpr, bool stmtNoOp) : ASTNode(), expr(std::move(stmtExpr)), noOp(stmtNoOp) {}

void ExpressionStmt::print(int indent) {
    printIndent(indent);
    std::cout << "ExpressionStmt(noOp=" << (noOp ? "true": "false") << ")" << std::endl;
    
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