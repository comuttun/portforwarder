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
 * $Id: TohCommon.c,v 1.7 2005/02/28 03:39:52 toh Exp $
 */

#include "PortForwarder_inc.h"

extern HANDLE g_hWnd;

/* Sleeps. Zzz... */
unsigned int sleep(unsigned int seconds)
{
    Sleep(seconds * 1000);
    return 0;
}

/* Gets error message from system. Seems not to work on WinCE?? */
char* GetErrMsg()
{
    DWORD rtn;
    LPVOID lpMsgBuf;
    static BOOL firstTime = TRUE;
    static char errMsg[4096];


    if (firstTime) {
        firstTime = FALSE;
        strcpy(errMsg, "Something happens, but what?");
    }

    rtn = WSAGetLastError();
    if (rtn == 0) {
        /* sometimes what we need is the error before last.
         * for example, in ssh_connect() in sshconnect.c,
         * the real error is overwritten by close().
         */
        return errMsg;
    }

    rtn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                        FORMAT_MESSAGE_FROM_SYSTEM | 
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL);

    if (rtn) {
        strcpy(errMsg, (LPTSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
    }
    else {
        strcpy(errMsg, TEXT("An error has been encountered."));
    }

    return errMsg;
}

#ifdef _PFPROXY_
BOOL isProxy = FALSE;    /* TRUE = using proxy command. FALSE = not */
HANDLE hProxy;           /* process handle of the proxy command */
HANDLE hWriteToProxy;    /* pipe handle: PF --> proxy's stdin */
HANDLE hReadFromProxy;   /* pipe handle: PF <-- proxy's stdout */
extern void fatal(const char *, ...);

/* pipedatalen returns actual length of data in the pipe from the proxy */
static DWORD pipedatalen(void)
{
    DWORD len = 0;
    if (!PeekNamedPipe(hReadFromProxy, NULL, 0, NULL, &len, NULL)) {
        if (GetLastError() == ERROR_BROKEN_PIPE)  {
            /* PIPE source is closed */
            /* ReadFile() will detects EOF */
            len = 1;
        }
        else {
            fatal("PeekNamedPipe() failed, GetLastError()=%lu\n", GetLastError());
        }
    }
    return len;
}
#endif /* _PFPROXY_ */

int write(SOCKET s, void *buf, int len)
{
#ifdef _PFPROXY_
    if (isProxy && s == PIPE_OUT_DUMMY_SOCKET) { /* request for writing to proxy? */
        DWORD n = 0;
        if (WriteFile(hWriteToProxy, buf, (DWORD)len, &n, NULL))
            return (int)n;
        else
            return -1;
        }
#endif /* _PFPROXY_ */
    return send(s, buf, len, 0);
}

int read(SOCKET s, void *buf, int len)
{
#ifdef _PFPROXY_
    if (isProxy && s == PIPE_IN_DUMMY_SOCKET) {  /* request for reading from proxy? */
        DWORD n = 0, l = (DWORD)len;
        if (l > 1) {                    /* if len == 1 then read one byte. */
            DWORD m = pipedatalen();    /* otherwise read actually pipe containing data */
            if (l > m) l = m;           /* for preventing ReadFile() will be blocked. */
            }
        if (ReadFile(hReadFromProxy, buf, l, &n, NULL))
            return (int)n;
        else
            return -1;
        }
#endif /* _PFPROXY_ */
    return recv(s, buf, len, 0);
}

int isatty(int handle)
{
    // always false!
    return 0;
}

#ifdef _PFPROXY_
/* myselect is pipe operation added select().                               */
/* if not using proxy command, then only call real select().                */
/* otherwise...                                                             */
/*   if rfds contains PIPE_IN_DUMMY_SOCKET then                             */
/*     repeat 10ms-wait select() and peeking at the pipe.                   */
/*     if select() returns positive and/or pipe contains data then return.  */
/*   if wfds contains PIPE_OUT_DUMMY_SOCKET then                            */
/*     call once select() and peek once at the pipe.                        */
/*     return positive because writing the pipe is always possible.         */
int myselect(int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds,
 const struct timeval *timeout)
{
    if (isProxy) {
        BOOL rflag = rfds && FD_ISSET(PIPE_IN_DUMMY_SOCKET, rfds);
        BOOL wflag = wfds && FD_ISSET(PIPE_OUT_DUMMY_SOCKET, wfds);

        if (rflag || wflag) {
            struct timeval tmo;
            int nloop = timeout ? timeout->tv_sec * 100 : -1;
            int r = 0, w = 0;
            if (rflag) FD_CLR(PIPE_IN_DUMMY_SOCKET, rfds);
            if (wflag) FD_CLR(PIPE_OUT_DUMMY_SOCKET, wfds);
            for(;;) {
                fd_set rs, *prs = rfds;
                int ret;
                tmo.tv_sec = 0;
                tmo.tv_usec = 10*1000;    /* 10 ms */
                if (rfds) {
                    rs = *rfds;
                    prs = &rs;
                    }
                ret = (select)(nfds, prs, wfds, efds, &tmo);
                if (nloop > 0) nloop--;
                if (rfds && (nloop == 0 || ret > 0)) *rfds = rs;
                if (rflag && pipedatalen() > 0) {
                    *rfds = rs;
                    if (ret <= 0) FD_ZERO(rfds);
                    FD_SET(PIPE_IN_DUMMY_SOCKET, rfds);
                    r = 1;
                    }
                if (wflag) {
                    if (ret <= 0) {
                        FD_ZERO(wfds);
                        if (!r)
                            FD_ZERO(rfds);
                        }
                    FD_SET(PIPE_OUT_DUMMY_SOCKET, wfds);
                    w = 1;
                    }
                if (wflag || nloop == 0 || ret > 0 || r || w)
                    return ret >= 0 ? ret+r+w : r || w ? r+w : nloop == 0 ? 0 : ret;
                }
            }
        }
    return (select)(nfds, rfds, wfds, efds, timeout);
}
#endif /* _PFPROXY_ */
