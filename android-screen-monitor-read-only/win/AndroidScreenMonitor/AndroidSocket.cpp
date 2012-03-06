#include "stdafx.h"
#include "AndroidSocket.h"

#define SEND_ID_UNKOWN					(0)
#define SEND_ID_TRACK_DEVICES			(1)
#define SEND_ID_TRANSPORT				(2)
#define SEND_ID_FRAME_BUFFER			(3)
#define SEND_ID_0						(4)

#define ADB_ADDR						_T("127.0.0.1")
#define ADB_PORT						(5037)

#define RECV_BUF_SIZE					(320 * 480 * 2)

#define RECV_SIZE_FRAME_BUFFER			(20)

CAndroidSocket::CAndroidSocket(CWnd* pWnd)
{
	TRACE("CAndroidSocket::CAndroidSocket()\n");

	m_pWnd = pWnd;
	m_DeviceSerialNumbers.RemoveAll();
	m_SendID = SEND_ID_UNKOWN;
	m_ReadOffset = 0;
	m_TotalRecvLen = 0;
	m_RecvBufLen = RECV_BUF_SIZE;
	m_pRecvBuf = (BYTE*)GlobalAlloc(GPTR, m_RecvBufLen);
	m_pBitmapInfo = NULL;
	m_FrameBufLen = 0;
	m_pFrameBuf = NULL;
}
	
CAndroidSocket::~CAndroidSocket()
{
	TRACE("CAndroidSocket::~CAndroidSocket()\n");

	Close();

	if (m_pFrameBuf)
	{
		GlobalFree(m_pFrameBuf);
		m_pFrameBuf = NULL;
	}

	if (m_pBitmapInfo)
	{
		GlobalFree(m_pBitmapInfo);
		m_pBitmapInfo = NULL;
	}

	if (m_pRecvBuf)
	{
		GlobalFree(m_pRecvBuf);
		m_pRecvBuf = NULL;
	}
}

LRESULT CAndroidSocket::OnAndroidSocket(WPARAM wParam, LPARAM lParam)
{
	TRACE("CAndroidSocket::OnAndroidSocket()\n");

	switch (wParam)
	{
	case AS_WPARAM_POST_TRACK_DEVICES:
		{
		}
		break;
	case AS_WPARAM_POST_TRANSPORT:
		{
			SendFrameBuffer();
		}
		break;
	case AS_WPARAM_POST_FRAME_BUFFER:
		{
			SendNudge();
		}
		break;
	case AS_WPARAM_POST_NUDGE:
		{
		}
		break;
	default:
		{
		}
		break;
	}

	return 0;
}

BOOL CAndroidSocket::ResetRecvBuf(DWORD size)
{
	TRACE("CAndroidSocket::ResetRecvBuf()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		if (m_pRecvBuf)
		{
			GlobalFree(m_pRecvBuf);
			m_pRecvBuf = NULL;
		}

		m_RecvBufLen = size;

		m_pRecvBuf = (BYTE*)GlobalAlloc(GPTR, m_RecvBufLen);

		if (m_pRecvBuf == NULL)
		{
			m_RecvBufLen = 0;
			bSuccess = FALSE;
		}
	}

	return bSuccess;
}

BOOL CAndroidSocket::ResetConnect()
{
	TRACE("CAndroidSocket::ResetConnect()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		Close();

Sleep(100);

		bSuccess = Create();
	}

	if (bSuccess)
	{
		bSuccess = Connect(ADB_ADDR, ADB_PORT);
	}

	if (!bSuccess)
	{
		Close();
	}

	return bSuccess;
}

BOOL CAndroidSocket::SendTrackDevices()
{
	TRACE("CAndroidSocket::SendTrackDevices()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = ResetConnect();
	}

	if (bSuccess)
	{
		bSuccess = SendAdbRequest(SEND_ID_TRACK_DEVICES, _T("host:track-devices"));
	}

	return bSuccess;
}

BOOL CAndroidSocket::SendTransport(const CString& serialNumber)
{
	TRACE("CAndroidSocket::SendTransport()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = ResetConnect();
	}

	if (bSuccess)
	{
		bSuccess = SendAdbRequest(SEND_ID_TRANSPORT, _T("host:transport:") + serialNumber);
	}

	return bSuccess;
}

