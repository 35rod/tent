load "bsearch.nl";

println("please enter numbers in sorted order");
set in_vec = [];
set cur = input("enter number: ");
while cur != "end" {
	vecpush("in_vec", stoll(cur));
	cur = input("enter number: ");
}
set target = stoll(input("target number: "));
set res = bsearch(in_vec, target);
if res == -1 {
	println("target value ", target, " not found");
	exit();
}
println("found target value ", target, " at index ", res);
