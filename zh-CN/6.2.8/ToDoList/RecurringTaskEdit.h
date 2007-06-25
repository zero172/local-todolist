// RecurrenceEdit.h: interface for the CRecurringTaskEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_)
#define AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\shared\enedit.h"
#include "..\shared\propertypagehost.h"
#include "..\shared\monthcombobox.h"

#include "ToDoCtrlData.h"

class CRecurringTaskEdit : public CEnEdit  
{
public:
	CRecurringTaskEdit();
	virtual ~CRecurringTaskEdit();

	CString GetRegularity() const;
	void GetRecurrenceOptions(TDIRECURRENCE& tr) const;
	void SetRecurrenceOptions(const TDIRECURRENCE& tr);

	void SetDefaultDate(const COleDateTime& date) { m_dtDefault = date; }

	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

	static CString GetRegularity(TDI_REGULARITY nRegularity);
	static int CalcMaxRegularityWidth(CDC* pDC, BOOL bIncOnce = TRUE);

protected:
	TDIRECURRENCE m_tr;
	BOOL m_bReadOnly;
	COleDateTime m_dtDefault;

protected:
	virtual void OnBtnClick(UINT nID);
	virtual void PreSubclassWindow();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecurringTaskEdit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnSetReadOnly(WPARAM wp, LPARAM lp);
	afx_msg void OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#endif 

/////////////////////////////////////////////////////////////////////////////
// CRecTaskDailyPage dialog

class CRecTaskDailyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRecTaskDailyPage)

// Construction
public:
	CRecTaskDailyPage();
	~CRecTaskDailyPage();

	BOOL HasValidData();

// Dialog Data
	//{{AFX_DATA(CRecTaskDailyPage)
	int		m_nNumDays;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRecTaskDailyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecTaskDailyPage)
	afx_msg void OnChangeValues();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRecTaskWeeklyPage dialog

class CRecTaskWeeklyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRecTaskWeeklyPage)

// Construction
public:
	CRecTaskWeeklyPage();
	~CRecTaskWeeklyPage();

	BOOL HasValidData();

// Dialog Data
	//{{AFX_DATA(CRecTaskWeeklyPage)
	CCheckListBox	m_lbWeekdays;
	int		m_nNumWeeks;
	//}}AFX_DATA
	DWORD m_dwWeekdays;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRecTaskWeeklyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecTaskWeeklyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeValues();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRecTaskMonthlyPage dialog

class CRecTaskMonthlyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRecTaskMonthlyPage)

// Construction
public:
	CRecTaskMonthlyPage();
	~CRecTaskMonthlyPage();

	BOOL HasValidData();

// Dialog Data
	//{{AFX_DATA(CRecTaskMonthlyPage)
	int		m_nNumMonths;
	int		m_nDay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRecTaskMonthlyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecTaskMonthlyPage)
	afx_msg void OnChangeValues();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRecTaskYearlyPage dialog

class CRecTaskYearlyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRecTaskYearlyPage)

// Construction
public:
	CRecTaskYearlyPage();
	~CRecTaskYearlyPage();

	BOOL HasValidData();

// Dialog Data
	//{{AFX_DATA(CRecTaskYearlyPage)
	CMonthComboBox	m_cbMonths;
	int 	m_nMonth;
	int		m_nDay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRecTaskYearlyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecTaskYearlyPage)
	afx_msg void OnChangeValues();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CRecurringTaskOptionDlg dialog

class CRecurringTaskOptionDlg : public CDialog
{
// Construction
public:
	CRecurringTaskOptionDlg(const TDIRECURRENCE& tr, const COleDateTime& dtDefault, CWnd* pParent = NULL);   // standard constructor

	void GetRecurrenceOptions(TDIRECURRENCE& tr) const;

protected:
// Dialog Data
	//{{AFX_DATA(CRecurringTaskOptionDlg)
	int		m_nRegularity;
	int		m_bRecalcFromDue;
	//}}AFX_DATA
	CPropertyPageHost m_host;
	CRecTaskYearlyPage m_pageYearly;
	CRecTaskMonthlyPage m_pageMonthly;
	CRecTaskWeeklyPage m_pageWeekly;
	CRecTaskDailyPage m_pageDaily;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecurringTaskOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecurringTaskOptionDlg)
	afx_msg void OnSelchangeRegularity();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnValueChange(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

// !defined(AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_)
