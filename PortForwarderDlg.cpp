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
 * $Id: PortForwarderDlg.cpp,v 1.15 2005/11/04 09:12:12 ozawa Exp $
 */

#include "stdafx.h"
#include "PortForwarder_inc.h"
#include "PortForwarder.h"
#include "PortForwarderDlg.h"
#include "LoadSaveInfo.h"
#include "FindFile.h"
#include "AskPassphraseDlg.h"
#include "app_version.h"

extern "C" {
#include <xmalloc.h>
void close_now();
void clear_fatal_cleanups();
void OpenLogFile();
void CloseLogFile();
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NI_MAXHOST 1025

/* Hosts that were most currently used. */
TCHAR g_hosts[MAX_HOST_NUM][MAX_PATH];
/* Name of config file. */
TCHAR g_configFile[MAX_PATH];
/* Flag indicating whether user requests to hide main dialog automatically. */
BOOL g_automaticallyHide;

extern "C" {
    HWND g_hWnd;
    char* user;
    extern char* host;      /* ssh.c */
    extern char* config;    /* ssh.c */
    /* Path of config folder. */
    char g_configFolder[MAX_PATH];
    char pf_option[64];     /* command-line option */
}


/* Name of the connecting host. */
static TCHAR hostTemp[NI_MAXHOST];

static CWinThread* sshThread = NULL;
static UINT DoMain(LPVOID pParam);
static UINT DoClientLoop(LPVOID pParam);
static BOOL iconFlag;
static BOOL firstTimeFlag = 1;

/////////////////////////////////////////////////////////////////////////////
// CPortForwarderDlg

CPortForwarderDlg::CPortForwarderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPortForwarderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPortForwarderDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPortForwarderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPortForwarderDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPortForwarderDlg, CDialog)
	//{{AFX_MSG_MAP(CPortForwarderDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PF_CONNECTBTN, OnButtonConnect)
	ON_BN_CLICKED(IDC_PF_CONFIGFILEBTN, OnConfigFileButton)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PF_HIDEBTN, OnHide)
	ON_COMMAND(ID_PF_SHOW, OnPfShow)
	//}}AFX_MSG_MAP
    ON_MESSAGE(MSG_FATAL_CALLED, OnFatalCalled)
    ON_MESSAGE(MSG_SHOW_MESSAGEBOX, OnShowMessageBox)
    ON_MESSAGE(MSG_GET_PASSPHRASE, OnGetPassphrase)
    ON_MESSAGE(MSG_CONNECTED, OnConnected)
    ON_MESSAGE(MSG_THREAD_END, OnThreadEnd)
    ON_MESSAGE(MSG_TRAY_CLICKED, OnTrayClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPortForwarderDlg

BOOL CPortForwarderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
    g_hWnd = m_hWnd;
    SetWindowText(APP_NAME " " APP_VERSION);
	
    strcpy(hostTemp, "");
    user = NULL;

    SetInfo();

#ifdef _LOG_
    OpenLogFile();
#endif /* _LOG_ */

    // check command line options
    char* cmdline = AfxGetApp()->m_lpCmdLine;
    if (strlen(cmdline) > NI_MAXHOST - 1) {
        MessageBox("Command-line option is too long.", APP_NAME,
                   MB_OK | MB_ICONERROR);
        EndDialog(0);
    }

    // search hyphen-prefixed command-line option
    const char *pCmdline = cmdline;     // command-line char pointer
    int optidx = 0;     // pf_option[] index
    while (*pCmdline && (*pCmdline == ' ' || *pCmdline == '-')) {
        if (*pCmdline == '-')   // option prefix ?
            pf_option[optidx++] = *++pCmdline;  // set parameter char
        pCmdline++;     // point the next char
    }
    pf_option[optidx] = '\0';   // set string terminator

    // search user & host name
    if (*pCmdline != '\0') {
        char* temp;
        char* pos;
        temp = new char[strlen(pCmdline) + 1];
        strcpy(temp, pCmdline);
        pos = strchr(temp, '@');
        if (pos) {
            // it must be 'user@host'
            // get user and host
            *pos = '\0';
            user = (char*)xmalloc(strlen(temp) + 1);
            strcpy(user, temp);
            strcpy(hostTemp, pos + 1);
            SendDlgItemMessage(IDC_PF_HOSTCOMBO, WM_SETTEXT,
                               (WPARAM)0, (LPARAM)(pos + 1));
        }
        else {
            // it must be 'host'
            strcpy(hostTemp, temp);
            SendDlgItemMessage(IDC_PF_HOSTCOMBO, WM_SETTEXT,
                               (WPARAM)0, (LPARAM)temp);
        }
        delete[] temp;
    }

	return TRUE;
}

