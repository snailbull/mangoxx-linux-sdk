#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <sys/socket.h>
#include <netinet/in.h>  
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#define CHK_ERR(err, s) if((err) == -1) { perror(s); return -1; }
#define CHK_RV(rv, s) if((rv) != 1) { printf("%s error\n", s); return -1; }
#define CHK_NULL(x, s) if((x) == NULL) { printf("%s error\n", s); return -1; }
#define CHK_SSL(err, s) if((err) == -1) { ERR_print_errors_fp(stderr);  return -1;}


/******************************************************************************
 * client
 */
BIO *bio_err = NULL;

int verify_callback(int ok, X509_STORE_CTX *ctx)
{
    char *s, buf[256];

	s = X509_NAME_oneline(X509_get_subject_name(ctx->current_cert), buf, 256);
	if (s != NULL) {
		if (ok)
			BIO_printf(bio_err, "depth=%d %s\n", ctx->error_depth, buf);
		else
			BIO_printf(bio_err, "depth=%d error=%d %s\n", ctx->error_depth, ctx->error, buf);
	}
    return (ok);
}

int ssl_client(void)
{
	int rv;
	int err;
	int listen_sd;
	struct sockaddr_in socketAddrClient;
	SSL_CTX *ctx = NULL;
	SSL *ssl = NULL;
	char buf[4096];
	int len;
	struct hostent *h;
	int i;
	int port = 443;

	rv = SSL_library_init();
	CHK_RV(rv, "SSL_library_init");

	if (bio_err == NULL)
		bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
	
	ctx = SSL_CTX_new(TLSv1_2_client_method());
	CHK_NULL(ctx, "SSL_CTX_new");
	
#if 0
	// cert files
	ssl_ca_crt_key_t *ssl_cck=NULL;
	ssl_cck = sslcert_load("../mango_test/cert/owin.cer", NULL, NULL);
	if (ssl_cck == NULL)
	{
		printf("sslfile_load error!\n");
		return -1;
	}
	X509 *cacrt = d2i_X509(NULL, (const unsigned char **)&ssl_cck->cacrt, ssl_cck->cacrt_len);
	if (cacrt == NULL)
	{
		printf("d2i_X509 error\n");
		return -1;
	}
	SSL_CTX_add_client_CA(ctx, cacrt);
	sslcert_free(ssl_cck);
#endif

	SSL_CTX_load_verify_locations(ctx, "../mango_test/cert/ca.crt", "../mango_test/cert/");
	SSL_CTX_use_certificate_file(ctx, "../mango_test/cert/client.crt", SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ctx, "../mango_test/cert/client.key", SSL_FILETYPE_PEM);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);

	// get ip
	// h = gethostbyname("dpos-pay.line-apps-beta.com");
	h = gethostbyname("192.168.31.141");
	if (h == NULL)
	{
		printf("gethostbyname error\n");
		return -1;
	}
	printf("connect %s:%d\n", inet_ntoa(*(struct in_addr*)(h->h_addr_list[0])), port);


	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	CHK_ERR(listen_sd, "socket");
	memset(&socketAddrClient, 0, sizeof(socketAddrClient));
	socketAddrClient.sin_family = AF_INET;
	socketAddrClient.sin_port = htons(port);
	socketAddrClient.sin_addr = *(struct in_addr*)(h->h_addr_list[0]);
 
	err = connect(listen_sd, (struct sockaddr *)&socketAddrClient, sizeof(socketAddrClient));
	CHK_ERR(err, "connect");
	
	ssl = SSL_new(ctx);
	CHK_NULL(ssl, "SSL_new");
	rv = SSL_set_fd(ssl, listen_sd);
	CHK_RV(rv, "SSL_set_fd");
	rv = SSL_connect(ssl);
	CHK_RV(rv, "SSL_connect");
	
	len = sprintf(buf, 
		"GET /readme.txt HTTP/1.1\r\n"
		// "GET /v1/devices/123456789001/orders?order_no=000012000002 HTTP/1.1\r\n"
		// "Host: dpos-pay.line-apps-beta.com\r\n"
		"Host: 192.168.31.141\r\n"
		// "Content-Type: text/plain; charset=utf-8\r\n"
		// "x-dpos-timestamp: 1534162153\r\n"
		// "x-dpos-signature: 07937abb507bee3ed8e337fe1dca141485df3a3e5c5aea0b1b76d8b71a272268\r\n"
		"Connection: keep-alive\r\n\r\n"
		);
	rv = SSL_write(ssl, buf, len);
	CHK_SSL(rv, "SSL_write");

	while (1)
	{
		rv = SSL_read(ssl, buf, sizeof(buf) - 1);
		if (rv <= 0)
		{
			break;
		}
		CHK_SSL(rv, "SSL_read");
		buf[rv] = '\0';
		printf("%s\n",buf);
	}
 
	SSL_shutdown(ssl);
	close(listen_sd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
 
	return 0;
}


/******************************************************************************
 * server
 */
int ssl_server(void)
{
	int rv, err;
	SSL_CTX *ctx = NULL;
	int listen_sd;
	int accept_sd;
	struct sockaddr_in socketAddrServer;
	struct sockaddr_in socketAddrClient;
	int socketAddrClientLen;
	SSL *ssl = NULL;
	char buf[4096];
 
	rv = SSL_library_init();
	CHK_RV(rv, "SSL_library_init");
 
	ctx = SSL_CTX_new(SSLv23_server_method());
	CHK_NULL(ctx, "SSL_CTX_new");
 
	rv = SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
	CHK_RV(rv, "SSL_CTX_use_certicificate_file");
 
	rv = SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);
	CHK_RV(rv, "SSL_CTX_use_PrivateKey_file");
	
	rv = SSL_CTX_check_private_key(ctx);
	CHK_RV(rv, "SSL_CTX_check_private_key");
	
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	CHK_ERR(listen_sd, "socket");
	memset(&socketAddrServer, 0, sizeof(socketAddrServer));
	socketAddrServer.sin_family = AF_INET;
	socketAddrServer.sin_port = htons(8443);
	socketAddrServer.sin_addr.s_addr = INADDR_ANY;
 
	err = bind(listen_sd, (struct sockaddr *)&socketAddrServer, sizeof(socketAddrServer));
	CHK_ERR(err, "bind");
	err = listen(listen_sd, 5);
	CHK_ERR(err, "listen");
 
	socketAddrClientLen = sizeof(socketAddrClient);
	accept_sd = accept(listen_sd, (struct sockaddr *)&socketAddrClient, &socketAddrClientLen);
	CHK_ERR(accept_sd, "accept");
	close(listen_sd);
	printf("Connect to %lx, port %x\n", socketAddrClient.sin_addr.s_addr, socketAddrClient.sin_port);
 
	ssl = SSL_new(ctx);
	CHK_NULL(ssl, "SSL_new");
	rv = SSL_set_fd(ssl, accept_sd);
	CHK_RV(rv, "SSL_set_fd");
	rv = SSL_accept(ssl);
	CHK_RV(rv, "SSL_accpet");
 
	rv = SSL_read(ssl, buf, sizeof(buf) - 1);
	CHK_SSL(rv, "SSL_read");
	buf[rv] = '\0';
	printf("Got %d chars :%s\n", rv, buf);
	rv = SSL_write(ssl, "I accept your request", strlen("I accept your request"));
	CHK_SSL(rv, "SSL_write");
 
	close(accept_sd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
 
	return 0;
}
