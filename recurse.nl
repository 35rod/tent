load "io";

set count = 0;

form recurse() {
	count++;

	println("yay!");

	while count < 5 {
		recurse();
	}

	return;
}

recurse();