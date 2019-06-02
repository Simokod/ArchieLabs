#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <elf.h>       
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>       
#include <sys/mman.h>
#include <unistd.h>


int debug_mode = 0, currentfd = -1, f_length = 128;
void *map_start;				/* will point to the start of the memory mapped file */
struct stat fd_stat; 			/* this is needed to get the size of the file */
Elf32_Ehdr *header; 			/* this will point to the header structure */

struct fun_desc {
	char *name;
	void (*func) ();
};
void toggle_debug(){
	if(debug_mode){
		printf("Debug flag now off\n");
		debug_mode = 0;
		return;
	}
	printf("Debug flag now on\n");
	debug_mode = 1;
}
void ex_elf_file(){
	char file_name[f_length], byte1, byte2, byte3;

	printf("Please enter file name\n");
	fgets(file_name, f_length, stdin);
	file_name[strlen(file_name)-1] = '\0'; 		// remove '\n' at the end of the input
	if(debug_mode)
		printf("file name is : %s", file_name);

	if(currentfd != -1){ 		// checking if a file was open
		close(currentfd);
	}
	if((currentfd = open(file_name, O_RDWR)) < 0) {
		perror("error in open");
		return;
	}
	if(fstat(currentfd, &fd_stat) != 0) {
		currentfd = -1;
		perror("stat failed");
		return;
	}
	if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, currentfd, 0)) == MAP_FAILED) {
		currentfd = -1;
		perror("mmap failed");
		return;
	}
	header = (Elf32_Ehdr *) map_start;

	byte1 = header->e_ident[EI_MAG1];
	byte2 = header->e_ident[EI_MAG2];
	byte3 = header->e_ident[EI_MAG3];
	if(byte1 != 'E' || byte2 != 'L' || byte3 != 'F'){
		printf("this file is not an ELF file: %s\n", file_name);
		munmap(map_start, fd_stat.st_size);
		close(currentfd);
		return;
	}
	printf("Magic:\t%x %x %x\n", byte1, byte2, byte3);
	switch (header->e_ident[EI_DATA]){
		case ELFDATA2LSB: printf("Data: \t\t\t\t\t2's complement, little-endian\n");
			 break;
		case ELFDATA2MSB: printf("Data: \t\t\t\t\t2's complement, big-endian\n");
			 break;   
	}
	printf("Entry point address:\t\t\t%#x\n", header->e_entry);
	printf("Start of section headers:\t\t%d (bytes into file)\n", header->e_shoff );
	printf("Number of section headers:\t\t%d\n", header->e_shnum);
	printf("Size of section headers:\t\t%d (bytes)\n", header->e_shentsize);
	printf("Start of program headers:\t\t%d (bytes into file)\n", header->e_phoff );
	printf("Number of program headers:\t\t%d\n", header->e_phnum);
	printf("Size of program headers:\t\t%d (bytes)\n", header->e_phentsize);

}
Elf32_Shdr *elf_sheader(Elf32_Ehdr *hdr) {
	return (Elf32_Shdr *)((int)hdr + hdr->e_shoff);
}
Elf32_Shdr *elf_section(Elf32_Ehdr *hdr, int idx) {
	return &elf_sheader(hdr)[idx];
}
char *elf_str_table(Elf32_Ehdr *hdr) {
    if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
    return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}
char *elf_lookup_string(Elf32_Ehdr *hdr, int offset) {
    char *strtab = elf_str_table(hdr);
    if(strtab == NULL) return NULL;
    return strtab + offset;
}
void print_sections(){
	int i;
	char *section_name;
	Elf32_Shdr *curr_section;

	if(currentfd < 0){
		fprintf(stderr, "File is not open\n");
		return;
	}
	curr_section = elf_section(header, 0);
	printf("Section Headers:\n");
	printf("[Nr] Name\t\t Address  Offset  Size  Type\n");
	for(i=0;i<header->e_shnum;i++){
		section_name = elf_lookup_string(header, curr_section->sh_name);
		printf("[%2d] %18s%09x%8x %5x %6d\n", i, section_name, curr_section->sh_addr, 
				curr_section->sh_offset, curr_section->sh_size, curr_section->sh_type);
		curr_section = elf_section(header, i+1);
	}
}
void print_symbols(){
	int i, j, symtab_entries, sym_addr;
	char *sym_name, *section_name;
	Elf32_Shdr *sym_tab;
	Elf32_Sym *symbol;
	uint32_t sec_type;
	if(currentfd < 0){
		fprintf(stderr, "File is not open\n");
		return;
	}
	for(j=0;j<header->e_shnum;j++){
		sym_tab = elf_section(header, j);
		sec_type = sym_tab->sh_type;
		if((sec_type == SHT_SYMTAB || sec_type == SHT_STRTAB || sec_type == SHT_DYNSYM) && sym_tab->sh_entsize != 0){
			symtab_entries = sym_tab->sh_size / sym_tab->sh_entsize;
			sym_addr = (int)header + sym_tab->sh_offset;
			section_name = elf_lookup_string(header, sym_tab->sh_name);
	
			printf("Symbol table: %s\n", section_name);
			printf("[Nr] %8s%15s%8s%13s\n", "Value", "Section Name", "Sec_ndx", "Symbol Name");
			for(i=0;i<symtab_entries;i++){
				symbol = &((Elf32_Sym *)sym_addr)[i];
				sym_name = elf_lookup_string(header, symbol->st_name);
				printf("[%2d] %08x%15s%8d%8d\n", i, symbol->st_value, sym_name, symbol->st_shndx, symbol->st_info);
			}
		}
	}
}
void quit() {
	munmap(map_start, fd_stat.st_size);
	if(debug_mode)
		printf("quitting\n");
	exit(0);
}

int main(int argc, char** argv) {
	int op, amount_of_funcs, i, jnk;
	struct fun_desc menu[] ={{"Toggle Debug Mode", toggle_debug}, {"Examine ELF File", ex_elf_file}, 
							 {"Print Section Names", print_sections}, {"Print Symbols", print_symbols},
							 {"Quit", quit}, {NULL, NULL}};
	amount_of_funcs = sizeof(menu)/sizeof(*menu)-1;

	while(1){
		if(debug_mode){
			printf("\n");
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
			menu[op].func();
		else
			printf("outside bounds\n");
	}
}