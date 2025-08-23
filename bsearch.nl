if ARG_COUNT < 2 {
	println("you need to pass 1 argument for the target and at least one extra for the vector to search");
	exit();
}

form bsearch(srcvec, targ) {
	set low = 0;
	set high = (veclen(srcvec)-1 - 1);
	set mid = 0-1;

	while low <= high {
		mid = low + (high - low)/2;
		if stoll(srcvec@(mid+1)) == targ {
			return mid;
		}
		if stoll(srcvec@(mid+1)) < targ {
			low = mid + 1;
		}
		if stoll(srcvec@(mid+1)) > targ {
			high = mid - 1;
		}
	}

	return 0-1;
}

set targ = stoll(ARGS@0);

` convert each element of ARGS to an int and put them in ARR.
set res = bsearch(ARGS, targ);
if res < 0 {
	println("did not find ", targ);
	exit();
}
println("found ", targ, " at index ", res);
