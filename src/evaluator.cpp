#include "evaluator.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <variant>

#include "args.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "native.hpp"
#include "opcodes.hpp"
#include "parser.hpp"
#include "types.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

std::vector<std::string> nativeLibs;

namespace {
class ScopedCallFrame {
  std::vector<CallFrame> &stack;

public:
  ScopedCallFrame(std::vector<CallFrame> &callStack, CallFrame frame)
      : stack(callStack) {
    stack.push_back(std::move(frame));
  }

  ~ScopedCallFrame() {
    if (!stack.empty()) {
      stack.pop_back();
    }
  }
};

class ScopedModuleContext {
  std::vector<std::string> &stack;

public:
  ScopedModuleContext(std::vector<std::string> &moduleStack,
                      std::string moduleKey)
      : stack(moduleStack) {
    stack.push_back(std::move(moduleKey));
  }

  ~ScopedModuleContext() {
    if (!stack.empty()) {
      stack.pop_back();
    }
  }
};

class ScopedFilename {
  std::string &fileNameRef;
  std::string originalName;

public:
  ScopedFilename(std::string &fname, std::string newName)
      : fileNameRef(fname), originalName(fname) {
    fileNameRef = std::move(newName);
  }

  ~ScopedFilename() { fileNameRef = originalName; }
};

class ScopedSetMembership {
  std::unordered_set<std::string> &setRef;
  std::string value;

public:
  ScopedSetMembership(std::unordered_set<std::string> &setRef_,
                      std::string trackedValue)
      : setRef(setRef_), value(std::move(trackedValue)) {
    setRef.insert(value);
  }

  ~ScopedSetMembership() { setRef.erase(value); }
};
} // namespace

Evaluator::Evaluator(std::string input, Diagnostics &diagnostics,
                     std::string fname, std::vector<std::string> search_dirs)
    : source(input), diags(diagnostics), filename(fname),
      file_search_dirs(search_dirs) {
  nativeMethods["type_int"]["parse"] = [&](const Value &,
                                           const std::vector<Value> &rhs) {
    if (!std::holds_alternative<std::string>(rhs[0].v)) {
      diags.report<TypeError>("int.parse(s: str[, b:int]): invalid argument(s) "
                              "passed: first argument must be 'string'",
                              rhs[0].span, "", filename);
    }

    int base = 0; // special value that allows for 0x10 or 0b010 or the like
    if (rhs.size() == 2 && std::holds_alternative<tn_int_t>(rhs[1].v))
      base = std::get<tn_int_t>(rhs[1].v);
    else if (rhs.size() == 2) {
      diags.report<TypeError>(
          "int.parse(s: str[, b: int]): invalid argument(s) passed: second "
          "argument must be 'int', if present",
          rhs[1].span, "", filename);
    }

    try {
      return Value(
          tn_int_t(std::stoi(std::get<std::string>(rhs[0].v), nullptr, base)));
    } catch (std::exception &) {
      diags.report<TypeError>("Error converting string to integer", rhs[0].span,
                              "", filename);
    }

    return Value();
  };

  nativeMethods["type_vec"]["fill"] = [&](const Value &,
                                          const std::vector<Value> &rhs) {
    // vec.fill(n: int[, v: any]): return a vector of size 'n', optionally
    // filled with 'v'.
    Value::VecT ret = std::make_shared<std::vector<Value>>(
        (std::vector<Value>::size_type)std::get<tn_int_t>(rhs[0].v));

    if (rhs.size() > 1) {
      if (!is_primitive_val(rhs[1])) {
        diags.report<TypeError>(
            "vec.fill(n: int[, v: any\\class_instance]): invalid argument(s) "
            "passed: second arugment is of an invalid type for this function",
            rhs[1].span, "", filename);
      }

      std::fill(ret->begin(), ret->end(), rhs[1]);
    }

    return Value(ret);
  };

  nativeMethods["str"]["toUpperCase"] = [](const Value &lhs,
                                           const std::vector<Value> &) {
    std::string str = std::get<std::string>(lhs.v);
    for (char &c : str)
      c = toupper(c);

    return Value(str);
  };

  nativeMethods["str"]["toLowerCase"] = [](const Value &lhs,
                                           const std::vector<Value> &) {
    std::string str = std::get<std::string>(lhs.v);
    for (char &c : str)
      c = tolower(c);

    return Value(str);
  };

  nativeMethods["str"]["len"] = [](const Value &lhs,
                                   const std::vector<Value> &) {
    const std::string &str = std::get<std::string>(lhs.v);
    return Value((tn_int_t)str.length());
  };

  nativeMethods["vec"]["len"] = [](const Value &lhs,
                                   const std::vector<Value> &) {
    Value::VecT vec = std::get<Value::VecT>(lhs.v);
    return Value((tn_int_t)vec->size());
  };
  nativeMethods["vec"]["push"] = [](const Value &lhs,
                                    const std::vector<Value> &rhs) {
    Value::VecT vec = std::get<Value::VecT>(lhs.v);
    vec->push_back(rhs[0]);
    return Value();
  };
  nativeMethods["vec"]["pop"] = [&](const Value &lhs,
                                    const std::vector<Value> &) {
    Value::VecT vec = std::get<Value::VecT>(lhs.v);

    if (vec->size() < 1) {
      diags.report<Error>(
          "attempted to pop an element back from an empty vector", lhs.span, "",
          filename);
    }

    Value ret = vec->back();
    vec->pop_back();
    return ret;
  };
}

