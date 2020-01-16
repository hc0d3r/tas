#include "tas/tty.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pty.h>
#include <utmp.h>

#include <sys/signalfd.h>
#include <signal.h>

static int sigwinchfd(void)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGWINCH);
	return signalfd(-1, &mask, SFD_CLOEXEC);
}

// SIGWINCH signal handler
void nothing(int a)
{
	(void)a;
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
	struct pollfd pfd[3];
	struct winsize ws;

	nfds_t nfds;

	char buf[1024], *bufptr;
	ssize_t n;

	pfd[0].fd = tty->stdin_fd;
	pfd[1].fd = tty->master;

	if (tty->stdin_fd == STDIN_FILENO) {
		pfd[2].fd = sigwinchfd();

		if (pfd[2].fd == -1) {
			nfds = 2;
		} else {
			pfd[2].events = POLLIN | POLLERR | POLLHUP;
			nfds = 3;

			// we need to set a handler to sigwinch, otherwise
			// the signal isn't send
			signal(SIGWINCH, nothing);
		}
	} else {
		nfds = 2;
	}

	pfd[0].events = POLLIN;
	pfd[1].events = POLLIN;

	while (poll(pfd, nfds, -1) != -1) {
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

		if ((nfds == 3) && (pfd[2].revents & POLLIN)) {
			if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != -1) {
				ioctl(tty->master, TIOCSWINSZ, &ws);
			}
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
