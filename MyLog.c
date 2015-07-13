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
 * $Id: MyLog.c,v 1.2 2003/12/09 14:11:37 toh Exp $
 */

#include <windows.h>

#ifdef _LOG_
static HANDLE logFile;
static BOOL logFlag = FALSE;

/* Opens log file. */
void OpenLogFile()
{
    char filename[] = TEXT("\\Temp\\PortForwarder.log");
    char startMessage[] = "#### PortForwarder started\r\n";
    char dateMessage[]  = "####   DATE ";
    char timeMessage[]  = "####   TIME ";
    char string[256];
    DWORD sizeWritten;

    if (logFlag)
        return;
    logFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (logFile == INVALID_HANDLE_VALUE) {
        logFlag = FALSE;
        return;
    }

    SetFilePointer(logFile, 0, NULL, FILE_END);

    WriteFile(logFile, startMessage, strlen(startMessage), &sizeWritten, NULL);
    WriteFile(logFile, dateMessage, strlen(dateMessage), &sizeWritten, NULL);
    GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, NULL, string, 256);
    WriteFile(logFile, string, strlen(string), &sizeWritten, NULL);
    WriteFile(logFile, "\r\n", strlen("\r\n"), &sizeWritten, NULL);
    WriteFile(logFile, timeMessage, strlen(timeMessage), &sizeWritten, NULL);
    GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, NULL, string, 256);
    WriteFile(logFile, string, strlen(string), &sizeWritten, NULL);
    WriteFile(logFile, "\r\n", strlen("\r\n"), &sizeWritten, NULL);

    logFlag = TRUE;
}

/* Closes log file. */
void CloseLogFile()
{
    char dateMessage[]  = "####   DATE ";
    char timeMessage[]  = "####   TIME ";
    char string[256];
    DWORD sizeWritten;

    if (!logFlag)
        return;

    WriteFile(logFile, dateMessage, strlen(dateMessage), &sizeWritten, NULL);
    GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, NULL, string, 256);
    WriteFile(logFile, string, strlen(string), &sizeWritten, NULL);
    WriteFile(logFile, "\r\n", strlen("\r\n"), &sizeWritten, NULL);
    WriteFile(logFile, timeMessage, strlen(timeMessage), &sizeWritten, NULL);
    GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, NULL, string, 256);
    WriteFile(logFile, string, strlen(string), &sizeWritten, NULL);
    WriteFile(logFile, "\r\n", strlen("\r\n"), &sizeWritten, NULL);
    WriteFile(logFile, "\r\n\r\n", strlen("\r\n\r\n"), &sizeWritten, NULL);
    CloseHandle(logFile);
    logFlag = FALSE;
}

/* Logs message. */
void MyLog(LPCTSTR message)
{
    DWORD sizeWritten;

    if (!logFlag)
        return;

    WriteFile(logFile, message, strlen(message), &sizeWritten, NULL);
    WriteFile(logFile, "\r\n", strlen("\r\n"), &sizeWritten, NULL);
    FlushFileBuffers(logFile);
}
#endif /* _LOG_ */
