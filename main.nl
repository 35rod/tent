set i = 0;

while i < 1000 {
	if i == 55 {
		i++;
		continue;
	} else if i > 90 {
		if i == 99 {
			i++;
			continue;
		}
	}

	println(i);
	i++;
}