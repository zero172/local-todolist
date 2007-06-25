#if !defined(AFX_FINDRISKPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_)
#define AFX_FINDRISKPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindRiskPage.h : header file
//

#include "tdlriskcombobox.h"

/////////////////////////////////////////////////////////////////////////////
// CFindRiskPage dialog

class CFindRiskPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindRiskPage)

// Construction
public:
	CFindRiskPage();
	~CFindRiskPage();

	void GetRange(int& nRiskFrom, int& nRiskTo);

protected:
// Dialog Data
	//{{AFX_DATA(CFindRiskPage)
	enum { IDD = IDD_FINDRISK_PAGE };
	int	m_nMoreThanRisk;
	int	m_nLessThanRisk;
	int	m_nStartRisk;
	int	m_nEndRisk;
	int	m_nRiskOption;
	int	m_nThisRisk;
	//}}AFX_DATA
   CTDLRiskComboBox m_cbMoreThanRisk;
   CTDLRiskComboBox m_cbLessThanRisk;
   CTDLRiskComboBox m_cbStartRisk;
   CTDLRiskComboBox m_cbEndRisk;
   CTDLRiskComboBox m_cbThisRisk;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindRiskPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindRiskPage)
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

#endif // !defined(AFX_FINDRISKPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_)
