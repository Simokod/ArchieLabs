#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//	----------------------------------------  Structs  ----------------------------------------------------
typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char sig[];
} virus;

typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};

struct fun_desc 
{
	char *name;
	void (*func) (void);
};
//  ---------------------------------------  Global Variables  --------------------------------------------
int SIZE = 100;
char suspectedFile[10000];
link *Gvirus_list = NULL;
//	--------------------------------------  private methods  ----------------------------------------------
void PrintHex(unsigned char *buffer, int length){
	for(int i=0; i<length; i++)
		printf("%02X ", buffer[i]);
	printf("\n");
}
FILE * getFile(){			//	Reading file name from user
	char file_name[SIZE];
	printf("%s\n", "Please enter file name");
	fgets(file_name, SIZE, stdin);
	sscanf(file_name, "%s", file_name);
	return fopen(file_name, "r");
}
void printVirus(virus *v){
	printf("%s%s\n", "Virus name: ", v->virusName);
	printf("%s%d\n", "Virus size: ", v->SigSize);
	printf("signature:\n");
	PrintHex(v->sig, v->SigSize);
	printf("\n");
}
// copies src string into dst string, including all null chars
char * mystrncpy(char *dest, const char *src, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
	   dest[i] = src[i];
	return dest;
}
 /* Print the data of every link in list. Each item followed by a newline character. */
void list_print(link *virus_list){
	if(virus_list == NULL)
		return;
	printVirus(virus_list-> vir);
	list_print(virus_list-> nextVirus);
}
 /* Add a new link with the given data to the list 
    (either at the end or the beginning, depending on what your TA tells you),
    and return a pointer to the list (i.e., the first link in the list).
    If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
	if(virus_list == NULL){
		virus_list = malloc(sizeof(data)+sizeof(link));
		virus_list-> vir = data;
		virus_list-> nextVirus = NULL;
		return virus_list;
	}
	virus_list-> nextVirus = list_append(virus_list-> nextVirus, data);
	return virus_list;
}
/* Free the memory allocated by the list. */
void list_free(link *virus_list){
	if(virus_list == NULL)
		return;
	list_free(virus_list-> nextVirus);
	free(virus_list-> vir);
	free(virus_list);
}
void load_signatures(link *virus_list, FILE * file){
	list_free(virus_list);	// clearing old virus_list
	virus *v = NULL;
	size_t bytes_read;
	char buffer[SIZE];

	while((bytes_read = fread(buffer, 2, 1, file)) != 0){		// reading size of virus
		v = malloc(buffer[0]*sizeof(char));
		v->SigSize = buffer[0] - 18;							// reducing by 2(size of virus) + 16(size of name)
		fread(buffer, sizeof(char), (16 + v->SigSize), file);	// reading description of virus(name + signature)
		mystrncpy(v->virusName, buffer, 16);
		mystrncpy(v->sig, (buffer+16), v->SigSize);
		Gvirus_list = list_append(Gvirus_list, v);
	}
	printf("%s\n", "Loaded viruses");
	fclose(file);
	return;
}	
void load_sig(){
	FILE *file = NULL;
	file = getFile();
	if(!file){		// checking if file exists
		printf("%s\n", "No such file");
		return;
	}
	load_signatures(Gvirus_list, file);
}
void print_signatures(link *virus_list){
	if(virus_list == NULL){
		return;
	}
	printVirus(virus_list-> vir);
	print_signatures(virus_list-> nextVirus);
}
//	prints all viruses in the linked list
void print_sig(){
	print_signatures(Gvirus_list);
}
// detects viruses
void detect_viruses(char *buffer, unsigned int size){
	link *virus_list = NULL;
	for(unsigned int i=0; i<size; i++){
		virus_list = Gvirus_list;
		while(virus_list != NULL){
			if(memcmp(&buffer[i], virus_list-> vir-> sig, virus_list-> vir-> SigSize) == 0)
				printf("Starting byte: %02X, Virus name: %s, Virus size: %d\n", i,virus_list-> vir-> virusName, virus_list-> vir-> SigSize);
			virus_list = virus_list-> nextVirus;
		}
	}
}
// getting file name
void detect_vir(){
	size_t bytes_read;
	FILE *file = NULL;
	unsigned int size;

	file = getFile();
	if(!file){		// checking if file exists
		printf("%s\n", "No such file");
		return;
	}
	bytes_read = fread(suspectedFile, sizeof(char), 10000, file);
	detect_viruses(suspectedFile, bytes_read);
}
void quit() {
	list_free(Gvirus_list);
	exit(0);
}
//	-------------------------------------------  Main  ----------------------------------------------------
int main(int argc, char** argv) {
	char buffer[SIZE];
	int op, amount_of_funcs;
	struct fun_desc func_array[] ={	{"Load signatures", load_sig}, {"Print signatures", print_sig},
									{"Detect viruses", detect_vir}, {"Quit", quit}, {NULL, NULL}};
	amount_of_funcs = sizeof(func_array)/sizeof(*func_array)-1;

	while(1){
		printf("%s\n", "1) Load signatures\n2) Print signatures\n3) Detect viruses\n4) Quit");
		op = atoi(fgets(buffer, SIZE, stdin));
		if(op >= 1 && op <= amount_of_funcs)
			func_array[op-1].func();
		else
			printf("outside bounds\n");
	}
}