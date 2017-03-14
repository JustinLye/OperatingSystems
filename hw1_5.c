#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>


int main() {
	printf("\nCS 4323 OS - HW1 Q5 - Justin Lye\nParent creates two child processes and kills the second child process immediately after the first child process terminates\n\n");
	pid_t c1_pid, c2_pid;						/* declare variables for child process ids */
	printf("Forking First Child\n");			/* print out first message just before forking first child */
	c1_pid = fork();							/* fork first child */
	if (c1_pid != 0) {							/* fork second child in parent process only */
		printf("Forking Second Child\n");		/* print out second message just before forking second child */
		c2_pid = fork();						/* fork second child */
		if (c2_pid == 0) {						/* execute code block only if second child process */
			raise(SIGSTOP);						/* stop second child process */
			printf("Second Child Running\n");	/* print out sixth message after second child process has been signaled to continue */
			while (1) {}						/* loop until parent process kills second child */
		}
	} else {									/* execute code block only if first child process */
		printf("First Child Running\n");        /* print out third message */
		sleep(3);                               /* put first child process to sleep for 3 seconds */
		printf("First Child Exiting\n");        /* print out fourth message when first child wakes up */
		exit(EXIT_SUCCESS);                     /* force first child process to exit successfully */
	}
	if (c1_pid != 0 && c2_pid != 0) {						/* execute code block only if parent process */
		int wstatus;										/* declare variable to capture status...not really needed */
		while (!waitpid(c1_pid, &wstatus, WNOHANG)) {}		/* loop while first child completes */
		printf("Waiting for First Child to Complete\n");	/* print out fifth message after first child completes */
		kill(c2_pid, SIGCONT);                              /* signal second child to continue, this will cause sixth message to be printed from second child process */
		kill(c2_pid, SIGKILL);                              /* kill second child process */
		printf("Killed Second Process\n");                  /* print out seventh message*/
	}
	exit(EXIT_SUCCESS);                                     /* force parent process to exit successfully */
}