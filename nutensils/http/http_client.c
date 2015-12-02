/*
 * Copyright (c) 2015 Michael Stuart.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the Appleseed project, <https://github.com/drmetal/app-l-seed>
 *
 * Author: Michael Stuart <spaceorbot@gmail.com>
 *
 */

/**
* @defgroup http HTTP Utilities
*
* @{
* @file http_client.c
*/

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/socket.h>
#include "http_client.h"
#include "sock_utils.h"
#include "cutensils.h"
#include "strutils.h"
#include "net.h"

static char* http_split_content(char* response);
static int http_receive_response(int fd, http_response_t* resp);
static int pack_header(http_request_t* request);
static void unpack_url(char* url, http_request_t* request);

const char* http_header_strings[] = HTTP_HEADER_DECODE;
const char* http_content_strings[] = HTTP_CONTENT_DECODE;

/**
 * sends a HTTP request.
 *
 * @param	request - pointer to a request data structure.
 * @param	response - pointer to a response data structure.
 * @retval  returns the response. if sending the request failed or no
 * 			response was read, returns NULL.
 *
\code

// example HTTP GET

// get some buffer memory
char buffer[512];

// configure the request
request.remote = "remote-hostname";
request.port = 80;
request.page = "/";
request.local = "local-hostname";
request.type = HTTP_GET;
request.content_type = HTTP_CONTENT_FIELD_PLAIN;
request.buffer = buffer;
request.content_length = 0;

// configure the response
memset(&response, 0, sizeof(http_response_t));
response.buffer = buffer;
response.size = sizeof(buffer);

if(http_request(&request, &response))
{
	printf("response: %d", response->status);
	printf("response: %s", response->message);
	printf("response: %s", response->body);
}

// example HTTP POST
char resp[512] = "{\"some-json-data\":\"some-value\"}";
char* body;
if(http_request(host, port, page, HTTP_POST, HTTP_CONTENT_JSON, resp, sizeof(resp)))
{
	printf("response: %s", resp);
    body = http_split_content(resp);
	printf("body: %s", body);
}

// example HTTP POST

// init post data buffer
char buffer[512] = "{\"some-json-data\":\"some-value\"}";

// configure the request
request.remote = "remote-hostname";
request.port = 80;
request.page = "/api/v1.0";
request.local = "local-hostname";
request.type = HTTP_POST;
request.content_type = HTTP_CONTENT_FIELD_JSON;
request.buffer = buffer;
request.content_length = strlen(buffer);

// configure the response
response.buffer = buffer;
response.size = sizeof(buffer);key_length

if(http_request(&request, &response))
{
	printf("response: %d", response->status);
	printf("response: %s", response->message);
	printf("response: %s", response->body);
}

\endcode
 *
 */
http_response_t* http_request(http_request_t* request, http_response_t* response)
{
    http_response_t* resp = NULL;
	logger_t log;
	int fd;
	int chunklen;
    int sent;
	char* header;
	int resplen = 0;
	char* status;
	char* end;

	log_init(&log, "http-request");

	header = malloc(HTTP_MAX_HEADER_LENGTH);

	if(header)
	{
        fd = sock_connect(request->remote, request->port, SOCK_STREAM, NULL);

        usleep(10000);

    	if(fd >= 0)
    	{
        	// make HTTP request
        	chunklen = snprintf(header, HTTP_MAX_HEADER_LENGTH - 1, HTTP_HEADER, request->type, request->page, request->local, request->content_length, http_content_strings[request->content_type]);
        	// send HTTP header
        	if(chunklen < (HTTP_MAX_HEADER_LENGTH - 1))
        	{
        	    sent = send(fd, header, chunklen, 0);

        	    if(sent == chunklen)
        	    {
                    // send body
                    if(request->content_length > 0)
                        send(fd, request->buffer, request->content_length, 0);

                	// receive the header and body
                	// TODO - receive header only, decode nice info like content length, then receive body...
                	while(resplen < response->size-1)
                	{
                		chunklen = recv(fd, response->buffer + resplen, response->size - 1 - resplen, 0);
                		if(chunklen <= 0)
                			break;
                		resplen += chunklen;
                	}

                    // null terminate response
                    response->buffer[resplen] = '\0';

                    response->status = 0;
                    response->body = NULL;
                    response->message = NULL;

                    // determine status code, message and body
                    status = strchr(response->buffer, ' ');
                    if(status)
                    {
                        status++;
                        end = strchr(status,  ' ');
                        *end = '\0';
                        response->status = atoi(status);

                        response->message = end + 1;
                        end = strchr(response->message, '\r');
                        *end = '\0';
                        end++;
                        end = http_split_content(end);
                        if(end < (response->buffer + resplen))
                            response->body = end;
                    }

                    resp = response;
        	    }
        	    else
        	        log_error(&log, "send header failed, %d/%dbytes", sent, chunklen);
        	}
        	else
        	    log_error(&log, "header too large, %d/%dbytes", chunklen, HTTP_MAX_HEADER_LENGTH);

    	    closesocket(fd);
        }
        else
            log_error(&log, "failed to connect, %d", fd);

        free(header);
	}
	else
	    log_error(&log, "mem alloc failed");

    return resp;
}

