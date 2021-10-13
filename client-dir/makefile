CC = gcc
CFLAGS = -Wall
othello_client: othello_client.o client.o
	$(CC) client.o othello_client.o -o client `pkg-config --libs gtk+-2.0`
client.o: client.c othello_client.h
	$(CC) -c client.c -o client.o $(CFLAGS) `pkg-config gtk+-2.0 --cflags`
othello_client.o: othello_client.c othello_client.h
	$(CC) -c othello_client.c -o othello_client.o $(CFLAGS) `pkg-config gtk+-2.0 --cflags`
clean:
	rm -f *.o othello_client