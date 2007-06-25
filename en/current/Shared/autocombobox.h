#if !defined(AFX_AUTOCOMBOBOX_H__34DD95FF_CCAE_4D8E_9162_D860B65CD448__INCLUDED_)
#define AFX_AUTOCOMBOBOX_H__34DD95FF_CCAE_4D8E_9162_D860B65CD448__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// autocombobox.h : header file
//

#include "Subclass.h"

const UINT WM_ACB_ITEMADDED = ::RegisterWindowMessage("WM_ACB_ITEMADDED");
const UINT WM_ACB_ITEMDELETED = ::RegisterWindowMessage("WM_ACB_ITEMDELETED");

/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox window

// replacement DDX routine
void AFXAPI DDX_AutoCBString(CDataExchange* pDX, int nIDC, CString& value);

class CAutoComboBox : public CComboBox, protected CSubclassWnd
{
	// Construction
public:
	CAutoComboBox(BOOL bAllowDelete = FALSE, BOOL bCaseSensitive = FALSE);
	virtual ~CAutoComboBox();
	
	// Operations
public:
    int AddString(LPCTSTR szItem) { return AddUniqueItem(szItem); }
    int InsertString(int nIndex, LPCTSTR szItem) { return InsertUniqueItem(nIndex, szItem); }
	
	virtual int AddUniqueItem(const CString& sItem); // returns index or CB_ERR
    virtual int InsertUniqueItem(int nIndex, const CString& sItem); // returns index or CB_ERR
	
    int AddUniqueItems(const CStringArray& aItems); // returns num items added
    int AddUniqueItems(const CAutoComboBox& cbItems); // returns num items added
	
    int GetItems(CStringArray& aItems) const; // returns item count
	
    // helpers
    int FindStringExact(int nIndexStart, const CString& sItem, BOOL bCaseSensitive) const;
    int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
		{ return FindStringExact(nIndexStart, lpszFind, FALSE); }
	
protected:
	BOOL m_bAllowDelete;
	HWND m_hwndListbox;
	BOOL m_bCaseSensitive;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoComboBox)
protected:
	//}}AFX_VIRTUAL
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CAutoComboBox)
	afx_msg BOOL OnKillfocus();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
		
protected:
	virtual BOOL DeleteSelectedLBItem();
	virtual void RefreshMaxDropWidth();
	
	BOOL IsSimpleCombo();
	virtual void HandleReturnKey();
	void NotifyParent(UINT nIDNotify);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOCOMBOBOX_H__34DD95FF_CCAE_4D8E_9162_D860B65CD448__INCLUDED_)
