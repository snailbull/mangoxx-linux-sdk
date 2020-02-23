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

mangoHttpClient_t* mango_connect(char* serverIP, uint16_t serverPort){
    mangoHttpClient_t* hc;
    
    MANGO_ENSURE(serverIP, ("?") );
    
    hc = mangoPort_malloc(sizeof(mangoHttpClient_t));
    if(!hc){
        return NULL;
    }else{
        memset(hc, 0, sizeof(mangoHttpClient_t));
    }
    
    hc->socketfd = mangoPort_connect(serverIP, serverPort, MANGO_SOCKET_CONNECT_TIMEOUT_MS);
    if(hc->socketfd < 0){
        mangoPort_free(hc);
        return NULL;
    }
    
    mangoSM_INIT(hc);
    
    return hc;
}

mangoHttpClient_t *mango_sslconnect(char* serverIP, uint16_t serverPort, ssl_ca_crt_key_t *ssl_cck, ssl_ca_crt_key_file_t *ssl_cck_file, 
	const SSL_METHOD *method, int verify_mode, int frag_len)
{
    mangoHttpClient_t *hc;
	int ret=0;
    
    MANGO_ENSURE(serverIP, ("?") );
    
    hc = mangoPort_malloc(sizeof(mangoHttpClient_t));
    if(!hc){
        return NULL;
    }else{
        memset(hc, 0, sizeof(mangoHttpClient_t));
    }
	hc->secure = 1;
	// ssl ctx
	if ((hc->ctx = SSL_CTX_new(method)) == NULL)
		goto failed;
	if (ssl_cck)
	{
		if (ssl_cck->cacrt)
		{
			X509 *cacrt = d2i_X509(NULL, (const unsigned char **)&ssl_cck->cacrt, ssl_cck->cacrt_len);
			if (cacrt == NULL)
				goto failed1;
			SSL_CTX_add_client_CA(hc->ctx, cacrt);
		}
		if (ssl_cck->cert && ssl_cck->key)
		{
			if (0 == SSL_CTX_use_certificate_ASN1(hc->ctx, ssl_cck->cert_len, ssl_cck->cert))
				goto failed1;
			if (0 == SSL_CTX_use_PrivateKey_ASN1(0, hc->ctx, ssl_cck->key, ssl_cck->key_len))
				goto failed1;
		}
		if (ssl_cck->cacrt)
		{
			SSL_CTX_set_verify(hc->ctx, verify_mode, NULL);
		}
		else
		{
			SSL_CTX_set_verify(hc->ctx, SSL_VERIFY_NONE, NULL);
		}
	}
	else if (ssl_cck_file)
	{
		if (ssl_cck_file->cafile && ssl_cck_file->capath)
		{
			SSL_CTX_load_verify_locations(hc->ctx, ssl_cck_file->cafile, ssl_cck_file->capath);
		}
		if (ssl_cck_file->certfile)
		{
			SSL_CTX_use_certificate_file(hc->ctx, ssl_cck_file->certfile, SSL_FILETYPE_PEM);
		}
		if (ssl_cck_file->keyfile)
		{
			SSL_CTX_use_PrivateKey_file(hc->ctx, ssl_cck_file->keyfile, SSL_FILETYPE_PEM);
		}
	}
	else
	{
		SSL_CTX_set_verify(hc->ctx, verify_mode, NULL);
	}

	// socket
	hc->socketfd = mangoPort_connect(serverIP, serverPort, MANGO_SOCKET_CONNECT_TIMEOUT_MS);
	if(hc->socketfd < 0)
		goto failed2;
	
	// SSL connect
	if ((hc->ssl = SSL_new(hc->ctx)) == NULL)
		goto failed2;
	SSL_set_fd(hc->ssl, hc->socketfd);
	if (SSL_connect(hc->ssl) != 1)
		goto failed3;

    mangoSM_INIT(hc);
    return hc;

failed3:
	MANGO_DBG(MANGO_DBG_LEVEL_PORT,("failed3:SSL_free(hc->ssl)\n"));
	SSL_free(hc->ssl);
failed2:
	MANGO_DBG(MANGO_DBG_LEVEL_PORT,("failed2:close(hc->socketfd)\n"));
	close(hc->socketfd);
failed1:
	MANGO_DBG(MANGO_DBG_LEVEL_PORT,("failed1:SSL_CTX_free(hc->ctx)\n"));
	SSL_CTX_free(hc->ctx);
failed:
	MANGO_DBG(MANGO_DBG_LEVEL_PORT,("failed:mangoPort_free(hc)\n"));
	mangoPort_free(hc);
	return NULL;
}

