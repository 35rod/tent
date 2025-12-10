#include "dynamic_value.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

template <typename T>
static T* alloc_with_val(T val) {
      T* const p = (T*)malloc(sizeof(T));
      *p = val;
      return p;
}

static char* alloc_string_copy(const char* str) {
	const size_t len = strlen(str) + 1;
	char* const copy = (char*)malloc(len);
	if (copy) memcpy(copy, str, len);
	return copy;
}

extern "C" DynamicValue box_int(int64_t val) {
	DynamicValue dv;
	dv.tag = TypeTag::INT;
	dv.data = alloc_with_val(val);
	return dv;
}

extern "C" DynamicValue box_float(double val) {
	DynamicValue dv;
	dv.tag = TypeTag::FLOAT;
	dv.data = alloc_with_val(val);
	return dv;
}

extern "C" DynamicValue box_string(const char* val) {
	DynamicValue dv;
	dv.tag = TypeTag::STRING;
	dv.data = alloc_string_copy(val);
	return dv;
}

extern "C" DynamicValue box_bool(bool val) {
	DynamicValue dv;
	dv.tag = TypeTag::BOOL;
	dv.data = alloc_with_val(val);
	return dv;
}
