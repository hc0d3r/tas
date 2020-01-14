#include "tas/tas.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

static void leet_output(void *tty, char **buf, size_t *n)
{
	(void)tty;

	for (size_t i = 0; i < *n; i++) {
		char c = (*buf)[i];

		switch (c) {
			case 'A':
			case 'a':
				(*buf)[i] = '4';
				break;
			case 'i':
			case 'I':
				(*buf)[i] = '1';
				break;
			case 'o':
			case 'O':
				(*buf)[i] = '0';
				break;
			case 'e':
			case 'E':
				(*buf)[i] = '3';
				break;
		}
	}
}

int main(void)
{
	struct termios tios;
	tas_tty tty = TAS_TTY_INIT;
	int status;

	pid_t pid = tas_forkpty(&tty);
	if (pid == 0) {
		printf("SPAWNING LEET SHELL HERE !!!\n");
		execvp("bash", (char *[]){ "bash", "-i", NULL });
		_exit(1);
	}

	else if(pid == -1) {
		printf("fork failed\n");
		exit(1);
	}

	tty.output_hook = leet_output;

	// set to raw mode
	raw_mode(&tios, STDIN_FILENO);

	// rw loop
	tty_loop(&tty);

	// restore
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);

	waitpid(pid, &status, 0);
	return status;
}
