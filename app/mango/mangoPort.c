/*
 * mango HTTP client
 *
 * Copyright (C) 2015,  Nikos Poulokefalos
 *
 * This file is part of mango HTTP client.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * npoulokefalos@gmail.com
*/

#include "mango.h"

#ifdef MANGO_IP_ENV__UNIX
    #include <stdint.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h> 

    #include <sys/time.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

#ifdef MANGO_IP_ENV__LWIP
    #include "lwip/sockets.h"
#endif


/**
 * @brief   Get the current timestamp in miliseconds
 */
uint32_t mangoPort_timeNow(){
    
#ifdef MANGO_OS_ENV__UNIX
    struct timeval  tv;
	gettimeofday(&tv, NULL);

    return  (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
#endif
    
#ifdef MANGO_OS_ENV__CHIBIOS
    return chTimeNow();
#endif
}

/**
 * @brief   Sleep for the specified number of miliseconds
 */
void mangoPort_sleep(uint32_t ms){
#ifdef MANGO_OS_ENV__UNIX
    usleep(ms * 1000);
#endif
    
#ifdef MANGO_OS_ENV__CHIBIOS
    chThdSleepMilliseconds(ms);
#endif
}

/**
 * @brief   Alocate "sz" bytes from heap
 */
void* mangoPort_malloc(uint32_t sz){
#ifdef MANGO_OS_ENV__UNIX
    return malloc(sz);
#endif
    
#ifdef MANGO_OS_ENV__CHIBIOS
    return chHeapAlloc(NULL, sz);
#endif
}

/**
 * @brief   Free the specific memory block
 */
void mangoPort_free(void* ptr){
#ifdef MANGO_OS_ENV__UNIX
    free(ptr);
#endif
    
#ifdef MANGO_OS_ENV__CHIBIOS
    chHeapFree(ptr);
#endif
}


/**
 * @brief   Read at most "datalen" bytes from the specified socket. Wait until at least 
 *          1 byte has been read or until the "timeout" [miliseconds] expires.
 *          NOTE: The function should return on the first succesfull read operation,
 *          even if the number of bytes read was smaller not equal to "datalen". "buflen"
 *          only defines the mamixum number of bytes that should be read.
 *
 * @retval  >= 0    The number of bytes read. 0 means timeout happend and no bytes were read.
 * @retval  < 0     Indicates connection error. In this case the function should return
 *                  even if the timeout has not been expired. mango will return with an error.
 */
int mangoPort_read(int socketfd, uint8_t* data, uint16_t datalen, uint32_t timeout){
    uint32_t received;
    uint32_t start;
    int socketerror;
    int retval;
	
    MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("Trying to read %u bytes\r\n", datalen) );
    
    received = 0;
    start = mangoPort_timeNow();
	while(received < datalen){
    
        retval = recv(socketfd, &data[received], datalen - received, 0);
        //printf("READ %d\r\n", retval);
        if(retval < 0){
            //socketerrorlen = sizeof(socketerror);
            //retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#ifdef MANGO_IP_ENV__UNIX
            socketerror = errno;
#endif
            
#ifdef MANGO_IP_ENV__LWIP
            socklen_t socketerrorlen;
            socketerrorlen = sizeof(socketerror);
            retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#endif
            
            MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("!!!!!!! READ SOCKET ERROR %d\r\n", socketerror) );
            
            if(socketerror == EWOULDBLOCK || socketerror == EAGAIN){
                mangoPort_sleep(64);
            }else{
                return -1;
            }
        }else{
            received += retval;
            return received;
        }
        
        if(mangoHelper_elapsedTime(start) > timeout){
            return received;
        }
	}
	
	MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("%u bytes read\r\n", retval) );
	
	return retval;
}


/**
 * @brief   Write exactly "datalen" bytes to the specified socket. Wait until all data 
 *          have been sent or until the "timeout" [miliseconds] expires. If the timeout 
 *          expires and not ALL data have been sent, mango will consider the connection 
 *          closed, even if the function didn't return a value < 0.
 *          Check MANGO_SOCKET_WRITE_TIMEOUT_MS definition for the default timeout
 *          value.
 *
 * @retval  >= 0    The number of bytes sent. If this number is not equal to "datalen"
 *                  mango will consider the connection closed.
 * @retval  < 0     Indicates connection error. In this case the function should return
 *                  even if the timeout has not been expired.
 */
