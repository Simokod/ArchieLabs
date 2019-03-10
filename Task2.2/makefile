 
all: TaskTwo

TaskTwo: TaskTwo.o
	gcc -m32 -g -Wall -o TaskTwo TaskTwo.c
	
TaskTwo.o: TaskTwo.c
	gcc -g -Wall -m32 -c -o TaskTwo.o TaskTwo.c
	
.PHONY: clean

clean:
	rm -f *.o TaskTwo
