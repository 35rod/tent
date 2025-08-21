form not(b) {
	println(b ^ 1); ` B XOR 1 flips the least significant bit, effectively working as a substitute NOT operation
}

set my_int = 0b101;
set index = 2;
println((my_int >> index) & 1);
