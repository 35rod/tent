#pragma once

#include <variant>
#include <vector>
#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include "opcodes.hpp"
#include "misc.hpp"

typedef int64_t nl_int_t;
typedef float nl_dec_t;
typedef bool nl_bool_t;

class FunctionStmt;
struct Value;

struct Value {
    struct ClassInstance {
        std::string name;
        std::unordered_map<std::string, Value> fields;
        std::unordered_map<std::string, FunctionStmt*> methods;

        ClassInstance(std::string name_init) : name(name_init) {};
    };

    using VecT = std::shared_ptr<std::vector<Value>>;
    using DicT = std::shared_ptr<std::map<Value, Value>>;
    std::variant<nl_int_t, nl_dec_t, nl_bool_t, std::string, VecT, DicT, ClassInstance, NullLiteral> v;
    bool typeInt = false;
    bool typeFloat = false;
    bool typeStr = false;
    bool typeBool = false;
    bool typeVec = false;
    bool typeDic = false;
    bool isReturn = false;
    bool isExit = false;

    Value() : v(NullLiteral()) {}
    Value(nl_int_t i) : v(i) {}
    Value(nl_dec_t d) : v(d) {}
    Value(nl_bool_t b) : v(b) {}
    Value(std::string s) : v(s) {}
    Value(VecT vec) : v(vec) {}
    Value(DicT dic) : v(dic) {}
    Value(ClassInstance ci) : v(ci) {}
};

inline Value make_vec(const std::vector<Value>& elems) {
    return Value(std::make_shared<std::vector<Value>>(elems));
}

inline constexpr bool is_primitive_val(const Value& val) {
	return !std::holds_alternative<NullLiteral>(val.v) && !std::holds_alternative<Value::ClassInstance>(val.v);
}

int64_t ipow(int64_t base, uint8_t exp);
bool isRightAssoc(const TokenType& op);
