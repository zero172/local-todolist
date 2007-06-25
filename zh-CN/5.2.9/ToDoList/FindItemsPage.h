#if !defined(AFX_FINDITEMSPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_)
#define AFX_FINDITEMSPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTextPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindItemsPage dialog

class CFindItemsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindItemsPage)

// Construction
public:
	CFindItemsPage();
	~CFindItemsPage();

//	void SetItems(const CStringArray& aItems);
	int GetItems(CStringArray& aItems);
	BOOL GetMatchAllItems();

// Dialog Data
	//{{AFX_DATA(CFindItemsPage)
	enum { IDD = IDD_FINDITEMS_PAGE };
	CComboBox	m_cbItems;
	CString	m_sItems;
	//}}AFX_DATA
	BOOL m_bMatchAllItems;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindItemsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindItemsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int FindSearchInCombo(LPCTSTR szLookFor) const;
	CString GetRegKey() const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDITEMSPAGE_H__F490515C_BB36_497E_BC0B_0C9D4DE38CA2__INCLUDED_)
