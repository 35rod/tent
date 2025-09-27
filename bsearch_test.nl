load "io";
load "stdnl";
load "bsearch.nl";

println("please enter numbers in sorted order");
in_vec = [];
cur = input("enter number: ");
while cur != "end" {
	v = int.parse(cur);
	if !isErr(v) {
		in_vec.push(v);
	} else {
		println("invalid integer");
	}
	cur = input("enter number: ");
}
target = int.parse(input("target number: "));
res = bsearch(in_vec, target);
if res == -1 {
	println("target value ", target, " not found");
} else {
	println("found target value ", target, " at index ", res);
}
