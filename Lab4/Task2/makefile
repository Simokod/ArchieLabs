
all: flame2

flame2: start.o flame.o util.o
	ld -m elf_i386 start.o flame.o util.o -o flame2

start.o: lab4_start.s
	nasm -f elf lab4_start.s -o start.o

util.o: lab4_util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o util.o

flame.o: flame.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector flame.c -o flame.o

.PHONY : clean
clean : 
	rm -f *.o flame