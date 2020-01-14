#include "tas/getinode.h"

ino_t getinode(const char *file)
{
	struct stat st;
	if (stat(file, &st) != -1) {
		return st.st_ino;
	}

	return -1;
}