mangoErr_t mango_httpRequestNew(mangoHttpClient_t* hc, char* URI, mangoHttpMethod_e method){
    char* token;
    uint16_t tokenlen;

    hc->httpMethod = method;
	
	hc->workingBufferIndexRight = 0;
	hc->workingBufferIndexLeft = 0;
	
    switch(hc->httpMethod){
        case MANGO_HTTP_METHOD_GET:
            token = "GET ";
            break;
        case MANGO_HTTP_METHOD_HEAD:
            token = "HEAD ";
            break;    
        case MANGO_HTTP_METHOD_POST:
            token = "POST ";
            break;
        case MANGO_HTTP_METHOD_PUT:
            token = "PUT ";
            break;     
        default:
            goto handleError;
            break;
    }
    
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc) ){
        goto handleError;
    }
    
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    token       = URI;
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
        goto handleError;
    }
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    token       = " HTTP/1.1\r\n\r\n";
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
        goto handleError;
    }
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    hc->workingBuffer[hc->workingBufferIndexRight] = '\0';
    
    return MANGO_OK;
    
    handleError:
        memset(hc->workingBuffer, 0, MANGO_WORKING_BUFFER_SZ);
        hc->workingBufferIndexLeft = 0;
        hc->workingBufferIndexRight = 0;
        return MANGO_ERR;
}


mangoErr_t mango_httpAuthSet(mangoHttpClient_t* hc, mangoHttpAuth_t auth, char* username, char* password){
	char*       token;
    uint16_t    prevRequestLen;
    uint16_t    tokenlen;
	char tmpBuf[32];
	int retval;
	
	MANGO_ENSURE(hc->workingBufferIndexRight > 2, ("?") );
    MANGO_ENSURE(hc, ("?") );
    
    hc->workingBufferIndexRight -= 2;
    
    prevRequestLen = hc->workingBufferIndexRight;
    
	if(auth == MANGO_HTTP_AUTH__BASIC){
		/*
		*	Server: WWW-Authenticate: Basic realm="nmrs_m7VKmomQ2YM3:"
		* 	Client: Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
		*/
		token       = MANGO_HDR__AUTHORIZATION;
		tokenlen    = strlen(token);
		if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
			goto handleError;
		}
		memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
		hc->workingBufferIndexRight += tokenlen;
		
		token       = ": Basic ";
		tokenlen    = strlen(token);
		if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
			goto handleError;
		}
		memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
		hc->workingBufferIndexRight += tokenlen;
		
		if(sizeof(tmpBuf) < strlen(username) + strlen(password) + 1 + 1){
			goto handleError;
		}
		
		strcpy(tmpBuf, "");
		strcpy(&tmpBuf[strlen(tmpBuf)], username);
		strcpy(&tmpBuf[strlen(tmpBuf)], ":");
		strcpy(&tmpBuf[strlen(tmpBuf)], password);
		
		retval = mangoCrypto_base64Encode(tmpBuf, strlen(tmpBuf), (char*) &hc->workingBuffer[hc->workingBufferIndexRight], MANGO_WB_FREE_SZ(hc));
		if(retval < 0){
			goto handleError;
		}
		hc->workingBufferIndexRight += retval;

		token       = "\r\n\r\n";
		tokenlen    = strlen(token);
		if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
			goto handleError;
		}
		memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
		hc->workingBufferIndexRight += tokenlen;
    }else{
		return MANGO_ERR;
	}
	
	
    return MANGO_OK;
    
handleError:
	hc->workingBufferIndexRight = prevRequestLen;
	hc->workingBuffer[hc->workingBufferIndexRight++] = '\r';
	hc->workingBuffer[hc->workingBufferIndexRight++] = '\n';
	
	return MANGO_ERR;
}


							  
							  
mangoErr_t mango_httpHeaderSet(mangoHttpClient_t* hc, char* headerName, char* headerValue){
    char*       token;
    uint16_t    prevRequestLen;
    uint16_t    tokenlen;
    
    MANGO_ENSURE(hc->workingBufferIndexRight > 2, ("?") );
    MANGO_ENSURE(hc, ("?") );
    MANGO_ENSURE(headerName, ("?") );
    
    hc->workingBufferIndexRight -= 2;
    
    prevRequestLen = hc->workingBufferIndexRight;
    
    token       = headerName;
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
        goto handleError;
    }
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    token       = ": ";
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
        goto handleError;
    }
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    token       = headerValue;
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
        goto handleError;
    }
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    token       = "\r\n\r\n";
    tokenlen    = strlen(token);
    if(hc->workingBufferIndexRight + tokenlen > MANGO_WB_TOT_SZ(hc)){
        goto handleError;
    }
    memcpy(&hc->workingBuffer[hc->workingBufferIndexRight], token, tokenlen);
    hc->workingBufferIndexRight += tokenlen;
    
    return MANGO_OK;
    
    handleError:
        hc->workingBufferIndexRight = prevRequestLen;
        hc->workingBuffer[hc->workingBufferIndexRight++] = '\r';
        hc->workingBuffer[hc->workingBufferIndexRight++] = '\n';
        
        return MANGO_ERR;
}

