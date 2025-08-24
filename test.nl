set MAX = 10;
if ARG_COUNT > 0 {
	MAX = stoll(ARGS@0);
}
set a = 0;
set b = 0;
while a < MAX {
	b = 0;
	while b <= a {
		print("*");
		b = b + 1;
	}
	println("");
	a = a + 1;
}
