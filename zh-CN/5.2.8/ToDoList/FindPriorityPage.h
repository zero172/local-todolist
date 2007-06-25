#if !defined(AFX_FINDPRIORITYPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_)
#define AFX_FINDPRIORITYPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindPriorityPage.h : header file
//

#include "tdlPrioritycombobox.h"

/////////////////////////////////////////////////////////////////////////////
// CFindPriorityPage dialog

class CFindPriorityPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindPriorityPage)

// Construction
public:
	CFindPriorityPage();
	~CFindPriorityPage();

	void SetPriorityColors(const CDWordArray& aColors);
	void GetRange(int& nPriorityFrom, int& nPriorityTo);

protected:
// Dialog Data
	//{{AFX_DATA(CFindPriorityPage)
	enum { IDD = IDD_FINDPRIORITY_PAGE };
	int	m_nMoreThanPriority;
	int	m_nLessThanPriority;
	int	m_nStartPriority;
	int	m_nEndPriority;
	int	m_nPriorityOption;
	int	m_nThisPriority;
	//}}AFX_DATA
   CTDLPriorityComboBox m_cbMoreThanPriority;
   CTDLPriorityComboBox m_cbLessThanPriority;
   CTDLPriorityComboBox m_cbStartPriority;
   CTDLPriorityComboBox m_cbEndPriority;
   CTDLPriorityComboBox m_cbThisPriority;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFindPriorityPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindPriorityPage)
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

#endif // !defined(AFX_FINDPRIORITYPAGE_H__D025EBDC_A1C6_4507_9AEA_29A0B2C0C786__INCLUDED_)
