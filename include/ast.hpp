#pragma once

#include "opcodes.hpp"
#include "span.hpp"
#include "types.hpp"
#include "visitor.hpp"
#include <memory>
#include <string>
#include <vector>

using ASTPtr = std::unique_ptr<ASTNode>;

class IntLiteral : public ASTNode {
public:
  tn_int_t value;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  IntLiteral(tn_int_t literalValue, Span s);
};

class FloatLiteral : public ASTNode {
public:
  tn_dec_t value;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  FloatLiteral(tn_dec_t literalValue, Span s);
};

class StrLiteral : public ASTNode {
public:
  std::string value;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  StrLiteral(std::string literalValue, Span s);
};

class BoolLiteral : public ASTNode {
public:
  tn_bool_t value;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  BoolLiteral(tn_bool_t literalValue, Span s);
};

class VecLiteral : public ASTNode {
public:
  std::vector<ASTPtr> elems;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  VecLiteral(std::vector<ASTPtr> literalValue, Span s);
};

class DicLiteral : public ASTNode {
public:
  std::map<ASTPtr, ASTPtr> dic;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  DicLiteral(std::map<ASTPtr, ASTPtr> literalDic, Span s);
};

class TypeInt : public ASTNode {
public:
  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  TypeInt(Span s);
};

class TypeFloat : public ASTNode {
public:
  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  TypeFloat(Span s);
};

class TypeStr : public ASTNode {
public:
  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  TypeStr(Span s);
};

class TypeBool : public ASTNode {
public:
  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  TypeBool(Span s);
};

class TypeVec : public ASTNode {
public:
  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  TypeVec(Span s);
};

class TypeDic : public ASTNode {
public:
  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  TypeDic(Span s);
};

class Variable : public ASTNode {
public:
  std::string name;
  ASTPtr value;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  Variable(std::string varName, Span s, ASTPtr varValue = nullptr);
};

class UnaryOp : public ASTNode {
public:
  TokenType op;
  ASTPtr operand;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  UnaryOp(TokenType opOp, ASTPtr opOperand, Span s);
};

class BinaryOp : public ASTNode {
public:
  TokenType op;
  ASTPtr left;
  ASTPtr right;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  BinaryOp(TokenType opOp, ASTPtr opLeft, ASTPtr opRight, Span s);
};

class ExpressionStmt : public ASTNode {
public:
  ASTPtr expr;
  bool noOp;
  bool isBreak;
  bool isContinue;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  ExpressionStmt(ASTPtr expr, Span s, bool exprNoOp = false,
                 bool exprIsBreak = false, bool exprIsContinue = false);
};

class IfStmt : public ASTNode {
public:
  ASTPtr condition;
  std::vector<ExpressionStmt> thenClauseStmts;
  std::vector<ExpressionStmt> elseClauseStmts;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  IfStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> thenStmts, Span s,
         std::vector<ExpressionStmt> elseStmts = {});
};

class WhileStmt : public ASTNode {
public:
  ASTPtr condition;
  std::vector<ExpressionStmt> stmts;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  WhileStmt(ASTPtr stmtCondition, std::vector<ExpressionStmt> stmtStmts,
            Span s);
};

class ForStmt : public ASTNode {
public:
  std::string var;
  ASTPtr iter;
  std::vector<ExpressionStmt> stmts;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  ForStmt(std::string stmtVar, ASTPtr stmtIter,
          std::vector<ExpressionStmt> stmtStmts, Span s);
};

class FunctionCall : public ASTNode {
public:
  std::string name;
  std::vector<ASTPtr> params;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  FunctionCall(std::string callName, std::vector<ASTPtr> callParams, Span s);
};

class ReturnStmt : public ASTNode {
public:
  ASTPtr value;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  ReturnStmt(ASTPtr stmtValue, Span s);
};

class FunctionStmt : public ASTNode {
public:
  std::string name;
  std::vector<ASTPtr> params;
  std::vector<ExpressionStmt> stmts;
  ASTPtr returnValue;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  FunctionStmt(std::string stmtName, std::vector<ASTPtr> stmtParams,
               std::vector<ExpressionStmt> stmtStmts, Span s,
               ASTPtr stmtReturnValue = nullptr);
};

class ClassStmt : public ASTNode {
public:
  std::string name;
  std::vector<ASTPtr> params;
  std::vector<ExpressionStmt> stmts;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  ClassStmt(std::string literalName, std::vector<ASTPtr> literalParams,
            std::vector<ExpressionStmt> literalStmts, Span s);
};

class Program : public ASTNode {
public:
  std::vector<ExpressionStmt> statements;

  void print(int indent) override;
  Value accept(ASTVisitor &visitor) override;

  Program(std::vector<ExpressionStmt> &&programStatements, Span s);
};
