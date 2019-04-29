all: looper

looper: looper.o 
	gcc -g -W -m32 looper.o  -o looper
	
looper.o: looper.c
	gcc -g -Wall -m32 -c looper.c -o looper.o

.PHONY: clean
clean:
	rm -f *.o looper