/**
 * split http content from the response buffer.
 */
char* http_split_content(char* response) {
	while(*response)
	{
		if(strncmp(HTTP_EOH, response, sizeof(HTTP_EOH)-1) == 0)
		{
			*response = '\0';
			return response + sizeof(HTTP_EOH)-1;
		}
		response++;
	}
	return NULL;
}

/**
 * only decodes fields if found - the response structure should be initialized with default values that make sense.
 */
int http_receive_response(int fd, http_response_t* response)
{
    // receive header and possibly part of body
    int resplen = 0;
    int length = 0;
    int field;
    char* value;
    char* end;

    // use the end of the buffer to save values...
    char* save = response->buffer + (response->size - 1);

    while(resplen < response->size-1)
    {
        length = recv(fd, response->buffer + resplen, 1, 0);

        if(length != 1)
            return -1;

        if(response->buffer[resplen] != '\n')
            resplen++;
        else
        {
            // EOH
            if(resplen == 1)
                return 0;

            // terminate line at '\r'
            response->buffer[resplen-1] = '\0';
            resplen = 0;

            value = strchr(response->buffer, HTTP_COLON_CHAR);

            if(value)
            {
                *value = '\0';
                value += 2;

                // check for known header field
                field = string_in_list(response->buffer, strlen(response->buffer), http_header_strings);

                switch(field)
                {
                    case HTTP_HEADER_FIELD_HOST:
                        save -= strlen(value);
                        strcpy(save, value);
                        response->host = save;
                        save--;
                    break;
                    case HTTP_HEADER_FIELD_SERVER:
                        save -= strlen(value);
                        strcpy(save, value);
                        response->server = save;
                        save--;
                    break;
                    case HTTP_HEADER_FIELD_CONTENT_LENGTH:
                        response->content_length = atoi(value);
                    break;
                    case HTTP_HEADER_FIELD_CONTENT_TYPE:
                        response->content_type = string_in_list(value, strlen(value), http_content_strings);
                    break;
                }
            }
            else
            {
                value = strchr(response->buffer, HTTP_SPACE_CHAR);
                value++;
                end = strchr(value, HTTP_SPACE_CHAR);
                if(end)
                {
                    *end = '\0';
                    end++;
                    response->status = atoi(value);

                    save -= strlen(end);
                    strcpy(save, (const char*)end);
                    response->message = save;
                    save--;
                }
            }
        }
    }

    return -1;
}

int pack_header(http_request_t* request)
{
    return snprintf(request->buffer, request->size - 1, HTTP_HEADER,
            request->type, request->page, request->local,
            request->content_length, http_content_strings[request->content_type]);
}

/**
 * get the remote, page and port fields out of a URL string and populate the request structure.
 *
 * http://abc.com:80/index.html
 * =>
 *  request.remote == "abc.com"
 *  request.page == "/index.html"
 *  request.port == 80
 *
 * abc.com:80/index.html
 * =>
 *  request.remote == "abc.com"
 *  request.page == "/index.html"
 *  request.port == 80
 *
 * abc.com/index.html
 * =>
 *  request.remote == "abc.com"
 *  request.page == "/index.html"
 *  request.port == not set
 *
 */
void unpack_url(char* url, http_request_t* request)
{
    char* portstr;
    char* pathstr;

    // url may not be null and must have some length
    if(url && *url)
    {
        // check for http://
        if(strncmp(HTTP_SCHEMA, url, sizeof(HTTP_SCHEMA)-1) == 0)
            url += sizeof(HTTP_SCHEMA)-1;

        // save remote host
        request->remote = url;

        // start points to the colon: the beginning of the port number if any
        portstr = strchr(url, HTTP_COLON_CHAR);
        // end points to the slash: the beginning of the path if any
        pathstr = strchr(url, HTTP_SLASH_CHAR);

        // have path
        if(pathstr)
        {
            *pathstr = '\0';
            pathstr++;
            if(*pathstr)
                request->page = pathstr;
        }

        // have a port number
        if(portstr)
        {
            *portstr = '\0';
            portstr++;
            // save port
            request->port = atoi(portstr);
        }
    }
}

