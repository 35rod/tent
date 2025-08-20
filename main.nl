set a = 0;
set b = 0;
while a < 10 {
	b = 0;
	while b <= a {
		print("*");
		b = b + 1;
	}
	println("");
	a = a + 1;
}