Value Evaluator::evalProgram(ASTPtr program,
                             const std::vector<std::string> args) {
  Value last;

  {
    auto vecPtr = std::make_shared<std::vector<Value>>();
    vecPtr->reserve(args.size());
    for (const std::string &s : args)
      vecPtr->push_back(Value(s));
    variables["ARGS"] = Value(vecPtr);
  }

  variables["ARG_COUNT"] = Value(tn_int_t(args.size()));
  variables["EOF"] = Value(tn_int_t(EOF));

  Program *p = static_cast<Program *>(program.get());

  for (ExpressionStmt &stmt : p->statements) {
    last = evalStmt(stmt);
    if (last.isExit)
      break;
  }

  return last;
}

Value Evaluator::evalStmt(ExpressionStmt &stmt) {
  ASTNode *expr = stmt.expr.get();

  if (!expr) {
    diags.report<Error>("Invalid expression", stmt.span, "", filename);
  }

  return evalExpr(expr);
}

Value Evaluator::evalExpr(ASTNode *node) {
  if (!node) {
    diags.report<Error>("Null AST node in evaluator", Span(), "", filename);
    exitErrors();
  }

  return node->accept(*this);
}

std::vector<TracebackFrame> Evaluator::collectTraceback() const {
  std::vector<TracebackFrame> frames;
  frames.reserve(callStack.size());

  for (const CallFrame &frame : callStack) {
    if (frame.callableName.empty())
      continue;

    frames.emplace_back(
        frame.callableName, frame.callSite,
        frame.callsiteFilename.empty() ? filename : frame.callsiteFilename);
  }

  return frames;
}

void Evaluator::reportRuntimeError(const std::string &msg, const Span &span,
                                   const std::string &hint) {
  diags.report<RuntimeError>(msg, span, hint, filename, collectTraceback());
}

std::optional<std::string> Evaluator::activeModuleKey() const {
  if (!callStack.empty() && !callStack.back().moduleKey.empty()) {
    return callStack.back().moduleKey;
  }

  if (!module_context_stack.empty()) {
    return module_context_stack.back();
  }

  return std::nullopt;
}

ModuleState *Evaluator::getModuleState(const std::string &moduleKey) {
  auto it = modules.find(moduleKey);
  if (it == modules.end()) {
    return nullptr;
  }

  return &it->second;
}

const ModuleState *
Evaluator::getModuleState(const std::string &moduleKey) const {
  auto it = modules.find(moduleKey);
  if (it == modules.end()) {
    return nullptr;
  }

  return &it->second;
}

std::string Evaluator::moduleBindingNameFor(const std::string &target) const {
  std::filesystem::path modulePath(target);
  std::string bindingName = modulePath.stem().string();
  if (bindingName.empty()) {
    bindingName = modulePath.filename().string();
  }

  if (bindingName.empty()) {
    bindingName = target;
  }

  return bindingName;
}

Value Evaluator::bindModuleValue(const std::string &bindingName,
                                 const std::string &moduleKey,
                                 const Span &span) {
  Value moduleValue(Value::ModuleRef(bindingName, moduleKey));
  moduleValue.setSpan(span);

  if (!callStack.empty()) {
    callStack.back().locals[bindingName] = moduleValue;
    return moduleValue;
  }

  auto moduleKeyOpt = activeModuleKey();
  if (moduleKeyOpt.has_value()) {
    ModuleState *state = getModuleState(moduleKeyOpt.value());
    if (state != nullptr) {
      state->variables[bindingName] = moduleValue;
      return moduleValue;
    }
  }

  variables[bindingName] = moduleValue;
  return moduleValue;
}

Value Evaluator::callNative(const NativeFn &fn,
                            const std::vector<ASTPtr> &params) {
  std::vector<Value> evalArgs;
  evalArgs.reserve(params.size());

  for (const auto &param : params) {
    if (!param) {
      diags.report<Error>("Null parameter AST node", Span(), "", filename);
      exitErrors();
    }

    evalArgs.push_back(evalExpr(param.get()));
  }

  return fn(evalArgs);
}

Value Evaluator::executeFunction(FunctionStmt *func,
                                 const std::vector<ASTPtr> &params,
                                 const Span &span,
                                 const std::string &callableName,
                                 const std::string &moduleKey) {
  if (func == nullptr) {
    diags.report<Error>("Attempted to call null function", span, "", filename);
    exitErrors();
  }

  if (params.size() != func->params.size()) {
    diags.report<Error>("Parameter count mismatch in function call to " +
                            func->name,
                        span, "", filename);
    exitErrors();
  }

  CallFrame frame;
  frame.callableName = callableName;
  frame.callSite = span;
  frame.callsiteFilename = filename;
  frame.moduleKey = moduleKey;

  for (size_t i = 0; i < func->params.size(); i++) {
    Variable *formalParam = dynamic_cast<Variable *>(func->params[i].get());
    if (!formalParam) {
      diags.report<Error>("Function parameter is not a variable", func->span,
                          "", filename);
      exitErrors();
    }

    frame.locals[formalParam->name] = evalExpr(params[i].get());
  }

  ScopedCallFrame scopedFrame(callStack, std::move(frame));

  Value result;

  for (ExpressionStmt &stmt : func->stmts) {
    result = evalStmt(stmt);

    if (result.isReturn) {
      result.isReturn = false;
      return result;
    }

    if (result.isExit) {
      return result;
    }
  }

  return result;
}

