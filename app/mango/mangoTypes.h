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

#ifndef __MANGO_TYPES_H__
#define __MANGO_TYPES_H__

#include <stdint.h>         /* uint32_t, ... */
#include <stddef.h>
#include <openssl/ssl.h>

#include "mangoConfig.h"


typedef enum{
	MANGO_HTTP_AUTH__BASIC,
}mangoHttpAuth_t;

typedef enum{
	MANGO_WS_FRAME_TYPE_CONT		 	= 0x00,
	MANGO_WS_FRAME_TYPE_TEXT 			= 0x01,
	MANGO_WS_FRAME_TYPE_BINARY 			= 0x02,
	MANGO_WS_FRAME_TYPE_CLOSE 			= 0x08,
	MANGO_WS_FRAME_TYPE_PING 			= 0x09,
	MANGO_WS_FRAME_TYPE_PONG 			= 0x0A,
}mangoWsFrameType_t;


typedef enum{
    MANGO_OK = 0,
    MANGO_ERR = 1,
    
    /* 
    * Specific error codes 
    */
    
    MANGO_ERR_CONNECTION,               /* The connection was closed */
    MANGO_ERR_ABORTED,                  /* The operation was aborted due to parsing error */
	MANGO_ERR_APICALLNOTSUPPORTED,      /* The current status of the connection does not permit the specific API call */
	MANGO_ERR_INVALIDREQHEADERS,		/* Application provided conflicting/invalid HTTP Request headers */
	MANGO_ERR_RESPTIMEOUT,              /* HTTP Response timeout reached [MANGO_HTTP_RESPONSE_TIMEOUT_MS] */
    MANGO_ERR_RESPFORMAT,               /* The HTTP response format is not supported */
    MANGO_ERR_DATAPROCESSING,           /* An error occured during the processing of an input/output data stream */
    MANGO_ERR_WORKBUFSMALL,             /* The working buffer was small and the operation could not be completed [MANGO_WORKING_BUFFER_SZ] */
	MANGO_ERR_CONTENTLENGTH,            /* User provided more or less data during a POST/PUT request (relative to the specified Content-Length header) */
	MANGO_ERR_WRITETIMEOUT,             /* The maximum timeout for a write operation reached, connection is considered closed [MANGO_SOCKET_WRITE_TIMEOUT_MS] */
    MANGO_ERR_TEMPBUFSMALL,             /* The temporary buffer used was not enough for a specific operation */
    MANGO_ERR_APPABORTED,               /* Application aborted the connection [for esxample by returning MANGO_ERR from the app callback] */
    MANGO_ERR_MOREDATANEEDED,           /* Data processor needs more data to continue */
    MANGO_ERR_WEBSOCKETCLOSED,          /* For websockets, it indicates that the remote peer sent a close packet and the connection is considered closed */
	
	/* 
    * HTTP status codes 
    */
    
	MANGO_ERR_HTTP_100 = 100,	        /* Expectation was succesfull, continue with the data */
	MANGO_ERR_HTTP_101 = 101,	        /* Switching Protocols */

    
    MANGO_ERR_HTTP_200 = 200,           /* OK */
    MANGO_ERR_HTTP_201 = 201,           /* Created */
    MANGO_ERR_HTTP_202 = 202,           /* Accepted */
    MANGO_ERR_HTTP_203 = 203,           /* Non-Authoritative Information */
	MANGO_ERR_HTTP_204 = 204,           /* No Content */
    MANGO_ERR_HTTP_205 = 205,           /* Reset Content */
    MANGO_ERR_HTTP_206 = 206,           /* Partial content */

	MANGO_ERR_HTTP_300 = 300,           /* Multiple Choices */
    MANGO_ERR_HTTP_301 = 301,           /* Moved Permanently */
    MANGO_ERR_HTTP_302 = 302,           /* Found */
    MANGO_ERR_HTTP_303 = 303,           /* See Other  */
    MANGO_ERR_HTTP_304 = 304,           /* Not Modified */
    MANGO_ERR_HTTP_305 = 305,           /* Use Proxy */
    MANGO_ERR_HTTP_306 = 306,           /* Switch Proxy */
    MANGO_ERR_HTTP_307 = 307,           /* Temporary Redirect  */
    MANGO_ERR_HTTP_308 = 308,           /*  Permanent Redirect */
    
    MANGO_ERR_HTTP_400 = 400,           /* Bad Request */
    MANGO_ERR_HTTP_401 = 401,           /* Unauthorized */
    MANGO_ERR_HTTP_403 = 403,           /* Forbidden */
    MANGO_ERR_HTTP_404 = 404,           /* Not Found */
    MANGO_ERR_HTTP_405 = 405,           /* Method Not Allowed */
    MANGO_ERR_HTTP_406 = 406,           /* Not Acceptable */
    MANGO_ERR_HTTP_407 = 407,           /* Proxy Authentication Required */
    MANGO_ERR_HTTP_408 = 408,           /* Request Timeout */
    MANGO_ERR_HTTP_409 = 409,           /* Conflict */
    MANGO_ERR_HTTP_410 = 410,           /* Gone */
    MANGO_ERR_HTTP_411 = 411,           /* Length Required */
    MANGO_ERR_HTTP_412 = 412,           /* Precondition Failed */
    MANGO_ERR_HTTP_413 = 413,           /* Request Entity Too Large */
    MANGO_ERR_HTTP_414 = 414,           /* Request-URI Too Long */
    MANGO_ERR_HTTP_415 = 415,           /* Unsupported Media Type */
    MANGO_ERR_HTTP_416 = 416,           /* Requested Range Not Satisfiable */
    MANGO_ERR_HTTP_417 = 417,           /* Expectation Failed */
    MANGO_ERR_HTTP_426 = 426,           /* Upgrade Required */
    MANGO_ERR_HTTP_429 = 429,           /* Too Many Requests */
    
    MANGO_ERR_HTTP_500 = 500,           /* Internal Server Error */
    MANGO_ERR_HTTP_501 = 501,           /* Not Implemented */
    MANGO_ERR_HTTP_503 = 503,           /* Service Unavailable */
    MANGO_ERR_HTTP_505 = 505,           /* HTTP Version Not Supported */
    
	MANGO_ERR_HTTP_599 = 599,
}mangoErr_t;


