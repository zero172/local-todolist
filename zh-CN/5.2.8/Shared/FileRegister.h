// FileRegister.h: interface for the CFileRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREGISTER_H__67F5AB1F_4CE5_405C_A6F5_DE7B3802790A__INCLUDED_)
#define AFX_FILEREGISTER_H__67F5AB1F_4CE5_405C_A6F5_DE7B3802790A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileRegister  
{
public:
	CFileRegister(LPCTSTR sExt, LPCTSTR szFileType = NULL);
	~CFileRegister();

	BOOL IsRegisteredAppInstalled();
	CString GetRegisteredAppPath(BOOL bFilenameOnly = FALSE);
	BOOL RegisterFileType(LPCTSTR szFileDesc, int nIcon, BOOL bAllowShellOpen = TRUE, LPCTSTR szParams = NULL, BOOL bAskUser = TRUE);
	BOOL UnRegisterFileType();
	BOOL IsRegisteredApp();

	static BOOL IsRegisteredAppInstalled(LPCTSTR szExt);
	static CString GetRegisteredAppPath(LPCTSTR szExt, BOOL bFilenameOnly = FALSE);
	static BOOL RegisterFileType(LPCTSTR szExt, LPCTSTR szFileType, LPCTSTR szFileDesc, int nIcon, BOOL bAllowShellOpen = TRUE, LPCTSTR szParams = NULL, BOOL bAskUser = TRUE);
	static BOOL UnRegisterFileType(LPCTSTR szExt, LPCTSTR szFileType);
	static BOOL IsRegisteredApp(LPCTSTR szExt, LPCTSTR szFileType = NULL);

protected:
	CString m_sExt;
	CString m_sFileType;
	CString m_sAppPath;
};

#endif // !defined(AFX_FILEREGISTER_H__67F5AB1F_4CE5_405C_A6F5_DE7B3802790A__INCLUDED_)
