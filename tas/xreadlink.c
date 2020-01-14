#include "tas/xreadlink.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char *xreadlink(const char *path)
{
	char *ptr, *chunk = NULL;
	ssize_t n = 0, len = 128;

loop:
	if ((ptr = realloc(chunk, len)) == NULL) {
		if(chunk) chunk[n] = 0x0;
		return chunk;
	}

	n = readlink(path, ptr, len);

	if (n == -1) {
		free(ptr);
		ptr = NULL;
	}

	else if (n == len) {
		len <<= 1;
		goto loop;
	}

	else {
		ptr[n] = 0x0;
	}

	return ptr;
}
