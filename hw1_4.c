#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int main(int argc, char* argv[]) {
	printf("\nCS 4323 OS - HW1 Q4 - Justin Lye\nIllustration of various process states.\n\n");
	pid_t pid = fork(); /* fork new child process */
	if(pid == 0) { /* execute if child process */
		printf("child (pid %d) is in new state\n", getpid());
		sleep(0.001f); /* inactivate child (i.e. force it out of ready state).*/
		printf("child (pid %d) is in ready state\n", getpid()); /* child will be in ready/running state when it wakes */
		raise(SIGSTOP); /* stop the child and wait on signal from parent. */
		printf("child (pid %d) is in running state\n", getpid());
		sleep(0.001f); /* inactivate child*/
		exit(0);
	} else { /* execute if parent process */
		int* wstatus = malloc(sizeof(int)); /* allocate some memory for status */
		printf("parent (pid %d) is in waiting state\n", getpid());
		while (waitpid(pid, wstatus, WNOHANG | WUNTRACED) == 0) {} /* wait for child */
		kill(pid, SIGCONT); /* signal child to continue */
		printf("parent (pid %d) is in waiting state\n", getpid());
		while (waitpid(pid, wstatus, WNOHANG | WUNTRACED) != -1) {} /* wait for child */
		if (WIFEXITED(*wstatus)) /* check if child exited */
			printf("child (pid %d) is in terminated state.\n", pid);
		free(wstatus); /* release memory */
	}
	printf("\n\n");
	return EXIT_SUCCESS;
}