#pragma once

#include <cstdint>

enum TypeTag {
	INT,
	FLOAT,
	STRING,
	BOOL
};

struct DynamicValue {
	int32_t tag;
	void* data;
};