int mangoPort_write(int socketfd, uint8_t* data, uint16_t datalen, uint32_t timeout){
    uint32_t sent;
    uint32_t start;
    int socketerror;
    int retval;
    
	MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("Trying to write %u bytes\r\n", datalen) );
	
    sent = 0;
    start = mangoPort_timeNow();
    while(sent < datalen){
        retval = write(socketfd, &data[sent], datalen - sent);
        if(retval < 0){
            //socketerrorlen = sizeof(socketerror);
            //retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#ifdef MANGO_IP_ENV__UNIX
            socketerror = errno;
#endif
            
#ifdef MANGO_IP_ENV__LWIP
            socklen_t socketerrorlen;
            socketerrorlen = sizeof(socketerror);
            retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#endif
            
            MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("!!!!!!! WRITE SOCKET ERROR %d\r\n", socketerror) );
            
            if(socketerror == EWOULDBLOCK || socketerror == EAGAIN){
                mangoPort_sleep(64);
            }else{
                return -1;
            }
        }else{
            sent += retval;
        }
        
        if(mangoHelper_elapsedTime(start) > timeout){
            return sent;
        }
    };
    
    return sent;
}

/**
 * @brief   Close the connection with the specific socket ID
 */
void mangoPort_disconnect(int socketfd){
	close(socketfd);
}

/**
 * @brief   Connect to the specified IP address and port. Wait until at least 
 *          for at most "timeout" [miliseconds] until the connection is established,
 *          else abort.
 *
 * @retval  >= 0    The connection was succesfull and the return value indicates the 
 *                  socket ID.
 * @retval  < 0     Connection failed.
 */
int mangoPort_connect(char* serverIP, uint16_t serverPort, uint32_t timeout){
    int retval;
    struct sockaddr_in s_addr_in;
    int socketfd;
	struct hostent *h;
    
	h = gethostbyname(serverIP);
	if (h == NULL)
	{
		return -1;
	}

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&s_addr_in, 0, sizeof(s_addr_in));
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_port   = htons (serverPort);
    s_addr_in.sin_addr   = *(struct in_addr*)(h->h_addr_list[0]);//inet_addr(serverIP);

    retval = connect(socketfd, (struct sockaddr *) &s_addr_in, sizeof(s_addr_in));

    if(retval == 0){
		// O_NONBLOCK socket cause ssl handshake failed.
        // retval = fcntl(socketfd, F_SETFL, O_NONBLOCK);
        // if(retval == 0){   
        // }
		return socketfd;
    }else{
        close(socketfd);
        return -1;
    }
}

/**
 * ssl
 */
int mangoPort_sslread(mangoHttpClient_t *hc, uint8_t* data, uint16_t datalen, uint32_t timeout){
    uint32_t received;
    uint32_t start;
    int socketerror;
    int retval;
	
    MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("Trying to read %u bytes\r\n", datalen) );
    
    received = 0;
    start = mangoPort_timeNow();
	while(received < datalen){
        retval = SSL_read(hc->ssl, &data[received], datalen - received);
        //printf("READ %d\r\n", retval);
        if(retval < 0){
            //socketerrorlen = sizeof(socketerror);
            //retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#ifdef MANGO_IP_ENV__UNIX
            socketerror = errno;
#endif
            
#ifdef MANGO_IP_ENV__LWIP
            socklen_t socketerrorlen;
            socketerrorlen = sizeof(socketerror);
            retval = getsockopt(hc->socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#endif
            
            MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("!!!!!!! READ SOCKET ERROR %d\r\n", socketerror) );
            
            if(socketerror == EWOULDBLOCK || socketerror == EAGAIN){
                mangoPort_sleep(64);
            }else{
                return -1;
            }
        }else{
            received += retval;
            return received;
        }
        
        if(mangoHelper_elapsedTime(start) > timeout){
            return received;
        }
	}
	
	MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("%u bytes read\r\n", retval) );
	
	return retval;
}

