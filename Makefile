CC = c99
CFLAGS = -Wall # Show all reasonable warnings
LDFLAGS = 

all: Client Server


Client: Client.o
	gcc -std=c99 -o Client Client.o

Client.o: Client.c
	gcc -std=c99 -c Client.c


Server: Server.o
	gcc -std=c99 -o Server Server.o

Server.o: Server.c
	gcc -std=c99 -c Server.c


debug: BugCli BugSer

BugCli: Client.c
	gcc -std=c99 -g -o debug_client Client.c

BugSer: Server.c
	gcc -std=c99 -g -o debug_server Server.c


clean:
	rm -f Client Server *.o *.exe

.PHONY: all clean