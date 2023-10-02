all: two_pipe.o p_open_close.o
	gcc -o two_pipe two_pipe.o
	gcc -o p_open_close p_open_close.o
p_open_close.o: p_open_close.c 
	gcc -c p_open_close.c
two_pipe.o: two_pipe.c 
	gcc -c two_pipe.c	
	