mangoErr_t mango_httpHeaderGet(char* response, char* headerName, char* headerValue, uint16_t headerValueLen){
    int retval;
    
    retval = mangoHelper_httpHeaderGet(response, headerName, headerValue, headerValueLen);
    if(retval < 0){
        /* Header not found */
        return MANGO_ERR; 
    }else if(retval == 0){
        /* Header found but headerValueLen was not big enough to copy the value */
        return MANGO_ERR_TEMPBUFSMALL;
    }else{
        /* Header found and value was copied to <headerValue> */
        return MANGO_OK;
    }
}


mangoErr_t mango_httpRequestProcess(mangoHttpClient_t* hc, mangoErr_t (*userFunc)(mangoArg_t* userFunc, void* userArgs), void* userArgs){
	mangoArg_t funcArgs;
	
    hc->userFunc = userFunc;
    hc->userArgs = userArgs;
	
	if(!hc->userFunc){
		return MANGO_ERR_APPABORTED;
	}
	
    MANGO_WB_NULLTERMINATE();
    
	funcArgs.buf = MANGO_WB_PTR(hc);
	funcArgs.buflen = MANGO_WB_USED_SZ(hc); //strlen((char*)funcArgs.buf);
	funcArgs.argType = MANGO_ARG_TYPE_HTTP_REQUEST_READY;
	hc->userFunc(&funcArgs, hc->userArgs);
	
	hc->stats.rxBytes = 0;
	hc->stats.txBytes = 0;
	hc->stats.time = mangoPort_timeNow();

	mangoErr_t err;
	
	hc->smAPICallArgs = NULL;
	
	err = mangoSM_PROCESS(hc, EVENT_APICALL_httpRequestProcess);
	return err;
}


mangoErr_t mango_httpDataSend(mangoHttpClient_t* hc, uint8_t* buf, uint32_t buflen){
	mangoHTTPDataSendArgs_t HTTPDataSendArgs;
	mangoErr_t err;
	
	HTTPDataSendArgs.buf = buf;
	HTTPDataSendArgs.buflen = buflen;

	hc->smAPICallArgs = &HTTPDataSendArgs;

	err = mangoSM_PROCESS(hc, EVENT_APICALL_httpDataSend);
	
	return err;
}





mangoErr_t mango_wsPoll(mangoHttpClient_t* hc, uint32_t timeout){
	mangoErr_t err;
	mangoWSPollArgs_t WSPollArgs;
	
	WSPollArgs.timeout = timeout;

	hc->smAPICallArgs = &WSPollArgs;

	err = mangoSM_PROCESS(hc, EVENT_APICALL_wsPoll);
	
	return err;
}


mangoErr_t mango_wsFrameSend(mangoHttpClient_t* hc, uint8_t* buf, uint32_t buflen, mangoWsFrameType_t type){
	mangoErr_t err;

	MANGO_ENSURE( (type == MANGO_WS_FRAME_TYPE_TEXT) || (type == MANGO_WS_FRAME_TYPE_BINARY), ("?") );
	
	mangoWSFrameSendArgs_t WSFrameSendArgs;
	
	WSFrameSendArgs.buf = buf;
	WSFrameSendArgs.buflen = buflen;
	WSFrameSendArgs.type = type;
	
	hc->smAPICallArgs = &WSFrameSendArgs;

	err = mangoSM_PROCESS(hc, EVENT_APICALL_wsFrameSend);

	return err;
}

mangoErr_t mango_wsClose(mangoHttpClient_t* hc){
	mangoErr_t err;

	hc->smAPICallArgs = NULL;

	err = mangoSM_PROCESS(hc, EVENT_APICALL_wsClose);

	return err;
}


void mango_disconnect(mangoHttpClient_t* hc){
	MANGO_ENSURE(hc, ("?") );
	
	mangoPort_disconnect(hc->socketfd);
	
	mangoPort_free(hc);
}
void mango_ssldisconnect(mangoHttpClient_t* hc){
	MANGO_ENSURE(hc, ("?") );
	
	mangoPort_ssldisconnect(hc);
	
	mangoPort_free(hc);
}
