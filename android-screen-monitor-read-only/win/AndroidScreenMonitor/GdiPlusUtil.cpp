#include "stdafx.h"
#include "GdiPlusUtil.h"

CGdiPlusToken::CGdiPlusToken()
:
m_GdiPlusToken(NULL)
{
}

CGdiPlusToken::~CGdiPlusToken()
{
	Destroy();
}

Status
CGdiPlusToken::Initialize()
{
	Status				status = Ok;
	GdiplusStartupInput	startupInput;

	status = GdiplusStartup(&m_GdiPlusToken, &startupInput, NULL);

	return status;
}

void
CGdiPlusToken::Destroy()
{
	if (m_GdiPlusToken)
	{
		GdiplusShutdown(m_GdiPlusToken);
		m_GdiPlusToken = NULL;
	}
}

int GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid)
{
	BOOL			bSuccess		= TRUE;
	UINT			num				= 0;
	UINT			size			= 0;
	ImageCodecInfo*	pImageCodecInfo	= NULL;
	int				ret				= -1;

	if (bSuccess)
	{
		if ((GetImageEncodersSize(&num, &size) != Ok)	||
			(num	== 0)								||
			(size	== 0))
		{
			bSuccess = FALSE;
		}
	}

	if (bSuccess)
	{
		if ((pImageCodecInfo = (ImageCodecInfo*)(GlobalAlloc(GPTR, size))) == NULL)
		{
			bSuccess = FALSE;
		}
	}

	if (bSuccess)
	{
		if (GetImageEncoders(num, size, pImageCodecInfo) != Ok)
		{
			bSuccess = FALSE;
		}
	}

	if (bSuccess)
	{	
		for (UINT i = 0; i < num; ++i)
		{
			if (wcscmp(pImageCodecInfo[i].MimeType, pszFormat) == 0)
			{
				*pClsid = pImageCodecInfo[i].Clsid;
				ret = i;
				break;
			} 
		}
	}

	if (pImageCodecInfo)
	{
		GlobalFree(pImageCodecInfo);
		pImageCodecInfo = NULL;
	}

	return ret;
}
