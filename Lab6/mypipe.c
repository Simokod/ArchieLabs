#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int pid, pipefd[2];
	char input[2048];
	FILE *writeFile, *readFile;

	pipe(pipefd);
	if(!(pid = fork())){
		writeFile = fdopen(pipefd[1], "w");
		fprintf(writeFile, "hello\n");
	}
	else{
		readFile = fdopen(pipefd[0], "r");
		fgets(input, 2048, readFile);
		printf("%s\n", input);
		exit(0);
	}
}