// Misc.cpp: implementation of the CMisc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Misc.h"

#include <Lmcons.h>
#include <math.h>
#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void Misc::CopyTexttoClipboard(const CString& sText, HWND hwnd) 
{
	if (sText.IsEmpty())
		return;

	if (!::OpenClipboard(hwnd)) 
		return; 
	
    ::EmptyClipboard(); 
	
    // Allocate a global memory object for the text. 
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (sText.GetLength() + 1) * sizeof(TCHAR)); 
	
	if (!hglbCopy) 
	{ 
		CloseClipboard(); 
		return; 
	} 
	
	// Lock the handle and copy the text to the buffer. 
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
	
	memcpy(lptstrCopy, (LPVOID)(LPCTSTR)sText, sText.GetLength() * sizeof(TCHAR)); 
	
	lptstrCopy[sText.GetLength()] = (TCHAR) 0;    // null character 
	GlobalUnlock(hglbCopy); 
	
	// Place the handle on the clipboard. 
	::SetClipboardData(CF_TEXT, hglbCopy); 
	
	::CloseClipboard();
}

BOOL Misc::GuidFromString(LPCTSTR szGuid, GUID& guid)
{
#pragma comment(lib, "Rpcrt4.lib")

	RPC_STATUS rpcs = UuidFromString((unsigned char*)szGuid, &guid);
					
	if (rpcs != RPC_S_OK)
	{
		ZeroMemory(&guid, sizeof(GUID));
		return FALSE;
	}

	return TRUE;
}

BOOL Misc::GuidToString(const GUID& guid, CString& sGuid)
{
#pragma comment(lib, "Rpcrt4.lib")

	unsigned char* pszGuid;
	
	if (RPC_S_OK == UuidToString((GUID*)&guid, &pszGuid))
		sGuid = CString(pszGuid);
	else
		sGuid.Empty();
	
	RpcStringFree(&pszGuid);
	
	return !sGuid.IsEmpty();
}

BOOL Misc::GuidIsNull(const GUID& guid)
{
	static GUID NULLGUID = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

   return SameGuids(guid, NULLGUID);
}

BOOL Misc::SameGuids(const GUID& guid1, const GUID& guid2)
{
	return (memcmp(&guid1, &guid2, sizeof(GUID)) == 0);
}

void Misc::NullGuid(GUID& guid)
{
   ZeroMemory(&guid, sizeof(guid));
}