Value Evaluator::instantiateClass(ClassStmt *classDef,
                                  const std::vector<ASTPtr> &params,
                                  const Span &span,
                                  const std::string &moduleKey) {
  if (classDef == nullptr) {
    diags.report<Error>("Attempted to instantiate null class", span, "",
                        filename);
    exitErrors();
  }

  if (params.size() != classDef->params.size()) {
    diags.report<Error>("Parameter count mismatch in class call to " +
                            classDef->name,
                        span, "", filename);
    exitErrors();
  }

  Value::ClassInstance instance(classDef->name, moduleKey);

  CallFrame frame;
  frame.callableName = "class " + classDef->name + "()";
  frame.callSite = span;
  frame.callsiteFilename = filename;
  frame.moduleKey = moduleKey;

  for (size_t i = 0; i < params.size(); i++) {
    Variable *paramVar = dynamic_cast<Variable *>(classDef->params[i].get());
    if (!paramVar) {
      diags.report<Error>("Class parameter is not a variable", classDef->span,
                          "", filename);
      exitErrors();
    }

    Value argVal = evalExpr(params[i].get());
    instance.fields[paramVar->name] = argVal;
    frame.locals[paramVar->name] = argVal;
  }

  ScopedCallFrame scopedFrame(callStack, std::move(frame));

  for (ExpressionStmt &stmt : classDef->stmts) {
    if (auto *fn = dynamic_cast<FunctionStmt *>(stmt.expr.get())) {
      instance.methods[fn->name] = fn;
    } else if (auto *bin = dynamic_cast<BinaryOp *>(stmt.expr.get())) {
      if (auto *var = dynamic_cast<Variable *>(bin->left.get())) {
        instance.fields[var->name] = evalExpr(bin->right.get());
      }
    } else if (auto *varStmt = dynamic_cast<Variable *>(stmt.expr.get())) {
      instance.fields[varStmt->name] = Value();
    } else {
      evalStmt(stmt);
    }
  }

  return Value(instance);
}

// Literal nodes

Value Evaluator::visit(IntLiteral &node) {
  return Value(node.value).setSpan(node.span);
}

Value Evaluator::visit(FloatLiteral &node) {
  return Value(node.value).setSpan(node.span);
}

Value Evaluator::visit(StrLiteral &node) {
  return Value(node.value).setSpan(node.span);
}

Value Evaluator::visit(BoolLiteral &node) {
  return Value(node.value).setSpan(node.span);
}

Value Evaluator::visit(VecLiteral &node) {
  std::vector<Value> elems;
  elems.reserve(node.elems.size());

  for (auto &elem : node.elems) {
    elems.push_back(evalExpr(elem.get()).setSpan(elem->span));
  }

  return Value(std::make_shared<std::vector<Value>>(elems)).setSpan(node.span);
}

Value Evaluator::visit(DicLiteral &node) {
  std::map<std::string, Value> dic;

  for (auto &pair : node.dic) {
    const Value key = evalExpr(pair.first.get()).setSpan(pair.first->span);
    if (!std::holds_alternative<std::string>(key.v)) {
      diags.report<TypeError>("dictionary key must be a string", node.span, "",
                              filename);
    }

    dic[std::get<std::string>(key.v)] =
        evalExpr(pair.second.get()).setSpan(pair.second->span);
  }

  return Value(std::make_shared<std::map<std::string, Value>>(dic))
      .setSpan(node.span);
}

// Type nodes

Value Evaluator::visit(TypeInt &node) {
  Value res;
  res.typeInt = true;
  return res.setSpan(node.span);
}

Value Evaluator::visit(TypeFloat &node) {
  Value res;
  res.typeFloat = true;
  return res.setSpan(node.span);
}

Value Evaluator::visit(TypeStr &node) {
  Value res;
  res.typeStr = true;
  return res.setSpan(node.span);
}

Value Evaluator::visit(TypeBool &node) {
  Value res;
  res.typeBool = true;
  return res.setSpan(node.span);
}

Value Evaluator::visit(TypeVec &node) {
  Value res;
  res.typeVec = true;
  return res.setSpan(node.span);
}

Value Evaluator::visit(TypeDic &node) {
  Value res;
  res.typeDic = true;
  return res.setSpan(node.span);
}

// Control flow

Value Evaluator::visit(IfStmt &node) {
  const bool condition = std::get<tn_bool_t>(evalExpr(node.condition.get()).v);
  Value cur_res;

  auto &branch = condition ? node.thenClauseStmts : node.elseClauseStmts;

  for (ExpressionStmt &stmt : branch) {
    if (stmt.isBreak || stmt.isContinue)
      break;

    cur_res = evalStmt(stmt);

    if (cur_res.isReturn || cur_res.isExit)
      return cur_res;
  }

  return cur_res;
}

Value Evaluator::visit(WhileStmt &node) {
  bool break_while_loop = false;

  while (std::get<tn_bool_t>(evalExpr(node.condition.get()).v) == true &&
         !break_while_loop) {
    for (ExpressionStmt &stmt : node.stmts) {
      if (stmt.isBreak) {
        break_while_loop = true;
        break;
      } else if (stmt.isContinue) {
        break;
      }

      Value res = evalStmt(stmt);

      if (res.isReturn || res.isExit)
        return res;
    }
  }

  return Value();
}

