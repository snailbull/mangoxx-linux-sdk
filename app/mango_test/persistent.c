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

#define PRINTF              printf



/*
* This example performs a GET HTTP request followed by a HEAD HTTP request
* in an infinite while(1) loop to demonstrate HTTP peristent conenctions.
* 
* The target URL is the home page of stackoverflow (http://stackoverflow.com/)
*/
#define SERVER_IP           "198.252.206.140"
#define SERVER_HOSTNAME     "stackoverflow.com"
#define SERVER_PORT         80
#define RESOURCE_URL        "/post.php"


static mangoErr_t mangoApp_handler(mangoArg_t* mangoArgs, void* userArgs)
{
	mangoErr_t err;
    
	switch(mangoArgs->argType){
		case MANGO_ARG_TYPE_HTTP_REQUEST_READY:
        {
            /*
            * This is the HTTP request that mango is going to send to the Server.
            */
            PRINTF("\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("HTTP REQUEST:\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("%s", mangoArgs->buf);
			PRINTF("-----------------------------------------------------------------\r\n");
            break;
        }
        case MANGO_ARG_TYPE_HTTP_RESP_RECEIVED:
        {
            /*
            * HTTP response headers received.
            */
            PRINTF("\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("HTTP RESPONSE [status code %u]:\r\n", mangoArgs->statusCode);
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("%s", mangoArgs->buf);
			PRINTF("-----------------------------------------------------------------\r\n");
            
#if 0
            /*
            * If a header value is needed it can be extracted as follows:
            */
            char headerValue[64];
            err = mango_httpHeaderGet((char*) mangoArgs->buf, MANGO_HDR__CONTENT_LENGTH, headerValue, sizeof(headerValue));
            if(err == MANGO_OK){
                PRINTF("The value of the specified header is '%s'\r\n", headerValue);
            }else{
                PRINTF("Header not found or temporary buffer was small!\r\n");
            }
#endif
            
            break;
        }
        case MANGO_ARG_TYPE_HTTP_DATA_RECEIVED:
        {
            /*
            * Data were received from the Server (GET request)
            */
            PRINTF("\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("HTTP DATA RECEIVED: [%u bytes]\r\n", mangoArgs->buflen);
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("%s\r\n", mangoArgs->buf);
			PRINTF("-----------------------------------------------------------------\r\n");
			
            break;
        }
        case MANGO_ARG_TYPE_WEBSOCKET_DATA_RECEIVED:
        {
            /*
            * Data were received from the websocket
            */
            PRINTF("\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("WEBSOCKET DATA RECEIVED: [%u bytes, Frame ID %u]\r\n", mangoArgs->buflen, mangoArgs->frameID);
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("%s\r\n", mangoArgs->buf);
			PRINTF("-----------------------------------------------------------------\r\n");
			
			break;
        }
        case MANGO_ARG_TYPE_WEBSOCKET_CLOSE:
        {
			/*
            * Server requested to close the websocket, a closed frame was sent automatically
			* in response. Poll will return with an error.
            */
            PRINTF("\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("WEBSOCKET CLOSE FRAME RECEIVED!\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
            break;
        }
        case MANGO_ARG_TYPE_WEBSOCKET_PING:
        {
			/*
            * Server sent us a Ping frame and a pong frame was sent automatically.
            */
            PRINTF("\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
			PRINTF("WEBSOCKET PING FRAME RECEIVED!\r\n");
			PRINTF("-----------------------------------------------------------------\r\n");
            break;
        }
	};
	
    return MANGO_OK;
};

static mangoErr_t httpHead(mangoHttpClient_t* httpClient)
{
    mangoErr_t err;

    /*
    * http://httpbin.org/ allows to test GET requests which return CHUNKED data
	* in reponse..
    */
    err = mango_httpRequestNew(httpClient, RESOURCE_URL,  MANGO_HTTP_METHOD_HEAD);
    if(err != MANGO_OK){ return MANGO_ERR; }
    
    /*
    * The "host: xxxx" header is required by almost all servers so we need
    * to add it.
    */
    err = mango_httpHeaderSet(httpClient, MANGO_HDR__HOST, SERVER_HOSTNAME);
    if(err != MANGO_OK){ return MANGO_ERR; }
    
    /*
    * Add this if it is desirable to continue with other HTTP requests after
    * the current one without closing the existing connection.
    */
    err = mango_httpHeaderSet(httpClient, MANGO_HDR__CONNECTION, "keep-alive");
    if(err != MANGO_OK){ return MANGO_ERR; }
    
    /*
    * Send the HTTP request and receive the response
    */
    err = mango_httpRequestProcess(httpClient, mangoApp_handler, NULL);
    if(err >= MANGO_ERR_HTTP_100 && err <= MANGO_ERR_HTTP_599){
		/*
		* A valid HTTP response was received.
		*/
        return err;
    }else{
		/*
		* Fatal request error
        */
		return MANGO_ERR;
    }
    
    
    return MANGO_ERR;
}

static mangoErr_t httpGet(mangoHttpClient_t* httpClient)
{
    mangoErr_t err;

    /*
    * http://httpbin.org/ allows to test GET requests which return CHUNKED data
	* in reponse..
    */
    err = mango_httpRequestNew(httpClient, RESOURCE_URL,  MANGO_HTTP_METHOD_GET);
    if(err != MANGO_OK){ return MANGO_ERR; }
    
    /*
    * The "host: xxxx" header is required by almost all servers so we need
    * to add it.
    */
    err = mango_httpHeaderSet(httpClient, MANGO_HDR__HOST, SERVER_HOSTNAME);
    if(err != MANGO_OK){ return MANGO_ERR; }
    
    /*
    * Add this if it is desirable to continue with other HTTP requests after
    * the current one without closing the existing connection.
    */
    err = mango_httpHeaderSet(httpClient, MANGO_HDR__CONNECTION, "keep-alive");
    if(err != MANGO_OK){ return MANGO_ERR; }
    
    /*
    * Send the HTTP request and receive the response
    */
    err = mango_httpRequestProcess(httpClient, mangoApp_handler, NULL);
    if(err >= MANGO_ERR_HTTP_100 && err <= MANGO_ERR_HTTP_599){
		/*
		* A valid HTTP response was received.
		*/
        return err;
    }else{
		/*
		* Fatal request error
        */
		return MANGO_ERR;
    }
    
    
    return MANGO_ERR;
}



int persistent_test(void)
{
    mangoHttpClient_t* httpClient;
    mangoErr_t err;
    
    /*
    * Connect to server
    */
    httpClient = mango_connect(SERVER_IP, SERVER_PORT);
    if(!httpClient){
        PRINTF("mangoHttpClient_connect() FAILED!");
        return MANGO_ERR;
    }
   
    /*
    * Enter an infinite loop making a GET followed by a HEAD request
    * using the same HTTP connection (HTTP persistent connection)
    */
    while(1){
        
        /*
        * Do the HTTP GET 
        */
        err = httpGet(httpClient);
        if(err >= MANGO_ERR_HTTP_100 && err <= MANGO_ERR_HTTP_599){ 
            /*
            * HTTP request completed succesfully, go on
            */
            PRINTF("HTTP response code was %d\r\n", err);
        }else {
            /*
            * Fatal request error, should disconnect
            */
            PRINTF("HTTP request failed with error %d\r\n", err);
            break;
        }
        
        /*
        * Don't set this too high because server may close the connection
        * due to inactivity
        */
        mangoPort_sleep(800);
        
        
        /*
        * Do the HTTP HEAD 
        */
        err = httpHead(httpClient);
        if(err >= MANGO_ERR_HTTP_100 && err <= MANGO_ERR_HTTP_599){ 
            /*
            * HTTP request completed succesfully, go on
            */
            PRINTF("HTTP response code was %d\r\n", err);
        }else {
            /*
            * Fatal request error, should disconnect
            */
            PRINTF("HTTP request failed with error %d\r\n", err);
            break;
        }
        
        
        /*
        * Don't set this too high because server may close the connection
        * due to inactivity
        */
        mangoPort_sleep(800);
    }
    
    /*
    * Disconnect from server
    */
    mango_disconnect(httpClient);
    
    return 0;
}
