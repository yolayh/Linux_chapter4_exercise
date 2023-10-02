all: msq_client.o msq_server.o
	gcc -o msq_client msq_client.o
	gcc -o msq_server msq_server.o
msq_client.o: msq_client.c 
	gcc -c msq_client.c
msq_server.o: msq_server.c 
	gcc -c msq_server.c