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

typedef int64_t tn_int_t;
typedef double tn_dec_t;
typedef bool tn_bool_t;

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
    using DicT = std::shared_ptr<std::map<std::string, Value>>;
    std::variant<tn_int_t, tn_dec_t, tn_bool_t, std::string, VecT, DicT, ClassInstance, NullLiteral> v;
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
};

inline Value make_vec(const std::vector<Value>& elems) {
    return Value(std::make_shared<std::vector<Value>>(elems));
}

inline constexpr bool is_primitive_val(const Value& val) {
	return !std::holds_alternative<NullLiteral>(val.v) && !std::holds_alternative<Value::ClassInstance>(val.v);
}

static inline std::string getLineText(std::string source, int line) {
    size_t start = 0;
    int current = 1;

    while (current < line) {
        size_t next_newline = source.find('\n', start);

        if (next_newline == std::string::npos) {
            return ""; 
        }

        start = next_newline + 1;
        current++;
    }

    size_t end = source.find('\n', start);

    if (end == std::string::npos) {
        return source.substr(start);
    }

    return source.substr(start, end - start);
}

int64_t ipow(int64_t base, uint8_t exp);
bool isRightAssoc(const TokenType& op);
