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


int main(int argc, char *argv[])
{
	int i;
	struct hostent *h;

	printf("gethostbyname\n");
	h = gethostbyname("120.77.1.207");
	if (h == NULL)
	{
		printf("gethostbyname error\n");
		return -1;
	}
	if (h->h_name)
	{
		printf("h_name:%s\n", h->h_name);
	}
	printf("h_aliases:\n");
	for (i=0; ;i++)
	{
		if (h->h_aliases[i] == NULL)
		{
			printf("break i=%d\n", i);
			break;
		}

		printf("  %s\n", h->h_aliases[i]);
	}
	printf("h_addrtype:%d\n", h->h_addrtype);
	printf("h_length:%d\n", h->h_length);
	printf("h_addr_list:\n");
	for (i=0; ; i++)
	{
		if (h->h_addr_list[i] == NULL)
		{
			printf("break i=%d\n", i);
			break;
		}

		printf("  %s\n", inet_ntoa(*(struct in_addr*)(h->h_addr_list[i])));
	}


	/***************************************************************/
	printf("gethostbyaddr\n");
	struct in_addr sin_addr;
	sin_addr.s_addr = inet_addr("120.77.1.207");
	h = gethostbyaddr((char*)&sin_addr.s_addr, 4, AF_INET);

	if (h == NULL)
	{
		printf("gethostbyaddr error\n");
		return -1;
	}
	if (h->h_name)
	{
		printf("h_name:%s\n", h->h_name);
	}
	printf("h_aliases:\n");
	for (i=0; ;i++)
	{
		if (h->h_aliases[i] == NULL)
		{
			printf("break i=%d\n", i);
			break;
		}

		printf("  %s\n", h->h_aliases[i]);
	}
	printf("h_addrtype:%d\n", h->h_addrtype);
	printf("h_length:%d\n", h->h_length);
	printf("h_addr_list:\n");
	for (i=0; ; i++)
	{
		if (h->h_addr_list[i] == NULL)
		{
			printf("break i=%d\n", i);
			break;
		}

		printf("  %s\n", inet_ntoa(*(struct in_addr*)(h->h_addr_list[i])));
	}


	return 0;
}



