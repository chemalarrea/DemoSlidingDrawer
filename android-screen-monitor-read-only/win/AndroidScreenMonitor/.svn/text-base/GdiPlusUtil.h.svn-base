#ifndef __GDIPLUSUTIL_H__
#define __GDIPLUSUTIL_H__

#include "GdiPlus.h"

using namespace Gdiplus;

#pragma comment(lib, "GdiPlus.lib")

class CGdiPlusToken
{
public:
					CGdiPlusToken();

	virtual			~CGdiPlusToken();
	
	Status			Initialize();

	void			Destroy();

protected:
	ULONG_PTR		m_GdiPlusToken;
};

int GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid);

#endif //__GDIPLUSUTIL_H__
