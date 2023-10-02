all: convert.o lookup1.o main.o
	gcc -o convert convert.o
	gcc -o file_lookup lookup1.o main.o
main.o: main.c dict.h
	gcc -c main.c
lookup1.o: lookup1.c dict.h
	gcc -c lookup1.c
convert.o: convert.c dict.h
	gcc -c convert.c
	
