// AndroidScreenMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AndroidScreenMonitor.h"
#include "AndroidScreenMonitorDlg.h"

#include "ADBCDlg.h"
#include "SelectDeviceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_ASM_SHOW_ADBC				(WM_USER + 1)
#define WM_USER_ASM_SHOW_SELECT_DEVICE		(WM_USER + 2)
#define WM_USER_ASM_TRANSPORT				(WM_USER + 3)
#define WM_USER_ASM_SEND_NUDGE				(WM_USER + 4)

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnBnClickedButtonAccess();
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ACCESS, &CAboutDlg::OnBnClickedButtonAccess)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedButtonAccess()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	ShellExecute(NULL, _T("open"), _T("http://www.adakoda.com/adakoda/"), _T(""), _T(""), SW_SHOWNORMAL);
}


// CAndroidScreenMonitorDlg dialog




CAndroidScreenMonitorDlg::CAndroidScreenMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAndroidScreenMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pAndroidSocket = NULL;
	m_aSerialNumber = _T("");
	m_FrameBufLen = 0;
	m_pFrameBuf = NULL;
	m_FrameWidth = 0;
	m_FrameHeight = 0;

	m_InvalidateSize = TRUE;
	m_Scale = 1.0f;
	m_Rotation = FALSE;
	m_Saving = FALSE;
}

void CAndroidScreenMonitorDlg::ShowADBC()
{
	PostMessage(WM_USER_ASM_SHOW_ADBC);
}

void CAndroidScreenMonitorDlg::ShowSelectDevice()
{
	PostMessage(WM_USER_ASM_SHOW_SELECT_DEVICE);
}

void CAndroidScreenMonitorDlg::Transport()
{
	PostMessage(WM_USER_ASM_TRANSPORT);
}

void CAndroidScreenMonitorDlg::SendNudge()
{
	PostMessage(WM_USER_ASM_SEND_NUDGE);
}

void CAndroidScreenMonitorDlg::Zoom(float scale)
{
	m_Scale = scale;

	m_InvalidateSize = TRUE;
	Invalidate();
}

void CAndroidScreenMonitorDlg::Rotate()
{
	m_Rotation = !m_Rotation;

	m_InvalidateSize = TRUE;
	Invalidate();
}

BOOL CAndroidScreenMonitorDlg::DestroyWindow()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	if (m_pFrameBuf)
	{
		GlobalFree(m_pFrameBuf);
		m_pFrameBuf = NULL;
	}

	if (m_pAndroidSocket)
	{
		delete m_pAndroidSocket;
		m_pAndroidSocket = NULL;
	}

	return CDialog::DestroyWindow();
}

void CAndroidScreenMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAndroidScreenMonitorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_SAVE, &OnFileSave)

	ON_MESSAGE(WM_USER_ASM_SHOW_ADBC, OnShowADBC)
	ON_MESSAGE(WM_USER_ASM_SHOW_SELECT_DEVICE, OnShowSelectDevice)
	ON_MESSAGE(WM_USER_ASM_TRANSPORT, OnTransport)
	ON_MESSAGE(WM_USER_ASM_SEND_NUDGE, OnSendNudge)
	ON_MESSAGE(WM_USER_ANDROID_SOCKET, OnAndroidSocket)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_ZOOM_1, &CAndroidScreenMonitorDlg::OnViewZoom1)
	ON_COMMAND(ID_VIEW_ZOOM_2, &CAndroidScreenMonitorDlg::OnViewZoom2)
	ON_COMMAND(ID_VIEW_ZOOM_5, &CAndroidScreenMonitorDlg::OnViewZoom5)
	ON_COMMAND(ID_VIEW_ZOOM_7, &CAndroidScreenMonitorDlg::OnViewZoom7)
	ON_COMMAND(ID_VIEW_ROTATE, &CAndroidScreenMonitorDlg::OnViewRotate)
END_MESSAGE_MAP()


// CAndroidScreenMonitorDlg message handlers

