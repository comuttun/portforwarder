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
 * $Id: LoadSaveInfo.cpp,v 1.6 2007/09/20 15:06:06 toh Exp $
 */

#include <stdafx.h>
#include "PortForwarder_inc.h"
#include "IniFile.h"
#include <direct.h>

#define SPACES TEXT(" \t\r\n")

static TCHAR iniFileName[MAX_PATH];

extern TCHAR g_hosts[][MAX_PATH];
extern "C" {
    extern char g_configFolder[];
}
extern TCHAR g_configFile[];
extern BOOL g_automaticallyHide;

extern "C" {
    extern char *host;
    extern HWND g_hWnd;
}

void SaveInfo();

/* Get option value */
LPTSTR GetOption(LPTSTR line,
                 LPTSTR string)
{
    LPTSTR temp;

    temp = line + _tcslen(string);

    while (*temp != TEXT('\0')) {
        if (_tcschr(SPACES, *temp)) {
            temp ++;
        }
        else {
            return temp;
        }
    }

    return NULL;
}

/* Check a line */
static void CheckLine(LPTSTR line)
{
    TCHAR lineBuf[MAX_LINE];
    LPTSTR temp;
    static int hostCount = 0;

    if (line[0] == TEXT('#')) return;

    temp = _tcsrchr(line, TEXT('\n'));
    if (temp) {
        *temp = TEXT('\0');
    }
    _tcscpy(lineBuf, line);

    temp = _tcstok(lineBuf, SPACES);
    if (!temp) return;

    if (!_tcscmp(temp, CONF_RECENTHOST)) {
        if (hostCount >= MAX_HOST_NUM) return;
        temp = _tcstok(NULL, SPACES);
        if (!temp) return;

        _tcscpy(g_hosts[hostCount], temp);

        hostCount ++;
    }
    else if (!_tcscmp(temp, CONF_CONFIGFOLDER)) {
        temp = GetOption(line, CONF_CONFIGFOLDER);
        if (!temp) return;

        _tcscpy(g_configFolder, temp);
    }
    else if (!_tcscmp(temp, CONF_CONFIGFILE)) {
        temp = GetOption(line, CONF_CONFIGFILE);
        if (!temp) return;

        _tcscpy(g_configFile, temp);
    }
    else if (!_tcscmp(temp, CONF_AUTOMATICALLYHIDE)) {
        temp = GetOption(line, CONF_AUTOMATICALLYHIDE);
        if (!temp) return;

        if (!_tcscmp(temp, STRING_TRUE)) {
            g_automaticallyHide = TRUE;
        }
        else {
            g_automaticallyHide = FALSE;
        }
    }
}

/* Set default values */
static void SetDefaultValues()
{
    int i;

    for (i = 0; i < MAX_HOST_NUM; i ++) {
        _tcscpy(g_hosts[i], TEXT(""));
    }

    g_automaticallyHide = DEFAULT_AUTOMATICALLYHIDE;

    _tcscpy(g_configFolder, DEFAULT_CONFIGFOLDER);
    _tcscpy(g_configFile, DEFAULT_CONFIG);
}

/* Gets ini file name */
static void GetIniFile()
{
    TCHAR modulePath[MAX_PATH];
    LPTSTR temp;

    GetModuleFileName(NULL, modulePath, MAX_PATH);

    temp = _tcsrchr(modulePath, TEXT('\\'));
    if (temp) {
        *temp = TEXT('\0');
        /* change the working directory to the directory of PortForwarder.exe */
        _tchdir(modulePath);

        _tcscpy(iniFileName, modulePath);
        _tcscat(iniFileName, TEXT("\\") APP_NAME TEXT(".ini"));
    }
    else {
        /* change the working directory to the directory of PortForwarder.exe */
        _tchdir("\\");
        _tcscpy(iniFileName, TEXT("\\") APP_NAME TEXT(".ini"));
    }
}

/* Gets values from ini file. */
BOOL LoadInfo()
{
    FILE *file;
    TCHAR line[MAX_LINE];

    SetDefaultValues();

    GetIniFile();
    file = _tfopen(iniFileName, TEXT("r"));
    if (!file) {
        SaveInfo();
        return TRUE;
    }

    while (_fgetts(line, MAX_LINE, file)) {
        CheckLine(line);
    }
    fclose(file);
    
    return TRUE;
}

/* Sets some values to ini file. */
void SaveInfo()
{
    FILE *file;
    TCHAR TRUEorFALSE[10];

    GetIniFile();
    file = _tfopen(iniFileName, TEXT("w"));
    if (!file) {
        msgboxinfo info;
        info.caption = APP_NAME;
        info.message = "Could not open ini file for writing.";
        info.type = MB_OK | MB_ICONERROR;
        SendMessage(g_hWnd, MSG_SHOW_MESSAGEBOX, (WPARAM)&info, 0);
        return;
    }

    /* Write help message. */
    _ftprintf(file, HELP_LINE_HOST);
    _ftprintf(file, HELP_LINE_CONFIGFOLDER);
    _ftprintf(file, HELP_LINE_CONFIGFILE);
    _ftprintf(file, HELP_LINE_AUTOMATICALLYHIDE);
    _ftprintf(file, TEXT("\n"));

    if (!host) {
        /* This must be the first launch. */
        /* Do not write "RecentHost" lines */
    }
    else {
        int i, j;

        j = 0;
        for (i = 0; i < MAX_HOST_NUM; i ++) {
            LPTSTR temp;
            if (i == 0) {
                temp = host;
            }
            else {
                if (!_tcscmp(host, g_hosts[j])) {
                    j ++;
                }
                temp = g_hosts[j];
                j ++;
            }

            if (!_tcscmp(temp, TEXT("")))
                break;

            _ftprintf(file, TEXT("%s         %s\n"), CONF_RECENTHOST, temp);
        }
    }
    
    if (g_automaticallyHide) {
        _tcscpy(TRUEorFALSE, STRING_TRUE);
    }
    else {
        _tcscpy(TRUEorFALSE, STRING_FALSE);
    }

    _ftprintf(file, TEXT("%s       %s\n"), CONF_CONFIGFOLDER, g_configFolder);
    _ftprintf(file, TEXT("%s         %s\n"), CONF_CONFIGFILE, g_configFile);
    _ftprintf(file, TEXT("%s  %s\n"), CONF_AUTOMATICALLYHIDE, TRUEorFALSE);

    fclose(file);
}
