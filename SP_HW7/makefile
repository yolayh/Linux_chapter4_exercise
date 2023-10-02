all: sig_waitinfo.o sig_suspend.o sig_function.o
	gcc -o 22-3 sig_waitinfo.o
	gcc -o suspend sig_suspend.o
	gcc -o 22-4 sig_function.o
sig_function.o: sig_function.c
	gcc -c sig_function.c
sig_suspend.o: sig_suspend.c 
	gcc -c sig_suspend.c
sig_waitinfo.o: sig_waitinfo.c 
	gcc -c sig_waitinfo.c