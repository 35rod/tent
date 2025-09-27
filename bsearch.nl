form bsearch(srcvec, targ) {
	low = 0;
	high = srcvec.len() - 1;
	mid = -1;

	while low <= high {
		mid = low + (high - low)/2;
		if srcvec@mid < targ {
			low = mid + 1;
		} else if srcvec@mid > targ {
			high = mid - 1;
		} else if srcvec@mid == targ {
			return mid;
		}
	}

	return -1;
}
