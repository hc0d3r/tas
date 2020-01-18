#ifndef __TAS_TTY_H__
#define __TAS_TTY_H__

#include <unistd.h>
#include <poll.h>
#include <termios.h>

#define TAS_TTY_INIT (tas_tty){ -1, 0, -1, NULL, NULL }

typedef void (*tas_input_cb)(void *, char **, size_t *);
typedef void (*tas_output_cb)(void *, char **, size_t *);

typedef struct {
	int master;
	int stdin_fd;
	int output_fd;
	tas_input_cb input_hook;
	tas_output_cb output_hook;
} tas_tty;


pid_t tas_forkpty(tas_tty *tty);
void tas_tty_loop(tas_tty *tty);
void raw_mode(struct termios *tios, int fd);

#endif
