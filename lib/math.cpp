#include "native.hpp"
#include <iostream>
#include <cmath>
#include <random>

#define generic_math_func_1arg(func_name, ret_for_int)										\
Value nl_math__##func_name(const std::vector<Value>& args) { 									\
	if (std::holds_alternative<nl_int_t>(args[0].v)) { 										\
		return Value(ret_for_int(std::func_name(std::get<nl_int_t>(args[0].v)))); 						\
	} else if (std::holds_alternative<nl_dec_t>(args[0].v)) { 									\
		return Value(nl_dec_t(std::func_name(std::get<nl_dec_t>(args[0].v)))); 						\
	} else { 																	\
		std::cerr << "Passed non-numeric argument to first parameter of `" #func_name "`" << std::endl; 	\
	} 																		\
																			\
	return Value(); 																\
}
#define generic_math_func_1arg_dec(func_name) generic_math_func_1arg(func_name, nl_dec_t)

#define generic_math_func_2arg(func_name, ret_for_int)										\
Value nl_math__##func_name(const std::vector<Value>& args) { 									\
	if (std::holds_alternative<nl_int_t>(args[0].v) && std::holds_alternative<nl_int_t>(args[1].v)) { 		\
		return Value(ret_for_int(std::func_name(std::get<nl_int_t>(args[0].v), std::get<nl_int_t>(args[1].v)))); \
	} else if (std::holds_alternative<nl_dec_t>(args[0].v) && std::holds_alternative<nl_dec_t>(args[1].v)) { 	\
		return Value(nl_dec_t(std::func_name(std::get<nl_dec_t>(args[0].v), std::get<nl_dec_t>(args[1].v)))); \
	} else if (std::holds_alternative<nl_dec_t>(args[0].v) && std::holds_alternative<nl_int_t>(args[1].v)) { 	\
		return Value(nl_dec_t(std::func_name(std::get<nl_dec_t>(args[0].v), std::get<nl_int_t>(args[1].v)))); \
	} else if (std::holds_alternative<nl_int_t>(args[0].v) && std::holds_alternative<nl_dec_t>(args[1].v)) { 	\
		return Value(nl_dec_t(std::func_name(std::get<nl_int_t>(args[0].v), std::get<nl_dec_t>(args[1].v)))); \
	} else { 																	\
		std::cerr << "Passed incompatible and/or non-numeric arguments to parameters of `" #func_name "`" << std::endl; \
	} 																		\
																			\
	return Value(); 																\
}
#define generic_math_func_2arg_dec(func_name) generic_math_func_2arg(func_name, nl_dec_t)

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

generic_math_func_1arg(floor, nl_int_t);
generic_math_func_1arg(ceil, nl_int_t);
generic_math_func_1arg(round, nl_int_t);
generic_math_func_1arg(abs, nl_int_t);
generic_math_func_1arg_dec(sqrt);
generic_math_func_2arg_dec(atan2);
nl_int_t signi(nl_int_t i) {
	return (i > 0) ? 1 : ((i < 0) ? -1 : 0);
}
nl_int_t signf(nl_dec_t f) {
	return (f > 0) ? 1 : ((f < 0) ? -1 : 0);
}
Value nl_math__sign(const std::vector<Value>& args) { 									\
	if (std::holds_alternative<nl_int_t>(args[0].v)) { 										\
		return Value(signi(std::get<nl_int_t>(args[0].v))); 						\
	} else if (std::holds_alternative<nl_dec_t>(args[0].v)) { 									\
		return Value(signf((std::get<nl_dec_t>(args[0].v)))); 						\
	} else { 																	\
		std::cerr << "Passed non-numeric argument to first parameter of `sign`" << std::endl; 	\
	} 																		\
																			\
	return Value(); 																\
}

Value nl_math__random(const std::vector<Value>&) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    return Value((nl_dec_t)dis(gen));
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["ln"]		= nl_math__log;
	table["log10"]	= nl_math__log10;
	table["log2"]	= nl_math__log2;
	table["cos"]	= nl_math__cos;
	table["sin"]	= nl_math__sin;
	table["tan"]	= nl_math__tan;
	table["acos"]	= nl_math__acos;
	table["asin"]	= nl_math__asin;
	table["atan"]	= nl_math__atan;
	table["cosh"]	= nl_math__cosh;
	table["sinh"]	= nl_math__sinh;
	table["tanh"]	= nl_math__tanh;
	table["acosh"]	= nl_math__acosh;
	table["asinh"]	= nl_math__asinh;
	table["atanh"]	= nl_math__atanh;
	table["floor"]	= nl_math__floor;
	table["ceil"]	= nl_math__ceil;
	table["round"]	= nl_math__round;
	table["abs"]	= nl_math__abs;
	table["sqrt"]	= nl_math__sqrt;
	table["atan2"]	= nl_math__atan2;
<<<<<<< Updated upstream
	table["rand"] = nl_math__random;
=======
	table["sign"]	= nl_math__sign;
>>>>>>> Stashed changes
}
