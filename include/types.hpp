#pragma once

#include "misc.hpp"
#include "opcodes.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

typedef int64_t tn_int_t;
typedef double tn_dec_t;
typedef bool tn_bool_t;

class FunctionStmt;
struct Value;

struct Value {
  struct ClassInstance {
    std::string name;
    std::string moduleKey;
    std::unordered_map<std::string, Value> fields;
    std::unordered_map<std::string, FunctionStmt *> methods;

    ClassInstance(std::string name_init, std::string key_init = "")
        : name(std::move(name_init)), moduleKey(std::move(key_init)) {};
  };

  struct ModuleRef {
    std::string name;
    std::string key;

    ModuleRef(std::string moduleName, std::string moduleKey)
        : name(std::move(moduleName)), key(std::move(moduleKey)) {}
  };

  using VecT = std::shared_ptr<std::vector<Value>>;
  using DicT = std::shared_ptr<std::map<std::string, Value>>;
  std::variant<tn_int_t, tn_dec_t, tn_bool_t, std::string, VecT, DicT,
               ClassInstance, ModuleRef, NullLiteral>
      v;
  Span span;
  bool typeInt = false;
  bool typeFloat = false;
  bool typeStr = false;
  bool typeBool = false;
  bool typeVec = false;
  bool typeDic = false;
  bool isReturn = false;
  bool isExit = false;

  Value() : v(NullLiteral()) {}
  Value(tn_int_t i) : v(i) {}
  Value(tn_dec_t d) : v(d) {}
  Value(tn_bool_t b) : v(b) {}
  Value(std::string s) : v(s) {}
  Value(VecT vec) : v(vec) {}
  Value(DicT dic) : v(dic) {}
  Value(ClassInstance ci) : v(ci) {}
  Value(ModuleRef module) : v(std::move(module)) {}

  Value &setSpan(Span newSpan) {
    span = newSpan;
    return *this;
  }

  std::string getTypeName() const {
    if (std::holds_alternative<tn_int_t>(v)) {
      return "int";
    } else if (std::holds_alternative<tn_dec_t>(v)) {
      return "float";
    } else if (std::holds_alternative<tn_bool_t>(v)) {
      return "bool";
    } else if (std::holds_alternative<std::string>(v)) {
      return "string";
    } else if (std::holds_alternative<VecT>(v)) {
      return "vector";
    } else if (std::holds_alternative<DicT>(v)) {
      return "dictionary";
    } else if (std::holds_alternative<ClassInstance>(v)) {
      return std::get<ClassInstance>(v).name;
    } else if (std::holds_alternative<ModuleRef>(v)) {
      return "module";
    } else if (std::holds_alternative<NullLiteral>(v)) {
      return "null";
    }
    return "unknown";
  }
};

inline Value make_vec(const std::vector<Value> &elems) {
  return Value(std::make_shared<std::vector<Value>>(elems));
}

inline constexpr bool is_primitive_val(const Value &val) {
  return !std::holds_alternative<NullLiteral>(val.v) &&
         !std::holds_alternative<Value::ClassInstance>(val.v) &&
         !std::holds_alternative<Value::ModuleRef>(val.v);
}

static inline std::string_view getLineText(const std::string &source,
                                           size_t line) {
  size_t startIndex = 0;
  size_t currentLine = 1;

  while (currentLine < line) {
    size_t next_newline = source.find('\n', startIndex);

    if (next_newline == std::string::npos) {
      return "";
    }

    startIndex = next_newline + 1;
    currentLine++;
  }

  size_t end = source.find('\n', startIndex);

  if (end == std::string::npos) {
    return std::string_view(source.data() + startIndex);
  }

  return std::string_view(source.data() + startIndex, end - startIndex);
}

int64_t ipow(int64_t base, uint8_t exp);
bool isRightAssoc(const TokenType &op);
bool getCompoundAssignOp(const TokenType &op, TokenType &out);
