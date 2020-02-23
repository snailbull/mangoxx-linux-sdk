#ifndef _MANGO_HTTPC_TEST_
#define _MANGO_HTTPC_TEST_

#include <stdint.h>

int basicAuth_test(void);
int get_test(char *server, uint16_t port, char *url);
int sslget_test(char *server, uint16_t port, char *url);

int persistent_test(void);
int post_test(void);
int shoutcast_test(void);
int websockets_test(void);

int ssl_client(void);
int ssl_server(void);

#endif
