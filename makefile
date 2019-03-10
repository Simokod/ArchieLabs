 
all: arrP

arrP: arrP.o
	gcc -m32 -g -Wall -o arrP arrP.c
	
arrP.o: arrP.c
	gcc -g -Wall -m32 -c -o arrP.o arrP.c
	
.PHONY: clean

clean:
	rm -f *.o arrP
