
all: hexeditplus

hexeditplus: hexeditplus.o 
	gcc -g -W -m32 hexeditplus.o  -o hexeditplus
	
hexeditplus.o: hexeditplus.c
	gcc -g -Wall -m32 -c hexeditplus.c -o hexeditplus.o

.PHONY: clean
clean:
	rm -f *.o hexeditplus