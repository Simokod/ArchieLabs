all: mypipe

mypipe: mypipe.o 
	gcc -g -W -m32 mypipe.o  -o mypipe
	
mypipe.o: mypipe.c
	gcc -g -Wall -m32 -c mypipe.c -o mypipe.o

.PHONY: clean
clean:
	rm -f *.o mypipe