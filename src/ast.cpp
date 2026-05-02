#include "ast.hpp"

#include <iostream>

#include "misc.hpp"
#include "visitor.hpp"

Value NoOp::accept(ASTVisitor &v) { return v.visit(*this); }

Value IntLiteral::accept(ASTVisitor &v) { return v.visit(*this); }
IntLiteral::IntLiteral(tn_int_t literalValue, Span s)
    : ASTNode(s), value(literalValue) {}

void IntLiteral::print(int indent) {
  printIndent(indent);
  std::cout << "IntLiteral(value=" << value << ")\n";
}

Value FloatLiteral::accept(ASTVisitor &v) { return v.visit(*this); }
FloatLiteral::FloatLiteral(tn_dec_t literalValue, Span s)
    : ASTNode(s), value(literalValue) {}

void FloatLiteral::print(int indent) {
  printIndent(indent);
  std::cout << "FloatLiteral(value=" << value << ")\n";
}

Value StrLiteral::accept(ASTVisitor &v) { return v.visit(*this); }
StrLiteral::StrLiteral(std::string literalValue, Span s)
    : ASTNode(s), value(literalValue) {}

void StrLiteral::print(int indent) {
  printIndent(indent);
  std::cout << "StringLiteral(value=" << value << ")\n";
}

Value BoolLiteral::accept(ASTVisitor &v) { return v.visit(*this); }
BoolLiteral::BoolLiteral(tn_bool_t literalValue, Span s)
    : ASTNode(s), value(literalValue) {}

void BoolLiteral::print(int indent) {
  printIndent(indent);
  std::cout << "BoolLiteral(value=" << (value ? "true" : "false") << ")\n";
}

Value VecLiteral::accept(ASTVisitor &v) { return v.visit(*this); }
VecLiteral::VecLiteral(std::vector<ASTPtr> literalValue, Span s)
    : ASTNode(s), elems(std::move(literalValue)) {}

void VecLiteral::print(int indent) {
  printIndent(indent);
  std::cout << "VecLiteral(size=" << elems.size() << ")\n";
}

Value DicLiteral::accept(ASTVisitor &v) { return v.visit(*this); }
DicLiteral::DicLiteral(std::map<ASTPtr, ASTPtr> literalDic, Span s)
    : ASTNode(s), dic(std::move(literalDic)) {}

void DicLiteral::print(int indent) {
  printIndent(indent);
  std::cout << "DicLiteral(size=" << dic.size() << ")\n";
}

Value TypeInt::accept(ASTVisitor &v) { return v.visit(*this); }
TypeInt::TypeInt(Span s) : ASTNode(s) {}

void TypeInt::print(int indent) {
  printIndent(indent);
  std::cout << "TypeInt()" << std::endl;
}

Value TypeFloat::accept(ASTVisitor &v) { return v.visit(*this); }
TypeFloat::TypeFloat(Span s) : ASTNode(s) {}

void TypeFloat::print(int indent) {
  printIndent(indent);
  std::cout << "TypeFloat()" << std::endl;
}

Value TypeStr::accept(ASTVisitor &v) { return v.visit(*this); }
TypeStr::TypeStr(Span s) : ASTNode(s) {}

void TypeStr::print(int indent) {
  printIndent(indent);
  std::cout << "TypeStr()" << std::endl;
}

Value TypeBool::accept(ASTVisitor &v) { return v.visit(*this); }
TypeBool::TypeBool(Span s) : ASTNode(s) {}

void TypeBool::print(int indent) {
  printIndent(indent);
  std::cout << "TypeBool()" << std::endl;
}

Value TypeVec::accept(ASTVisitor &v) { return v.visit(*this); }
TypeVec::TypeVec(Span s) : ASTNode(s) {}

void TypeVec::print(int indent) {
  printIndent(indent);
  std::cout << "TypeVec()" << std::endl;
}

Value TypeDic::accept(ASTVisitor &v) { return v.visit(*this); }
TypeDic::TypeDic(Span s) : ASTNode(s) {}

void TypeDic::print(int indent) {
  printIndent(indent);
  std::cout << "TypeDic()" << std::endl;
}

Value Variable::accept(ASTVisitor &v) { return v.visit(*this); }
Variable::Variable(std::string varName, Span s, ASTPtr varValue)
    : ASTNode(s), name(varName), value(std::move(varValue)) {}

void Variable::print(int indent) {
  printIndent(indent);
  std::cout << "Variable(name=" << name << ")\n";
  printIndent(indent + 2);
  std::cout << "Value:\n";

  if (value) {
    value->print(indent + 4);
  }
}

