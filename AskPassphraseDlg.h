#if !defined(AFX_ASKPASSPHRASEDLG_H__7E6F220C_81E4_42A5_887D_9D89ED0E66C6__INCLUDED_)
#define AFX_ASKPASSPHRASEDLG_H__7E6F220C_81E4_42A5_887D_9D89ED0E66C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AskPassphraseDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CAskPassphraseDlg ダイアログ

class CAskPassphraseDlg : public CDialog
{
// コンストラクション
public:
	CAskPassphraseDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CAskPassphraseDlg)
	enum { IDD = IDD_PF_ASK_PASSPHRASE_DLG };
	CString	m_passphrase;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CAskPassphraseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CAskPassphraseDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    askpassinfo* info;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_ASKPASSPHRASEDLG_H__7E6F220C_81E4_42A5_887D_9D89ED0E66C6__INCLUDED_)