int mangoPort_sslwrite(mangoHttpClient_t *hc, uint8_t* data, uint16_t datalen, uint32_t timeout){
    uint32_t sent;
    uint32_t start;
    int socketerror;
    int retval;
    
	MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("Trying to write %u bytes\r\n", datalen) );
	
    sent = 0;
    start = mangoPort_timeNow();
    while(sent < datalen){
        retval = SSL_write(hc->ssl, &data[sent], datalen - sent);
        if(retval < 0){
            //socketerrorlen = sizeof(socketerror);
            //retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#ifdef MANGO_IP_ENV__UNIX
            socketerror = errno;
#endif
            
#ifdef MANGO_IP_ENV__LWIP
            socklen_t socketerrorlen;
            socketerrorlen = sizeof(socketerror);
            retval = getsockopt(hc->socketfd, SOL_SOCKET, SO_ERROR, &socketerror, (socklen_t *) &socketerrorlen);
#endif
            
            MANGO_DBG(MANGO_DBG_LEVEL_PORT, ("!!!!!!! WRITE SOCKET ERROR %d\r\n", socketerror) );
            
            if(socketerror == EWOULDBLOCK || socketerror == EAGAIN){
                mangoPort_sleep(64);
            }else{
                return -1;
            }
        }else{
            sent += retval;
        }
        
        if(mangoHelper_elapsedTime(start) > timeout){
            return sent;
        }
    };
    
    return sent;
}

void mangoPort_ssldisconnect(mangoHttpClient_t *hc)
{
	shutdown(hc->socketfd, SHUT_RDWR);
    close(hc->socketfd);
	SSL_free(hc->ssl);
    SSL_CTX_free(hc->ctx);
}

/**
 * cert file load
 */
ssl_ca_crt_key_t *sslcert_load(char *cacrt, char *cert, char *key)
{
	ssl_ca_crt_key_t *file;
	struct stat s;
	int fd;
	int n, br;

	file = (ssl_ca_crt_key_t*)malloc(sizeof(ssl_ca_crt_key_t));
	if (file == NULL)
		return NULL;
	memset(file, 0, sizeof(ssl_ca_crt_key_t));

	if (cacrt)
	{
		fd = open(cacrt, O_RDONLY);
		if (fd < 0)
			goto failed;
		fstat(fd, &s);
		file->cacrt_len = s.st_size;
		file->cacrt = (uint8_t*)malloc(s.st_size+8);
		if (file->cacrt == NULL)
			goto failed1;
		n = 0;
		while ((br = read (fd, (void*)(file->cacrt+n), 1024)) > 0)
		{
			n += br;
		}
		close(fd);
	}

	if (cert)
	{
		fd = open(cert, O_RDONLY);
		if (fd < 0)
			goto failed2;
		fstat(fd, &s);
		file->cert_len = s.st_size;
		file->cert = (uint8_t*)malloc(s.st_size+8);
		if (file->cert == NULL)
			goto failed2;
		n = 0;
		while ((br = read (fd, file->cert+n, 1024)) > 0)
		{
			n += br;
		}
		close(fd);
	}

	if (key)
	{
		fd = open(key, O_RDONLY);
		if (fd < 0)
			goto failed3;
		fstat(fd, &s);
		file->key_len = s.st_size;
		file->key = (uint8_t*)malloc(s.st_size+8);
		if (file->key == NULL)
			goto failed3;
		n = 0;
		while ((br = read (fd, file->key+n, 1024)) > 0)
		{
			n += br;
		}
		close(fd);
	}

	return file;

failed3:
	if (file->cert)
		free(file->cert);
failed2:
	if (file->cacrt)
		free((void*)(file->cacrt));
failed1:
	close(fd);
failed:
	if (file)
		free(file);
	return NULL;
}

void sslcert_free(ssl_ca_crt_key_t *s)
{
	if (s == NULL)
	{
		return ;
	}
	if (s->cacrt)
	{
		free((void*)(s->cacrt));
		s->cacrt = 0;
	}
	if (s->cert)
	{
		free(s->cert);
		s->cert = 0;
	}
	if (s->key)
	{
		free(s->key);
		s->key = 0;
	}
	if (s)
	{
		free(s);
		s = 0;
	}
}
