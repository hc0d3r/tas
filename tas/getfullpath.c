#include "tas/getfullpath.h"
#include "tas/globals.h"

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int isgroupmember(gid_t gid);

char *getfullpath(const char *filename)
{
	char *aux, *path, *pathname, *ret = NULL;
	struct stat st;

	path = strdup(getenv("PATH"));

	if (path == NULL)
		return NULL;

	pathname = malloc(strlen(path) + strlen(filename) + 1);

 	do {
		if ((aux = strchr(path, ':'))) {
			*aux = 0x0;
		}

		sprintf(pathname, "%s/%s", path, filename);

		if (stat(pathname, &st) == 0 && st.st_ino != inode_ign) {
			if(st.st_mode & S_IXOTH) {
				ret = pathname;
				break;
			}

			if((st.st_mode & S_IXUSR) && getuid() == st.st_uid) {
				ret = pathname;
				break;
			}

			if((st.st_mode & S_IXGRP) && isgroupmember(st.st_gid)) {
				ret = pathname;
				break;
			}
		}

		path = aux + 1;
	} while (aux);

	if (ret == NULL)
		free(pathname);

	return ret;
}

static int isgroupmember(gid_t gid)
{
	gid_t *gplist;
	int ngroups, i;

	if ((ngroups = getgroups(0, NULL)) == -1) {
		return 0;
	}

	gplist = malloc(sizeof(gid_t) * ngroups);
	if (gplist && getgroups(ngroups, gplist) != -1) {
		for (i = 0; i < ngroups; i++) {
			if (gplist[i] == gid) {
				free(gplist);
				return 1;
			}
		}

		free(gplist);
	}

	return 0;
}