/**
 * perform HTTP get, saving the response to a file.
 *
\code

http_response_t response;
char buffer[128];
char url[] = "http://abc.com:80/index.html";
char* output = "/tmp/index.html"
mkdir("/tmp");

if(http_get_file(url, &response, output, buffer, sizeof(buffer)))
{

}

\endcode
 *
 * @param   url - the full URL to get eg http://host:port/path/to/file.html
 * @param   response - a pointer to an http response object.
 * 			even if the request fails, the response mesage field may be populated.
 * @param   output - a pointer a filepath to save the url endpoint to, eg "./file.html"
 * @param   buffer - working area, used to store received header, response string
 *              fields will end up pointing to parts of this memory.
 * @param   size - the length of the buffer in bytes.
 */
http_response_t* http_get_file(char* url, http_response_t* response, const char* output, char* buffer, int size)
{
    http_response_t* resp = NULL;
    logger_t log;
    int fd;
    int outfd;
    int length;
    int transfer;

    log_init(&log, "http-get");

    http_request_t request = {
        .remote = NULL,
        .port = HTTP_DEFAULT_PORT,
        .page = HTTP_BASE_PAGE,
        .local = net_lip(),
        .type = HTTP_GET,
        .content_type = HTTP_CONTENT_FIELD_PLAIN,
        .buffer = buffer,
        .size = size,
        .content_length = 0
    };

    // init the response
    memset(response, 0, sizeof(http_response_t));
    response->buffer = buffer;
    response->size = size;
	response->message = response->buffer;

    outfd = open(output, O_WRONLY | O_TRUNC | O_CREAT);

    if(outfd == -1)
    {
    	snprintf(response->buffer, response->size, "error opening target file %s", output);
        log_error(&log, response->buffer);
        return NULL;
    }

    unpack_url(url, &request);

    log_info(&log, "performing get: %s:%d %s", request.remote, request.port, request.page);

    if(!request.remote)
    {
    	snprintf(response->buffer, response->size, "invalid host in url %s", url);
		log_error(&log, response->buffer);
        close(outfd);
        return NULL;
    }

    log_init(&log, "http_get_file");

    if(!request.buffer)
    {
    	snprintf(response->buffer, response->size, "mem alloc failed");
		log_error(&log, response->buffer);
        close(outfd);
        return NULL;
    }

    fd = sock_connect(request.remote, request.port, SOCK_STREAM, NULL);

    if(fd == -1)
    {
    	snprintf(response->buffer, response->size, "failed to connect to %s:%d", request.remote, request.port);
		log_error(&log, response->buffer);
        close(outfd);
        return NULL;
    }

    // make HTTP request
    length = pack_header(&request);

    // send HTTP header
    if(length >= request.size)
    {
    	snprintf(response->buffer, response->size, "header too large, %d/%dbytes", length, request.size);
		log_error(&log, response->buffer);

        closesocket(fd);
        close(outfd);
        return NULL;
    }
    transfer = send(fd, request.buffer, length, 0);
    if(transfer != length)
    {
    	snprintf(response->buffer, response->size, "send header failed, %d/%dbytes", transfer, length);
		log_error(&log, response->buffer);
        closesocket(fd);
        close(outfd);
        return NULL;
    }

    // receive response
    if(http_receive_response(fd, response) == 0)
    {
        log_debug(&log, HTTP_SERVER"%s", response->server);
        log_debug(&log, HTTP_HOST"%s", response->host);
        log_debug(&log, "status: %d", response->status);
        log_debug(&log, "message: %s", response->message);
        log_debug(&log, HTTP_CONTENT_LENGTH"%d", response->content_length);
        log_debug(&log, HTTP_CONTENT_TYPE"%s", http_content_strings[response->content_type]);

        // receive the body
        for(length = 0; length < response->content_length;)
        {
            transfer = recv(fd, buffer, size, 0);
            if(transfer <= 0)
                break;
            length += transfer;
            write(outfd, buffer, transfer);
        }

        resp = response;
    }

    closesocket(fd);
    close(outfd);

    return resp;
}


/**
 * @}
 */
