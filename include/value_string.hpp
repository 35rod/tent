#pragma once

#include <string>
#include <sstream>
#include <cstdio>
#include <map>
#include <variant>
#include "types.hpp"

#define VALUE_STRING_MAX_DEC_LEN 50

inline std::string value_to_string(const Value& val, bool quote_string = false);

inline std::string vec_to_string(const Value::VecT& vecPtr) {
	std::ostringstream oss;
	oss << "[";
	if (vecPtr) {
		for (size_t i = 0; i < vecPtr->size(); i++) {
			oss << value_to_string((*vecPtr)[i], true);
			if (i + 1 < vecPtr->size()) oss << ", ";
		}
	}
	oss << "]";
	return oss.str();
}

inline std::string dic_to_string(const Value::DicT& dicPtr) {
	std::ostringstream oss;
	oss << "{";
	if (dicPtr) {
		for (auto it = dicPtr->begin(); it != dicPtr->end(); ++it) {
			oss << "\"" << it->first << "\": " << value_to_string(it->second, true);
			if (std::next(it) != dicPtr->end()) oss << ", ";
		}
	}
	oss << "}";
	return oss.str();
}

inline std::string value_to_string(const Value& val, bool quote_string) {
	if (std::holds_alternative<tn_int_t>(val.v))
		return std::to_string(std::get<tn_int_t>(val.v));
	else if (std::holds_alternative<tn_dec_t>(val.v)) {
		char str_buf[VALUE_STRING_MAX_DEC_LEN + 1];
		std::snprintf(str_buf, VALUE_STRING_MAX_DEC_LEN, "%.*g", 6, std::get<tn_dec_t>(val.v));
		return std::string(str_buf);
	} else if (std::holds_alternative<tn_bool_t>(val.v))
		return std::get<tn_bool_t>(val.v) ? "true" : "false";
	else if (std::holds_alternative<std::string>(val.v)) {
		if (quote_string)
			return "\"" + std::get<std::string>(val.v) + "\"";
		return std::get<std::string>(val.v);
	} else if (std::holds_alternative<Value::VecT>(val.v))
		return vec_to_string(std::get<Value::VecT>(val.v));
	else if (std::holds_alternative<Value::DicT>(val.v))
		return dic_to_string(std::get<Value::DicT>(val.v));
	else if (std::holds_alternative<Value::ClassInstance>(val.v))
		return "<" + std::get<Value::ClassInstance>(val.v).name + ">";
	else if (std::holds_alternative<Value::ModuleRef>(val.v))
		return "<module " + std::get<Value::ModuleRef>(val.v).name + ">";
	else
		return "null";
}
