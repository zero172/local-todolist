#if !defined(AFX_FINDTEXTPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_)
#define AFX_FINDTEXTPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTextPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindTextPage dialog

enum
{
	FTP_SHOWMATCHCASE		= 0x01,
	FTP_SHOWMATCHWHOLEWORD	= 0x02,	
	FTP_SHOWMATCHALLWORDS	= 0x04,
};

class CFindTextPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindTextPage)

// Construction
public:
	CFindTextPage(DWORD dwShowFlags = FTP_SHOWMATCHCASE | FTP_SHOWMATCHWHOLEWORD/* | FTP_SHOWMATCHALLWORDS*/);
	~CFindTextPage();

	CString GetText();
	BOOL GetMatchCase();
	BOOL GetMatchWholeWord();
	BOOL GetMatchAllItems();

// Dialog Data
	//{{AFX_DATA(CFindTextPage)
	enum { IDD = IDD_FINDTEXT_PAGE };
	CComboBox	m_cbText;
	CString	m_sText;
	BOOL	m_bMatchWholeWord;
	BOOL	m_bMatchCase;
	BOOL	m_bMatchAll;
	//}}AFX_DATA
	DWORD	m_dwShowFlags;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindTextPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindTextPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int FindSearchInCombo(LPCTSTR szLookFor) const;
	CString GetRegKey() const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTEXTPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_)
