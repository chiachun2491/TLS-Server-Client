all: Server Client

CC = gcc
CFLAG = -lcrypto -lssl 

Server: server.c 
	$(CC) -o Server server.c mySSLTool.c myFileTool.c $(CFLAG)

Client: client.c mySSLTool.h
	$(CC) -o Client client.c mySSLTool.c myFileTool.c $(CFLAG)

.PHONY: clean,run
clean:
	rm Server Client 

cleancrt:
	rm ca* host.crt host.key client.crt client.key

cleanvol:
	rm -r downloads storage

runserver:
	./Server

runclient:
	./Client localhost 4433

init:
	mkdir storage
	mkdir downloads
	cp *.c *.h storage
	openssl genrsa -des3 -out ca.key 4096
	openssl req -x509 -new -nodes -key ca.key -sha256 -days 1024 -out ca.crt
	openssl req -nodes -newkey rsa:4096 -keyout host.key -out host.csr -subj "/C=TW/ST=Taiwan/L=Taipei/O=NTUST/OU=CSIE/CN=host.ntust.edu.tw"
	openssl x509 -req -in host.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out host.crt -days 30 -sha256
	rm host.csr
	openssl req -nodes -newkey rsa:4096 -keyout client.key -out client.csr -subj "/C=TW/ST=Taiwan/L=Taipei/O=NTUST/OU=CSIE/CN=client.ntust.edu.tw"
	openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 30 -sha256
	rm client.csr
	make