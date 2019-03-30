
all: patch

patch: start.o patch.o util.o
	ld -m elf_i386 start.o patch.o util.o -o patch

start.o: lab4_start.s
	nasm -f elf lab4_start.s -o start.o

util.o: lab4_util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o util.o

patch.o: patch.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector patch.c -o patch.o

.PHONY : clean
clean : 
	rm -f *.o patch