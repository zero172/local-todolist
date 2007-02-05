#if !defined(AFX_FILTERDLG_H__9BFA276C_9BD6_4B23_A714_BB9D059CC2EB__INCLUDED_)
#define AFX_FILTERDLG_H__9BFA276C_9BD6_4B23_A714_BB9D059CC2EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterDlg.h : header file
//

#include "filteredtodoctrl.h"
#include "tdlprioritycombobox.h"
#include "tdlriskcombobox.h"
#include "tdlcategorycombobox.h"

#include "..\shared\tabbedcombobox.h"
#include "..\shared\dialoghelper.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

class CFilterDlg : public CDialog, public CDialogHelper
{
// Construction
public:
	CFilterDlg(BOOL bMultiCategorySel, CWnd* pParent = NULL);   // standard constructor

	int DoModal(const CFilteredToDoCtrl& tdc, 
				const CDWordArray& aPriorityColors);
	void GetFilter(FTDCFILTER& filter);

protected:
// Dialog Data
	//{{AFX_DATA(CFilterDlg)
	enum { IDD = IDD_FILTER_DIALOG };
	//}}AFX_DATA
	CTDLCategoryComboBox	m_cbCategoryFilter;
	CComboBox	m_cbStatusFilter;
	CComboBox	m_cbAllocToFilter, m_cbAllocByFilter;
	CTabbedComboBox	m_cbTaskFilter;
	CTDLPriorityComboBox	m_cbPriorityFilter;
	CTDLRiskComboBox	m_cbRiskFilter;
	FTDCFILTER m_filter;
	CStringArray m_aAllocTo;
	CStringArray m_aAllocBy;
	CStringArray m_aCategory;
	CStringArray m_aStatus;
	CDWordArray m_aPriorityColors;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClearfilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDLG_H__9BFA276C_9BD6_4B23_A714_BB9D059CC2EB__INCLUDED_)
