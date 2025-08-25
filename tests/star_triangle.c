#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int32_t main(int32_t argc, char **argv)
{
	int64_t MAX = 10;
	if (argc > 1)
		MAX = strtoll(argv[1], NULL, 10);

	for (int64_t a = 0; a < MAX; ++a)
	{
		for (int64_t b = 0; b <= a; ++b)
			fputc('*', stdout);
		fputc('\n', stdout);
	}	

	return 0;
}
