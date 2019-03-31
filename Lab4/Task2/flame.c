#include "lab4_util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define GET_DENTS 141
#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct ent {
	int inode;
	int offset;
	short len;
	char buf[];
} ent;

int main(int argc, char **argv) 
{
	char buffer[8192], *current_file = ".", *prefix;
	int i, fd, total_bytes, total_bytes_read = 0, debug_mode = 0, prefix_len = 0, prefix_mode = 0;
	ent *entp = buffer;

	for(i=1;i<argc;i++){
		if(strcmp(argv[i], "-D") == 0)
			debug_mode = 1;
		if(strncmp(argv[i], "-p", 2) == 0){
			prefix = &argv[i][2];
			prefix_len = strlen(prefix);
			prefix_mode = 1;
		}
	}
	fd = system_call(SYS_OPEN, current_file, 0, 0777);
	if(debug_mode){
		system_call(SYS_WRITE, STDERR, "ID: ", strlen("ID: "));
		system_call(SYS_WRITE, STDERR, itoa(SYS_READ), 1);
		system_call(SYS_WRITE, STDERR, ", return code: ", strlen(", return code: "));
		system_call(SYS_WRITE, STDERR, itoa(fd), 1);
		system_call(SYS_WRITE, STDERR, "\n", 1);
	}

	total_bytes = system_call(GET_DENTS, fd, entp, 8192);
	if(total_bytes < 0)					/*error*/
		system_call(SYS_EXIT, 0x55);

	if(debug_mode){
		system_call(SYS_WRITE, STDERR, "ID: ", strlen("ID: "));
		system_call(SYS_WRITE, STDERR, itoa(GET_DENTS), 1);
		system_call(SYS_WRITE, STDERR, ", return code: ", strlen(", return code: "));
		system_call(SYS_WRITE, STDERR, itoa(total_bytes), 1);
		system_call(SYS_WRITE, STDERR, "\n", 1);
	}
	while(total_bytes_read < total_bytes){		 

		if(strncmp(prefix, entp->buf, prefix_len) == 0) {
			system_call(SYS_WRITE, STDOUT, "Filename: ", sizeof("Filename: "));
			system_call(SYS_WRITE, STDOUT, entp->buf, strlen(entp->buf));
			if(debug_mode){
				system_call(SYS_WRITE, STDOUT, ", length: ", sizeof("length: "));
				system_call(SYS_WRITE, STDOUT, itoa(entp->len), strlen(itoa(entp->len)));
			}
			if(prefix_mode){
				system_call(SYS_WRITE, STDOUT, ", file type: ", sizeof(", file type: "));		/* TODO: ASK SHAKED */
				system_call(SYS_WRITE, STDOUT, itoa(*(buffer + total_bytes_read + entp->len - 1)), 1);
			}

			system_call(SYS_WRITE, STDOUT, "\n", 1);		 /*newLine*/

			if(debug_mode){
				system_call(SYS_WRITE, STDERR, "ID: ", sizeof("ID: "));
				system_call(SYS_WRITE, STDERR, itoa(SYS_WRITE), 1);
				system_call(SYS_WRITE, STDERR, ", Rreturn code: ", sizeof(", Return code: "));
				system_call(SYS_WRITE, STDERR, itoa(1), 1);
				system_call(SYS_WRITE, STDERR, "\n", 1);
			}
		}
		total_bytes_read += entp -> len;
		entp = buffer + total_bytes_read;
	}
	system_call(SYS_CLOSE, current_file);
	return 0;
}