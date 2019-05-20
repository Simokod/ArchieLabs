#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int digit_cnt(char *str){
	int counter = 0;
	while(str[counter] != '\0')
		counter++;
	return counter;
}

int main(int argc, char** argv) {
	int count = 0;
	count = digit_cnt(argv[1]);
	printf("the amount of digits is: %d\n", count);
}