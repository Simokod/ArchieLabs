#include "lab4_util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define STDIN 0
#define STDOUT 1
#define STDERR 2

int main(int argc, char **argv) 
{
	int i, return_code;
	int debug = 0, read_input = STDIN, write_output = STDOUT;
	char buffer[1], *in_file_name, *out_file_name;

	for(i=1;i<argc;i++){
		if(strcmp(argv[i], "-D") == 0)
			debug = 1;
		if(strncmp(argv[i], "-i", 2) == 0){
			in_file_name = &argv[i][2];
			read_input = system_call(SYS_OPEN, in_file_name, 0, 0777);
		}
		if(strncmp(argv[i], "-o", 2) == 0){
			out_file_name = &argv[i][2];
			write_output = system_call(SYS_OPEN, out_file_name, 64 | 2, 0777);
		}
	}

	while((return_code = system_call(SYS_READ, read_input, buffer, 1)) != 0){
		if(buffer[0] != '\n'){
			if(debug){
				system_call(SYS_WRITE, STDERR, "ID: ", strlen("ID: "));
				system_call(SYS_WRITE, STDERR, itoa(SYS_READ), 1);
				system_call(SYS_WRITE, STDERR, ", return code: ", strlen(", return code: "));
				system_call(SYS_WRITE, STDERR, itoa(return_code), 1);
				system_call(SYS_WRITE, STDERR, "\n", 1);
			}
			if((buffer[0] >= 'A') & (buffer[0] <= 'Z'))	
				buffer[0] = buffer[0] + ' ';
			return_code = system_call(SYS_WRITE, write_output, buffer, 1);
			if(debug){
				system_call(SYS_WRITE, STDERR, "ID: ", strlen("ID: "));
				system_call(SYS_WRITE, STDERR, itoa(SYS_WRITE), 1);
				system_call(SYS_WRITE, STDERR, ", return code: ", strlen(", return code: "));
				system_call(SYS_WRITE, STDERR, itoa(return_code), 1);
				system_call(SYS_WRITE, STDERR, "\n", 1);
			}
		}
	}
	if(read_input != STDIN)
		system_call(SYS_CLOSE, read_input);
	if(write_output != STDOUT)
		system_call(SYS_CLOSE, write_output);
	return 0;
}
