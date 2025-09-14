#pragma once

#include <variant>
#include <vector>
#include <cstdint>
#include <string>
#include "opcodes.hpp"
#include "misc.hpp"

typedef int64_t nl_int_t;
typedef float nl_dec_t;
typedef bool nl_bool_t;

class FunctionLiteral;

struct Value {
    struct ClassInstance {
        std::string name;
        std::unordered_map<std::string, Value> fields;
        std::unordered_map<std::string, FunctionLiteral*> methods;
    };

    using VecT = std::shared_ptr<std::vector<Value>>;
    std::variant<nl_int_t, nl_dec_t, nl_bool_t, std::string, VecT, ClassInstance, NoOp> v;
    bool isReturn = false;

    Value() : v(NoOp()), isReturn(false) {}
    Value(nl_int_t i) : v(i), isReturn(false) {}
    Value(nl_dec_t d) : v(d), isReturn(false) {}
    Value(nl_bool_t b) : v(b), isReturn(false) {}
    Value(std::string s) : v(s), isReturn(false) {}
    Value(VecT vec) : v(vec), isReturn(false) {}
    Value(ClassInstance ci) : v(ci), isReturn(false) {}
    Value(NoOp n) : v(n), isReturn(false) {}
};

inline Value make_vec(const std::vector<Value>& elems) {
    return Value(std::make_shared<std::vector<Value>>(elems));
}

int64_t ipow(int64_t base, uint8_t exp);
bool isRightAssoc(const TokenType& op);
