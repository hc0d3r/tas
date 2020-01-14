#include "tas/globals.h"
#include "tas/xreadlink.h"
#include "tas/getinode.h"

__attribute__((constructor))
static void init_some_shit(void)
{
	current_exe = xreadlink("/proc/self/exe");
	if (current_exe)
		inode_ign = getinode(current_exe);
	else
		inode_ign = 0;
}
