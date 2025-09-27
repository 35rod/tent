#pragma once

#include <variant>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include "opcodes.hpp"
#include "misc.hpp"

typedef int64_t nl_int_t;
typedef float nl_dec_t;
typedef bool nl_bool_t;

class FunctionLiteral;
struct Value;

struct Value {
    struct ClassInstance {
        std::string name;
        std::unordered_map<std::string, Value> fields;
        std::unordered_map<std::string, FunctionLiteral*> methods;

        ClassInstance(std::string name_init) : name(name_init) {};
    };

    using VecT = std::shared_ptr<std::vector<Value>>;
    std::variant<nl_int_t, nl_dec_t, nl_bool_t, std::string, VecT, ClassInstance, NoOp> v;
    bool typeInt = false;
    bool typeFloat = false;
    bool typeStr = false;
    bool typeBool = false;
    bool typeVec = false;
    bool isReturn = false;
    bool isExit = false;

    Value() : v(NoOp()) {}
    Value(nl_int_t i) : v(i) {}
    Value(nl_dec_t d) : v(d) {}
    Value(nl_bool_t b) : v(b) {}
    Value(std::string s) : v(s) {}
    Value(VecT vec) : v(vec) {}
    Value(ClassInstance ci) : v(ci) {}
    Value(NoOp n) : v(n) {}
};

inline Value make_vec(const std::vector<Value>& elems) {
    return Value(std::make_shared<std::vector<Value>>(elems));
}

inline constexpr bool is_primitive_val(const Value& val) {
	return !std::holds_alternative<NoOp>(val.v) && !std::holds_alternative<Value::ClassInstance>(val.v);
}

int64_t ipow(int64_t base, uint8_t exp);
bool isRightAssoc(const TokenType& op);
