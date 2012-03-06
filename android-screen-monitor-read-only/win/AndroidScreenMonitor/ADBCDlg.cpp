// ADBCDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AndroidScreenMonitor.h"
#include "ADBCDlg.h"

#define WM_USER_ADBC_START_SERVER			(WM_USER + 1)
#define WM_USER_ADBC_TERMINATE_SERVER		(WM_USER + 2)
#define WM_USER_ADBC_RESTART_SERVER			(WM_USER + 3)

// CADBCDlg ダイアログ

IMPLEMENT_DYNAMIC(CADBCDlg, CDialog)

CADBCDlg::CADBCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CADBCDlg::IDD, pParent)
{

}

CADBCDlg::~CADBCDlg()
{
}

void CADBCDlg::StartServer()
{
	PostMessage(WM_USER_ADBC_START_SERVER);
}

void CADBCDlg::TerminateServer()
{
	PostMessage(WM_USER_ADBC_TERMINATE_SERVER);
}

void CADBCDlg::RestartServer()
{
	PostMessage(WM_USER_ADBC_RESTART_SERVER);
}

BOOL CADBCDlg::ProcessCommand(LPTSTR pszCommandLine)
{
	BOOL bSuccess = TRUE;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	
	bSuccess = CreateProcess(NULL, pszCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (bSuccess)
	{
		WaitForSingleObject(pi.hProcess, 10 * 1000);
	}

	if (pi.hThread)
	{
		CloseHandle(pi.hThread);
		pi.hThread = NULL;
	}

	if (pi.hProcess)
	{
		CloseHandle(pi.hProcess);
		pi.hProcess = NULL;
	}

	return bSuccess;
}

BOOL CADBCDlg::ProcessADBStartServer()
{
	BOOL bSuccess = TRUE;
	TCHAR g_pszCommandLine[] = _T("adb start-server");

	bSuccess = ProcessCommand(g_pszCommandLine);

	if (!bSuccess)
	{
		MessageBox(_T("Failed to execute \"adb start-server\" command.\n\
Please check a \"PATH\" environment variable on computer."),
			_T("Android Debug Bridge (adb) starting error"),
			MB_ICONWARNING | MB_OK);
	}

	return bSuccess;
}

BOOL CADBCDlg::ProcessADBTerminateServer()
{
	BOOL bSuccess = TRUE;
	TCHAR g_pszCommandLine[] = _T("adb kill-server");

	{
		CWaitCursor waitCursor;

		bSuccess = ProcessCommand(g_pszCommandLine);

		Sleep(1000);
	}

	if (!bSuccess)
	{
		MessageBox(_T("Failed to execute \"adb kill-server\" command.\n\
Please check a \"PATH\" environment variable on computer."),
			_T("Android Debug Bridge (adb) terminating error"),
			MB_ICONWARNING | MB_OK);
	}

	return bSuccess;
}

void CADBCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CADBCDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_START, &CADBCDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATE, &CADBCDlg::OnBnClickedButtonTerminate)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CADBCDlg::OnBnClickedButtonRestart)

	ON_MESSAGE(WM_USER_ADBC_START_SERVER, OnStartServer)
	ON_MESSAGE(WM_USER_ADBC_TERMINATE_SERVER, OnTerminateServer)
	ON_MESSAGE(WM_USER_ADBC_RESTART_SERVER, OnRestartServer)
END_MESSAGE_MAP()


// CADBCDlg メッセージ ハンドラ

BOOL CADBCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	StartServer();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CADBCDlg::OnBnClickedButtonStart()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	StartServer();
}

void CADBCDlg::OnBnClickedButtonTerminate()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	TerminateServer();
}

void CADBCDlg::OnBnClickedButtonRestart()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	RestartServer();
}

LRESULT CADBCDlg::OnStartServer(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = ProcessADBStartServer();
	}

	if (bSuccess)
	{
		EndDialog(IDOK);
	}

	return lResult;
}

LRESULT CADBCDlg::OnTerminateServer(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = ProcessADBTerminateServer();
	}

	return lResult;
}

LRESULT CADBCDlg::OnRestartServer(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = ProcessADBTerminateServer();
	}

	if (bSuccess)
	{
		bSuccess = ProcessADBStartServer();
	}

	if (bSuccess)
	{
		EndDialog(IDOK);
	}

	return lResult;
}
