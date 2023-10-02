all: hostinfo.o mydate.o printdir.o mycat.o pipe_ls.o
	gcc -o hostinfo hostinfo.o
	gcc -o mydate mydate.o
	gcc -o printdir printdir.o
	gcc -o mycat mycat.o
	gcc -o pipe_ls pipe_ls.o
pipe_ls.o: pipe_ls.c 
	gcc -c pipe_ls.c
mycat.o: mycat.c
	gcc -c mycat.c
printdir.o: printdir.c
	gcc -c printdir.c
mydate.o: mydate.c 
	gcc -c mydate.c
hostinfo.o: hostinfo.c
	gcc -c hostinfo.c
	
