#pragma once

#include <variant>
#include <cstdint>
#include <string>

#include "opcodes.hpp"

typedef int64_t nl_int_t;
typedef float nl_dec_t;
typedef bool nl_bool_t;

struct Value {
    using VecT = std::vector<std::shared_ptr<Value>>;
    std::variant<nl_int_t, nl_dec_t, nl_bool_t, std::string, VecT, NoOp> v;

    Value() : v(NoOp()) {}
    Value(nl_int_t i) : v(i) {}
    Value(nl_dec_t d) : v(d) {}
    Value(nl_bool_t b) : v(b) {}
    Value(std::string s) : v(s) {}
    Value(VecT vec) : v(vec) {}
    Value(NoOp n) : v(n) {}
};

inline Value make_vec(const std::vector<Value>& elems) {
    return Value(std::make_shared<Value::VecT>(elems));
}

int64_t ipow(int64_t base, uint8_t exp);
bool isRightAssoc(const TokenType& op);
