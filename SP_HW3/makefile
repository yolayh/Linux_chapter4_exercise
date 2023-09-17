all: shell.o run_command.o builtin.o is_background.o parse.o 
	gcc -g -o my_shell parse.o is_background.o builtin.o run_command.o shell.o 
shell.o: shell.c shell.h
	gcc -c shell.c
run_command.o: run_command.c shell.h
	gcc -c run_command.c
builtin.o: builtin.c shell.h
	gcc -c builtin.c
is_background.o: is_background.c shell.h
	gcc -c is_background.c
parse.o: parse.c shell.h
	gcc -c parse.c	
	
