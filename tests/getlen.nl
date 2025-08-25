if ARG_COUNT > 0 {
	print(len(ARGS@0));
	exit();
}
set l = 0;
while getc() != EOF {
	++l;
}
print(l);
