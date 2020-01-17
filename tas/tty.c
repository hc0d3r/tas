#include "tas/tty.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pty.h>
#include <utmp.h>

#include <signal.h>
#include <errno.h>

volatile int sigwinch = 0;

// SIGWINCH signal handler
void sigwinch_handler(int s)
{
	(void)s;
	sigwinch = 1;
}

pid_t tas_forkpty(tas_tty *tty)
{
	struct termios tios, *tmptr;
	struct winsize ws, *wsptr;

	int *master = &(tty->master);

	wsptr = &ws;
	tmptr = &tios;

	if (ioctl(tty->stdin_fd, TIOCGWINSZ, wsptr) == -1) {
		wsptr = NULL;
	}

	if (tcgetattr(tty->stdin_fd, tmptr) == -1) {
		tmptr = NULL;
	}

	return forkpty(master, NULL, tmptr, wsptr);
}

void tty_loop(tas_tty *tty)
{
	struct pollfd pfd[2];
	struct winsize ws;

	char buf[1024], *bufptr;
	ssize_t n;

	pfd[0].fd = tty->stdin_fd;
	pfd[1].fd = tty->master;

	if (tty->stdin_fd == STDIN_FILENO) {
		signal(SIGWINCH, sigwinch_handler);
	}

	pfd[0].events = POLLIN;
	pfd[1].events = POLLIN;

	while (poll(pfd, 2, -1) != -1 || errno == EINTR) {
		// resize signal
		if (sigwinch) {
			if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != -1) {
				ioctl(tty->master, TIOCSWINSZ, &ws);
			}

			sigwinch = 0;
			continue;
		}

		bufptr = buf;

		if (pfd[0].revents & POLLIN) {
			if ((n = read(pfd[0].fd, buf, 1024)) <= 0)
				break;

			if (tty->input_hook)
				tty->input_hook(tty, &bufptr, (size_t *)&n);

			write(pfd[1].fd, bufptr, n);
		}

		if (pfd[1].revents & POLLIN) {
			if ((n = read(pfd[1].fd, buf, 1024)) <= 0)
				break;

			if (tty->output_hook)
				tty->output_hook(tty, &bufptr, (size_t *)&n);

			write(tty->stdin_fd, buf, n);
		}

		else if (pfd[1].revents & POLLHUP) {
			break;
		}
	}
}

void raw_mode(struct termios *tios, int fd)
{
	struct termios aux;

	tcgetattr(fd, tios);
	memcpy(&aux, tios, sizeof(struct termios));
	aux.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);

	tcsetattr(fd, TCSAFLUSH, &aux);
}
