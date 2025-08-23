#ifndef MISC_HPP
#define MISC_HPP

#include <variant>

typedef int64_t nl_int_t;
typedef float nl_dec_t;
typedef bool nl_bool_t;

using NonVecEvalExpr = std::variant<nl_int_t, nl_dec_t, nl_bool_t, std::string>;

#endif /* MISC_HPP */