Value Evaluator::visit(ForStmt &node) {
  bool break_for_loop = false;
  int index = 0;
  int length = 0;
  Value::DicT::element_type::iterator dic_it;
  bool is_dic = false;

  Value iter = evalExpr(node.iter.get()).setSpan(node.iter->span);

  if (std::holds_alternative<tn_int_t>(iter.v)) {
    length = std::get<tn_int_t>(iter.v);
  } else if (std::holds_alternative<std::string>(iter.v)) {
    length = std::get<std::string>(iter.v).size();
  } else if (std::holds_alternative<Value::VecT>(iter.v)) {
    length = std::get<Value::VecT>(iter.v)->size();
  } else if (std::holds_alternative<Value::DicT>(iter.v)) {
    dic_it = std::get<Value::DicT>(iter.v)->begin();
    is_dic = true;
  }

  while (((is_dic && dic_it != std::get<Value::DicT>(iter.v)->end()) ||
          (index < length)) &&
         !break_for_loop) {
    auto assignLoopVar = [&](Value value) {
      if (!callStack.empty()) {
        callStack.back().locals[node.var] = value;
        return;
      }

      auto moduleKey = activeModuleKey();
      if (moduleKey.has_value()) {
        ModuleState *state = getModuleState(moduleKey.value());
        if (state != nullptr) {
          state->variables[node.var] = value;
          return;
        }
      }

      variables[node.var] = value;
    };

    if (std::holds_alternative<tn_int_t>(iter.v)) {
      assignLoopVar(Value((tn_int_t)index));
    } else if (std::holds_alternative<std::string>(iter.v)) {
      assignLoopVar(
          Value(std::string(1, std::get<std::string>(iter.v)[index])));
    } else if (std::holds_alternative<Value::VecT>(iter.v)) {
      assignLoopVar((*std::get<Value::VecT>(iter.v))[index]);
    } else if (std::holds_alternative<Value::DicT>(iter.v)) {
      std::vector<Value> single = {Value(dic_it->first), dic_it->second};
      assignLoopVar(Value(std::make_shared<Value::VecT::element_type>(single)));
      dic_it++;
    }

    for (ExpressionStmt &stmt : node.stmts) {
      if (stmt.isBreak) {
        break_for_loop = true;
        break;
      } else if (stmt.isContinue) {
        break;
      }

      Value res = evalStmt(stmt);

      if (res.isReturn || res.isExit)
        return res;
    }

    index++;
  }

  return Value();
}

// Functions, classes, and modules

Value Evaluator::visit(FunctionStmt &node) {
  auto moduleKey = activeModuleKey();
  if (moduleKey.has_value()) {
    ModuleState *state = getModuleState(moduleKey.value());
    if (state != nullptr) {
      state->functions[node.name] = &node;
      return Value();
    }
  }

  functions[node.name] = &node;
  return Value();
}

Value Evaluator::visit(ReturnStmt &node) {
  Value v = evalExpr(node.value.get()).setSpan(node.span);
  v.isReturn = true;
  return v;
}

Value Evaluator::visit(ClassStmt &node) {
  auto moduleKey = activeModuleKey();
  if (moduleKey.has_value()) {
    ModuleState *state = getModuleState(moduleKey.value());
    if (state != nullptr) {
      state->classes[node.name] = &node;
      return Value();
    }
  }

  classes[node.name] = &node;
  return Value();
}

Value Evaluator::visit(FunctionCall &node) {
  std::optional<std::string> moduleKey = activeModuleKey();

  if (moduleKey.has_value()) {
    const ModuleState *state = getModuleState(moduleKey.value());
    if (state != nullptr) {
      auto classIt = state->classes.find(node.name);
      if (classIt != state->classes.end()) {
        return instantiateClass(classIt->second, node.params, node.span,
                                moduleKey.value());
      }

      auto fnIt = state->functions.find(node.name);
      if (fnIt != state->functions.end()) {
        return executeFunction(fnIt->second, node.params, node.span,
                               "form " + node.name + "()", moduleKey.value());
      }

      auto nativeIt = state->nativeFunctions.find(node.name);
      if (nativeIt != state->nativeFunctions.end()) {
        return callNative(nativeIt->second, node.params);
      }
    }
  }

  auto classIt = classes.find(node.name);
  if (classIt != classes.end()) {
    return instantiateClass(classIt->second, node.params, node.span, "");
  }

  auto fnIt = functions.find(node.name);
  if (fnIt != functions.end()) {
    return executeFunction(fnIt->second, node.params, node.span,
                           "form " + node.name + "()", "");
  }

  auto nativeIt = nativeFunctions.find(node.name);
  if (nativeIt != nativeFunctions.end()) {
    return callNative(nativeIt->second, node.params);
  }

  diags.report<Error>("Undefined function: " + node.name, node.span, "",
                      filename);
  exitErrors();
  return Value();
}

