// AndroidScreenMonitor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "GdiPlusUtil.h"

// CAndroidScreenMonitorApp:
// See AndroidScreenMonitor.cpp for the implementation of this class
//

class CAndroidScreenMonitorApp : public CWinApp
{
private:
	CGdiPlusToken	m_GdiPlusToken;

public:
	CAndroidScreenMonitorApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAndroidScreenMonitorApp theApp;