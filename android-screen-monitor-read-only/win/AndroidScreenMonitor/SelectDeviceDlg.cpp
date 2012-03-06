// SelectAndroidDevice.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AndroidScreenMonitor.h"
#include "SelectDeviceDlg.h"

CString CSelectDeviceDlg::m_aDeviceSerialNumber = _T("");

#define WM_USER_SD_TRACK_DEVICES			(WM_USER + 1)

// CSelectDeviceDlg ダイアログ

IMPLEMENT_DYNAMIC(CSelectDeviceDlg, CDialog)

CSelectDeviceDlg::CSelectDeviceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDeviceDlg::IDD, pParent)
{
	m_pAndroidSocket = NULL;
	m_ShowMsg = TRUE;
}

CSelectDeviceDlg::~CSelectDeviceDlg()
{
	if (m_pAndroidSocket)
	{
		delete m_pAndroidSocket;
		m_pAndroidSocket = NULL;
	}
}

void CSelectDeviceDlg::TrackDevices()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_ShowMsg = TRUE;

	PostMessage(WM_USER_SD_TRACK_DEVICES);
}

void CSelectDeviceDlg::UpdateListDevice(const CStringArray& deviceSerialNumbers)
{
	m_ListCtrlDevice.DeleteAllItems();

	if (deviceSerialNumbers.GetCount() > 0)
	{
		int selIndex = -1;

		for (int i = 0; i < deviceSerialNumbers.GetCount(); i++)
		{
			CString aDeviceSerialNumber = deviceSerialNumbers.GetAt(i);
			int index = m_ListCtrlDevice.InsertItem(i, aDeviceSerialNumber);
			
			if (aDeviceSerialNumber.Compare(m_aDeviceSerialNumber) == 0)
			{
				selIndex = index;
			}
			
			TRACE(aDeviceSerialNumber);
			TRACE(_T("\n"));
		}

#if 0
{
	for (int i = 0; i < 10; i++)
	{
		m_ListCtrlDevice.InsertItem(0, _T("This is dummy for vertical scroll bar checking"));
	}
}
#endif
		if (selIndex < 0)
		{
			selIndex = 0;
		}

		for (int i = 0; i < m_ListCtrlDevice.GetItemCount(); i++)
		{
			UINT state = 0;

			if (i == selIndex)
			{
				state = LVIS_SELECTED;
			}
			
			m_ListCtrlDevice.SetItemState(i, state, LVIS_SELECTED);
		}
	}

	if (m_ListCtrlDevice.GetItemCount() <= 0)
	{
		if (m_ShowMsg)
		{
			ShowAndroidDeviceNotFound();
			m_ShowMsg = FALSE;
		}
	}
}

void CSelectDeviceDlg::PreOK()
{
	m_aDeviceSerialNumber = _T("");

	if (m_ListCtrlDevice.GetSafeHwnd())
	{
		POSITION pos = m_ListCtrlDevice.GetFirstSelectedItemPosition();

		if (pos)
		{
			int index = m_ListCtrlDevice.GetNextSelectedItem(pos);

			if (index >= 0)
			{
				m_aDeviceSerialNumber = m_ListCtrlDevice.GetItemText(index, 0);
			}
		}
	}
}

void CSelectDeviceDlg::ShowAndroidDeviceNotFound()
{
	MessageBox(_T("There was no Android devices connecting this computer.\n\
Please check connection."),
		_T("Android device not found"),
		MB_ICONWARNING | MB_OK);
}

void CSelectDeviceDlg::ShowConnectingADBServerError()
{
	MessageBox(_T("Failed to connect Android Debug Bridge Server (adb).\n\
Please check whether the adb server is running.\n\
You can start the adb server by \"adb start-server\" commands.\n\
To restart server, Use \"adb kill-server\" commands to terminate it and then start it."),
		_T("Connecting adb server error"),
		MB_ICONWARNING | MB_OK);
}

void CSelectDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_ListCtrlDevice);
}


BEGIN_MESSAGE_MAP(CSelectDeviceDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEVICE, &CSelectDeviceDlg::OnNMDblclkListDevice)
	ON_BN_CLICKED(IDOK, &CSelectDeviceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSelectDeviceDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CSelectDeviceDlg::OnBnClickedButtonRefresh)

	ON_MESSAGE(WM_USER_SD_TRACK_DEVICES, OnTrackDevices)
	ON_MESSAGE(WM_USER_ANDROID_SOCKET, OnAndroidSocket)
END_MESSAGE_MAP()


// CSelectDeviceDlg メッセージ ハンドラ

BOOL CSelectDeviceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	m_ListCtrlDevice.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListCtrlDevice.InsertColumn(0, _T("Serial number"), LVCFMT_LEFT, 260);

	TrackDevices();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CSelectDeviceDlg::OnNMDblclkListDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	*pResult = 0;

	PreOK();

	EndDialog(IDOK);
}

void CSelectDeviceDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	PreOK();

	OnOK();
}

void CSelectDeviceDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	OnCancel();
}

void CSelectDeviceDlg::OnBnClickedButtonRefresh()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	TrackDevices();
}

LRESULT CSelectDeviceDlg::OnTrackDevices(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	BOOL bSuccess = TRUE;

	try
	{
		if (m_pAndroidSocket)
		{
			delete m_pAndroidSocket;
			m_pAndroidSocket = NULL;
		}

		m_pAndroidSocket = new CAndroidSocket((CWnd*)this);

		bSuccess = m_pAndroidSocket->SendTrackDevices();
	}
	catch (...)
	{
		bSuccess = FALSE;

		if (m_pAndroidSocket)
		{
			delete m_pAndroidSocket;
			m_pAndroidSocket = NULL;
		}
	}

	if (!bSuccess)
	{
		ShowConnectingADBServerError();
	}

	return lResult;
}

LRESULT CSelectDeviceDlg::OnAndroidSocket(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	if (m_pAndroidSocket)
	{
		switch (wParam)
		{
		case AS_WPARAM_POST_TRACK_DEVICES:
			{
				UpdateListDevice(m_pAndroidSocket->GetDeviceSerialNumbers());
			}
			break;
		default:
			{
			}
			break;
		}

		lResult = m_pAndroidSocket->OnAndroidSocket(wParam, lParam);
	}

	return lResult;
}
