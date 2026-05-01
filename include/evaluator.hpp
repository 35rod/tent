#pragma once

#include "ast.hpp"
#include "diagnostics.hpp"
#include "opcodes.hpp"
#include "types.hpp"
#include "visitor.hpp"
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

struct CallFrame {
  std::unordered_map<std::string, Value> locals;
  std::string callableName;
  Span callSite;
  std::string callsiteFilename;
};

class Evaluator : public ASTVisitor {
  std::string source;

  bool program_should_terminate = false;

  std::vector<CallFrame> callStack;
  std::map<std::string, Value> variables;
  std::vector<FunctionStmt *> functions;
  std::unordered_map<std::string, ClassStmt *> classes;
  std::unordered_map<
      std::string,
      std::unordered_map<
          std::string,
          std::function<Value(const Value &, const std::vector<Value> &)>>>
      nativeMethods;

  Diagnostics &diags;
  std::string filename;

  Value evalBinaryOp(const Value &left, const Value &right, TokenType op);
  Value evalUnaryOp(const Value &operand, TokenType op);
  Value evalStmt(ExpressionStmt &stmt);
  Value evalExpr(ASTNode *node);
  std::vector<TracebackFrame> collectTraceback() const;
  void reportRuntimeError(const std::string &msg, const Span &span,
                          const std::string &hint = "");

  void exitErrors();

  Value visit(IntLiteral &node) override;
  Value visit(FloatLiteral &node) override;
  Value visit(StrLiteral &node) override;
  Value visit(BoolLiteral &node) override;
  Value visit(VecLiteral &node) override;
  Value visit(DicLiteral &node) override;
  Value visit(TypeInt &node) override;
  Value visit(TypeFloat &node) override;
  Value visit(TypeStr &node) override;
  Value visit(TypeBool &node) override;
  Value visit(TypeVec &node) override;
  Value visit(TypeDic &node) override;
  Value visit(Variable &node) override;
  Value visit(UnaryOp &node) override;
  Value visit(BinaryOp &node) override;
  Value visit(IfStmt &node) override;
  Value visit(WhileStmt &node) override;
  Value visit(ForStmt &node) override;
  Value visit(FunctionCall &node) override;
  Value visit(ReturnStmt &node) override;
  Value visit(FunctionStmt &node) override;
  Value visit(ClassStmt &node) override;
  Value visit(Program &node) override;
  Value visit(ExpressionStmt &node) override;
  Value visit(NoOp &node) override;

  friend class VM;

public:
  Value evalProgram(ASTPtr program, const std::vector<std::string> args = {});

  Evaluator(std::string input, Diagnostics &diagnostics, std::string fname);
};
