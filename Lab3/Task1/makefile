
all: virusDetector

virusDetector : virusDetector.o
	gcc -g -W -m32 -o virusDetector virusDetector.o

virusDetector.o : virusDetector.c
	gcc -g -W -m32 -c -o virusDetector.o virusDetector.c

.PHONY : clean
clean : 
	rm -f *.o virusDetector