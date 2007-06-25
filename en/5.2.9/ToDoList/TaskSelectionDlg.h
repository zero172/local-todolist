#if !defined(AFX_TASKSELECTIONDLG_H__3A5D7E88_CEC3_47DD_8BA6_79EC2C3B7167__INCLUDED_)
#define AFX_TASKSELECTIONDLG_H__3A5D7E88_CEC3_47DD_8BA6_79EC2C3B7167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskSelectionDlg.h : header file
//

const UINT WM_TASKSELDLG_CHANGE = ::RegisterWindowMessage("WM_TASKSELDLG_CHANGE");

/////////////////////////////////////////////////////////////////////////////
// CTaskSelectionDlg dialog

class CTaskSelectionDlg : public CDialog
{
// Construction
public:
	CTaskSelectionDlg(LPCTSTR szRegKey = NULL); 

   BOOL Create(UINT nIDRefFrame, CWnd* pParent, UINT nID = (UINT)-1);
   int DoModal() { ASSERT (0); return IDCANCEL; }

	BOOL GetWantSelectedTasks() { return m_bSelectedTasks; }
	BOOL GetWantSelectedSubtasks() { return m_bSelectedTasks && m_bSelectedSubtasks; }
	BOOL GetWantCompletedTasks() { return m_bSelectedTasks || m_bCompletedTasks; }
	BOOL GetWantInCompleteTasks() { return m_bSelectedTasks || m_bIncompleteTasks; }

	void SetWantSelectedTasks(BOOL bWant = TRUE);
	void SetWantCompletedTasks(BOOL bWant = TRUE);
	void SetWantInCompleteTasks(BOOL bWant = TRUE);

protected:
// Dialog Data
	//{{AFX_DATA(CTaskSelectionDlg)
	enum { IDD = IDD_TASKSELECTION_DIALOG };
	//}}AFX_DATA
	BOOL	m_bSelectedSubtasks;
	BOOL	m_bCompletedTasks;
	BOOL	m_bIncompleteTasks;
	int		m_bSelectedTasks;
   CString m_sRegKey;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskSelectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskSelectionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	afx_msg void OnChangetasksOption();
	afx_msg void OnIncludeDone();
	afx_msg void OnIncludeNotDone();
	virtual BOOL OnInitDialog();
   afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKSELECTIONDLG_H__3A5D7E88_CEC3_47DD_8BA6_79EC2C3B7167__INCLUDED_)
