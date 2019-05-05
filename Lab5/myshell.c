#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;							/* the parsed command line*/
    pid_t pid;								/* the process id that is running the command*/
    int status;								/* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;					/* next process in chain */
} process;

// receives a parsed line and invokes the program specified in the cmdLine using execv
void execute(cmdLine *pCmdLine){
	if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1){
		perror("execv error");
		_exit(1);
	}
}
void change_dir(cmdLine *pCmdLine){
	if(chdir(pCmdLine->arguments[1]) == -1)
		perror("cd error");
}
void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
	process *proc = malloc(sizeof(process));
	proc->cmd = cmd;
	proc->pid = pid;
	proc->status = RUNNING;
	proc->next = NULL;

	if(*process_list == NULL){ 					// inserting first process
		*process_list = proc;
		return;
	}
	process *curr_process = *process_list;		// inserting processes after the first
	while(curr_process->next != NULL) 
		curr_process = curr_process->next;

	curr_process->next = proc;
}
void updateProcessStatus(process* process_list, int pid, int status){
	if(process_list != NULL){
		if(process_list->pid == pid)
			process_list->status = status;
		else
			updateProcessStatus(process_list->next, pid, status);
	}
}
void updateProcessList(process **process_list){
	process *curr_process = *process_list;
	int wstatus;
	while(curr_process != NULL){
		if(curr_process->status != -1){
			if(waitpid(curr_process->pid, &wstatus, WNOHANG) && (WIFEXITED(wstatus) || WIFSTOPPED(wstatus))) {
				curr_process->status = TERMINATED;
			}
			else{
				if(errno == ECHILD)
					printf("no child exists\n");
			}
		}
		curr_process = curr_process->next;
	}
}
void printProcessList(process** process_list) {
	int index = 0, j;
	if(*process_list == NULL)
		return;
	updateProcessList(process_list);

	printf("Index\t\tPID\t\tStatus\t\tCommand\n");
	process *curr_process = *process_list, *prev_process = NULL;
 
	while(curr_process != NULL) {
		printf("%d\t\t", index);
		printf("%d\t\t", curr_process->pid);
		switch (curr_process->status) {
			case(-1): 	printf("Terminated\t");
						if(prev_process == NULL){
							freeCmdLines(curr_process->cmd);
							*process_list = curr_process->next;
							free(curr_process);
						}
						else {
							freeCmdLines(curr_process->cmd);
							prev_process->next = curr_process->next;
							free(curr_process);
						}
						break;
			case(0): 	printf("Suspended\t");
						break;
			case(1):	printf("Running\t\t");
						break;
			default:;
		}
		if(curr_process->status != -1)
			for(j=0;j<curr_process->cmd->argCount;j++)
				printf("%s ", curr_process->cmd->arguments[j]);
		printf("\n");
		index++;
		prev_process = curr_process;
		curr_process = curr_process->next;
	}
}
// converts input string to number for switch case
int get_command_num(char *input){
	if(strcmp(input, "quit") == 0)
		return 1;
	if(strcmp(input, "cd") == 0)
		return 2;
	if(strcmp(input, "procs") == 0)
		return 3;
	if(strcmp(input, "suspend") == 0)
		return 4;
	if(strcmp(input, "kill") == 0)
		return 5;
	if(strcmp(input, "wake") == 0)
		return 6;
	return 0;
}
void freeProcessList(process* process_list){
	if(process_list != NULL){
		freeCmdLines(process_list->cmd);
		freeProcessList(process_list->next);
		free(process_list);
	}
}
int main(int argc, char **argv) {
	char buffer[PATH_MAX], input[2048];
	char *prompt_str = buffer;
	int pid, i, debug_mode = 0, command_num;
	process **process_list;
	cmdLine *pCmdLine;
	process_list = calloc(1, sizeof(int));

	for(i=1;i<argc;i++) {
		if(strcmp(argv[i], "-D") == 0)
			debug_mode = 1;
	}

	while(1) {
		prompt_str = getcwd(prompt_str, PATH_MAX);
		printf("%s> \n", prompt_str);
		fgets(input, 2048, stdin);
		pCmdLine = parseCmdLines(input);

		command_num = get_command_num(pCmdLine->arguments[0]);
		switch(command_num){
			case 1:	freeCmdLines(pCmdLine);				// quit
					freeProcessList(*process_list);
					free(process_list);
					exit(0);
			case 2:	change_dir(pCmdLine); 				// cd
					freeCmdLines(pCmdLine);
					break;
			case 3:	printProcessList(process_list); 	// procs
					freeCmdLines(pCmdLine);
					break;
			case 4: if(kill(strtol(pCmdLine->arguments[1], NULL, 10), SIGTSTP) == -1)		// suspend
						perror("suspend error");
					else{
						updateProcessStatus(*process_list, strtol(pCmdLine->arguments[1], NULL, 10), SUSPENDED);
						freeCmdLines(pCmdLine);
					}
					break;
			case 5: if(kill(strtol(pCmdLine->arguments[1], NULL, 10), SIGINT) == -1)		// kill
						perror("kill error");
					else
						freeCmdLines(pCmdLine);
					break;
			case 6:	if(kill(strtol(pCmdLine->arguments[1], NULL, 10), SIGCONT) == -1)		// wake
						perror("wake error");
					else{
						updateProcessStatus(*process_list, strtol(pCmdLine->arguments[1], NULL, 10), RUNNING);
						freeCmdLines(pCmdLine);
					}
					break;
			default:
				if(!(pid=fork())){
					if(debug_mode){
						fprintf(stderr, "PID: %d\n", pid);
						fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
					}
					execute(pCmdLine);
					exit(1);
				}
				addProcess(process_list, pCmdLine, pid);
				if(pCmdLine->blocking)
					waitpid(pid, NULL, 0);	
		}
	}
}
