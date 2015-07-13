/*
 * Copyright (C) 2004 YAMAKURA Makoto (yakina@spnet.ne.jp)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: auth-pageant.c,v 1.3 2006/11/05 14:23:27 ozawa Exp $
 */

#include <errno.h>
#include <process.h>
#include "includes.h"
#include "log.h"
#include "buffer.h"
#include "key.h"
#include "misc.h"
#include "authfd.h"
#include "auth-pageant.h"

#define AGENT_COPYDATA_ID 0x804e50ba   /* random goop */
#define AGENT_MAX_MSGLEN  8192

static int pageant_query(void *sock);
static int pageant_socket;
static int sock_port = -1;

/*
 * return Window Handle of the pageant
 */
HWND
pageant_get_handle(void)
{
	HWND hwnd;
	hwnd = FindWindow("Pageant", "Pageant");
	return hwnd;
}

/*
 * port number for authentication
 */
int
pageant_get_port(void)
{
	return sock_port;
}

/*
 * open a port and listen authentication requests
 * and start proxy auth server
 */
int
pageant_create_auth_socket(void)
{
	int s;
	struct sockaddr_in	addr;
	int	addrlen;

	debug("open a socket for the agent");
	/*
	 * Prepate a socket to authentication
	 */
	if((s = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)
		return 0;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addrlen = sizeof(addr);
	if(bind(s,(struct sockaddr*)&addr,addrlen) == SOCKET_ERROR) {
		close(s);
		return 0;
	}
	addrlen = sizeof(struct sockaddr_in);
	if (getsockname(s, (struct sockaddr *)&addr, &addrlen) == SOCKET_ERROR) {
		close(s);
		return 0;
	}
	sock_port = ntohs(addr.sin_port);
	/*
	 * Listen the socket
	 */
	if(listen(s,2) == SOCKET_ERROR) {
		close(s);
		return 0;
	}
	pageant_socket = s;

	debug("auth activate");
	if (!pageant_get_handle())
		return 0;
	if (sock_port>=0)
		_beginthread(pageant_auth_thread, 8192, NULL);
	return 1;
}

/*
 * Proxy authentication server
 * Start threads which communicate with the pageant
 */
void
pageant_auth_thread(void *dummy)
{
	int s;
	struct sockaddr_in addr;
	int	addrlen;

	debug("entering the thread");
    addrlen = sizeof(addr);
	while (1) {
		if((s = accept(pageant_socket,(struct sockaddr*)&addr,&addrlen)) < 0) {
			debug("accept: %s", strerror(errno));
			return;
		}
		debug("call: socket %d", s);
		_beginthread(pageant_query, 8192, (void *)s);
	}
	sock_port = -1;
}

/*
 * forward socket stream to the agent and vice versa
 */
static int
pageant_query(void *sock)
{
	HWND hwnd;
	char mapname[64];
	HANDLE filemap;
	unsigned char *p;
	int l, id, len;
	COPYDATASTRUCT cds;

	char buf[2048];
	Buffer request, reply;
	int s = (int)sock;

	debug("query: socket %d", s);
	
	hwnd = pageant_get_handle();
	if (!hwnd)
		return 0;

	while (1) {
		buffer_init(&request);
		buffer_init(&reply);
	
		/*
		 * Wait for response from the agent.  First read the length of the
		 * response packet.
		 */
		len = 4;
		while (len > 0) {
			l = read(s, buf + 4 - len, len);
			if (l == -1 && (errno == EAGAIN || errno == EINTR))
				continue;
			if (l <= 0) {
				//error("Error reading response length from authentication socket.a");
				break;
			}
			len -= l;
		}
		if (len > 0)
			break;
	
		/* Extract the length, and check it for sanity. */
		len = get_u32(buf);
		if (len > 256 * 1024)
			fatal("Authentication response too long: %d", len);
		debug("query: socket %d, len %d", s, len);
	
		/* Read the rest of the response in to the buffer. */
		buffer_clear(&request);
		while (len > 0) {
			l = len;
			if (l > sizeof(buf))
				l = sizeof(buf);
			l = read(s, buf, l);
			if (l == -1 && (errno == EAGAIN || errno == EINTR))
				continue;
			if (l <= 0) {
				//error("Error reading response from authentication socket.b");
				break;
			}
			buffer_append(&request, buf, l);
			len -= l;
		}
		if (len > 0)
			break;
		debug("query: socket %d, data %d", s, buffer_len(&request));
	
		sprintf(mapname, "PageantRequest%08x", (unsigned)GetCurrentThreadId());
		filemap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		                            0, AGENT_MAX_MSGLEN, mapname);
		if (!filemap) {
			error("Error creating file mapping.");
			break;
		}
		p = MapViewOfFile(filemap, FILE_MAP_WRITE, 0, 0, 0);
		len = buffer_len(&request);
		put_u32(p, len);
		memcpy(p+4, buffer_ptr(&request), len);
		cds.dwData = AGENT_COPYDATA_ID;
		cds.cbData = 1 + strlen(mapname);
		cds.lpData = mapname;
		id = SendMessage(hwnd, WM_COPYDATA, (WPARAM) NULL, (LPARAM) & cds);
		if (id > 0) {
			/* Extract the length, and check it for sanity. */
			len = get_u32(p);
			if (len > 256 * 1024)
				fatal("Authentication response too long: %d", len);
		debug("query: socket %d, pageant query", s);
			buffer_clear(&reply);
			buffer_append(&reply, p+4, len);
		}
		UnmapViewOfFile(p);
		CloseHandle(filemap);
		if (id <= 0) {
			error("Error reading response from pageant.");
			break;
		}
	
		len = buffer_len(&reply);
		put_u32(buf, len);
		write(s, buf, 4);
		write(s, buffer_ptr(&reply), len);
	}

	buffer_free(&request);
	buffer_free(&reply);
	debug("query: socket %d, data %d, done", s, len);
	return 1;
}
