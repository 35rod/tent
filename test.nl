load "io";

inline nth_fib(n) {
    if n <= 1 {
        return n;
    }

    return nth_fib(n-1) + nth_fib(n-2);
}

println(nth_fib(20));