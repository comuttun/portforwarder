/*
 * Copyright (C) 2003 Fujio Nobori (toh@fuji-climb.org)
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
 * $Id: PortForwarder_inc.h,v 1.6 2006/11/05 14:23:25 ozawa Exp $
 */

#ifndef PORTFORWARDER_INC_H
#define PORTFORWARDER_INC_H

#define FD_SETSIZE 128  /* default is 64 in winsock2.h. is this value safe enough? */
#include <winsock2.h>

#define APP_NAME "PortForwarder"
#define PF_STAT_NOT_CONNECTED   0
#define PF_STAT_CONNECTING      1
#define PF_STAT_CONNECTED       2
#define PF_STAT_DISCONNECTING   3

#define PF_STRING_NOT_CONNECTED TEXT("Not connected")
#define PF_STRING_CONNECTING TEXT("Connecting...")
#define PF_STRING_CONNECTED TEXT("Connected")
#define PF_STRING_DISCONNECTING TEXT("Disconnecting...")

#define MSG_THREAD_END          WM_APP + 1
#define MSG_TRAY_CLICKED        WM_APP + 2
#define MSG_FATAL_CALLED        WM_APP + 3
#define MSG_CONNECTED           WM_APP + 4
#define MSG_SHOW_MESSAGEBOX     WM_APP + 6
#define MSG_GET_PASSPHRASE      WM_APP + 7

#define MAX_LINE        1024
#define MAX_HOST_NUM    10

#define IN_LOOPBACKNET 127

#define EINPROGRESS WSAEWOULDBLOCK

#define strcasecmp(s1, s2) _stricmp(s1, s2)
#define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#define strerror(errno) GetErrMsg()

#ifndef _INC_IO
/* io.h has write() and read(), but it seems those doesn't
 * work for sockets.
 */
#define close(s) closesocket(s)
int write(SOCKET s, void *buf, int len);
int read(SOCKET s, void *buf, int len);
int isatty(int handle);

#ifdef _PFPROXY_
extern BOOL isProxy;            /* TRUE = using proxy command. FALSE = not */
extern HANDLE hProxy;           /* process handle of the proxy command */
extern HANDLE hWriteToProxy;    /* pipe handle: PF --> proxy's stdin */
extern HANDLE hReadFromProxy;   /* pipe handle: PF <-- proxy's stdout */
#define PIPE_IN_DUMMY_SOCKET    ((SOCKET)(-2))  /* dummy for variable 'connection_in' */
#define PIPE_OUT_DUMMY_SOCKET   ((SOCKET)(-3))  /* dummy for variable 'connection_out' */
int myselect(int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds,
 const struct timeval *timeout);    /* pipe operation added select() */
#define select(n, r, w, e, t)   (myselect(n, r, w, e, t))
#endif /* _PFPROXY_ */
#endif /* _INC_IO */

char* GetErrMsg();
void ThreadExit();
unsigned int sleep(unsigned int seconds);

/* Struct for MessageBox info. */
typedef struct msgboxinfo {
    char* caption;
    const char* message;
    unsigned int type;
    unsigned int result;
} msgboxinfo;

/* Struct for AskPassphraseDialog info. */
typedef struct askpassinfo {
    const char* prompt;
    char* result;
} askpassinfo;

/* dummy __attribute__ to compile by non GCC */
#ifndef __attribute__
#define __attribute__(x)
#endif /* __attribute__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
int main();
int GetStatus();
void SetStatus(int stat);
extern char pf_option[64];
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* PORTFORWARDER_INC_H */