BOOL CAndroidSocket::SendFrameBuffer()
{
	TRACE("CAndroidSocket::SendFrameBuffer()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = SendAdbRequest(SEND_ID_FRAME_BUFFER, _T("framebuffer:"));
	}

	return bSuccess;
}

BOOL CAndroidSocket::SendNudge()
{
	TRACE("CAndroidSocket::SendNudge()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		bSuccess = SendString(SEND_ID_0, _T("0"));
	}

	return bSuccess;
}

CString CAndroidSocket::GetAdbRequest(const CString& request) const
{
	TRACE("CAndroidSocket::GetAdbRequest()\n");

	CString adbRequest = _T("");

	adbRequest.Format(_T("%04X%s"), request.GetLength(), request);

	return adbRequest;
}

BOOL CAndroidSocket::SendAdbRequest(int sendID, const CString& request)
{
	TRACE("CAndroidSocket::SendAdbRequest()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		CString	adbRequest = GetAdbRequest(request);

		bSuccess = SendString(sendID, adbRequest);
	}

	return bSuccess;
}

BOOL CAndroidSocket::SendString(int sendID, CString string)
{
	TRACE("CAndroidSocket::SendString()\n");

	BOOL bSuccess = TRUE;

	if (bSuccess)
	{
		if (string.IsEmpty())
		{
			bSuccess = FALSE;
		}
	}

	if (bSuccess)
	{
		USES_CONVERSION;

		LPCSTR pszString = T2A(string);
		size_t stringLength = strlen(pszString);

		if (Send((void*)pszString, (int)stringLength) != stringLength)
		{
			bSuccess = FALSE;
		}
	}

	if (bSuccess)
	{
		m_SendID = sendID;
	}
	else
	{
		m_SendID = SEND_ID_UNKOWN;
	}

	m_ReadOffset = 0;
	m_TotalRecvLen	= 0;

//Sleep(100);

	return bSuccess;
}

void CAndroidSocket::OnReceive(int nErrorCode)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	TRACE("CAndroidSocket::OnReceive()\n");

	BOOL bSuccess = TRUE;
	DWORD recvLen = 0;

	if (bSuccess)
	{
		if (!IOCtl(FIONREAD, &recvLen))
		{
			bSuccess = FALSE;
		}
	}

	if (bSuccess)
	{
		if ((recvLen + m_TotalRecvLen) > m_RecvBufLen)
		{
			bSuccess = ResetRecvBuf(recvLen + m_TotalRecvLen);
		}
	}

	if (bSuccess)
	{
		if (Receive(m_pRecvBuf + m_TotalRecvLen, recvLen) == recvLen)
		{
			m_TotalRecvLen += recvLen;
		}
		else
		{
			bSuccess = FALSE;
		}
	}
	
	if (bSuccess)
	{
		switch (m_SendID)
		{
		case SEND_ID_TRACK_DEVICES:
			{
				OnRecvTrackDevices();
			}
			break;
		case SEND_ID_TRANSPORT:
			{
				OnRecvTransport();
			}
			break;
		case SEND_ID_FRAME_BUFFER:
			{
				OnRecvFrameBuffer();
			}
			break;
		case SEND_ID_0:
			{
				OnRecvNudge();
			}
			break;
		}
	}

	CSocket::OnReceive(nErrorCode);
}

