#include "lab4_util.h"

#define SYS_LSEEK 19
#define SYS_OPEN 5
#define SYS_WRITE 4
#define STDOUT 1

int main (int argc , char* argv[], char* envp[])
{
	char *file_name, *new_name, *filler;
	int fd, size_diff;
	int offset = 0x291;

	if (argc != 3)
		return 0;

	file_name = argv[1];
	new_name = argv[2];

	size_diff = strlen(new_name - 5);
	fd = system_call(SYS_OPEN, file_name, 2, 0777);			// opening file
	system_call(SYS_LSEEK, fd, offset, 0);					// moving pointer to Shira
	system_call(SYS_WRITE, fd, new_name, strlen(new_name));	// writing the new name


	return 0;
}
