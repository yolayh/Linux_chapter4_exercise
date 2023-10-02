all: fifo_server_nowrite.o fifo_server.o fifo_client.o fifo_client_malicious.o fifo_client_nonblock.o
	gcc -o fifo_s_nowrite fifo_server_nowrite.o
	gcc -o fifo_server fifo_server.o
	gcc -o fifo_client fifo_client.o
	gcc -o fifo_bad_client fifo_client_malicious.o
	gcc -o fifo_c_nonblock fifo_client_nonblock.o
fifo_client_nonblock.o: fifo_client_nonblock.c
	gcc -c fifo_client_nonblock.c
fifo_client_malicious.o: fifo_client_malicious.c
	gcc -c fifo_client_malicious.c
fifo_client.o: fifo_client.c 
	gcc -c fifo_client.c
fifo_server.o: fifo_server.c 
	gcc -c fifo_server.c
fifo_server_nowrite.o: fifo_server_nowrite.c 
	gcc -c fifo_server_nowrite.c