#pragma once

#include "types.hpp"

class IntLiteral;
class FloatLiteral;
class StrLiteral;
class BoolLiteral;
class VecLiteral;
class DicLiteral;
class TypeInt;
class TypeFloat;
class TypeStr;
class TypeBool;
class TypeVec;
class TypeDic;
class Variable;
class UnaryOp;
class BinaryOp;
class IfStmt;
class WhileStmt;
class ForStmt;
class FunctionCall;
class ReturnStmt;
class FunctionStmt;
class ClassStmt;
class Program;
class ExpressionStmt;
class NoOp;

class ASTVisitor {
public:
  virtual Value visit(IntLiteral &) = 0;
  virtual Value visit(FloatLiteral &) = 0;
  virtual Value visit(StrLiteral &) = 0;
  virtual Value visit(BoolLiteral &) = 0;
  virtual Value visit(VecLiteral &) = 0;
  virtual Value visit(DicLiteral &) = 0;
  virtual Value visit(TypeInt &) = 0;
  virtual Value visit(TypeFloat &) = 0;
  virtual Value visit(TypeStr &) = 0;
  virtual Value visit(TypeBool &) = 0;
  virtual Value visit(TypeVec &) = 0;
  virtual Value visit(TypeDic &) = 0;
  virtual Value visit(Variable &) = 0;
  virtual Value visit(UnaryOp &) = 0;
  virtual Value visit(BinaryOp &) = 0;
  virtual Value visit(IfStmt &) = 0;
  virtual Value visit(WhileStmt &) = 0;
  virtual Value visit(ForStmt &) = 0;
  virtual Value visit(FunctionCall &) = 0;
  virtual Value visit(ReturnStmt &) = 0;
  virtual Value visit(FunctionStmt &) = 0;
  virtual Value visit(ClassStmt &) = 0;
  virtual Value visit(Program &) = 0;
  virtual Value visit(ExpressionStmt &) = 0;
  virtual Value visit(NoOp &) = 0;

  virtual ~ASTVisitor() = default;
};