Value Evaluator::visit(LoadStmt &node) {
  const std::string bindingName = moduleBindingNameFor(node.fname);

  if (std::filesystem::path(node.fname).extension() == ".tent") {
    const auto foundFile = checkSearchPathsFor(node.fname, file_search_dirs);
    if (!foundFile.has_value()) {
      std::cerr << "File error: could not find file'" << node.fname << "'"
                << std::endl;
      exit(1);
    }

    const std::filesystem::path foundPath =
        std::filesystem::path(foundFile.value().first) /
        foundFile.value().second;
    std::error_code canonicalErr;
    std::filesystem::path canonicalPath =
        std::filesystem::weakly_canonical(foundPath, canonicalErr);
    if (canonicalErr) {
      canonicalPath = foundPath.lexically_normal();
    }

    const std::string moduleKey = canonicalPath.string();
    auto moduleIt = modules.find(moduleKey);
    if (moduleIt != modules.end()) {
      return bindModuleValue(bindingName, moduleKey, node.span);
    }

    ModuleState &state = modules[moduleKey];
    state.key = moduleKey;
    state.name = bindingName;

    if (modules_in_progress.count(moduleKey)) {
      return bindModuleValue(bindingName, moduleKey, node.span);
    }

    {
      ScopedSetMembership loadingGuard(modules_in_progress, moduleKey);
      ScopedModuleContext scopedModule(module_context_stack, moduleKey);
      ScopedFilename scopedFile(filename, moduleKey);

      std::ifstream fileHandle(canonicalPath.string());

      std::string output, line;

      while (std::getline(fileHandle, line)) {
        output += line;
        output.push_back('\n');
      }

      Lexer lexer(output, diags, moduleKey);
      lexer.nextChar();
      lexer.getTokens();

      Parser parser(lexer.tokens, diags, moduleKey);
      ASTPtr parsed = parser.parse_program();
      Program *p = static_cast<Program *>(parsed.get());

      loaded_programs.push_back(std::move(parsed));

      for (ExpressionStmt &stmt : p->statements) {
        evalStmt(stmt);
      }
    }

    state.initialized = true;
    return bindModuleValue(bindingName, moduleKey, node.span);
  } else {
    using RegisterFn = void (*)(std::unordered_map<std::string, NativeFn> &);
    const std::string moduleKey = "native:" + node.fname;
    auto moduleIt = modules.find(moduleKey);
    if (moduleIt != modules.end()) {
      return bindModuleValue(bindingName, moduleKey, node.span);
    }

    ModuleState &state = modules[moduleKey];
    state.key = moduleKey;
    state.name = bindingName;
    if (modules_in_progress.count(moduleKey)) {
      return bindModuleValue(bindingName, moduleKey, node.span);
    }

    ScopedSetMembership loadingGuard(modules_in_progress, moduleKey);

#if defined(_WIN32) || defined(_WIN64)
    HMODULE handle = LoadLibraryA(("lib" + node.fname).c_str());
    if (!handle)
      handle = LoadLibraryA(("lib" + node.fname + ".dll").c_str());

    if (!handle) {
      for (const std::string &dir : file_search_dirs) {
        if (handle)
          break;
        handle = LoadLibraryA((dir + "/lib" + node.fname).c_str());
      }
    }

    if (!handle) {
      std::cerr << "Failed to load library: " << node.fname << std::endl;
      exit(1);
    }

    RegisterFn reg = reinterpret_cast<RegisterFn>(
        GetProcAddress(handle, "registerFunctions"));

    if (!reg) {
      std::cerr << "Library missing registerFunctions symbol" << std::endl;
      FreeLibrary(handle);
      exit(1);
    }

    reg(state.nativeFunctions);
#else
    void *handle = NULL;
    for (const std::string &dir : file_search_dirs) {
      if (handle)
        break;
      handle = dlopen((dir + "/lib" + node.fname).c_str(), RTLD_LAZY);
      if (!handle)
        handle =
            dlopen((dir + "/lib" + node.fname + ".dylib").c_str(), RTLD_LAZY);
      if (!handle)
        handle = dlopen((dir + "/lib" + node.fname + ".so").c_str(), RTLD_LAZY);
    }

    if (!handle) {
      char *err_str = dlerror();
      std::cerr << "Failed to load library: " << node.fname
                << (err_str ? (": \n" + std::string(err_str)) : "")
                << std::endl;
      exit(1);
    }

    RegisterFn reg =
        reinterpret_cast<RegisterFn>(dlsym(handle, "registerFunctions"));

    if (!reg) {
      std::cerr << "Library missing registerFunctions symbol" << std::endl;
      dlclose(handle);
      exit(1);
    }

    reg(state.nativeFunctions);
#endif

    nativeLibs.push_back(node.fname);
    state.initialized = true;
    return bindModuleValue(bindingName, moduleKey, node.span);
  }
}

// Variables and operators

Value Evaluator::visit(Variable &node) {
  if (!callStack.empty()) {
    auto &frame = callStack.back();
    auto found = frame.locals.find(node.name);
    if (found != frame.locals.end()) {
      return found->second;
    }
  }

  auto moduleKey = activeModuleKey();
  if (moduleKey.has_value()) {
    ModuleState *state = getModuleState(moduleKey.value());
    if (state != nullptr) {
      auto found = state->variables.find(node.name);
      if (found != state->variables.end()) {
        return found->second;
      }
    }
  }

  if (variables.count(node.name)) {
    return variables[node.name];
  } else {
    diags.report<SyntaxError>("Undefined variable: " + node.name, node.span, "",
                              filename);
    exitErrors();
  }

  return Value();
}

Value Evaluator::visit(UnaryOp &node) {
  if (node.op != TokenType::INCREMENT && node.op != TokenType::DECREMENT) {
    return evalUnaryOp(evalExpr(node.operand.get()).setSpan(node.operand->span),
                       node.op)
        .setSpan(node.span);
  }

  if (auto var = dynamic_cast<Variable *>(node.operand.get())) {
    Value *target = nullptr;

    if (!callStack.empty()) {
      auto &frame = callStack.back();
      auto found = frame.locals.find(var->name);
      if (found != frame.locals.end()) {
        target = &found->second;
      }
    }

    if (target == nullptr) {
      auto moduleKey = activeModuleKey();
      if (moduleKey.has_value()) {
        ModuleState *state = getModuleState(moduleKey.value());
        if (state != nullptr) {
          auto found = state->variables.find(var->name);
          if (found != state->variables.end()) {
            target = &found->second;
          }
        }
      }
    }

    if (target == nullptr) {
      auto found = variables.find(var->name);
      if (found != variables.end()) {
        target = &found->second;
      }
    }

    if (target == nullptr) {
      diags.report<SyntaxError>("Undefined variable: " + var->name, var->span,
                                "", filename);
    }

    if (target != nullptr && std::holds_alternative<tn_int_t>(target->v)) {
      if (node.op == TokenType::INCREMENT) {
        return *target = std::get<tn_int_t>(target->v) + 1;
      } else {
        return *target = std::get<tn_int_t>(target->v) - 1;
      }
    } else if (target != nullptr) {
      diags.report<TypeError>("Increment/decrement operator requires integer",
                              node.operand->span, "", filename);
    }
  } else {
    diags.report<TypeError>(
        "Increment/decrement operator applied to non-variable",
        node.operand->span, "", filename);
  }

  return Value();
}

