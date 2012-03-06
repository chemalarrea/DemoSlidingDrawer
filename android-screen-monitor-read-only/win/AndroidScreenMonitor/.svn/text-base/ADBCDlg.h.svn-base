#pragma once
#include "afxwin.h"


// CADBCDlg ダイアログ

class CADBCDlg : public CDialog
{
	DECLARE_DYNAMIC(CADBCDlg)

public:
	CADBCDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CADBCDlg();

// ダイアログ データ
	enum { IDD = IDD_ADBC };

protected:
	void		StartServer();
	void		TerminateServer();
	void		RestartServer();

	BOOL		ProcessCommand(LPTSTR pszCommandLine);
	BOOL		ProcessADBStartServer();
	BOOL		ProcessADBTerminateServer();
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonTerminate();
	afx_msg void OnBnClickedButtonRestart();

	LRESULT	OnStartServer(WPARAM wParam, LPARAM lParam);
	LRESULT	OnTerminateServer(WPARAM wParam, LPARAM lParam);
	LRESULT	OnRestartServer(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