Value UnaryOp::accept(ASTVisitor &v) { return v.visit(*this); }
UnaryOp::UnaryOp(TokenType opOp, ASTPtr opOperand, Span s)
    : ASTNode(s), op(opOp), operand(std::move(opOperand)) {}

void UnaryOp::print(int indent) {
  printIndent(indent);
  std::cout << "UnaryOp(op=\"" << (uint16_t)op << "\")\n";
  printIndent(indent);
  std::cout << "Operand:\n";

  if (operand) {
    operand->print(indent + 2);
  } else {
    printIndent(indent + 2);
    std::cout << "nullptr\n";
  }
}

Value BinaryOp::accept(ASTVisitor &v) { return v.visit(*this); }
BinaryOp::BinaryOp(TokenType opOp, ASTPtr opLeft, ASTPtr opRight, Span s)
    : ASTNode(s), op(opOp), left(std::move(opLeft)), right(std::move(opRight)) {
}

void BinaryOp::print(int indent) {
  printIndent(indent);
  std::cout << "BinaryOp(op=\"" << (uint16_t)op << "\")" << std::endl;
  printIndent(indent + 2);
  std::cout << "Left:\n";

  if (left) {
    left->print(indent + 4);
  } else {
    printIndent(indent + 4);
    std::cout << "nullptr\n";
  }

  printIndent(indent + 2);
  std::cout << "Right:\n";

  if (right) {
    right->print(indent + 4);
  } else {
    printIndent(indent + 4);
    std::cout << "nullptr\n";
  }
}

Value ExpressionStmt::accept(ASTVisitor &v) { return v.visit(*this); }
ExpressionStmt::ExpressionStmt(ASTPtr stmtExpr, Span s, bool stmtNoOp,
                               bool exprIsBreak, bool exprIsContinue)
    : ASTNode(s), expr(std::move(stmtExpr)), noOp(stmtNoOp),
      isBreak(exprIsBreak), isContinue(exprIsContinue) {}

void ExpressionStmt::print(int indent) {
  printIndent(indent);
  std::cout << "ExpressionStmt(break=" << (isBreak ? "true" : "false") << ")"
            << std::endl;

  if (expr) {
    expr->print(indent + 2);
  } else {
    printIndent(indent + 2);
    std::cout << "nullptr\n";
  }
}

Value IfStmt::accept(ASTVisitor &v) { return v.visit(*this); }
IfStmt::IfStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> thenStmts,
               Span s, std::vector<ExpressionStmt> elseStmts)
    : ASTNode(s), condition(std::move(stmtCondition)),
      thenClauseStmts(std::move(thenStmts)),
      elseClauseStmts(std::move(elseStmts)) {}

void IfStmt::print(int indent) {
  printIndent(indent);
  std::cout << "IfStmt(thenStmts=" << thenClauseStmts.size()
            << ", elseStmts=" << elseClauseStmts.size() << ")\n";
  printIndent(indent + 2);
  std::cout << "Condition:\n";
  condition->print(indent + 4);

  printIndent(indent + 2);
  std::cout << "ThenClauseStatements:\n";
  for (ExpressionStmt &stmt : thenClauseStmts) {
    stmt.print(indent + 4);
  }

  printIndent(indent + 2);
  std::cout << "ElseClauseStatements:\n";
  for (ExpressionStmt &stmt : elseClauseStmts) {
    stmt.print(indent + 4);
  }
}

Value WhileStmt::accept(ASTVisitor &v) { return v.visit(*this); }
WhileStmt::WhileStmt(ASTPtr stmtCondition,
                     std::vector<ExpressionStmt> stmtStmts, Span s)
    : ASTNode(s), condition(std::move(stmtCondition)),
      stmts(std::move(stmtStmts)) {}

void WhileStmt::print(int indent) {
  printIndent(indent);
  std::cout << "WhileStmt(statements=" << stmts.size() << ")\n";
  printIndent(indent + 2);
  std::cout << "Condition:\n";
  condition->print(indent + 4);
  printIndent(indent + 2);
  std::cout << "Statements:\n";

  for (ExpressionStmt &stmt : stmts) {
    stmt.print(indent + 4);
  }
}

Value ForStmt::accept(ASTVisitor &v) { return v.visit(*this); }
ForStmt::ForStmt(std::string stmtVar, ASTPtr stmtIter,
                 std::vector<ExpressionStmt> stmtStmts, Span s)
    : ASTNode(s), var(std::move(stmtVar)), iter(std::move(stmtIter)),
      stmts(std::move(stmtStmts)) {}