Value Evaluator::visit(BinaryOp &node) {
  auto resolveVariableRef = [&](const std::string &name,
                                bool createFallback = false) -> Value * {
    if (!callStack.empty()) {
      auto &locals = callStack.back().locals;
      auto found = locals.find(name);
      if (found != locals.end()) {
        return &found->second;
      }
    }

    auto moduleKey = activeModuleKey();
    if (moduleKey.has_value()) {
      ModuleState *state = getModuleState(moduleKey.value());
      if (state != nullptr) {
        auto found = state->variables.find(name);
        if (found != state->variables.end()) {
          return &found->second;
        }

        if (createFallback && callStack.empty()) {
          return &state->variables[name];
        }
      }
    }

    auto foundGlobal = variables.find(name);
    if (foundGlobal != variables.end()) {
      return &foundGlobal->second;
    }

    if (createFallback) {
      return &variables[name];
    }

    return nullptr;
  };

  if (isRightAssoc(node.op)) {
    if (auto *leftIndex = dynamic_cast<BinaryOp *>(node.left.get())) {
      if (leftIndex->op == TokenType::INDEX && node.op == TokenType::ASSIGN) {
        if (auto *vecVar = dynamic_cast<Variable *>(leftIndex->left.get())) {
          Value *holder = resolveVariableRef(vecVar->name);
          if (holder == nullptr) {
            diags.report<SyntaxError>("Undefined variable: " + vecVar->name,
                                      vecVar->span, "", filename);
            exitErrors();
          }

          if (holder != nullptr &&
              std::holds_alternative<Value::VecT>(holder->v)) {
            auto vecPtr = std::get<Value::VecT>(holder->v);

            if (!vecPtr) {
              diags.report<Error>("null vector", vecVar->span, "", filename);
            }

            Value idxVal = evalExpr(leftIndex->right.get());
            if (!std::holds_alternative<tn_int_t>(idxVal.v)) {
              diags.report<TypeError>("index must be an integer", vecVar->span,
                                      "", filename);
            }

            tn_int_t idx = std::get<tn_int_t>(idxVal.v);

            if (idx < 0 || (size_t)idx >= vecPtr->size()) {
              diags.report<Error>("index " + std::to_string(idx) +
                                      " is out of bounds for vector of size " +
                                      std::to_string(vecPtr->size()),
                                  vecVar->span, "", filename);
            }

            Value rhs = evalExpr(node.right.get());
            (*vecPtr)[(size_t)idx] = rhs;

            return rhs;
          } else if (holder != nullptr &&
                     std::holds_alternative<Value::DicT>(holder->v)) {
            auto dictPtr = std::get<Value::DicT>(holder->v);

            if (!dictPtr) {
              diags.report<Error>("null dictionary", vecVar->span, "",
                                  filename);
            }

            Value idxVal = evalExpr(leftIndex->right.get());

            if (!std::holds_alternative<std::string>(idxVal.v)) {
              diags.report<TypeError>("dictionary key must be a string",
                                      vecVar->span, "", filename);
            }

            std::string idx = std::get<std::string>(idxVal.v);

            Value rhs = evalExpr(node.right.get());
            (*dictPtr)[idx] = rhs;
          }
        } else {
          diags.report<TypeError>(
              "Left-hand side of indexed assignment must be a variable",
              leftIndex->span, "", filename);
        }
      }
    } else if (auto *varNode = dynamic_cast<Variable *>(node.left.get())) {
      Value right = evalExpr(node.right.get());

      if (node.op == TokenType::ASSIGN) {
        if (!callStack.empty()) {
          auto &frame = callStack.back();
          frame.locals[varNode->name] = right;
          return right;
        } else {
          auto moduleKey = activeModuleKey();
          if (moduleKey.has_value()) {
            ModuleState *state = getModuleState(moduleKey.value());
            if (state != nullptr) {
              return state->variables[varNode->name] = right;
            }
          }

          return variables[varNode->name] = right;
        }
      } else {
        Value *target = resolveVariableRef(varNode->name, true);
        if (target == nullptr) {
          diags.report<SyntaxError>("Undefined variable: " + varNode->name,
                                    varNode->span, "", filename);
          exitErrors();
        }

        TokenType compoundOp;
        if (!getCompoundAssignOp(node.op, compoundOp)) {
          diags.report<SyntaxError>("invalid compound assignment operator: " +
                                        tokenTypeToString(node.op),
                                    node.span, "", filename);
        }

        *target = evalBinaryOp(*target, right, compoundOp);

        return target->setSpan(node.span);
      }
    }
  } else if (node.op == TokenType::DOT) {
    Value lhs = evalExpr(node.left.get()).setSpan(node.left->span);

    if (auto fc = dynamic_cast<FunctionCall *>(node.right.get())) {
      std::string name = fc->name;

      if (auto module = std::get_if<Value::ModuleRef>(&lhs.v)) {
        ModuleState *state = getModuleState(module->key);
        if (state == nullptr) {
          diags.report<TypeError>("Unknown module: " + module->name, fc->span,
                                  "", filename);
          exitErrors();
        }

        auto classIt = state->classes.find(name);
        if (classIt != state->classes.end()) {
          return instantiateClass(classIt->second, fc->params, fc->span,
                                  module->key);
        }

        auto fnIt = state->functions.find(name);
        if (fnIt != state->functions.end()) {
          return executeFunction(fnIt->second, fc->params, fc->span,
                                 "form " + module->name + "." + name + "()",
                                 module->key);
        }

        auto nativeIt = state->nativeFunctions.find(name);
        if (nativeIt != state->nativeFunctions.end()) {
          return callNative(nativeIt->second, fc->params);
        }

        diags.report<TypeError>("Unknown module member '" + name + "' for '" +
                                    module->name + "'",
                                fc->span, "", filename);
        exitErrors();
      } else if (auto inst = std::get_if<Value::ClassInstance>(&lhs.v)) {
        auto it = inst->methods.find(name);

        if (it != inst->methods.end()) {
          FunctionStmt *method = it->second;

          if (fc->params.size() != method->params.size()) {
            diags.report<Error>("Parameter count mismatch in method call to " +
                                    inst->name + "." + name,
                                fc->span, "", filename);
            exitErrors();
          }

          CallFrame frame;
          frame.callableName = "method " + inst->name + "." + name + "()";
          frame.callSite = fc->span;
          frame.callsiteFilename = filename;
          frame.moduleKey = inst->moduleKey;

          for (auto &[fieldName, fieldVal] : inst->fields) {
            frame.locals[fieldName] = fieldVal;
          }

          for (size_t i = 0; i < method->params.size(); i++) {
            Variable *formalParam =
                dynamic_cast<Variable *>(method->params[i].get());
            frame.locals[formalParam->name] = evalExpr(fc->params[i].get());
          }

          ScopedCallFrame scopedFrame(callStack, std::move(frame));
          Value result;

          for (ExpressionStmt &stmt : method->stmts) {
            result = evalStmt(stmt);

            if (result.isReturn) {
              result.isReturn = false;

              for (auto &[k, v] : callStack.back().locals) {
                inst->fields[k] = v;
              }

              return result;
            }

            if (result.isExit)
              return result;
          }

          for (auto &[k, v] : callStack.back().locals) {
            inst->fields[k] = v;
          }

          return result;
        } else {
          diags.report<TypeError>("Unknown method '" + name + "' for class '" +
                                      inst->name + "'",
                                  fc->span, "", filename);
        }
      } else if (auto strPtr = std::get_if<std::string>(&lhs.v)) {
        if (nativeMethods["str"].count(name)) {
          std::vector<Value> args;
          for (auto &param : fc->params)
            args.push_back(evalExpr(param.get()));

          return nativeMethods["str"][name](*strPtr, args);
        } else {
          diags.report<TypeError>("Unknown string method: " + name, fc->span,
                                  "", filename);
        }
      } else if (auto vecPtr = std::get_if<Value::VecT>(&lhs.v)) {
        if (nativeMethods["vec"].count(name)) {
          std::vector<Value> args;
          for (auto &param : fc->params)
            args.push_back(evalExpr(param.get()));

          return nativeMethods["vec"][name](*vecPtr, args);
        } else {
          diags.report<TypeError>("Unknown vector method: " + name, fc->span,
                                  "", filename);
        }
      } else if (std::get_if<NullLiteral>(&lhs.v)) {
        if (lhs.typeInt) {
          if (nativeMethods["type_int"].count(name)) {
            std::vector<Value> args;
            for (auto &param : fc->params)
              args.push_back(evalExpr(param.get()));

            return nativeMethods["type_int"][name](Value(), args);
          }
        } else if (lhs.typeVec) {
          if (nativeMethods["type_vec"].count(name)) {
            std::vector<Value> args;
            for (auto &param : fc->params)
              args.push_back(evalExpr(param.get()));

            return nativeMethods["type_vec"][name](Value(), args);
          }
        }
      } else {
        diags.report<TypeError>("Method call not supported on this type",
                                fc->span, "", filename);
      }
    } else if (auto var = dynamic_cast<Variable *>(node.right.get())) {
      std::string propName = var->name;

      if (auto inst = std::get_if<Value::ClassInstance>(&lhs.v)) {
        auto fieldIt = inst->fields.find(propName);

        if (fieldIt != inst->fields.end()) {
          return fieldIt->second;
        }

        diags.report<TypeError>("Unknown property '" + propName +
                                    "' for class '" + inst->name + "'",
                                var->span, "", filename);
      } else if (auto module = std::get_if<Value::ModuleRef>(&lhs.v)) {
        ModuleState *state = getModuleState(module->key);
        if (state != nullptr) {
          auto exportIt = state->variables.find(propName);
          if (exportIt != state->variables.end()) {
            return exportIt->second;
          }
        }

        diags.report<TypeError>("Unknown module export '" + propName +
                                    "' for '" + module->name + "'",
                                var->span, "", filename);
        exitErrors();
      } else if (auto strPtr = std::get_if<std::string>(&lhs.v)) {
        if (propName == "length")
          return tn_int_t(strPtr->length());

        diags.report<TypeError>("Unknown string property: " + propName,
                                var->span, "", filename);
      } else {
        diags.report<TypeError>("Property access not supported on this type",
                                var->span, "", filename);
      }
    }
  }

  Value left = evalExpr(node.left.get());
  Value right = evalExpr(node.right.get());

  return evalBinaryOp(std::move(left), std::move(right), node.op);
}

