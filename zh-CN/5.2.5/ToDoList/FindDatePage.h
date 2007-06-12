#if !defined(AFX_FINDDATEPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_)
#define AFX_FINDDATEPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDatePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDatePage dialog

class CFindDatePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindDatePage)

// Construction
public:
	CFindDatePage();
	~CFindDatePage();

	void GetDateRange(COleDateTime& dateFrom, COleDateTime& dateTo);

protected:
// Dialog Data
	//{{AFX_DATA(CFindDatePage)
	enum { IDD = IDD_FINDDATE_PAGE };
	COleDateTime	m_dateAfter;
	COleDateTime	m_dateBefore;
	COleDateTime	m_dateStart;
	COleDateTime	m_dateEnd;
	int		m_nDateOption;
	COleDateTime	m_dateThis;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindDatePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindDatePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeOption();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls(int nOption);
	CString GetRegKey() const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDATEPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_)
