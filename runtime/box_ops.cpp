#include "dynamic_value.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

static int64_t* alloc_int64(int64_t val) {
	int64_t* p = (int64_t*)malloc(sizeof(int64_t));
	*p = val;
	return p;
}

static double* alloc_double(double val) {
	double* p = (double*)malloc(sizeof(double));
	*p = val;
	return p;
}

static char* alloc_string_copy(const char* str) {
	size_t len = strlen(str) + 1;
	char* copy = (char*)malloc(len);
	if (copy) memcpy(copy, str, len);
	return copy;
}

static bool* alloc_bool(bool val) {
	bool* p = (bool*)malloc(sizeof(bool));
	*p = val;
	return p;
}

extern "C" DynamicValue box_int(int64_t val) {
	DynamicValue dv;
	dv.tag = TypeTag::INT;
	dv.data = alloc_int64(val);
	return dv;
}

extern "C" DynamicValue box_float(double val) {
	DynamicValue dv;
	dv.tag = TypeTag::FLOAT;
	dv.data = alloc_double(val);
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
	dv.data = alloc_bool(val);
	return dv;
}