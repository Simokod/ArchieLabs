#include <stdio.h>
#include <unistd.h>

void sigtstp_handler(int signum){
    printf("Looper handling %s\n", strsignal(signum));
    signal(signum, SIG_DFL);
    signum(SIGCONT, sigcont_handler);
    raise(SIGTSTP);
}

void sigcont_handler(int signum){
    printf("Looper handling %s\n", strsignal(signum));
    signal(signum, SIG_DFL);
    signum(SIGTSTP, sigtstp_handler);
    raise(SIGCONT);
}

void sigint_handler(int signum){
	printf("exitting\n");
	_exit(1);
}

int main(int argc, char **argv){ 
	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, sigtstp_handler);
	signal(SIGCONT, sigcont_handler);
	printf("Starting the program\n");

	while(1) {
		sleep(2);
	}

	return 0;
}