void CPortForwarderDlg::OnPaint() 
{
    if (firstTimeFlag) {
        firstTimeFlag = 0;
        if (strcmp(hostTemp, "")) {
            // there must be a command-line option
            // and hostTemp is already set
            OnButtonConnect();
        }
    }

	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPortForwarderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPortForwarderDlg::OnButtonConnect() 
{
    if (!GetInfo()) {
        return;
    }

    /* Got all the needed info */
    GetDlgItem(IDC_PF_CONNECTBTN)->EnableWindow(FALSE);
    GetDlgItem(IDC_PF_HOSTCOMBO)->EnableWindow(FALSE);
    GetDlgItem(IDC_PF_CONFIGFILEBTN)->EnableWindow(FALSE);
    SetStatus(PF_STAT_CONNECTING);
    GetDlgItem(IDC_PF_STATUSSTATIC)->SetWindowText(PF_STRING_CONNECTING);
    host = hostTemp;
    config = g_configFile;
    SaveInfo();
    AdjustFileNames();
        // this way I don't have to delete host nor config in ssh.c
    sshThread = AfxBeginThread(DoMain, NULL, THREAD_PRIORITY_NORMAL, 0,
                               CREATE_SUSPENDED);
    sshThread->m_bAutoDelete = FALSE;
    sshThread->ResumeThread();
}

void CPortForwarderDlg::CloseConnection()
{
    SetStatus(PF_STAT_DISCONNECTING);
    GetDlgItem(IDC_PF_STATUSSTATIC)->SetWindowText(PF_STRING_DISCONNECTING);
    close_now();
    WaitForSingleObject(sshThread->m_hThread, 2000);
    /* wait for two seconds.  if thread doesn't end in time,
     * just exit the app.
     */
}

LRESULT CPortForwarderDlg::OnFatalCalled(WPARAM wParam, LPARAM lParam)
{
    OnPfShow();
    MessageBox((LPCTSTR)wParam, APP_NAME, MB_OK | MB_ICONERROR);

    if (sshThread) {
        /* wait for thread to exit for a while. */
        WaitForSingleObject(sshThread->m_hThread, 2000);
        /* even if timeout occurs, just exit the app. */
    }

    // exit the application on fatal error!
    EndDialog(0);
    return 0;
}

LRESULT CPortForwarderDlg::OnShowMessageBox(WPARAM wParam, LPARAM lParam)
{
    msgboxinfo* info = (msgboxinfo*)wParam;

    OnPfShow();
    info->result = MessageBox(info->message, info->caption, info->type);
    return 0;
}

LRESULT CPortForwarderDlg::OnGetPassphrase(WPARAM info, LPARAM lParam)
{
    SetForegroundWindow();
    CAskPassphraseDlg dlg;
    dlg.info = (askpassinfo*)info;
    int rtn = dlg.DoModal();
    if (rtn == IDOK) {
        dlg.info->result = xstrdup(dlg.m_passphrase);
        return 0;
    }
    else {
        /* user canceled */
        dlg.info->result = NULL;
        return 0;
    }
}

void CPortForwarderDlg::OnConfigFileButton() 
{
    TCHAR fileBuf[MAX_PATH];
    fileBuf[0] = TEXT('\0');
    if (FindFile(g_hWnd, TEXT("Open: Config File"),
                 TEXT("All Files\0*.*\0\0"),
                 g_configFolder, fileBuf, MAX_PATH)) {
        LPTSTR temp;
        _tcscpy(g_configFolder, fileBuf);
        SendDlgItemMessage(IDC_PF_CONFIGFILEEDIT, WM_SETTEXT,
                           (WPARAM)0, (LPARAM)g_configFolder);

        CreateHostsList(g_configFolder);

        temp = _tcsrchr(g_configFolder, TEXT('\\'));
        *temp = TEXT('\0');
        _tcscpy(g_configFile, temp + 1);
        temp = _tcschr(g_configFolder, TEXT('\\'));
        if (!temp) {
            /* Must be root directory. */
            _tcscat(g_configFolder, TEXT("\\"));
        }
    }
    return;
}

/* Insert hosts into host combo box. */
void CPortForwarderDlg::CreateHostsList(LPCTSTR configFile)
{
    int i;
    FILE *file;
    TCHAR line[MAX_LINE];

    SendDlgItemMessage(IDC_PF_HOSTCOMBO, CB_RESETCONTENT, 0, 0);

    SendDlgItemMessage(IDC_PF_HOSTCOMBO, WM_SETTEXT,
                       (WPARAM)0, (LPARAM)g_hosts[0]);

    for (i = 0; i < MAX_HOST_NUM; i ++) {
        if (!_tcscmp(g_hosts[i], TEXT(""))) {
            break;
        }
        SendDlgItemMessage(IDC_PF_HOSTCOMBO, CB_ADDSTRING,
                           (WPARAM)0, (LPARAM)g_hosts[i]);
    }

    file = _tfopen(configFile, TEXT("r"));
    if (!file) {
        return;
    }

    while (_fgetts(line, MAX_LINE, file)) {
        LPTSTR cp;
        LPTSTR temp;

        temp = _tcsrchr(line, TEXT('\n'));
        if (temp) {
            *temp = TEXT('\0');
        }

        cp = line + _tcsspn(line, TEXT(" \t\r\n"));
        if (!*cp || *cp == TEXT('\n') || *cp == TEXT('#')) {
            continue;
        }

        if (_tcsncmp(cp, TEXT("HostName"), _tcslen(TEXT("HostName"))) &&
            !_tcsncmp(cp, TEXT("Host"), _tcslen(TEXT("Host")))) {
            temp = GetOption(cp, TEXT("Host"));
            if (!temp) {
                continue;
            }
            if (_tcschr(temp, TEXT('*')) || _tcschr(temp, TEXT(' '))) {
                continue;
            }

            if (CB_ERR == SendDlgItemMessage(IDC_PF_HOSTCOMBO,
                                             CB_FINDSTRINGEXACT, (WPARAM)0,
                                             (LPARAM)temp)) {
                SendDlgItemMessage(IDC_PF_HOSTCOMBO, CB_ADDSTRING,
                                   (WPARAM)0, (LPARAM)temp);
            }
        }
    }

    fclose(file);
    UpdateData(FALSE);
}

/* Gets information on main dialog. */
BOOL CPortForwarderDlg::GetInfo()
{
    UpdateData(TRUE);
    if (strcmp(hostTemp, "")) {
        // there must be a command-line option
        // and hostTemp is already set
        // do nothing...
    }
    else {
        SendDlgItemMessage(IDC_PF_HOSTCOMBO, WM_GETTEXT,
                           (WPARAM)NI_MAXHOST, (LPARAM)hostTemp);
    }

    if (BST_CHECKED == SendDlgItemMessage(IDC_PF_AUTOHIDECHK,
                                          BM_GETCHECK, 0, 0)) {
        g_automaticallyHide = TRUE;
    }
    else {
        g_automaticallyHide = FALSE;
    }

    if (_tcschr(hostTemp, TEXT(' '))) {
        SetForegroundWindow();
        MessageBox(TEXT("\"Host\" can't contain spaces!"), APP_NAME,
                   MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!_tcscmp(hostTemp, TEXT("")) ||
        !_tcscmp(g_configFolder, TEXT(""))) {
        SetForegroundWindow();
        MessageBox(TEXT("Fill all the blanks!"), APP_NAME,
                   MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    return TRUE;
}

void CPortForwarderDlg::SetInfo()
{
    TCHAR buf[MAX_PATH];

    if (!LoadInfo()) {
        MessageBox("Failed to get info from ini file.", APP_NAME,
                   MB_OK | MB_ICONERROR);
        EndDialog(0);
    }

    if (!_tcscmp(g_configFolder, TEXT(""))) {
        SendDlgItemMessage(IDC_PF_CONFIGFILEEDIT, WM_SETTEXT,
                           (WPARAM)0, (LPARAM)TEXT(""));
    }
    else {
        _tcscpy(buf, g_configFolder);
        if (g_configFolder[_tcslen(g_configFolder) - 1] != TEXT('\\')) {
            _tcscat(buf, TEXT("\\"));
        }
        _tcscat(buf, g_configFile);
        SendDlgItemMessage(IDC_PF_CONFIGFILEEDIT, WM_SETTEXT,
                           (WPARAM)0, (LPARAM)buf);
    }

    CreateHostsList(buf);

    SendDlgItemMessage(IDC_PF_AUTOHIDECHK, BM_SETCHECK,
                       (WPARAM)g_automaticallyHide, (LPARAM)0);

    SendDlgItemMessage(IDC_PF_STATUSSTATIC, WM_SETTEXT,
                       (WPARAM)0, (LPARAM)PF_STRING_NOT_CONNECTED);

    GetDlgItem(IDC_PF_HIDEBTN)->EnableWindow(FALSE);
}

/* Adjusts file names. */
void CPortForwarderDlg::AdjustFileNames()
{
    TCHAR buf[MAX_PATH];
    BOOL rootFlag;

    /* Check if config folder is root directory. */
    if (g_configFolder[_tcslen(g_configFolder) - 1] == TEXT('\\')) {
        rootFlag = TRUE;
    }
    else {
        rootFlag = FALSE;
    }

    _tcscpy(buf, g_configFile);
    _tcscpy(g_configFile, g_configFolder);
    if (!rootFlag) {
        _tcscat(g_configFile, TEXT("\\"));
    }
    _tcscat(g_configFile, buf);
}

LRESULT CPortForwarderDlg::OnConnected(WPARAM info, LPARAM lParam)
{
    SetStatus(PF_STAT_CONNECTED);
    GetDlgItem(IDC_PF_STATUSSTATIC)->SetWindowText(PF_STRING_CONNECTED);
    GetDlgItem(IDC_PF_AUTOHIDECHK)->EnableWindow(FALSE);
    GetDlgItem(IDC_PF_HIDEBTN)->EnableWindow(TRUE);
    GetDlgItem(IDC_PF_HIDEBTN)->SetFocus();
    if (g_automaticallyHide) {
        OnHide();
    }
    return 0;
}

LRESULT CPortForwarderDlg::OnThreadEnd(WPARAM info, LPARAM lParam)
{
    if (GetStatus() == PF_STAT_CONNECTED) {
        /* server must disconnect cleanly (not fatally).
         * could this happen?
         */
        OnPfShow();
        MessageBox("Connection closed by server.", APP_NAME,
                   MB_OK | MB_ICONEXCLAMATION);
        if (sshThread) {
            delete sshThread;
            sshThread = NULL;
        }
        EndDialog(0);
    }
    else if (GetStatus() == PF_STAT_DISCONNECTING) {
        /* client requested to disconnect. */
        SetStatus(PF_STAT_NOT_CONNECTED);
        GetDlgItem(IDC_PF_STATUSSTATIC)->SetWindowText(PF_STRING_NOT_CONNECTED);
    }
    else {
        /* never happens */
    }
    return 0;
}

void CPortForwarderDlg::OnCancel() 
{
    ShowWindow(SW_SHOWNORMAL);
    int rtn;
    rtn =  MessageBox("Do you really want to exit?",
                      APP_NAME, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (rtn != IDYES) {
        return;
    }

    if (GetStatus() == PF_STAT_CONNECTED) {
        CloseConnection();
    }
	
	CDialog::OnCancel();
}

LRESULT CPortForwarderDlg::OnTrayClicked(UINT iconResource, LONG mouseMessage)
{
    if (iconResource == IDR_MAINFRAME) {
        switch (mouseMessage) {
            case WM_RBUTTONUP:
            {
                SetForegroundWindow();

                CMenu menu;
                menu.LoadMenu(IDR_PF_MENU);
                CMenu* popup = menu.GetSubMenu(0);

                CPoint curPoint;
                GetCursorPos(&curPoint);

                popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                      curPoint.x, curPoint.y,
                                      AfxGetMainWnd());

                PostMessage(WM_USER);

                break;
            }
            case WM_LBUTTONDOWN:
                SetForegroundWindow();
                break;
            case WM_LBUTTONDBLCLK:
                OnPfShow();
                break;
            default:
                break;
        }
    }

    return 0;
}

/********************/
/* Thread functions */
/********************/
static UINT DoMain(LPVOID pParam)
{
    main();

#if 0 /* This cleanup is not necessary. So let's do not do this, for now. */
    clear_fatal_cleanups();
#endif /* 0 */

    PostMessage(g_hWnd, MSG_THREAD_END, 0, 0);

    return 0;
}

void CPortForwarderDlg::AddIcon()
{
    if (iconFlag) {
        return;
    }

    m_trayIcon.cbSize = sizeof(NOTIFYICONDATA);
    m_trayIcon.hWnd = m_hWnd;
    m_trayIcon.uID = IDR_MAINFRAME;
    m_trayIcon.uCallbackMessage = MSG_TRAY_CLICKED;
#if 1
    /* get the 16x16 icon. */
    m_trayIcon.hIcon = (HICON)LoadImage(AfxGetInstanceHandle(),
                                        MAKEINTRESOURCE(IDR_MAINFRAME),
                                        IMAGE_ICON, 16, 16, 0);
#else /* 0 */
    /* get the 32x32 icon. it is automatically fitted into
     * 16x16. but not clear...
     */
	m_trayIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#endif /* 0 */
    strcpy(m_trayIcon.szTip, hostTemp);
    m_trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    Shell_NotifyIcon(NIM_ADD, &m_trayIcon);
	
    iconFlag = 1;
}

void CPortForwarderDlg::OnDestroy() 
{
	CDialog::OnDestroy();

    DeleteIcon();
#ifdef _LOG_
    CloseLogFile();
#endif /* _LOG_ */
}

void CPortForwarderDlg::DeleteIcon()
{
    if (!iconFlag) {
        return;
    }

    Shell_NotifyIcon(NIM_DELETE, &m_trayIcon);

    iconFlag = 0;
}

void CPortForwarderDlg::OnHide() 
{
    ShowWindow(SW_SHOWMINIMIZED);
    ShowWindow(SW_HIDE);
    AddIcon();
}

void CPortForwarderDlg::OnPfShow() 
{
    // first SW_SHOW then SW_SHOWNORMAL.
    // i like this trick!
    if (iconFlag) {
        DeleteIcon();
        ShowWindow(SW_SHOWMINIMIZED);
        Sleep(200);
    }
    ShowWindow(SW_SHOWNORMAL);
    GetDlgItem(IDC_PF_HIDEBTN)->SetFocus();
    SetForegroundWindow();
}
