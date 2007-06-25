#if !defined(AFX_FINDNUMPAGE_H__5B74306F_38B9_41B0_A255_D4C405E24D26__INCLUDED_)
#define AFX_FINDNUMPAGE_H__5B74306F_38B9_41B0_A255_D4C405E24D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindNumPage.h : header file
//

#include "..\shared\maskedit.h"
#include "..\shared\dialoghelper.h"

/////////////////////////////////////////////////////////////////////////////
// CFindNumPage dialog

class CFindNumPage : public CPropertyPage, public CDialogHelper
{
	DECLARE_DYNCREATE(CFindNumPage)

// Construction
public:
	CFindNumPage(BOOL bInteger = TRUE, BOOL bAllowNegative = TRUE);
	~CFindNumPage();

	void GetNumberRange(int& nFrom, int& nTo);
	void GetNumberRange(double& dFrom, double& dTo);

protected:
// Dialog Data
	//{{AFX_DATA(CFindNumPage)
	enum { IDD = IDD_FINDNUM_PAGE };
	CMaskEdit	m_eThis;
	CMaskEdit	m_eEnd;
	CMaskEdit	m_eStart;
	CMaskEdit	m_eLessThan;
	CMaskEdit	m_eMoreThan;
	int		m_nNumberOption;
	CString	m_sMoreThanValue;
	CString	m_sLessThanValue;
	CString	m_sStartValue;
	CString	m_sEndValue;
	CString	m_sThisValue;
	//}}AFX_DATA
	BOOL m_bAllowNegative;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindNumPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindNumPage)
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

#endif // !defined(AFX_FINDNUMPAGE_H__5B74306F_38B9_41B0_A255_D4C405E24D26__INCLUDED_)
