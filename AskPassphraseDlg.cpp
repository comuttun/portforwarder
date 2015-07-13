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
 * $Id: AskPassphraseDlg.cpp,v 1.2 2003/12/09 14:11:34 toh Exp $
 */

#include "stdafx.h"
#include "PortForwarder_inc.h"
#include "resource.h"
#include "AskPassphraseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAskPassphraseDlg


CAskPassphraseDlg::CAskPassphraseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAskPassphraseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAskPassphraseDlg)
	m_passphrase = _T("");
	//}}AFX_DATA_INIT
}


void CAskPassphraseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAskPassphraseDlg)
	DDX_Text(pDX, IDC_PF_PASSPHRASE_EDIT, m_passphrase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAskPassphraseDlg, CDialog)
	//{{AFX_MSG_MAP(CAskPassphraseDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAskPassphraseDlg

BOOL CAskPassphraseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    SetWindowText(APP_NAME);
    GetDlgItem(IDC_PF_ASKPASS_PROMPT_STATIC)->SetWindowText(info->prompt);
	
	return TRUE;
}
