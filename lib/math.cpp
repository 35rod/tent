#include "native.hpp"
#include <iostream>
#include <cmath>
#include <random>

#define generic_math_func_1arg(func_name, ret_for_int)										\
Value tn_math__##func_name(const std::vector<Value>& args) { 									\
	if (std::holds_alternative<tn_int_t>(args[0].v)) { 										\
		return Value(ret_for_int(std::func_name(std::get<tn_int_t>(args[0].v)))); 						\
	} else if (std::holds_alternative<tn_dec_t>(args[0].v)) { 									\
		return Value(tn_dec_t(std::func_name(std::get<tn_dec_t>(args[0].v)))); 						\
	} else { 																	\
		std::cerr << "Passed non-numeric argument to first parameter of `" #func_name "`" << std::endl; 	\
	} 																		\
																			\
	return Value(); 																\
}
#define generic_math_func_1arg_dec(func_name) generic_math_func_1arg(func_name, tn_dec_t)

#define generic_math_func_2arg(func_name, ret_for_int)										\
Value tn_math__##func_name(const std::vector<Value>& args) { 									\
	if (std::holds_alternative<tn_int_t>(args[0].v) && std::holds_alternative<tn_int_t>(args[1].v)) { 		\
		return Value(ret_for_int(std::func_name(std::get<tn_int_t>(args[0].v), std::get<tn_int_t>(args[1].v)))); \
	} else if (std::holds_alternative<tn_dec_t>(args[0].v) && std::holds_alternative<tn_dec_t>(args[1].v)) { 	\
		return Value(tn_dec_t(std::func_name(std::get<tn_dec_t>(args[0].v), std::get<tn_dec_t>(args[1].v)))); \
	} else if (std::holds_alternative<tn_dec_t>(args[0].v) && std::holds_alternative<tn_int_t>(args[1].v)) { 	\
		return Value(tn_dec_t(std::func_name(std::get<tn_dec_t>(args[0].v), std::get<tn_int_t>(args[1].v)))); \
	} else if (std::holds_alternative<tn_int_t>(args[0].v) && std::holds_alternative<tn_dec_t>(args[1].v)) { 	\
		return Value(tn_dec_t(std::func_name(std::get<tn_int_t>(args[0].v), std::get<tn_dec_t>(args[1].v)))); \
	} else { 																	\
		std::cerr << "Passed incompatible and/or non-numeric arguments to parameters of `" #func_name "`" << std::endl; \
	} 																		\
																			\
	return Value(); 																\
}
#define generic_math_func_2arg_dec(func_name) generic_math_func_2arg(func_name, tn_dec_t)

generic_math_func_1arg_dec(log);
generic_math_func_1arg_dec(log10);
generic_math_func_1arg_dec(log2);

generic_math_func_1arg_dec(cos);
generic_math_func_1arg_dec(sin);
generic_math_func_1arg_dec(tan);
generic_math_func_1arg_dec(acos);
generic_math_func_1arg_dec(asin);
generic_math_func_1arg_dec(atan);

generic_math_func_1arg_dec(cosh);
generic_math_func_1arg_dec(sinh);
generic_math_func_1arg_dec(tanh);
generic_math_func_1arg_dec(acosh);
generic_math_func_1arg_dec(asinh);
generic_math_func_1arg_dec(atanh);

generic_math_func_1arg(floor, tn_int_t);
generic_math_func_1arg(ceil, tn_int_t);
generic_math_func_1arg(round, tn_int_t);
generic_math_func_1arg(abs, tn_int_t);
generic_math_func_1arg_dec(sqrt);
generic_math_func_2arg_dec(atan2);
tn_int_t signi(tn_int_t i) {
	return (i > 0) ? 1 : ((i < 0) ? -1 : 0);
}
tn_int_t signf(tn_dec_t f) {
	return (f > 0) ? 1 : ((f < 0) ? -1 : 0);
}
Value tn_math__sign(const std::vector<Value>& args) { 							\
	if (std::holds_alternative<tn_int_t>(args[0].v)) { 							\
		return Value(signi(std::get<tn_int_t>(args[0].v))); 						\
	} else if (std::holds_alternative<tn_dec_t>(args[0].v)) { 						\
		return Value(signf((std::get<tn_dec_t>(args[0].v)))); 					\
	} else { 														\
		std::cerr << "Passed non-numeric argument to first parameter of `sign`" << std::endl; \
	} 															\
																\
	return Value(); 													\
}

Value tn_math__random(const std::vector<Value>&) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    return Value((tn_dec_t)dis(gen));
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["ln"]		= tn_math__log;
	table["log10"]	= tn_math__log10;
	table["log2"]	= tn_math__log2;
	table["cos"]	= tn_math__cos;
	table["sin"]	= tn_math__sin;
	table["tan"]	= tn_math__tan;
	table["acos"]	= tn_math__acos;
	table["asin"]	= tn_math__asin;
	table["atan"]	= tn_math__atan;
	table["cosh"]	= tn_math__cosh;
	table["sinh"]	= tn_math__sinh;
	table["tanh"]	= tn_math__tanh;
	table["acosh"]	= tn_math__acosh;
	table["asinh"]	= tn_math__asinh;
	table["atanh"]	= tn_math__atanh;
	table["floor"]	= tn_math__floor;
	table["ceil"]	= tn_math__ceil;
	table["round"]	= tn_math__round;
	table["abs"]	= tn_math__abs;
	table["sqrt"]	= tn_math__sqrt;
	table["atan2"]	= tn_math__atan2;
	table["rand"]	= tn_math__random;
	table["sign"]	= tn_math__sign;
}
