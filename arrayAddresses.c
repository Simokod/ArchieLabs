#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
	int iarray[3];
	float farray[3];
	double darray[3];
	char carray[3];


	printf("- &iarray[0]: %p\n",&iarray[0]);
    printf("- &iarray[1]: %p\n",&iarray[1]);
    printf("- &iarray[2]: %p\n",&iarray[2]);
    printf("- &iarray+1: %p\n",&iarray+1);

    printf("- &farray[0]: %p\n",&farray[0]);
    printf("- &farray[1]: %p\n",&farray[1]);
    printf("- &farray[2]: %p\n",&farray[2]);
    printf("- &farray+1: %p\n",&farray+1);

    printf("- &darray[0]: %p\n",&darray[0]);
    printf("- &darray[1]: %p\n",&darray[1]);
    printf("- &darray[2]: %p\n",&darray[2]);
    printf("- &darray+1: %p\n",&darray+1);
    
    printf("- &carray[0]: %p\n",&carray[0]);
    printf("- &carray[1]: %p\n",&carray[1]);
    printf("- &carray[2]: %p\n",&carray[2]);
    printf("- &carray+1: %p\n",&carray+1);
}