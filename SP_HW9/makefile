all: copy.o share_mem_reader.o share_mem_writer.o svmsg_file_client.o svmsg_file_server.o
	gcc -o cp copy.o
	gcc -o reader share_mem_reader.o
	gcc -o writer share_mem_writer.o
	gcc -o mq_client svmsg_file_client.o
	gcc -o mq_server svmsg_file_server.o
copy.o: copy.c
	gcc -c copy.c
share_mem_reader.o: share_mem_reader.c
	gcc -c share_mem_reader.c
share_mem_writer.o: share_mem_writer.c
	gcc -c share_mem_writer.c
svmsg_file_client.o: svmsg_file_client.c
	gcc -c svmsg_file_client.c
svmsg_file_server.o: svmsg_file_server.c
	gcc -c svmsg_file_server.c