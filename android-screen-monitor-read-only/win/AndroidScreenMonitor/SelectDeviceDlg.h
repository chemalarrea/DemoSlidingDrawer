#pragma once

#include "AndroidSocket.h"
#include "afxwin.h"
#include "afxcmn.h"

// CSelectDeviceDlg ダイアログ

class CSelectDeviceDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectDeviceDlg)

public:
	CSelectDeviceDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CSelectDeviceDlg();

	static CString&	GetSerialNumber() {return (CString&)m_aDeviceSerialNumber;}

// ダイアログ データ
	enum { IDD = IDD_SELECT_DEVICE };

protected:
	static CString		m_aDeviceSerialNumber;

	CAndroidSocket*		m_pAndroidSocket;
	
	BOOL				m_ShowMsg;

	CListCtrl m_ListCtrlDevice;

	void		TrackDevices();
	void		UpdateListDevice(const CStringArray& deviceSerialNumbers);
	void		PreOK();

	void		ShowAndroidDeviceNotFound();
	void		ShowConnectingADBServerError();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	virtual BOOL OnInitDialog();

	afx_msg void OnNMDblclkListDevice(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonRefresh();

	LRESULT	OnTrackDevices(WPARAM wParam, LPARAM lParam);
	LRESULT	OnAndroidSocket(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
