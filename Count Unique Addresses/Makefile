all: count.c hashtable.o
	gcc -Wall -o count count.c hashtable.o

hashtable.o: hashtable.c hashtable.h
	gcc -c hashtable.c
clean:
	-rm count
	-rm hashtable.o
	-rm ./*.o
