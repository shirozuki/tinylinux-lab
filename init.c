#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/wait.h>

int
main(void)
{
	if (mount("none", "/proc", "proc", 0, NULL) == -1) {
		perror("mount /proc");
		return -1;
	}

	if (mount("none", "/sys", "sysfs", 0, NULL) == -1) {
		perror("mount /sys");
		return -1;
	}

	if (mount("none", "/dev", "devtmpfs", 0, NULL) == -1) {
		perror("mount /dev");
		return -1;
	}

	for (;;) {
		pid_t shell;
		shell = fork();

		if (shell == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (shell == 0) {
			if (setsid() == -1) {
				perror("setsid");
				_exit(EXIT_FAILURE);
			}

			static char *nargv[] = { "sh", NULL };
			static char *nenv[] = { "PATH=/bin:/sbin:/usr/bin:/usr/sbin", "TERM=linux", NULL };

			int fd;
			if ((fd = open("/dev/ttyS0", O_RDWR)) == -1) {
				perror("open");
				_exit(EXIT_FAILURE);
			}

			if (dup2(fd, STDIN_FILENO) == -1) {
				perror("dup2");
				_exit(EXIT_FAILURE);
			}

			if (dup2(fd, STDOUT_FILENO) == -1) {
				perror("dup2");
				_exit(EXIT_FAILURE);
			}

			if (dup2(fd, STDERR_FILENO) == -1) {
				perror("dup2");
				_exit(EXIT_FAILURE);
			}

			if (fd > 2) {
				if (close(fd) == -1)
					perror("close");
			}

			execve("/bin/sh", nargv, nenv);
			perror("execve");
			_exit(1);
		}

		for (;;) {
			int status;
			pid_t wshell = waitpid(-1, &status, 0);

			if (wshell == shell)
				break;
		}
	}

	return 0;
}
