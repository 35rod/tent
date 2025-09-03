set x = 0;

if x == 0 {
	println("Yay!");
	x = x + 1;
} else if x == 1 {
	println("Wow!");
	x = -1;
} else {
	println("No...");
	x = x - 1;
}

println(x);