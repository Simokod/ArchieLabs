
all: myshell

myshell: myshell.o LineParser.o
	gcc -g -W -m32 myshell.o LineParser.o -o myshell

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c LineParser.c -o LineParser.o

myshell.o: myshell.c
	gcc -g -Wall -m32 -c myshell.c -o myshell.o

.PHONY: clean
clean:
	rm -f *.o myshell