void ForStmt::print(int indent) {
  printIndent(indent);
  std::cout << "ForStmt(statements=" << stmts.size() << ")\n";
  printIndent(indent + 2);
  std::cout << "Var:\n";
  printIndent(indent + 4);
  printf("%s", (var + '\n').c_str());
  printIndent(indent + 2);
  std::cout << "Iter:\n";
  iter->print(indent + 4);
  printIndent(indent + 2);
  std::cout << "Statements:\n";

  for (ExpressionStmt &stmt : stmts) {
    stmt.print(indent + 4);
  }
}

Value FunctionCall::accept(ASTVisitor &v) { return v.visit(*this); }
FunctionCall::FunctionCall(std::string callName, std::vector<ASTPtr> callParams,
                           Span s)
    : ASTNode(s), name(callName), params(std::move(callParams)) {}

void FunctionCall::print(int indent) {
  printIndent(indent);
  std::cout << "FunctionCall(name=" << name << ", parameters=" << params.size()
            << ")\n";
  printIndent(indent + 2);
  std::cout << "Parameters:\n";

  for (const auto &param : params) {
    param->print(indent + 4);
  }
}

Value ReturnStmt::accept(ASTVisitor &v) { return v.visit(*this); }
ReturnStmt::ReturnStmt(ASTPtr stmtValue, Span s)
    : ASTNode(s), value(std::move(stmtValue)) {}

void ReturnStmt::print(int indent) {
  printIndent(indent);
  std::cout << "ReturnStmt()\n";

  if (value) {
    value->print(indent + 2);
  } else {
    printIndent(indent + 2);
    std::cout << "nullptr\n";
  }
}

Value FunctionStmt::accept(ASTVisitor &v) { return v.visit(*this); }
FunctionStmt::FunctionStmt(std::string stmtName, std::vector<ASTPtr> stmtParams,
                           std::vector<ExpressionStmt> stmtStmts, Span s,
                           ASTPtr stmtReturnValue)
    : ASTNode(s), name(stmtName), params(std::move(stmtParams)),
      stmts(std::move(stmtStmts)), returnValue(std::move(stmtReturnValue)) {}

void FunctionStmt::print(int indent) {
  printIndent(indent);
  std::cout << "FunctionStmt(name=" << name << ", statements=" << stmts.size()
            << ", parameters=" << params.size() << ")\n";
  printIndent(indent + 2);
  std::cout << "Parameters:\n";

  for (const auto &param : params) {
    param->print(indent + 4);
  }

  printIndent(indent + 2);
  std::cout << "Statements:\n";

  for (ExpressionStmt &stmt : stmts) {
    stmt.print(indent + 4);
  }
}

Value ClassStmt::accept(ASTVisitor &v) { return v.visit(*this); }
ClassStmt::ClassStmt(std::string stmtName, std::vector<ASTPtr> stmtParams,
                     std::vector<ExpressionStmt> stmtStmts, Span s)
    : ASTNode(s), name(stmtName), params(std::move(stmtParams)),
      stmts(std::move(stmtStmts)) {}

void ClassStmt::print(int indent) {
  printIndent(indent);
  std::cout << "ClassStmt(name=" << name << ", statements=" << stmts.size()
            << ", parameters=" << params.size() << ")\n";
  printIndent(indent + 2);
  std::cout << "Parameters:\n";

  for (const auto &param : params) {
    param->print(indent + 4);
  }

  printIndent(indent + 2);
  std::cout << "Statements:\n";

  for (ExpressionStmt &stmt : stmts) {
    stmt.print(indent + 4);
  }
}

Value LoadStmt::accept(ASTVisitor &v) { return v.visit(*this); }
LoadStmt::LoadStmt(std::string fname, Span s) : ASTNode(s), fname(fname) {}

void LoadStmt::print(int indent) {
  printIndent(indent);
  std::cout << "LoadStmt(fname=" << fname << ")\n";
  printIndent(indent + 2);
}

Value Program::accept(ASTVisitor &v) { return v.visit(*this); }
Program::Program(std::vector<ExpressionStmt> &&programStatements, Span s)
    : ASTNode(s), statements(std::move(programStatements)) {}

void Program::print(int indent) {
  printIndent(indent);
  std::cout << "Program(statements=" << statements.size() << ")" << std::endl;

  for (ExpressionStmt &stmt : statements) {
    stmt.print(indent + 2);
  }
}
