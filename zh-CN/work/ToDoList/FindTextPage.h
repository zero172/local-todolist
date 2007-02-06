#if !defined(AFX_FINDTEXTPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_)
#define AFX_FINDTEXTPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTextPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindTextPage dialog

class CFindTextPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindTextPage)

// Construction
public:
	CFindTextPage(BOOL bShowMatchCase = TRUE, BOOL bShowMatchWholeWord = TRUE);
	~CFindTextPage();

	CString GetText();
	BOOL GetMatchCase();
	BOOL GetMatchWholeWord();

// Dialog Data
	//{{AFX_DATA(CFindTextPage)
	enum { IDD = IDD_FINDTEXT_PAGE };
	CComboBox	m_cbText;
	CString	m_sText;
	BOOL	m_bMatchWholeWord;
	BOOL	m_bMatchCase;
	//}}AFX_DATA
	BOOL	m_bShowMatchWholeWord;
	BOOL	m_bShowMatchCase;


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
