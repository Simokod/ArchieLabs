
all: Task0a

Task0a: Task0a.o 
	gcc -g -W -m32 Task0a.o  -o Task0a
	
Task0a.o: Task0a.c
	gcc -g -Wall -m32 -c Task0a.c -o Task0a.o

.PHONY: clean
clean:
	rm -f *.o Task0a