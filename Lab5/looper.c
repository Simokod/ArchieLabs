#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void sig_handler(int signum){
    if(signum == SIGTSTP){
	    printf("Looper handling SIGTSTP\n");
	    signal(SIGTSTP, SIG_DFL);
	    signal(SIGCONT, sig_handler);
	    raise(SIGTSTP);
	}
	else {
	    printf("Looper handling SIGCONT\n");
	    signal(SIGCONT, SIG_DFL);
	    signal(SIGTSTP, sig_handler);
	    raise(SIGCONT);
	}
}

void sigint_handler(int signum){
	printf("Looper handling SIGINT\n");
	_exit(1);
}

int main(int argc, char **argv){ 
	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, sig_handler);
	signal(SIGCONT, sig_handler);
	printf("Starting the program\n");

	while(1) {
		sleep(2);
	}

	return 0;
}
