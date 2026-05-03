#pragma once

#include "ast.hpp"
#include "diagnostics.hpp"
#include "native.hpp"
#include "opcodes.hpp"
#include "types.hpp"
#include "visitor.hpp"
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct CallFrame {
  std::unordered_map<std::string, Value> locals;
  std::string callableName;
  Span callSite;
  std::string callsiteFilename;
  std::string moduleKey;
};

struct ModuleState {
  std::string key;
  std::string name;
  std::unordered_map<std::string, Value> variables;
  std::unordered_map<std::string, FunctionStmt *> functions;
  std::unordered_map<std::string, ClassStmt *> classes;
  std::unordered_map<std::string, NativeFn> nativeFunctions;
  bool initialized = false;
};

class Evaluator : public ASTVisitor {
  std::string source;

  bool program_should_terminate = false;

  std::vector<CallFrame> callStack;
  std::unordered_map<std::string, Value> variables;
  std::unordered_map<std::string, FunctionStmt *> functions;
  std::unordered_map<std::string, ClassStmt *> classes;
  std::unordered_map<std::string, ModuleState> modules;
  std::unordered_set<std::string> modules_in_progress;
  std::vector<std::string> module_context_stack;
  std::unordered_map<
      std::string,
      std::unordered_map<
          std::string,
          std::function<Value(const Value &, const std::vector<Value> &)>>>
      nativeMethods;

  Diagnostics &diags;
  std::string filename;
  const std::vector<std::string> file_search_dirs;
  std::vector<ASTPtr> loaded_programs;

  Value evalBinaryOp(const Value &left, const Value &right, TokenType op);
  Value evalUnaryOp(const Value &operand, TokenType op);
  Value evalStmt(ExpressionStmt &stmt);
  Value evalExpr(ASTNode *node);
  std::vector<TracebackFrame> collectTraceback() const;
  void reportRuntimeError(const std::string &msg, const Span &span,
                          const std::string &hint = "");
  std::optional<std::string> activeModuleKey() const;
  ModuleState *getModuleState(const std::string &moduleKey);
  const ModuleState *getModuleState(const std::string &moduleKey) const;
  std::string moduleBindingNameFor(const std::string &target) const;
  Value bindModuleValue(const std::string &bindingName,
                        const std::string &moduleKey, const Span &span);
  Value callNative(const NativeFn &fn, const std::vector<ASTPtr> &params);
  Value executeFunction(FunctionStmt *func, const std::vector<ASTPtr> &params,
                        const Span &span, const std::string &callableName,
                        const std::string &moduleKey);
  Value instantiateClass(ClassStmt *classDef, const std::vector<ASTPtr> &params,
                         const Span &span, const std::string &moduleKey);

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
  Value visit(LoadStmt &node) override;
  Value visit(Program &node) override;
  Value visit(ExpressionStmt &node) override;
  Value visit(NoOp &node) override;

  friend class VM;

public:
  Value evalProgram(ASTPtr program, const std::vector<std::string> args = {});

  Evaluator(std::string input, Diagnostics &diagnostics, std::string fname,
            std::vector<std::string> search_dirs);
};
