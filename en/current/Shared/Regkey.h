#if !defined(AFX_REGKEY_H__E451D194_D4C3_11D1_AB17_0000E8425C3E__INCLUDED_)
#define AFX_REGKEY_H__E451D194_D4C3_11D1_AB17_0000E8425C3E__INCLUDED_

#include <winreg.h>
#include <afxtempl.h>

class CRegKey
{

// Construction
public:
	CRegKey();
	CRegKey(CRegKey& regKey);
	virtual ~CRegKey();

	CRegKey& operator=(CRegKey regKey); 

// Operations
public:
	// note: calling open on an already open key will assert
	// you must call Close() first if you want to re-use CRegKey objects
	LONG Open(HKEY hKeyRoot, const char* pszPath); 
	void Close();
	CString GetPath() const { return m_sPath; }
	HKEY GetKey() const { return m_hKey; }
	HKEY GetKeyRoot() const { return m_hKeyRoot; }

	LONG Write(const char* pszKey, DWORD dwVal);
	LONG Write(const char* pszKey, const char* pszVal);
	LONG Write(const char* pszKey, const BYTE* pData, DWORD dwLength);

	LONG Read(const char* pszKey, DWORD& dwVal) const;
	LONG Read(const char* pszKey, CString& sVal) const;
	LONG Read(const char* pszKey, BYTE* pData, DWORD& dwLength) const;

	static LONG Delete(HKEY hKeyRoot, const char* pszPath);
	static BOOL KeyExists(HKEY hKeyRoot, const char* pszPath);

   int GetSubkeyNames(CStringArray& aNames) const;
   int GetValueNames(CStringArray& aNames) const;
   BOOL HasValues() const;

   BOOL ExportToIni(LPCTSTR szIniPath) const;
   BOOL ImportFromIni(LPCTSTR szIniPath);

#ifndef _NOT_USING_MFC_
	static CString GetAppRegPath(LPCTSTR szAppName = NULL);
#endif

protected:
	HKEY 	m_hKey;
	HKEY 	m_hKeyRoot;
	CString m_sPath;

protected:
	static LONG RecurseDeleteKey(HKEY key, LPCTSTR lpszKey);
   BOOL ExportKeyToIni(const char* pszKey, CStdioFile& file) const;
   BOOL ExportValueToIni(DWORD nIndex, CStdioFile& file) const;
   BOOL ImportSectionFromIni(const CString& sSection, CStdioFile& file, CString& sNextSection);
};

#endif
