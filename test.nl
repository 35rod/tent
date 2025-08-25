set buf = "";
read_in("buf", 4);
if buf == (chr(0x1b) + "[A" + chr(0x0a)) {
	println("pressed up");
}
