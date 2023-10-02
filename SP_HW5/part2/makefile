all: shell.o run_command.o is_background.o builtin.o pipe_command.o pipe_present.o redirect_in.o redirect_out.o parse.o
	gcc -o myshell shell.o run_command.o is_background.o builtin.o pipe_command.o pipe_present.o redirect_in.o redirect_out.o parse.o
shell.o: shell.c shell.h
	gcc -c shell.c
run_command.o: run_command.c shell.h
	gcc -c run_command.c
is_background.o: is_background.c shell.h
	gcc -c is_background.c
builtin.o: builtin.c shell.h
	gcc -c builtin.c
pipe_command.o: pipe_command.c shell.h
	gcc -c pipe_command.c
pipe_present.o: pipe_present.c shell.h
	gcc -c pipe_present.c
redirect_in.o: redirect_in.c shell.h
	gcc -c redirect_in.c
redirect_out.o: redirect_out.c shell.h
	gcc -c redirect_out.c
parse.o: parse.c shell.h
	gcc -c parse.c
	
