#if !defined(AFX_TDLTRANSFORMDIALOG_H__1A62F94F_687F_421C_97D2_300BAC4A3E7C__INCLUDED_)
#define AFX_TDLTRANSFORMDIALOG_H__1A62F94F_687F_421C_97D2_300BAC4A3E7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TDLTransformDialog.h : header file
//

#include "..\shared\fileedit.h"
#include "TaskSelectionDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLTransformDialog dialog

class CTDLTransformDialog : public CDialog
{
// Construction
public:
	CTDLTransformDialog(LPCTSTR szTitle, CWnd* pParent = NULL);   // standard constructor

	BOOL GetWantDate() { return m_bDate; }
	BOOL GetWantSelectedTasks() { return m_taskSel.GetWantSelectedTasks(); }
	BOOL GetWantSelectedSubtasks() { return m_taskSel.GetWantSelectedSubtasks(); }
	BOOL GetWantCompletedTasks() { return m_taskSel.GetWantCompletedTasks(); }
	BOOL GetWantInCompleteTasks() { return m_taskSel.GetWantInCompleteTasks(); }
	CString GetStylesheet() const { return m_sStylesheet; }
	CString GetTitle() const { return m_sTitle; }

protected:
// Dialog Data
	//{{AFX_DATA(CTDLTransformDialog)
	CFileEdit	m_eStylesheet;
	CString	m_sStylesheet;
    CTaskSelectionDlg m_taskSel;
	//}}AFX_DATA
	CString	m_sTitle;
	BOOL	m_bDate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLTransformDialog)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTDLTransformDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeStylesheet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLTRANSFORMDIALOG_H__1A62F94F_687F_421C_97D2_300BAC4A3E7C__INCLUDED_)
