#Compiler
CC = gcc

#Compiler flags
CFLAGS = -Wall -Wextra


#Build dispatcher executable
dispatcher: dispatcher.c
	$(CC) $(CFLAGS) -o dispatcher dispatcher.c -lrt -lpthread

#Build server executable
server: server.c
	$(CC) $(CFLAGS) -o server server.c -lrt -lpthread

#Build client executable
client: client.c
	$(CC) $(CFLAGS) -o client client.c -lrt -lpthread

#Run the dispatcher
run-dispatcher: dispatcher
	./dispatcher /shm1 /shm2 $(ARGS)

#Run the client with dynamic arguments
run-client: client
	./client /shm1 $(ARGS)

#Clean up executables
clean:
	rm -f dispatcher server client

#Delete the file "/tmp/server_on.txt" ==> sudo rm -r /tmp/server_on.txt



