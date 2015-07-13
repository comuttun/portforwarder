// PortForwarder.h : PORTFORWARDER アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_PORTFORWARDER_H__FC035639_2D50_4F0D_B6EF_A5B81671B557__INCLUDED_)
#define AFX_PORTFORWARDER_H__FC035639_2D50_4F0D_B6EF_A5B81671B557__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CPortForwarderApp:
// このクラスの動作の定義に関しては PortForwarder.cpp ファイルを参照してください。
//

class CPortForwarderApp : public CWinApp
{
public:
	CPortForwarderApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CPortForwarderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション

	//{{AFX_MSG(CPortForwarderApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PORTFORWARDER_H__FC035639_2D50_4F0D_B6EF_A5B81671B557__INCLUDED_)
