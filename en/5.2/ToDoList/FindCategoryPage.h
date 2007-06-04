#if !defined(AFX_FINDCATPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_)
#define AFX_FINDCATPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTextPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindCategoryPage dialog

class CFindCategoryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindCategoryPage)

// Construction
public:
	CFindCategoryPage();
	~CFindCategoryPage();

	int GetCategories(CStringArray& aCats);
	BOOL GetMatchAllCategories();

// Dialog Data
	//{{AFX_DATA(CFindCategoryPage)
	enum { IDD = IDD_FINDCATEGORY_PAGE };
	CComboBox	m_cbCategory;
	CString	m_sCategories;
	//}}AFX_DATA
	BOOL m_bMatchAllCategories;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindCategoryPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindCategoryPage)
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
