# makefile
CC=g++ -Wall

all: server client

server: server.o
	$(CC) -o server server.o

client: client.o
	$(CC) -o client -std=c++0x -pthread client.o

server.o: server.cpp
	$(CC) -c server.cpp

client.o: client.cpp
	$(CC) -c -std=c++0x -pthread client.cpp

clean:
	rm -f *.o core.* server client