BOOL CAndroidScreenMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_hAccelTable = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	ShowADBC();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAndroidScreenMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

BOOL CAndroidScreenMonitorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	if (m_hAccelTable)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAndroidScreenMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting

		if (m_pAndroidSocket)
		{
			if (m_pFrameBuf)
			{
				int w = m_FrameWidth;
				int h = m_FrameHeight;

				{
					Graphics graphics(dc.GetSafeHdc());
					
					graphics.SetInterpolationMode(InterpolationModeHighQuality);
					graphics.SetSmoothingMode(SmoothingModeHighQuality);

					Matrix matrix;

					matrix.Scale(m_Scale, m_Scale);
					
					if (m_Rotation)
					{
						matrix.Translate(0.0f, (float)w);
						matrix.Rotate(-90.0f);
					}

					graphics.SetTransform(&matrix);
					
					Bitmap bitmap(w, h, w * 2, PixelFormat16bppRGB565, m_pFrameBuf);

					Point pt = Point(0, 0);

					graphics.DrawImage(&bitmap, pt);
				}

				if (m_InvalidateSize)
				{
					if (m_Rotation)
					{
						int tmp = w;
						w = h;
						h = tmp;
					}

					CRect clientRect = CRect(0, 0, (int)(w * m_Scale), (int)(h * m_Scale));

					AdjustWindowRect(clientRect, GetStyle(), FALSE);

					WINDOWPLACEMENT wp;

					ZeroMemory(&wp, sizeof(wp));

					GetWindowPlacement(&wp);
					wp.rcNormalPosition.right = wp.rcNormalPosition.left + clientRect.Width();
					wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + clientRect.Height();
					SetWindowPlacement(&wp);

					m_InvalidateSize = FALSE;
				}
			}
		}
		else
		{
			CDialog::OnPaint();
		}
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAndroidScreenMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CAndroidScreenMonitorDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	BOOL bRet = TRUE;

	if ((m_pAndroidSocket) && (m_pAndroidSocket->GetBitmapInfo()))
	{
		bRet = FALSE;
	}
	else
	{
		bRet = CDialog::OnEraseBkgnd(pDC);
	}

	return bRet;
}

void CAndroidScreenMonitorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	Rotate();

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CAndroidScreenMonitorDlg::OnFileSave()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	m_Saving = TRUE;

	CString filter("PNG (*.png)|*.png||");

	CFileDialog dlg(FALSE, _T("png"), NULL, OFN_OVERWRITEPROMPT, filter);

	if (dlg.DoModal() == IDOK)
	{
		Bitmap src(m_FrameWidth, m_FrameHeight, m_FrameWidth * 2, PixelFormat16bppRGB565, m_pFrameBuf);

		int dstWidth = (int)(src.GetWidth() * m_Scale);
		int dstHeight = (int)(src.GetHeight() * m_Scale);

		if (m_Rotation)
		{
			int tmp = dstWidth;
			dstWidth = dstHeight;
			dstHeight = tmp;
		}

		Bitmap dst(dstWidth, dstHeight, dstWidth * 2, PixelFormat16bppRGB565, NULL);

		Graphics graphics(&dst);

		graphics.SetInterpolationMode(InterpolationModeHighQuality);
		graphics.SetSmoothingMode(SmoothingModeHighQuality);

		Matrix matrix;

		matrix.Scale(m_Scale, m_Scale);

		if (m_Rotation)
		{
			matrix.Translate(0.0f, (float)src.GetWidth());
			matrix.Rotate(-90.0f);
		}

		graphics.SetTransform(&matrix);

		Point pt = Point(0, 0);

		graphics.DrawImage(&src, pt);

		CLSID pngClsid;

		GetEncoderClsid(L"image/png", &pngClsid);

		dst.Save(dlg.GetFileName(), &pngClsid, NULL);
	}

	m_Saving = FALSE;
}

