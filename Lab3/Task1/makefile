
all: virus

virus : virus.o
	gcc -g -W -m32 -o virus virus.o

virus.o : virus.c
	gcc -g -W -m32 -c -o virus.o virus.c

.PHONY : clean
clean : 
	rm -f *.o virus