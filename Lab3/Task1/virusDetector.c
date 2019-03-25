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
int SIZE = 10000;
char suspectedFile[10000];
link *Gvirus_list = NULL;
char *VirFileName;
//	--------------------------------------------  methods  -------------------------------------------------
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
	unsigned int virus_size = 0;

	while((bytes_read = fread(buffer, 2, 1, file)) != 0){		// reading size of virus
		virus_size = buffer[1] << 8;
		virus_size = virus_size + (unsigned char) buffer[0];
		v = malloc(sizeof(virus) + virus_size*sizeof(char)-18);
		v->SigSize = virus_size - 18;							// reducing by (2(size of virus) + 16(size of name))
		fread(buffer, sizeof(char), (16 + v->SigSize), file);	// reading description of virus(name + signature)
		memcpy(v->virusName, buffer, 16);
		memcpy(v->sig, buffer+16, v->SigSize);
		Gvirus_list = list_append(Gvirus_list, v);
	}
	printf("%s\n", "Loaded viruses");
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
	fclose(file);
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
	fclose(file);
}
void kill_virus(char *fileName, int signitureOffset, int signitureSize){
	FILE *file = NULL;
	char NOP_str[signitureSize];
	for(int i=0;i<signitureSize;i++)
		NOP_str[i] = 0x90;
	file = fopen(fileName, "r+");
	fseek(file, signitureOffset, SEEK_SET);
	fwrite(NOP_str, sizeof(0x90), signitureSize, file);
	fclose(file);
}
// fixing the file
void fix_file(){
	int offset, sig_size;
	char buffer[SIZE];
	printf("%s\n", "Enter signature offset");
	offset = atoi(fgets(buffer, SIZE, stdin));
	printf("%s\n", "Enter signature size");
	sig_size = atoi(fgets(buffer, SIZE, stdin));
	kill_virus(VirFileName, offset, sig_size);
}
void quit() {
	list_free(Gvirus_list);
	exit(0);
}
//	-------------------------------------------  Main  ----------------------------------------------------
int main(int argc, char** argv) {
	char buffer[SIZE];
	int op, amount_of_funcs, i;
	struct fun_desc menu[] ={{"Load signatures", load_sig}, {"Print signatures", print_sig},
							{"Detect viruses", detect_vir}, {"Fix file",fix_file}, {"Quit", quit}, 
							 {NULL, NULL}};
	amount_of_funcs = sizeof(menu)/sizeof(*menu)-1;
	VirFileName = argv[1];
	while(1){
		i=0;	// printing the menu
		while(menu[i].name != NULL){
			printf("%d) ", i+1);
			printf("%s\n", menu[i].name);
			i++;
		}
		op = atoi(fgets(buffer, SIZE, stdin));
		if(op >= 1 && op <= amount_of_funcs)
			menu[op-1].func();
		else
			printf("outside bounds\n");
	}
}