// ── Nodes not reached through evalExpr ───────────────────────────────────────

Value Evaluator::visit(Program &node) {
  Value last;
  for (ExpressionStmt &stmt : node.statements) {
    last = evalStmt(stmt);
    if (last.isExit)
      break;
  }
  return last;
}

Value Evaluator::visit(ExpressionStmt &node) { return evalStmt(node); }

Value Evaluator::visit(NoOp &) { return Value(); }

Value Evaluator::evalBinaryOp(const Value &left, const Value &right,
                              TokenType op) {
  auto visitor = [&op, &left, &right, this](auto l, auto r) -> Value {
    using L = std::decay_t<decltype(l)>;
    using R = std::decay_t<decltype(r)>;

    if constexpr (std::is_same_v<L, NullLiteral> ||
                  std::is_same_v<R, NullLiteral>) {
      return Value();
    } else if constexpr (std::is_same_v<L, std::string> &&
                         std::is_same_v<R, std::string>) {
      switch (op) {
      case TokenType::ADD:
        return Value(l + r);
        break;
      case TokenType::EQEQ:
        return Value(l == r);
        break;
      case TokenType::NOTEQ:
        return Value(l != r);
        break;
      default:
        reportRuntimeError("invalid operator for string type: " +
                               tokenTypeToString(op),
                           Span::combine(left.span, right.span));
        exitErrors();
      }
    } else if constexpr (std::is_same_v<L, std::string> &&
                         std::is_integral_v<R>) {
      if (op == TokenType::INDEX) {
        tn_int_t idx = static_cast<tn_int_t>(r);
        if (idx < 0 || (size_t)idx >= l.size()) {
          diags.report<Error>("string index out of bounds",
                              Span::combine(left.span, right.span), "",
                              filename);
        }

        return Value(std::string(1, l[(size_t)idx]));
      }
    } else if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
      using ResultType =
          std::conditional_t<std::is_integral_v<L> && std::is_integral_v<R>,
                             tn_int_t, tn_dec_t>;

      ResultType a = static_cast<ResultType>(l);
      ResultType b = static_cast<ResultType>(r);

      if constexpr (std::is_integral_v<ResultType>) {
        using IntegralResultType =
            std::conditional_t<std::is_same_v<L, tn_bool_t>, tn_bool_t,
                               tn_int_t>;
        switch (op) {
        case TokenType::POW:
          return static_cast<IntegralResultType>(ipow(a, b));
        case TokenType::MOD:
          return static_cast<IntegralResultType>(a % b);
        case TokenType::FLOOR_DIV:
          return static_cast<IntegralResultType>(a / b);
        case TokenType::BIT_AND:
          return static_cast<IntegralResultType>(a & b);
        case TokenType::BIT_XOR:
          return static_cast<IntegralResultType>(a ^ b);
        case TokenType::BIT_OR:
          return static_cast<IntegralResultType>(a | b);
        case TokenType::LSHIFT:
          return static_cast<IntegralResultType>(a << b);
        case TokenType::RSHIFT:
          return static_cast<IntegralResultType>(a >> b);
        default:
          break;
        }
      }

      if (op >= TokenType::FLOOR_DIV && op <= TokenType::RSHIFT)
        diags.report<TypeError>(
            "failed to apply operator " + std::to_string((uint16_t)op) +
                " to non-integral operand(s)",
            Span::combine(left.span, right.span), "", filename);
      switch (op) {
      case TokenType::ADD:
        return a + b;
      case TokenType::SUB:
        return a - b;
      case TokenType::MUL:
        return a * b;
      case TokenType::DIV:
        if (b == 0) {
          reportRuntimeError("Division by zero",
                             Span::combine(left.span, right.span));
          exitErrors();
        }
        return a / b;
      case TokenType::MOD:
        return std::fmodf(a, b);
      case TokenType::POW:
        return std::powf(a, b);

      case TokenType::EQEQ:
        return a == b;
      case TokenType::NOTEQ:
        return a != b;
      case TokenType::LESS:
        return a < b;
      case TokenType::LESSEQ:
        return a <= b;
      case TokenType::GREATER:
        return a > b;
      case TokenType::GREATEREQ:
        return a >= b;

      case TokenType::AND:
        return a && b;
      case TokenType::OR:
        return a || b;
      default:
        diags.report<Error>("Unknown operator for arithmetic operands: " +
                                std::to_string((uint16_t)op),
                            Span::combine(left.span, right.span), "", filename);
      }
    } else if constexpr (std::is_same_v<L, Value::VecT> &&
                         std::is_integral_v<R>) {
      // op is vector index it just doesn't say so
      assert(op == TokenType::INDEX);
      auto vecPtr = l;

      if (!vecPtr) {
        diags.report<Error>("null vector", Span::combine(left.span, right.span),
                            "", filename);
      }

      tn_int_t idx = static_cast<tn_int_t>(r);

      if (idx < 0 || (size_t)idx >= vecPtr->size()) {
        diags.report<Error>("index " + std::to_string(idx) +
                                " is out of bounds for vector of size " +
                                std::to_string(vecPtr->size()),
                            Span::combine(left.span, right.span), "", filename);
      }

      return (*vecPtr)[(size_t)idx];
    } else if constexpr (std::is_same_v<L, Value::DicT> &&
                         std::is_same_v<R, std::string>) {
      assert(op == TokenType::INDEX);
      Value::DicT dictPtr = l;
      if (!dictPtr) {
        diags.report<Error>("null dictionary",
                            Span::combine(left.span, right.span), "", filename);
      }
      std::string idx = static_cast<std::string>(r);

      try {
        return dictPtr->at(idx);
      } catch (std::exception &) {
        diags.report<Error>("key '" + idx + "' was not found in dictionary",
                            Span::combine(left.span, right.span), "", filename);
      }
    }

    diags.report<TypeError>("Unsupported operand types for binary operation: " +
                                left.getTypeName() + " and " +
                                right.getTypeName(),
                            Span::combine(left.span, right.span), "", filename);

    exitErrors();
    return Value();
  };

  return Value(std::visit(visitor, left.v, right.v));
}

Value Evaluator::evalUnaryOp(const Value &operand, TokenType op) {
  if (op == TokenType::NOT) {
    return Value(!std::get<tn_bool_t>(operand.v));
  } else if (op == TokenType::BIT_NOT) {
    if (std::holds_alternative<tn_int_t>(operand.v))
      return Value(~std::get<tn_int_t>(operand.v));
    else if (std::holds_alternative<tn_bool_t>(operand.v))
      return Value(~std::get<tn_int_t>(operand.v));
    else {
      diags.report<TypeError>(
          "failed to apply operator BIT_NOT to non-integral operand",
          operand.span, "", filename);
    }
  } else if (op == TokenType::NEGATE) {
    if (std::holds_alternative<tn_int_t>(operand.v)) {
      return Value(-std::get<tn_int_t>(operand.v));
    } else if (std::holds_alternative<tn_dec_t>(operand.v)) {
      return Value(-std::get<tn_dec_t>(operand.v));
    } else {
      diags.report<TypeError>("Unary minus operator applied to non-mueric type",
                              operand.span, "", filename);
    }
  }

  return Value();
}

void Evaluator::exitErrors() {
  diags.print_errors();
  exit(1);
}
