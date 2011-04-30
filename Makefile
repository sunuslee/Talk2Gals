TOP = .
CFLAGS = -g -Wall -std=gnu99
s : server.c
	gcc $(CFLAGS) server.c -o server
c : client.c
	gcc $(CFLAGS) client.c -o client
clean :
	rm -rf client server
