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
	int		m_nNumDays;
	CCheckListBox	m_lbWeekdays;
	int		m_nNumWeeks;
	DWORD m_dwWeekdays;
	CMonthComboBox	m_cbMonths;
	int 	m_nMonth;
	int		m_nYearMonthDay;
	int		m_nNumMonths;
	int		m_nMonthDay;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecurringTaskOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecurringTaskOptionDlg)
	afx_msg void OnSelchangeRegularity();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnChangeDailyValues();
	afx_msg void OnChangeMonthlyValues();
	afx_msg void OnChangeWeeklyValues();
	afx_msg void OnChangeYearlyValues();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL HasValidData();
};

// !defined(AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_)