void CAndroidScreenMonitorDlg::OnViewZoom1()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	Zoom(1.0f);
}

void CAndroidScreenMonitorDlg::OnViewZoom2()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	Zoom(2.0f);
}

void CAndroidScreenMonitorDlg::OnViewZoom5()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	Zoom(0.5f);
}

void CAndroidScreenMonitorDlg::OnViewZoom7()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	Zoom(0.75f);
}

void CAndroidScreenMonitorDlg::OnViewRotate()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	Rotate();
}

LRESULT CAndroidScreenMonitorDlg::OnShowADBC(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	CADBCDlg dlg(this);

	if (dlg.DoModal() == IDOK)
	{
		ShowSelectDevice();
	}
	else
	{
		EndDialog(IDCANCEL);
	}

	return lResult;
}

LRESULT CAndroidScreenMonitorDlg::OnShowSelectDevice(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	CSelectDeviceDlg dlg(this);

	if (dlg.DoModal() == IDOK)
	{
		m_aSerialNumber= dlg.GetSerialNumber();

		if (m_aSerialNumber.IsEmpty())
		{
			EndDialog(IDCANCEL);
		}
		else
		{
			Transport();
		}
	}
	else
	{
		EndDialog(IDCANCEL);
	}

	return lResult;
}

LRESULT CAndroidScreenMonitorDlg::OnTransport(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	BOOL bSuccess = TRUE;

	try
	{
		if (m_pAndroidSocket == NULL)
		{
			m_pAndroidSocket = new CAndroidSocket((CWnd*)this);
		}

		bSuccess = m_pAndroidSocket->SendTransport(m_aSerialNumber);
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
		ShowADBC();
	}

	return lResult;
}

LRESULT CAndroidScreenMonitorDlg::OnSendNudge(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	BOOL bSuccess = TRUE;

	try
	{
		if (m_pAndroidSocket == NULL)
		{
			m_pAndroidSocket = new CAndroidSocket((CWnd*)this);
		}

		bSuccess = m_pAndroidSocket->SendNudge();
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
		ShowADBC();
	}

	return lResult;
}

LRESULT CAndroidScreenMonitorDlg::OnAndroidSocket(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;

	if (m_pAndroidSocket)
	{
/*
		switch (wParam)
		{
		case AS_WPARAM_POST_TRACK_DEVICES:
			{
			}
			break;
		case AS_WPARAM_POST_TRANSPORT:
			{
			}
			break;
		case AS_WPARAM_POST_FRAME_BUFFER:
			{
			}
			break;
		case AS_WPARAM_POST_NUDGE:
*/
			if (wParam == AS_WPARAM_POST_NUDGE)
			{
				if ((m_pFrameBuf == NULL) ||
					(m_pAndroidSocket->GetFrameBufferLength() != m_FrameBufLen))
				{
					if (m_pFrameBuf)
					{
						GlobalFree(m_pFrameBuf);
						m_pFrameBuf = NULL;
					}

					m_FrameBufLen = m_pAndroidSocket->GetFrameBufferLength();
					m_pFrameBuf = (BYTE*)GlobalAlloc(GPTR, m_FrameBufLen);
				}

				if (m_pFrameBuf)
				{
					if (!m_Saving)
					{
						BITMAPINFO* pBitmapInfo = m_pAndroidSocket->GetBitmapInfo();

						if (pBitmapInfo)
						{
							m_FrameWidth = pBitmapInfo->bmiHeader.biWidth;
							m_FrameHeight = abs(pBitmapInfo->bmiHeader.biHeight);
						}

						if (m_pFrameBuf)
						{	
							CopyMemory(m_pFrameBuf, m_pAndroidSocket->GetFrameBuffer(), m_FrameBufLen);
						}
					}

					Invalidate();

					SendNudge();
				}
			}
/*
			break;
		default:
			{
			}
			break;
		}
*/
		lResult = m_pAndroidSocket->OnAndroidSocket(wParam, lParam);
	}

	return lResult;
}
