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

BIO *bio_err = NULL;


// typedef struct ssl_ca_crt_key
// {
//     unsigned char *cacrt;
//     unsigned int cacrt_len;
//     unsigned char *cert;
//     unsigned int cert_len;
//     unsigned char *key;
//     unsigned int key_len;
// } ssl_ca_crt_key_t;

// /**
//  * cert file load
//  */
// ssl_ca_crt_key_t *sslcert_load(char *cacrt, char *cert, char *key)
// {
// 	ssl_ca_crt_key_t *file;
// 	struct stat s;
// 	int fd;
// 	int n, br;

// 	file = (ssl_ca_crt_key_t*)malloc(sizeof(ssl_ca_crt_key_t));
// 	if (file == NULL)
// 		return NULL;
// 	memset(file, 0, sizeof(ssl_ca_crt_key_t));

// 	if (cacrt)
// 	{
// 		fd = open(cacrt, O_RDONLY);
// 		if (fd < 0)
// 			goto failed;
// 		fstat(fd, &s);
// 		file->cacrt_len = s.st_size;
// 		file->cacrt = (uint8_t*)malloc(s.st_size+8);
// 		if (file->cacrt == NULL)
// 			goto failed1;
// 		n = 0;
// 		while ((br = read (fd, (void*)(file->cacrt+n), 1024)) > 0)
// 		{
// 			n += br;
// 		}
// 		close(fd);
// 	}

// 	if (cert)
// 	{
// 		fd = open(cert, O_RDONLY);
// 		if (fd < 0)
// 			goto failed2;
// 		fstat(fd, &s);
// 		file->cert_len = s.st_size;
// 		file->cert = (uint8_t*)malloc(s.st_size+8);
// 		if (file->cert == NULL)
// 			goto failed2;
// 		n = 0;
// 		while ((br = read (fd, file->cert+n, 1024)) > 0)
// 		{
// 			n += br;
// 		}
// 		close(fd);
// 	}

// 	if (key)
// 	{
// 		fd = open(key, O_RDONLY);
// 		if (fd < 0)
// 			goto failed3;
// 		fstat(fd, &s);
// 		file->key_len = s.st_size;
// 		file->key = (uint8_t*)malloc(s.st_size+8);
// 		if (file->key == NULL)
// 			goto failed3;
// 		n = 0;
// 		while ((br = read (fd, file->key+n, 1024)) > 0)
// 		{
// 			n += br;
// 		}
// 		close(fd);
// 	}

// 	return file;

// failed3:
// 	if (file->cert)
// 		free(file->cert);
// failed2:
// 	if (file->cacrt)
// 		free((void*)(file->cacrt));
// failed1:
// 	close(fd);
// failed:
// 	if (file)
// 		free(file);
// 	return NULL;
// }

// void sslcert_free(ssl_ca_crt_key_t *s)
// {
// 	if (s == NULL)
// 	{
// 		return ;
// 	}
// 	if (s->cacrt)
// 	{
// 		free((void*)(s->cacrt));
// 		s->cacrt = 0;
// 	}
// 	if (s->cert)
// 	{
// 		free(s->cert);
// 		s->cert = 0;
// 	}
// 	if (s->key)
// 	{
// 		free(s->key);
// 		s->key = 0;
// 	}
// 	if (s)
// 	{
// 		free(s);
// 		s = 0;
// 	}
// }

int verify_callback(int ok, X509_STORE_CTX *ctx)
{
    char *s, buf[256];

	s = X509_NAME_oneline(X509_get_subject_name(ctx->current_cert),
							buf, 256);
	if (s != NULL) {
		if (ok)
			BIO_printf(bio_err, "depth=%d %s\n", ctx->error_depth, buf);
		else
			BIO_printf(bio_err, "depth=%d error=%d %s\n",
					ctx->error_depth, ctx->error, buf);
	}
    return (ok);
}

int ssl_test(void)
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
	// cert files
	// ssl_ca_crt_key_t *ssl_cck=NULL;
	// ssl_cck = sslcert_load("../mango_test/cert/owin.cer", NULL, NULL);
	// if (ssl_cck == NULL)
	// {
	// 	printf("sslfile_load error!\n");
	// 	return -1;
	// }
	// X509 *cacrt = d2i_X509(NULL, (const unsigned char **)&ssl_cck->cacrt, ssl_cck->cacrt_len);
	// if (cacrt == NULL)
	// {
	// 	printf("d2i_X509 error\n");
	// 	return -1;
	// }
	// SSL_CTX_add_client_CA(ctx, cacrt);
	// sslcert_free(ssl_cck);
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

int main(int argc, char *argv[])
{
	ssl_test();
	return 0;
}

