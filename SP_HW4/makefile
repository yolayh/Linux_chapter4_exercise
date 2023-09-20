all: file_open_list.o draw_pid_tree.o process_list.o
	gcc -o one process_list.o
	gcc -o two draw_pid_tree.o
	gcc -o three file_open_list.o
file_open_list.o: file_open_list.c 
	gcc -c file_open_list.c
draw_pid_tree.o: draw_pid_tree.c 
	gcc -c draw_pid_tree.c
process_list.o: process_list.c 
	gcc -c process_list.c	
	
