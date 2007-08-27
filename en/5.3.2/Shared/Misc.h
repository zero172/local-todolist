// Misc.h: interface for the CMisc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISC_H__4B2FDA3E_63C5_4F52_A139_9512105C3AD4__INCLUDED_)
#define AFX_MISC_H__4B2FDA3E_63C5_4F52_A139_9512105C3AD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

enum { MFS_BOLD = 0x01, MFS_ITALIC = 0x02, MFS_UNDERLINED = 0x04, MFS_STRIKETHRU = 0x08, MFS_SYMBOL = 0x10};
enum { MKS_CTRL = 0x01, MKS_SHIFT = 0x02, MKS_ALT = 0x04 };

namespace Misc  
{
	void CopyTexttoClipboard(const CString& sText, HWND hwnd); 
	
	BOOL IsMultibyteString(const CString& sText);
	CString WideToMultiByte(wchar_t nChar, UINT nCodePage = CP_ACP);
	
	BOOL GuidFromString(LPCTSTR szGuid, GUID& guid);
	BOOL IsGuid(LPCTSTR szGuid);
	BOOL GuidToString(const GUID& guid, CString& sGuid);
	BOOL GuidIsNull(const GUID& guid);
	void NullGuid(GUID& guid);
	BOOL SameGuids(const GUID& guid1, const GUID& guid2);

	BOOL ArraysMatch(const CStringArray& array1, const CStringArray& array2, 
					 BOOL bOrderMatters = FALSE, BOOL bCaseMatters = FALSE);
	BOOL MatchAny(const CStringArray& array1, const CStringArray& array2, BOOL bCaseMatters = FALSE);
	CString FormatArray(const CStringArray& array);
	int ParseIntoArray(const CString& sText, CStringArray& array, BOOL bAllowEmpty = FALSE);
	int Find(const CStringArray& array, LPCTSTR szItem, BOOL bCaseMatters = FALSE);
	void Trace(const CStringArray& array);

	CString GetComputerName();
	CString GetUserName();
	CString GetListSeparator();
	CString GetDecimalSeparator();
	CString GetDefCharset();

	HCURSOR HandCursor();

	int CompareVersions(LPCTSTR szVersion1, LPCTSTR szVersion2);
	int Split(const CString& sText, char cDelim, CStringArray& aValues);

	HFONT CreateFont(HFONT hFont, DWORD dwFlags = 0);
	HFONT CreateFont(LPCTSTR szFaceName, int nPoint = -1, DWORD dwFlags = 0);
	BOOL CreateFont(CFont& font, LPCTSTR szFaceName, int nPoint = -1, DWORD dwFlags = 0);
	BOOL CreateFont(CFont& fontOut, HFONT fontIn, DWORD dwFlags = 0);

	DWORD GetFontFlags(HFONT hFont);
	int GetFontNameSize(HFONT hFont, CString& sFaceName);
	BOOL SameFont(HFONT hFont, LPCTSTR szFaceName, int nPoint);
	BOOL SameFontNameSize(HFONT hFont1, HFONT hFont2);
	CFont& WingDings();

	double Round(double dValue);
	float Round(float fValue);
	double Atof(const CString& sValue);
	CString Format(double dVal, int nDecPlaces = 2);

	BOOL IsWorkStationLocked();
	BOOL IsScreenSaverActive();

	void ProcessMsgLoop();

	int AFX_CDECL GetTextWidth(CDC* pDC, LPCTSTR lpszFormat, ...);
	float GetAverageCharWidth(CDC* pDC);

	int ParseSearchString(LPCTSTR szLookFor, CStringArray& aWords);
	BOOL FindWord(LPCTSTR szWord, LPCTSTR szText, BOOL bMatchCase, BOOL bMatchWholeWord);

	BOOL ModKeysArePressed(DWORD dwKeys); 
	BOOL KeyIsPressed(DWORD dwVirtKey);

	BOOL HasFlag(DWORD dwFlags, DWORD dwFlag);
};

#endif // !defined(AFX_MISC_H__4B2FDA3E_63C5_4F52_A139_9512105C3AD4__INCLUDED_)