void CAndroidSocket::OnRecvTrackDevices()
{
	TRACE("CAndroidSocket::OnRecvTrackDevices()\n");

	BOOL bSuccess = TRUE;
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		switch (m_ReadOffset)
		{
		case 0:
			{
				if (m_TotalRecvLen >= (0 + 4))
				{
					m_ReadOffset += 4;

					bSuccess = IsOKAY(&m_pRecvBuf[0]);
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		case 4:
			{
				if (m_TotalRecvLen >= (4 + 4))
				{
					m_ReadOffset += 4;
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		case 8:
			{
				DWORD msgLen = 0;
				CHAR lenBuf[5];

				ZeroMemory(lenBuf, sizeof(lenBuf));
				CopyMemory(lenBuf, &m_pRecvBuf[4], 4);
				sscanf_s(lenBuf, "%04X", &msgLen);

				if (m_TotalRecvLen >= (8 + msgLen))
				{
					m_ReadOffset += msgLen;

					CString aDevice = _T("");
					CStringArray devices;

					for (DWORD i = 0; i < msgLen; i++)
					{
						if (m_pRecvBuf[8 + i] != '\n')
						{
							aDevice += m_pRecvBuf[8 + i];
						}
						else
						{
							devices.Add(aDevice);
							aDevice = "";
						}
					}

					m_DeviceSerialNumbers.RemoveAll();

					for (int i = 0; i < devices.GetCount(); i++)
					{
						const CString& aDevice = devices.GetAt(i);
						CString deviceSerialNumber = _T("");

						for (int j = 0; j < aDevice.GetLength(); j++)
						{
							if (aDevice.GetAt(j) != '\t')
							{
								deviceSerialNumber += aDevice.GetAt(j);
							}
							else
							{
								m_DeviceSerialNumbers.Add(deviceSerialNumber);
								deviceSerialNumber = "";
								break;
							}
						}
					}

					if (bSuccess)
					{
						m_pWnd->PostMessage(WM_USER_ANDROID_SOCKET, AS_WPARAM_POST_TRACK_DEVICES, 0);
					}

					bContinue = FALSE;
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		default:
			{
				bSuccess = FALSE;
			}
			break;
		}

		if (!bSuccess)
		{
			bContinue = FALSE;
		}
	}

	if (!bSuccess)
	{
		TRACE("CAndroidSocket::OnRecvTrackDevices() Error\n");
	}
}

void CAndroidSocket::OnRecvTransport()
{
	TRACE("CAndroidSocket::OnRecvTransport()\n");

	BOOL bSuccess = TRUE;
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		switch (m_ReadOffset)
		{
		case 0:
			{
				if (m_TotalRecvLen >= (0 + 4))
				{
					m_ReadOffset += 4;

					bSuccess = IsOKAY(&m_pRecvBuf[0]);

					if (bSuccess)
					{
						m_pWnd->PostMessage(WM_USER_ANDROID_SOCKET, AS_WPARAM_POST_TRANSPORT, 0);
					}

					bContinue = FALSE;
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		default:
			{
				bSuccess = FALSE;
			}
			break;
		}

		if (!bSuccess)
		{
			bContinue = FALSE;
		}
	}
	
	if (!bSuccess)
	{
		TRACE("CAndroidSocket::OnRecvTransport() Error\n");
	}
}

void CAndroidSocket::OnRecvFrameBuffer()
{
	TRACE("CAndroidSocket::OnRecvFrameBuffer()\n");

	BOOL bSuccess = TRUE;
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		switch (m_ReadOffset)
		{
		case 0:
			{
				if (m_TotalRecvLen >= (0 + 4))
				{
					m_ReadOffset += 4;

					bSuccess = IsOKAY(&m_pRecvBuf[0]);
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		case 4:
			{
				if (m_TotalRecvLen >= (4 + 16))
				{
					m_ReadOffset += 16;

					int bpp			= GetInt(&m_pRecvBuf[4]);
					int imageSize	= GetInt(&m_pRecvBuf[8]);
					int imageWidth	= GetInt(&m_pRecvBuf[12]);
					int imageHeight	= GetInt(&m_pRecvBuf[16]);

					if ((bpp			!= 16)	||
						(imageSize		<= 0)	||
						(imageWidth		<= 0)	||
						(imageHeight	<= 0))
					{
						bSuccess = FALSE;
					}

					if (bSuccess)
					{
						DWORD bitmapInfoSize = sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 3; // 3 = bit mask

						if (m_pBitmapInfo == NULL)
						{
							m_pBitmapInfo = (BITMAPINFO*)GlobalAlloc(GPTR, bitmapInfoSize);	
						}

						if ((bpp			!= m_pBitmapInfo->bmiHeader.biBitCount	) ||
							(imageSize		!= m_pBitmapInfo->bmiHeader.biSizeImage	) ||
							(imageWidth		!= m_pBitmapInfo->bmiHeader.biWidth		) ||
							(-imageHeight	!= m_pBitmapInfo->bmiHeader.biHeight	))
						{
							if (m_pFrameBuf)
							{
								GlobalFree(m_pFrameBuf);
								m_pFrameBuf = NULL;
							}

//							ZeroMemory(m_pBitmapInfo, bitmapInfoSize);
							m_pBitmapInfo->bmiHeader.biSize				= bitmapInfoSize;
							m_pBitmapInfo->bmiHeader.biWidth			= imageWidth;
							m_pBitmapInfo->bmiHeader.biHeight			= -imageHeight;	// Top-Down
							m_pBitmapInfo->bmiHeader.biPlanes			= 1;
							m_pBitmapInfo->bmiHeader.biBitCount			= bpp;
							m_pBitmapInfo->bmiHeader.biCompression		= BI_BITFIELDS;
							m_pBitmapInfo->bmiHeader.biSizeImage		= imageSize;
							m_pBitmapInfo->bmiHeader.biXPelsPerMeter	= 0;
							m_pBitmapInfo->bmiHeader.biYPelsPerMeter	= 0;
							m_pBitmapInfo->bmiHeader.biClrUsed			= 0;
							m_pBitmapInfo->bmiHeader.biClrImportant		= 0;
							*((DWORD*)&m_pBitmapInfo->bmiColors[0])		= 0xF800;	// Red		5bit
							*((DWORD*)&m_pBitmapInfo->bmiColors[1])		= 0x07E0;	// Green	6bit
							*((DWORD*)&m_pBitmapInfo->bmiColors[2])		= 0x001F;	// Blue		5bit

							m_FrameBufLen = imageSize;
							m_pFrameBuf = (BYTE*)GlobalAlloc(GPTR, m_FrameBufLen);

							if (m_pFrameBuf == NULL)
							{
								m_FrameBufLen = 0;
								bSuccess = FALSE;
							}
						}
					}

					if (bSuccess)
					{
						if (m_FrameBufLen > m_RecvBufLen)
						{
							bSuccess = ResetRecvBuf(m_FrameBufLen);
						}
					}

					if (bSuccess)
					{
						m_pWnd->PostMessage(WM_USER_ANDROID_SOCKET, AS_WPARAM_POST_FRAME_BUFFER, 0);
					}

					bContinue = FALSE;
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		default:
			{
				bSuccess = FALSE;
			}
			break;
		}

		if (!bSuccess)
		{
			bContinue = FALSE;
		}
	}

	if (!bSuccess)
	{
		TRACE("CAndroidSocket::OnRecvFrameBuffer() Error\n");
	}
}

void CAndroidSocket::OnRecvNudge()
{
	TRACE("CAndroidSocket::OnRecvNudge()\n");

	BOOL bSuccess = TRUE;
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		switch (m_ReadOffset)
		{
		case 0:
			{
				if (m_TotalRecvLen >= (0 + m_FrameBufLen))
				{
					m_ReadOffset += m_FrameBufLen;

					CopyMemory(m_pFrameBuf, m_pRecvBuf, m_FrameBufLen);

					m_pWnd->PostMessage(WM_USER_ANDROID_SOCKET, AS_WPARAM_POST_NUDGE, 0);
//					m_pWnd->SendMessage(WM_USER_ANDROID_SOCKET, AS_WPARAM_POST_NUDGE, 0);
					
					bContinue = FALSE;
				}
				else
				{
					bContinue = FALSE; // There are not enough data at now, but it's not error. so wait for next coming data
				}
			}
			break;
		default:
			{
				bSuccess = FALSE;
			}
			break;
		}

		if (!bSuccess)
		{
			bContinue = FALSE;
		}
	}

	if (!bSuccess)
	{
		TRACE("CAndroidSocket::OnRecvNudge() Error\n");
	}
}

BOOL CAndroidSocket::IsOKAY(const BYTE reply[]) const
{
	TRACE("CAndroidSocket::IsOKAY()\n");
	
	return ((reply[0] == (BYTE)'O') &&
			(reply[1] == (BYTE)'K') &&
			(reply[2] == (BYTE)'A') &&
			(reply[3] == (BYTE)'Y'));
}

int CAndroidSocket::GetInt(const BYTE reply[]) const
{
	TRACE("CAndroidSocket::GetInt()\n");
	
	return (reply[0] | (reply[1] << 8) | (reply[2] << 16) | (reply[3] << 24));
}
