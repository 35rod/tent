form add(a, b) {
	return a + b;
}

while (true) {
	if ARG_COUNT > 2 {
		println("arg count > 2");
	} else if ARG_COUNT > 1 {
		println("arg count > 1");
	} else {
		println("arg count < 1");
	}
}