typedef enum{
    MANGO_ARG_TYPE_HTTP_REQUEST_READY,
    MANGO_ARG_TYPE_HTTP_RESP_RECEIVED,
    MANGO_ARG_TYPE_HTTP_DATA_RECEIVED,
    
	MANGO_ARG_TYPE_WEBSOCKET_DATA_RECEIVED,
    MANGO_ARG_TYPE_WEBSOCKET_CLOSE,
    MANGO_ARG_TYPE_WEBSOCKET_PING,
}mangoArgType_e;


typedef enum{
	MANGO_HTTP_METHOD_HEAD,
	MANGO_HTTP_METHOD_GET,
	MANGO_HTTP_METHOD_POST,
	MANGO_HTTP_METHOD_POST_CHUNKED,
	MANGO_HTTP_METHOD_PUT,
}mangoHttpMethod_e;


typedef struct{
    mangoArgType_e argType;
    uint8_t* buf;
    uint16_t buflen;
	uint16_t statusCode; /* App may need this to abort an invalid response with big body for example */
    uint8_t frameID; /* For websockets only. Fragmented frames are given to the app with the same ID, so it can merge them back */
}mangoArg_t;

typedef enum{
    EVENT_NONE = 0,
	EVENT_ENTRY,
    EVENT_PROCESS,
	EVENT_READ,
	EVENT_WRITE,
    EVENT_TIMEOUT,
	EVENT_APICALL_httpRequestProcess,
	EVENT_APICALL_httpDataSend,
	EVENT_APICALL_wsPoll,
	EVENT_APICALL_wsFrameSend,
	EVENT_APICALL_wsClose,
}mangoEvent_e;

typedef struct{
    uint32_t txBytes;
    uint32_t rxBytes;
    uint32_t time;
}mangoStats_t;

typedef struct{
	uint8_t* buf;
	uint16_t buflen;
	mangoWsFrameType_t type;
}mangoWSFrameSendArgs_t;

typedef struct{
	uint8_t* buf;
	uint16_t buflen;
}mangoHTTPDataSendArgs_t;


typedef struct{
	uint32_t timeout;
}mangoWSPollArgs_t;

typedef struct{
    uint32_t fileSz;
    uint32_t fileSzProcessed;
}mangoIDPArgsRaw_t;

typedef struct{
	uint8_t state;
	uint32_t chunkSz;
	uint32_t chunkSzProcessed;
}mangoIDPArgsChunked_t;

typedef struct{
	uint8_t state;
    uint8_t header[2];
	uint32_t frameSz;
	uint32_t frameSzProcessed;
	uint8_t frameID;
}mangoIDPArgsWebsocket_t;

typedef struct{
    uint32_t fileSz;
    uint32_t fileSzProcessed;
}mangoODPArgsRaw_t;


typedef struct{
	uint8_t* workingBuffer;
	uint16_t workingBufferSz;
}mangoODPArgsChunked_t;

typedef struct mangoHttpClient_t mangoHttpClient_t;

struct mangoHttpClient_t{
    int                     socketfd;
	SSL_CTX					*ctx;
	SSL						*ssl;
	uint8_t					secure;
    mangoHttpMethod_e       httpMethod;

	uint16_t				httpResponseStatusCode;
    
    uint8_t                 workingBuffer[MANGO_WORKING_BUFFER_SZ];
    uint16_t                workingBufferIndexLeft;
    uint16_t                workingBufferIndexRight;
    
    mangoErr_t              smExitError;
    uint32_t                smTimeout;
	uint32_t				smEventTimeout;

	void*					smAPICallArgs;

	uint32_t				smEntryTimestamp;
    mangoEvent_e            subscribedEvent;
	void 					(*curState)(mangoEvent_e event, mangoHttpClient_t* hc);
    void 					(*nxtState)(mangoEvent_e event, mangoHttpClient_t* hc);
    
    /* User function */
    mangoErr_t              (*userFunc)(mangoArg_t* mangoArgs, void* userArgs);
    void*                   userArgs;
    
    /* Data processors */
    void*                   dataProcessorArgs;
	uint8_t					dataProcessorCompleted; /* We need this to keep track of the SM between different events */
    mangoErr_t              (*inputDataProcessor) (mangoHttpClient_t* hc, uint8_t* buf, uint16_t buflen, void* vargs, uint32_t* processed, uint8_t* completed); // inputDataProcessor
    mangoErr_t              (*outputDataProcessor)(mangoHttpClient_t* hc, uint8_t* buf, uint16_t buflen, void* vargs, uint32_t* processed, uint8_t* completed);

    /* Input Data processor arguments */
    mangoIDPArgsRaw_t       IDPArgsRaw;
    mangoIDPArgsChunked_t   IDPArgsChunked;
	mangoIDPArgsWebsocket_t IDPArgsWebsocket;
    
	/* Output Data processor arguments */
	mangoODPArgsRaw_t       ODPArgsRaw;
    mangoODPArgsChunked_t   ODPArgsChunked;
	
	/* Stats */
	mangoStats_t			stats;
};



#endif