HFONT Misc::CreateFont(LPCTSTR szFaceName, int nPoint, BOOL bStrikeThru)
{
	if (nPoint <= 0)
		return NULL;
	
	HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	
	ASSERT (hDefFont);
	
	LOGFONT lf;
	::GetObject(hDefFont, sizeof(lf), &lf);
	
	// set the charset
	if (!lf.lfCharSet)
		lf.lfCharSet = DEFAULT_CHARSET;
	
	if (szFaceName && *szFaceName)
		lstrcpy(lf.lfFaceName, szFaceName);
	
	HDC hDC = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(abs(nPoint), GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	
	lf.lfWidth = 0;
	lf.lfStrikeOut = (BYTE)bStrikeThru;
	
	HFONT hFont = CreateFontIndirect(&lf);

	// verify the font creation
	if (!SameFont(hFont, szFaceName, nPoint))
	{
		AfxMessageBox("failed to create font");
		DeleteObject(hFont);
		hFont = NULL;
	}
	
	return hFont;
}

int Misc::GetFontNameSize(HFONT hFont, CString& sFaceName)
{
	if (!hFont)
	{
		sFaceName.Empty();
		return 0;
	}
	
	LOGFONT lf;
	::GetObject(hFont, sizeof(lf), &lf);
	
	sFaceName = lf.lfFaceName;
	
	HDC hDC = ::GetDC(NULL);
	int nPoint = MulDiv(abs(lf.lfHeight), 72, GetDeviceCaps(hDC, LOGPIXELSY));
	::ReleaseDC(NULL, hDC);
	
	return nPoint;
}

BOOL Misc::SameFont(HFONT hFont, LPCTSTR szFaceName, int nPoint)
{
	CString sFontName;
	int nFontSize = GetFontNameSize(hFont, sFontName);

	return (nPoint == nFontSize && sFontName.CompareNoCase(szFaceName) == 0);
}

BOOL Misc::SameFontNameSize(HFONT hFont1, HFONT hFont2)
{
	CString sName1;
	int nSize1 = GetFontNameSize(hFont1, sName1);

	return SameFont(hFont2, sName1, nSize1);
}

void Misc::ProcessMsgLoop()
{
	MSG msg;

	while (::PeekMessage((LPMSG) &msg, NULL, 0, 0, PM_REMOVE))
	{
		if (::IsDialogMessage(msg.hwnd, (LPMSG)&msg))
		{
			// do nothing - its already been done
		}
		else
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

int Misc::Split(const CString& sText, char cDelim, CStringArray& aValues)
{
	aValues.RemoveAll();

	int nPrevFind = -1;
	int nFind = sText.Find(cDelim, nPrevFind + 1);

	while (nFind != -1)
	{
		aValues.Add(sText.Mid(nPrevFind + 1, nFind - nPrevFind));

		// next try
		nPrevFind = nFind;
		nFind = sText.Find(cDelim, nPrevFind + 1);
	}

	// add whatever's left
	aValues.Add(sText.Mid(nPrevFind + 1));

	return aValues.GetSize();
}

CString Misc::GetComputerName()
{
	static CString sMachine;

	if (sMachine.IsEmpty())
	{
		DWORD LEN = MAX_COMPUTERNAME_LENGTH + 1;
		
		::GetComputerName(sMachine.GetBuffer(LEN), &LEN);
		sMachine.ReleaseBuffer();
	}

	return sMachine;
}

CString Misc::GetUserName()
{
	static CString sUser;

	if (sUser.IsEmpty())
	{
		DWORD LEN = UNLEN + 1;
		
		::GetUserName(sUser.GetBuffer(LEN), &LEN);
		sUser.ReleaseBuffer();
	}

	return sUser;
}

CString Misc::GetListSeparator()
{
	static CString sSep;
	const int BUFLEN = 10;
	
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, sSep.GetBuffer(BUFLEN), BUFLEN - 1);
	sSep.ReleaseBuffer();
	
	// Trim extra spaces
	sSep.TrimLeft();
	sSep.TrimRight();
	
	// If none found, use a comma
	if (!sSep.GetLength())
		sSep = ',';

	return sSep;
}

CString Misc::FormatArray(const CStringArray& array)
{
	int nCount = array.GetSize();

	if (nCount == 0)
		return "";

	CString sText;

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		if (nItem > 0 && array[nItem].GetLength())
		{
			sText += GetListSeparator();
			sText += ' ';
		}

		sText += array[nItem];
	}

	return sText;
}

void Misc::Trace(const CStringArray& array)
{
	int nCount = array.GetSize();

	for (int nItem = 0; nItem < nCount; nItem++)
		TRACE("%s, ", array[nItem]);

	TRACE("\n");
}

int Misc::ParseIntoArray(const CString& sText, CStringArray& array, BOOL bAllowEmpty)
{
	array.RemoveAll();

	int nSepPrev = -1;
	int nSep = sText.Find(GetListSeparator());
	
	while (nSep != -1)
	{
		CString sItem = sText.Mid(nSepPrev + 1, nSep - (nSepPrev + 1));
		sItem.TrimLeft();
		sItem.TrimRight();
		
		if (bAllowEmpty || !sItem.IsEmpty())
			array.Add(sItem);
		
		nSepPrev = nSep;
		nSep = sText.Find(GetListSeparator(), nSepPrev + 1);
	}
	
	// handle whatever's left so long as a separator was found
	// or there is something left
	if (nSepPrev != -1 || !sText.IsEmpty())
	{
		CString sItem = sText.Mid(nSepPrev + 1);
		sItem.TrimLeft();
		sItem.TrimRight();

		if (bAllowEmpty || !sItem.IsEmpty())
			array.Add(sItem);
	}

	return array.GetSize();
}

BOOL Misc::ArraysMatch(const CStringArray& array1, const CStringArray& array2, BOOL bOrderMatters)
{
	int nSize1 = array1.GetSize();
	int nSize2 = array2.GetSize();

	if (nSize1 != nSize2)
		return 0;

	if (bOrderMatters)
	{
		for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
		{
			// check for non-equality
			if (array1[nItem1] != array2[nItem1])
				return FALSE;
		}

		return TRUE;
	}

	// else order not important
	for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
	{
		BOOL bMatch = FALSE;

		// look for matching item
		for (int nItem2 = 0; nItem2 < nSize2 && !bMatch; nItem2++)
			bMatch = (array1[nItem1] == array2[nItem2]);

		// no-match found == not the same
		if (!bMatch)
			return FALSE;
	}

	return TRUE;
}

BOOL Misc::MatchAny(const CStringArray& array1, const CStringArray& array2) 
{
	int nSize1 = array1.GetSize();
	int nSize2 = array2.GetSize();

	for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
	{
		// look for matching item
		for (int nItem2 = 0; nItem2 < nSize2; nItem2++)
		{
			if (array1[nItem1] == array2[nItem2])
				return TRUE;
		}
	}
	
	return FALSE;
}

int Misc::Find(const CStringArray& array, LPCTSTR szItem, BOOL bCaseMatters)
{
	ASSERT (szItem);
	int nSize = array.GetSize();

	for (int nItem = 0; nItem < nSize; nItem++)
	{
		// look for matching item
		const CString& sItem = array[nItem];

		// special case: empty item
		if (sItem.IsEmpty() && !szItem[0])
			return nItem;

		if (bCaseMatters)
		{
			if (sItem == szItem)
				return nItem;
		}
		else
		{
			if (sItem.CompareNoCase(szItem) == 0)
				return nItem;
		}
	}

	return -1;
}

CFont& Misc::WingDings()
{
	static CFont font;
				
	if (!font.GetSafeHandle())
	{
		LOGFONT lf;
		HFONT hDef = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
					
		if (GetObject(hDef, sizeof(lf), &lf))
		{
			lstrcpy(lf.lfFaceName, "Wingdings");
			lf.lfCharSet = SYMBOL_CHARSET;
			lf.lfQuality = ANTIALIASED_QUALITY;
		}
					
		font.CreateFontIndirect(&lf);
	}

	return font;
}

CString Misc::GetDecimalSeparator()
{
	static CString sSep;
	const int BUFLEN = 10;

	if (sSep.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, sSep.GetBuffer(BUFLEN), BUFLEN - 1);
		sSep.ReleaseBuffer();

		// Trim extra spaces
		sSep.TrimLeft();
		sSep.TrimRight();
		
		// If none found, use a dot
		if (!sSep.GetLength())
			sSep = ".";
	}

	return sSep;
}

