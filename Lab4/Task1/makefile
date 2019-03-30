
all: encoder

encoder: start.o encoder.o util.o
	ld -m elf_i386 start.o encoder.o util.o -o encoder

start.o: lab4_start.s
	nasm -f elf lab4_start.s -o start.o

util.o: lab4_util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o util.o

encoder.o: encoder.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector encoder.c -o encoder.o

.PHONY : clean
clean : 
	rm -f *.o encoder