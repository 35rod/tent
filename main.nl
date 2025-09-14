load "io";

form nth_fib(n) {
	if n <= 1 {
		return n;
	}

	return nth_fib(n-1) + nth_fib(n-2);
}

set n = 6;
println(nth_fib(n));