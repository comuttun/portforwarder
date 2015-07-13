// PortForwarderDlg.h : ヘッダー ファイル
//

#if !defined(AFX_PORTFORWARDERDLG_H__AE793096_DECB_43D6_B17B_795DA7E90082__INCLUDED_)
#define AFX_PORTFORWARDERDLG_H__AE793096_DECB_43D6_B17B_795DA7E90082__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPortForwarderDlg ダイアログ

class CPortForwarderDlg : public CDialog
{
// 構築
public:
	CPortForwarderDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CPortForwarderDlg)
	enum { IDD = IDD_PORTFORWARDER_DIALOG };
		// メモ: この位置に ClassWizard によってデータ メンバが追加されます。
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CPortForwarderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	HICON m_hIcon;

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPortForwarderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnConfigFileButton();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnHide();
	afx_msg void OnPfShow();
	//}}AFX_MSG
    afx_msg LRESULT OnFatalCalled(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnShowMessageBox(WPARAM info, LPARAM lParam);
    afx_msg LRESULT OnGetPassphrase(WPARAM info, LPARAM lParam);
    afx_msg LRESULT OnConnected(WPARAM info, LPARAM lParam);
    afx_msg LRESULT OnThreadEnd(WPARAM info, LPARAM lParam);
    afx_msg LRESULT OnTrayClicked(UINT iconResource, LONG mouseMessage);
	DECLARE_MESSAGE_MAP()
private:
    NOTIFYICONDATA m_trayIcon;
	void CloseConnection();
    void CreateHostsList(LPCTSTR configFile);
    BOOL GetInfo();
    void SetInfo();
    void AdjustFileNames();
    void AddIcon();
    void DeleteIcon();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PORTFORWARDERDLG_H__AE793096_DECB_43D6_B17B_795DA7E90082__INCLUDED_)
