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


int currentfd = -1;
void *map_start;				/* will point to the start of the memory mapped file */
struct stat fd_stat; 			/* this is needed to get the size of the file */
Elf32_Ehdr *header; 			/* this will point to the header structure */

Elf32_Phdr *elf_pheader() {
	return (Elf32_Phdr *)((int)header + header->e_phoff);
}
Elf32_Phdr *elf_prog(int idx) {
	return &elf_pheader(header)[idx];
}

int main(int argc, char** argv) {
	char *file_name, byte1, byte2, byte3;
	int currentfd, i;
	Elf32_Phdr *curr_hdr;

	if(argc != 2){
		printf("wrong number of args\n");
		exit(-1);
	}
	file_name = argv[1];
	if((currentfd = open(file_name, O_RDWR)) < 0) {
		perror("error in open");
		exit(-1);
	}
	if(fstat(currentfd, &fd_stat) != 0) {
		currentfd = -1;
		perror("stat failed");
		exit(-1);
	}
	if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, currentfd, 0)) == MAP_FAILED) {
		close(currentfd);
		perror("mmap failed");
		exit(-1);
	}
	header = (Elf32_Ehdr *) map_start;

	byte1 = header->e_ident[EI_MAG1];
	byte2 = header->e_ident[EI_MAG2];
	byte3 = header->e_ident[EI_MAG3];
	if(byte1 != 'E' || byte2 != 'L' || byte3 != 'F'){
		printf("this file is not an ELF file: %s\n", file_name);
		munmap(map_start, fd_stat.st_size);
		close(currentfd);
		exit(-1);
	}
	printf("%s\t%s   %s   %s   %s %s  %s %s\n", "Type", "Offset", "VirtAddr", "PhysAddr", "FileSiz", "MemSiz", "Flg", "Align");
	for(i=0;i<header->e_phnum;i++){
		curr_hdr = elf_prog(i);
		printf("%d\t%#08x %#010x %#010x %#07x %#07x %d %#7x\n", 
			curr_hdr->p_type, curr_hdr->p_offset, curr_hdr->p_vaddr, curr_hdr->p_paddr, curr_hdr->p_filesz, curr_hdr->p_memsz, curr_hdr->p_flags , curr_hdr->p_align);
	}
	close(currentfd);
	munmap(map_start, fd_stat.st_size);
}