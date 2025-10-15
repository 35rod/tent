load "io";
load "stdnl";
load "file.nl";

f = File(ARGS@0);
if !f.isOpen() {
	println("failed to open file '" + ARGS@0 + "'");
	exit();
}

s = f.readFile();
print(s);

f.close();
