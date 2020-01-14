#include "tas/tas-execv.h"
#include "tas/globals.h"
#include "tas/getinode.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int tas_execv(const char *cmd, char * const *argv)
{
	char *path, *pathname, *aux;

	if (strchr(cmd, '/'))
		return execv(cmd, argv);

	path = getenv("PATH");
	if (path == NULL) {
		return execvp(cmd, argv);
	}

	else {
		pathname = malloc(strlen(cmd) + strlen(path) + 2);

		do {
			if ((aux = strchr(path, ':'))) {
				aux[0] = 0x0;
			}

			sprintf(pathname, "%s/%s", path, cmd);

			if (getinode(pathname) != inode_ign) {
				execv(pathname, argv);
			}

			path = aux + 1;
		} while(aux);
	}

	return -1;
}
