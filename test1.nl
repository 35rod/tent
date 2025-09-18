load "io.dylib";
load "stdnl.dylib";
load "../file.nl";

set f = File("../main.nl");
if !f.isOpen() {
	exit();
}
set str = f.readFile();

println(str);

f.closeFile();
