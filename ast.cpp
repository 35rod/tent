#include "ast.hpp"

void ASTNode::print() {
    std::cout << "ASTNode()" << std::endl;
}

void NoOp::print() {
    std::cout << "NoOp()" << std::endl;
}

IntLiteral::IntLiteral(int literalValue) : ASTNode(), value(literalValue) {}

void IntLiteral::print() {
    std::cout << "IntLiteral(" << value << ")\n";
}

BinaryOp::BinaryOp(std::string opOp, ASTPtr opLeft, ASTPtr opRight) :
ASTNode(), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {}

void BinaryOp::print() {
    std::cout << "BinaryOp(" << op << ")\n";
}

ExpressionStmt::ExpressionStmt(ASTPtr stmtExpr) : ASTNode(), expr(std::move(stmtExpr)) {}

void ExpressionStmt::print() {
    std::cout << "ExpressionStmt()" << std::endl;
}

Program::Program(std::vector<ExpressionStmt>&& programStatements) : ASTNode(), statements(std::move(programStatements)) {}

void Program::print() {
    std::cout << "Program()" << std::endl;
}