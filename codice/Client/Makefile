all: CrackedSoftware

CrackedSoftware: CrackedSoftware.o
	gcc CrackedSoftware.o -o CrackedSoftware.out

CrackedSoftware.o: CrackedSoftware.c keylogger.h
	gcc -c CrackedSoftware.c

clean:
	rm -f *.o
	rm -f *.out