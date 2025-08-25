#include <stdint.h>
#include <string.h>
#include <stdio.h>

int32_t main(int32_t argc, char **argv)
{
	if (argc > 1) {
		printf("%zu", strlen(argv[1]));
		return 0;
	}
	size_t l = 0;
	while (fgetc(stdin) != EOF)
		++l;

	printf("%zu", l);
	return 0;
}
