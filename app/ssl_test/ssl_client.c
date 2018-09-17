#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <netinet/in.h>  

#define CHK_ERR(err, s) if((err) == -1) { perror(s); return -1; }
#define CHK_RV(rv, s) if((rv) != 1) { printf("%s error\n", s); return -1; }
#define CHK_NULL(x, s) if((x) == NULL) { printf("%s error\n", s); return -1; }
#define CHK_SSL(err, s) if((err) == -1) { ERR_print_errors_fp(stderr);  return -1;}
 
int main(int argc, char *argv[])
{
	int rv;
	int err;
	int listen_sd;
	struct sockaddr_in socketAddrClient;
	SSL_CTX *ctx = NULL;
	SSL *ssl = NULL;
	char buf[4096];
 
	rv = SSL_library_init();
	CHK_RV(rv, "SSL_library_init");
 
	ctx = SSL_CTX_new(SSLv23_client_method());
	CHK_NULL(ctx, "SSL_CTX_new");
 
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	CHK_ERR(listen_sd, "socket");
	memset(&socketAddrClient, 0, sizeof(socketAddrClient));
	socketAddrClient.sin_family = AF_INET;
	socketAddrClient.sin_port = htons(443);
	socketAddrClient.sin_addr.s_addr = inet_addr("192.168.31.83");
 
	err = connect(listen_sd, (struct sockaddr *)&socketAddrClient, sizeof(socketAddrClient));
	CHK_ERR(err, "connect");
	ssl = SSL_new(ctx);
	CHK_NULL(ssl, "SSL_new");
	rv = SSL_set_fd(ssl, listen_sd);
	CHK_RV(rv, "SSL_set_fd");
	rv = SSL_connect(ssl);
	CHK_RV(rv, "SSL_connect");
 
	rv = SSL_write(ssl, "Hello, I am the client", strlen("Hello, I am the client"));
	CHK_SSL(rv, "SSL_write");
	rv = SSL_read(ssl, buf, sizeof(buf) - 1);
	CHK_SSL(rv, "SSL_read");
	buf[rv] = '\0';
	printf("Got %d chars :%s\n", rv, buf);
 
	SSL_shutdown(ssl);
	close(listen_sd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
 
	return 0;
}
