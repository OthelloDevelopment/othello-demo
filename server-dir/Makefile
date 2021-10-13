CC = gcc
CFLAGS = -Wall

othello_server: othello_server.o server.o
	$(CC) server.o othello_server.o -o server `pkg-config --libs gtk+-2.0`
server.o: server.c othello_server.h
	$(CC) -c server.c -o server.o $(CFLAGS) `pkg-config gtk+-2.0 --cflags`
othello_server.o: othello_server.c othello_server.h
	$(CC) -c othello_server.c -o othello_server.o $(CFLAGS) `pkg-config gtk+-2.0 --cflags`
clean:
	rm -f *.o othello_server








