#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MAX_SIZE = 100;

void PrintHex(unsigned char *buffer, int length){
	for(int i=0; i<length; i++)
		printf("%02X ", buffer[i]);
}

int main(int argc, char** argv) {
	size_t bytes_read;
	unsigned char buffer[MAX_SIZE];
	FILE *file = NULL;
	if(argc != 2)
		exit(0);
	file = fopen(argv[1], "r");
	while((bytes_read = fread(buffer, sizeof(char), bytes_read, file)) != 0)
		PrintHex(buffer, bytes_read);
	printf("\n");
	fclose(file);
}