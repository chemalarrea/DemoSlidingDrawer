// AndroidScreenMonitorDlg.h : header file
//

#pragma once

#include "AndroidSocket.h"

// CAndroidScreenMonitorDlg dialog
class CAndroidScreenMonitorDlg : public CDialog
{
// Construction
public:
	CAndroidScreenMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ANDROIDSCREENMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	void				ShowADBC();
	void				ShowSelectDevice();
	void				Transport();
	void				SendNudge();

	void				Zoom(float scale);
	void				Rotate();

	virtual BOOL DestroyWindow();

protected:
	HICON m_hIcon;
	HACCEL m_hAccelTable;

	CAndroidSocket*		m_pAndroidSocket;
	CString				m_aSerialNumber;
	DWORD				m_FrameBufLen;
	BYTE*				m_pFrameBuf;
	DWORD				m_FrameWidth;
	DWORD				m_FrameHeight;

	BOOL				m_InvalidateSize;
	float				m_Scale;
	BOOL				m_Rotation;
	BOOL				m_Saving;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	
	afx_msg void OnFileSave();
	afx_msg void OnViewZoom1();
	afx_msg void OnViewZoom2();
	afx_msg void OnViewZoom5();
	afx_msg void OnViewZoom7();
	afx_msg void OnViewRotate();

	LRESULT OnShowADBC(WPARAM wParam, LPARAM lParam);
	LRESULT OnShowSelectDevice(WPARAM wParam, LPARAM lParam);
	LRESULT OnTransport(WPARAM wParam, LPARAM lParam);
	LRESULT OnSendNudge(WPARAM wParam, LPARAM lParam);
	LRESULT	OnAndroidSocket(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
