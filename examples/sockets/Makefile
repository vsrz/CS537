
all: client server

client:
	gcc caseClient.c -o caseClient

server:
	gcc caseServer.c -o caseServer -lpthread

clean:
	rm -f caseServer && rm -f caseClient
