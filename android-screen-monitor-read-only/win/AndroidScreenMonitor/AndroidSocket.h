#pragma once

#define WM_USER_ANDROID_SOCKET		(WM_USER + 10000)

// WPARAM for WM_USER_ANDROID_SOCKET
#define AS_WPARAM_POST_TRACK_DEVICES	1
#define AS_WPARAM_POST_TRANSPORT		2
#define AS_WPARAM_POST_FRAME_BUFFER		3
#define AS_WPARAM_POST_NUDGE			4

class CAndroidSocket : public CSocket
{
public:
					CAndroidSocket(CWnd* pWnd);
	virtual			~CAndroidSocket();

	CStringArray&	GetDeviceSerialNumbers() const {return (CStringArray&)m_DeviceSerialNumbers;} 

	BITMAPINFO*		GetBitmapInfo()			const {return m_pBitmapInfo;}
	DWORD			GetFrameBufferLength()	const {return m_FrameBufLen;}
	BYTE*			GetFrameBuffer()		const {return m_pFrameBuf;}

	BOOL			SendTrackDevices();
	BOOL			SendTransport(const CString& serialNumber);
	BOOL			SendFrameBuffer();
	BOOL			SendNudge();

	LRESULT			OnAndroidSocket(WPARAM wParam, LPARAM lParam);

private:
	BOOL			ResetRecvBuf(DWORD size);

	BOOL			ResetConnect();

	CString			GetAdbRequest(const CString& request) const;
	int				SendAdbRequest(int sendID, const CString& request);
	BOOL			SendString(int sendID, CString string);

	virtual void	OnReceive(int nErrorCode);

	void			OnRecvTrackDevices();
	void			OnRecvTransport();
	void			OnRecvFrameBuffer();
	void			OnRecvNudge();

	BOOL			IsOKAY(const BYTE reply[]) const;
	int				GetInt(const BYTE reply[]) const;

	CWnd*			m_pWnd;
	CStringArray	m_DeviceSerialNumbers;
	int				m_SendID;
	DWORD			m_ReadOffset;
	DWORD			m_TotalRecvLen;
	DWORD			m_RecvBufLen;
	BYTE*			m_pRecvBuf;
	BITMAPINFO*		m_pBitmapInfo;
	DWORD			m_FrameBufLen;
	BYTE*			m_pFrameBuf;
};
