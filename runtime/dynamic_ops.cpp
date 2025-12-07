#include "dynamic_value.hpp"
#include "box_ops.hpp"
#include <iostream>

extern "C" void free_dynamic_value(DynamicValue dv) {
	if (!dv.data) return;

	free(dv.data);
}

static int64_t unbox_int(DynamicValue dv) {
	return *(int64_t*)dv.data;
}

static double unbox_float(DynamicValue dv) {
	return *(double*)dv.data;
}

extern "C" bool unbox_bool(DynamicValue dv) {
	return *(bool*)dv.data;
}

static DynamicValue coerce_to_float(DynamicValue dv) {
	if (dv.tag == TypeTag::FLOAT) return dv;
	if (dv.tag == TypeTag::INT) return box_float(unbox_int(dv));

	std::cerr << "Error: Cannot coerce type " << dv.tag << " to float" << std::endl;
	exit(1);
}

static DynamicValue compare_floats(DynamicValue L, DynamicValue R, const char* op) {
	DynamicValue FL = coerce_to_float(L);
	DynamicValue FR = coerce_to_float(R);
	double lv = unbox_float(FL);
	double rv = unbox_float(FR);

	bool result;

	if (strcmp(op, "==") == 0) result = (lv == rv);
	else if (strcmp(op, "!=") == 0) result = (lv != rv);
	else if (strcmp(op, "<") == 0) result = (lv < rv);
	else if (strcmp(op, ">") == 0) result = (lv > rv);
	else if (strcmp(op, "<=") == 0) result = (lv <= rv);
	else if (strcmp(op, ">=") == 0) result = (lv >= rv);
	else {
		std::cerr << "Error: Invalid comparison op" << std::endl;
		exit(1);
	}

	return box_bool(result);
}

extern "C" DynamicValue dynamic_eqeq(DynamicValue L, DynamicValue R) {
	if ((L.tag == TypeTag::INT || L.tag == TypeTag::FLOAT) &&
		(R.tag == TypeTag::INT || R.tag == TypeTag::FLOAT)
	) {
		return compare_floats(L, R, "==");
	} else if (L.tag == TypeTag::STRING && R.tag == TypeTag::STRING) {
		return box_bool(strcmp((char*)L.data, (char*)R.data) == 0);
	} else {
		return box_bool(L.tag == R.tag);
	}
}

extern "C" DynamicValue dynamic_noteq(DynamicValue L, DynamicValue R) {
	if ((L.tag == TypeTag::INT || L.tag == TypeTag::FLOAT) &&
		(R.tag == TypeTag::INT || R.tag == TypeTag::FLOAT)
	) {
		return compare_floats(L, R, "!=");
	} else if (L.tag == TypeTag::STRING && R.tag == TypeTag::STRING) {
		return box_bool(strcmp((char*)L.data, (char*)R.data) != 0);
	} else {
		return box_bool(L.tag != R.tag);
	}
}

// NEED TO IMPLEMENT OTHER COMPARISON OPERATORS, BUT FOR NOW, THIS WORKS :)

extern "C" DynamicValue dynamic_add(DynamicValue L, DynamicValue R) {
	if (L.tag == TypeTag::INT && R.tag == TypeTag::INT) {
		return box_int(unbox_int(L) + unbox_int(R));
	} else if ((L.tag == TypeTag::INT || L.tag == TypeTag::FLOAT) &&
		(R.tag == TypeTag::INT || R.tag == TypeTag::FLOAT)
	) {
		DynamicValue FL = coerce_to_float(L);
		DynamicValue FR = coerce_to_float(R);

		return box_float(unbox_float(FL) + unbox_float(FR));
	}

	std::cout << L.tag << std::endl;
	std::cout << R.tag << std::endl;

	std::cerr << "Cannot add types" << std::endl;
	exit(1);
}

extern "C" void print_dynamic_value(DynamicValue dv) {
	switch (dv.tag) {
		case TypeTag::INT: printf("%lld\n", unbox_int(dv)); break;
		case TypeTag::FLOAT: {
			double val = unbox_float(dv);
			printf("%g\n", val);
			break;
		} case TypeTag::STRING: printf("%s\n", (char*)dv.data); break;
		case TypeTag::BOOL: printf("%s\n", unbox_bool(dv) ? "true" : "false"); break;
		default: printf("Cannot print unknown type: %d\n", dv.tag);
	}
}