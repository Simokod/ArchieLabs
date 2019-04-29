#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

// receives a parsed line and invokes the program specified in the cmdLine using execv
void execute(cmdLine *pCmdLine){
	if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1){
		perror("execv error");
		exit(1);
	}
}
void change_dir(cmdLine *pCmdLine){
	if(chdir(pCmdLine->arguments[1]) == -1)
		perror("cd error");
}
int main(int argc, char **argv) {
	char buffer[PATH_MAX], input[2048];
	char *prompt_str = buffer, *input_str = input;
	int pid, i, debug_mode = 0;
	cmdLine *pCmdLine;

	for(i=1;i<argc;i++) {
		if(strcmp(argv[i], "-D") == 0)
			debug_mode = 1;
	}

	while(1) {
		prompt_str = getcwd(prompt_str, PATH_MAX);
		printf("%s> \n", prompt_str);
		input_str = fgets(input, 2048, stdin);
		if (strcmp("quit\n", input_str) == 0)
			exit(0);
		pCmdLine = parseCmdLines(input);
		if(strcmp(pCmdLine->arguments[0], "cd") == 0)
			change_dir(pCmdLine);
		else {
			if(!(pid=fork())){
				if(debug_mode){
					fprintf(stderr, "PID: %d\n", pid);
					fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
				}
				execute(pCmdLine);
				exit(1);
			}
			if(pCmdLine->blocking)
				waitpid(pid, NULL, 0);
		}
		freeCmdLines(pCmdLine);
	}
}