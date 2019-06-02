
all: myELF

myELF: myELF.o 
	gcc -g -W -m32 myELF.o  -o myELF
	
myELF.o: myELF.c
	gcc -g -Wall -m32 -c myELF.c -o myELF.o

.PHONY: clean
clean:
	rm -f *.o myELF