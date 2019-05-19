#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct state{
	char debug_mode;
	char file_name[128];
	int unit_size;
	unsigned char mem_buf[10000];
	size_t mem_count;
} state;

struct fun_desc {
	char *name;
	void (*func) (state *s);
};

void toggle_debug(state *s){
	if(s->debug_mode){
		printf("Debug flag now off\n");
		s->debug_mode = 0;
		return;
	}
	printf("Debug flag now on\n");
	s->debug_mode = 1;
}
void set_file_name(state *s){
	char f_length = 100;
	printf("please enter file name\n");
	fgets(s->file_name, f_length, stdin);
	s->file_name[strlen(s->file_name)-1] = '\0';
	if(s->debug_mode)
		printf("Debug: file name set to '%s'\n", s->file_name);
}
void set_unit_size(state *s){
	char x, jnk;
	printf("please enter unit size\n");
	x = fgetc(stdin) - '0';
	jnk = fgetc(stdin);
	if(x == 1 || x == 2 || x == 4){
		s->unit_size = x;
		if(s->debug_mode)
			printf("Deubg: set size to %d\n", x);
		return;
	}
	fprintf(stderr, "size not valid\n");
}
void load_in_mem(state *s){
	FILE *file;
	char input[100];
	int location = 0, length = 0, n = 0, bytes_read = 0;

	if(strlen(s->file_name) == 0){
		fprintf(stderr, "please set file name first\n");
		return;
	}
	file = fopen(s->file_name, "r");
	if(file == NULL)
		perror("fopen");
	printf("Please enter <location> <length>\n");
	fgets(input, 100, stdin);
	n = sscanf(input, "%x%d", &location, &length);
	if(n != 2){
		fprintf(stderr, "wrong number of args\n");
		return;
	}
	if(s->debug_mode)
		printf("file_name: %s, location: %x, length: %d\n", s->file_name, location, length);
	bytes_read = fread(s->mem_buf + location, s->unit_size, length, file);
	s->mem_count += bytes_read;
	printf("Loaded %d units into memory\n", bytes_read);
	fclose(file);
}
char* unit_to_hex_format(int unit_size) {
    static char* formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    return formats[unit_size-1];
}
char* unit_to_dec_format(int unit_size) {
    static char* formats[] = {"%#hhd\t\t", "%#hd\t\t", "No such unit", "%#d\t\t"};
    return formats[unit_size-1];
}
void print_units(unsigned char* buffer, int u, int unit_size) {
    unsigned char* end = buffer + unit_size*u;
    printf("Decimal\t\tHexadecimal\n");
    printf("===========================\n");
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        printf(unit_to_dec_format(unit_size), var);
        printf(unit_to_hex_format(unit_size), var);
        buffer += unit_size;
    }
}
void display_memory(state *s){
	int u = 0, addr = 0, n = 0;
	char input[100];
	void *addr_ptr = NULL;

	printf("please enter number of units to display, and address of memory\n");
	fgets(input, 100, stdin);
	n = sscanf(input, "%d%x", &u, &addr);
	if(n != 2){
		fprintf(stderr, "wrong number of args\n");
		return;
	}
	if(s->debug_mode)
		printf("units to display: %d, address: %x\n", u, addr);
	if(addr == 0)
		print_units(s->mem_buf, u, s->unit_size);
	else{
		addr_ptr = addr;
		print_units(addr_ptr, u, s->unit_size);
	}
}
int write_units(unsigned char* buffer, int unit_size, int count, FILE *output) {
    return fwrite(buffer, unit_size, count, output);
}
void save_in_file(state *s){
	int src_addr = 0, tar_loc = 0, length = 0, n = 0, file_size = 0, units_wrote = 0;
	char input[100];
	void *src_addr_ptr = NULL;
	FILE *file;

	if(strlen(s->file_name) == 0){
		fprintf(stderr, "please set file name first\n");
		return;
	}
	printf("Please enter <source-address> <target-location> <length>\n");
	fgets(input, 100, stdin);
	n = sscanf(input, "%x%x%d", &src_addr, &tar_loc, &length);
	if(n != 3){
		fprintf(stderr, "wrong number of args\n");
		return;
	}
	if(s->debug_mode)
		printf("file_name: %s, source-address: %x, target-location: %x, length: %d\n", s->file_name, src_addr, tar_loc, length);
	
	file = fopen(s->file_name, "r+");
	if(file == NULL)
		perror("fopen");
		// checking that target location is in bounds of file
	fseek(file, 0L, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	if(tar_loc > file_size){
		fprintf(stderr, "target-location is greater then file size\n");
		fclose(file);
		return;
	}
	fseek(file, tar_loc, SEEK_SET);
	if(src_addr == 0)
		units_wrote = write_units(s->mem_buf, s->unit_size, length, file);
	else{
		src_addr_ptr = src_addr;
		units_wrote = write_units(src_addr_ptr, s->unit_size, length, file);
	}
	if(s->debug_mode)
		printf("wrote %d units into file %s\n", units_wrote, s->file_name);
	fclose(file);
}
void modify_file(state *s){
	int location = 0, val = 0, n = 0, file_size = 0;
	unsigned char new_val[s->unit_size];
	char input[100];
	FILE *file;

	printf("Please enter <location> <val>\n");
	fgets(input, 100, stdin);
	n = sscanf(input, "%x%x", &location, &val);
	if(n != 2){
		fprintf(stderr, "wrong number of args\n");
		return;
	}
	if(s->debug_mode)
		printf("location: %x, val: %x\n", location, val);	
	file = fopen(s->file_name, "r+");
	if(file == NULL)
		perror("fopen");
		// checking that target location is in bounds of file
	fseek(file, 0L, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	if(location > file_size){
		fprintf(stderr, "target-location is greater then file size\n");
		fclose(file);
		return;
	}
	fseek(file, location, SEEK_SET);
	new_val[0] = val;
	write_units(new_val, s->unit_size, 1, file);
	fclose(file);
}
void quit(state *s) {
	if(s->debug_mode)
		printf("quitting\n");
	free(s);
	exit(0);
}
void init_state(state *s){
	s->debug_mode = 0;
	s->unit_size = 1;
	s->mem_count = 0;
}
int main(int argc, char** argv) {
	int op, amount_of_funcs, i, jnk;
	struct fun_desc menu[] ={{"Toggle Debug Mode", toggle_debug}, {"Set File Name", set_file_name},
							{"Set Unit Size", set_unit_size}, {"Load Into Memory", load_in_mem},
							{"Memory Display", display_memory}, {"Save Into File", save_in_file}, 
							{"File Modify", modify_file}, {"Quit", quit}, 
							{NULL, NULL}};
	amount_of_funcs = sizeof(menu)/sizeof(*menu)-1;
	state *s = malloc(sizeof(state));
	init_state(s);

	while(1){
		if(s->debug_mode){
			printf("unit_size: %d, file_name: '%s', mem_count: %d\n\n", s->unit_size, s->file_name, s->mem_count);
		}
		i=0;	// printing the menu
		while(menu[i].name != NULL){
			printf("%d) ", i);
			printf("%s\n", menu[i].name);
			i++;
		}
		op = getc(stdin) - '0';
		jnk = getc(stdin); 					// 			clearing junk from buffer
		if(op >= 0 && op <= amount_of_funcs)
			menu[op].func(s);
		else
			printf("outside bounds\n");
	}
}