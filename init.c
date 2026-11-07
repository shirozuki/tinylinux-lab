#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>

int
main(void)
{
	if (mount("none", "/proc", "proc", 0, NULL) == -1) {
		perror("mount");
		return -1;
	}

	if (mount("none", "/sys", "sysfs", 0, NULL) == -1) {
		perror("mount");
		return -1;
	}

	if (mount("none", "/dev", "devtmpfs", 0, NULL) == -1) {
		perror("mount");
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
				exit(EXIT_FAILURE);
			}

			static char *nargv[] = { "cttyhack", "sh", NULL };
			static char *nenv[] = { NULL };

			execve("/bin/cttyhack", nargv, nenv);
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