BOOL Misc::IsMultibyteString(const CString& sText)
{
	int nChar = sText.GetLength();

	while (nChar--)
	{
		if (IsDBCSLeadByte(sText[nChar]))
			return TRUE;
	}

	return FALSE;
}

CString Misc::WideToMultiByte(wchar_t nChar, UINT nCodePage)
{
	char ach[4]; // assuming 3 bytes max in the multibyte encoding 

	WideCharToMultiByte(nCodePage, 0, &nChar, 1, ach, sizeof(ach), 0, 0); 

	return ach;
}

double Misc::Round(double dValue)
{
	if (dValue > 0)
	{
		if ((dValue - (int)dValue) > 0.5)
			return ceil(dValue);
		else
			return floor(dValue);
	}
	else
	{
		if ((dValue - (int)dValue) > -0.5)
			return floor(dValue);
		else
			return ceil(dValue);
	}
}

float Misc::Round(float fValue)
{
	return (float)Round((double)fValue);
}

double Misc::Atof(const CString& sValue)
{
	// needs special care to handle decimal point properly
	// especially since we've no way of knowing how it is encoded.
	// so we assume that if a period is present then it's encoded
	// in 'english' else it's in native format
	char* szLocale = strdup(setlocale(LC_NUMERIC, NULL));

	if (sValue.Find('.') != -1)
		setlocale(LC_NUMERIC, "English");
	else
		setlocale(LC_NUMERIC, "");

	double dVal = (double)atof(sValue);

	// restore locale
	setlocale(LC_NUMERIC, szLocale);
	free(szLocale);
	
	return dVal; 
}

