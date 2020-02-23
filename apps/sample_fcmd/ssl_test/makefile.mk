

.PHONY: all 
all: ssl_client ssl_server

ssl_client:ssl_client.c
	gcc ssl_client.c -o ssl_client -lssl -lcrypto

ssl_server:ssl_server.c
	gcc ssl_server.c -o ssl_server -lssl -lcrypto

clean:
	rm ssl_client ssl_server