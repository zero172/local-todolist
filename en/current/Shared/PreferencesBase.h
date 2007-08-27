#if !defined(AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_)
#define AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "propertypagehost.h"
#include <afxtempl.h>

// PreferencesBase.h : header file
//

struct INIENTRY
{
	INIENTRY(LPCTSTR szName = NULL, LPCTSTR szValue = NULL) : sName(szName), sValue(szValue) {}
	
	CString sName;
	CString sValue;
};

typedef CMap<CString, LPCTSTR, INIENTRY, INIENTRY&> CIniEntryMap;

struct INISECTION
{
	INISECTION(LPCTSTR szName = NULL) : sSection(szName) {}
	
	CString sSection;
	CIniEntryMap aEntries;
};

typedef CArray<INISECTION*, INISECTION*> CIniSectionArray;

class CPreferencesStorage
{
public:
	CPreferencesStorage();
	~CPreferencesStorage();
	
	// directly from CWinApp
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault = 0) const;
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL) const;
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	// note: Binary not supported by ini file

	// extras
	double GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault = 0) const;
	BOOL WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);

	int GetArrayItems(LPCTSTR szRegKey, CStringArray& aItems) const;
	void WriteArrayItems(const CStringArray& aItems, LPCTSTR szRegKey);

protected:
	mutable CIniSectionArray m_aIni;
	BOOL m_bDirty;
	BOOL m_bIni;
	
	static POSITION GetEntry(INISECTION& section, LPCTSTR lpszEntry);
	static INISECTION* GetSection(CIniSectionArray& ini, LPCTSTR lpszSection, BOOL bCreateNotExist);
	
	static CString GetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	static void SetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR szValue);
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesPageBase dialog

class CPreferencesPageBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreferencesPageBase)
		
		// Construction
public:
	CPreferencesPageBase(UINT nID = 0);
	~CPreferencesPageBase();
	
	virtual void LoadPreferences(const CPreferencesStorage& /*prefs*/) {}
	virtual void SavePreferences(CPreferencesStorage& /*prefs*/) {}
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlgBase dialog

class CPreferencesDlgBase : public CDialog
{
	// Construction
public:
	CPreferencesDlgBase(UINT nID = 0, CWnd* pParent = NULL);   // standard constructor
	
	int DoModal(int nInitPage = -1);
	
protected:
	int m_nInitPage;
	
protected:
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
		
protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	
	virtual BOOL SetActivePage(int nPage);
	virtual BOOL SetActivePage(CPreferencesPageBase* pPage);
	virtual BOOL AddPage(CPreferencesPageBase* pPage);
	
	void LoadPreferences();
	void SavePreferences();
	
	BOOL CreatePPHost(UINT nHostID);
	BOOL CreatePPHost(LPRECT pRect);
	
	int GetActiveIndex() const { return m_pphost.GetActiveIndex(); }
	CPreferencesPageBase* GetActivePage() { return (CPreferencesPageBase*)m_pphost.GetActivePage(); }
	int FindPage(CPreferencesPageBase* pPage) { return m_pphost.FindPage(pPage); }
	
private:
	CPropertyPageHost m_pphost;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_)
