#ifndef __TAS_GETINODE_H__
#define __TAS_GETINODE_H__

#include <sys/stat.h>

ino_t getinode(const char *file);

#endif