BOOL Misc::IsWorkStationLocked()
{
	// note: we can't call OpenInputDesktop directly because it's not
	// available on win 9x
	typedef HDESK (WINAPI *PFNOPENDESKTOP)(LPSTR lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
	typedef BOOL (WINAPI *PFNCLOSEDESKTOP)(HDESK hDesk);
	typedef BOOL (WINAPI *PFNSWITCHDESKTOP)(HDESK hDesk);

	// load user32.dll once only
	static HMODULE hUser32 = LoadLibrary("user32.dll");

	if (hUser32)
	{
		static PFNOPENDESKTOP fnOpenDesktop = (PFNOPENDESKTOP)GetProcAddress(hUser32, "OpenDesktopA");
		static PFNCLOSEDESKTOP fnCloseDesktop = (PFNCLOSEDESKTOP)GetProcAddress(hUser32, "CloseDesktop");
		static PFNSWITCHDESKTOP fnSwitchDesktop = (PFNSWITCHDESKTOP)GetProcAddress(hUser32, "SwitchDesktop");

		if (fnOpenDesktop && fnCloseDesktop && fnSwitchDesktop)
		{
			HDESK hDesk = fnOpenDesktop("Default", 0, FALSE, DESKTOP_SWITCHDESKTOP);

			if (hDesk)
			{
				BOOL bLocked = !fnSwitchDesktop(hDesk);
					
				// cleanup
				fnCloseDesktop(hDesk);

				return bLocked;
			}
		}
	}

	// must be win9x
	return FALSE;
}

#ifndef SPI_GETSCREENSAVERRUNNING
#  define SPI_GETSCREENSAVERRUNNING 114
#endif

BOOL Misc::IsScreenSaverActive()
{
	BOOL bSSIsRunning = FALSE;
	::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &bSSIsRunning, 0);
	
	return bSSIsRunning; 
}

int Misc::CompareVersions(LPCTSTR szVersion1, LPCTSTR szVersion2)
{
	CStringArray aVer1, aVer2;

	Misc::Split(szVersion1, '.', aVer1);
	Misc::Split(szVersion2, '.', aVer2);

	// compare starting from the front
	for (int nItem = 0; nItem < aVer1.GetSize() && nItem < aVer2.GetSize(); nItem++)
	{
		int nThis = atoi(aVer1[nItem]);
		int nOther = atoi(aVer2[nItem]);

		if (nThis < nOther)
			return -1;

		else if (nThis > nOther)
			return 1;

		// else try next item
	}

	// if we got here then compare array lengths
	if (aVer1.GetSize() < aVer2.GetSize())
		return -1;

	else if (aVer1.GetSize() > aVer2.GetSize())
		return 1;

	// else
	return 0;
}

int AFX_CDECL Misc::GetTextWidth(CDC* pDC, LPCTSTR lpszFormat, ...)
{
	static char BUFFER[2048];

	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	_vstprintf(BUFFER, lpszFormat, argList);
	va_end(argList);

	return pDC->GetTextExtent(BUFFER).cx;
}

CString Misc::GetDefCharset()
{
	CString sDefCharset;
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, sDefCharset.GetBuffer(7), 6);
	sDefCharset.ReleaseBuffer();

	return sDefCharset;
}

float Misc::GetAverageCharWidth(CDC* pDC)
{
	ASSERT(pDC);
	
	int nExtent = pDC->GetTextExtent("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz").cx;
	
	return (nExtent / 52.0f);
}

