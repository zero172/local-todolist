#if !defined(AFX_EXPORTDLG_H__2F5B4FD1_E968_464E_9734_AC995DB13B35__INCLUDED_)
#define AFX_EXPORTDLG_H__2F5B4FD1_E968_464E_9734_AC995DB13B35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportDlg.h : header file
//

#include "TaskSelectionDlg.h"
#include "..\shared\fileedit.h"
#include "..\shared\importexportmgr.h"

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

enum { ED_HTMLFMT, ED_TEXTFMT };

class CExportDlg : public CDialog
{
// Construction
public:
	CExportDlg(const CImportExportMgr& mgr, BOOL bSingleTaskList, 
				LPCTSTR szFilePath = NULL, LPCTSTR szFolderPath = NULL, CWnd* pParent = NULL);

	BOOL GetExportAllTasklists();
	int GetExportFormat() { return s_nFormatOption; }
	CString GetExportPath(); // can be folder or path
	BOOL GetExportOneFile() { return (m_bSingleTaskList/* || m_bExportOneFile*/); }
	BOOL GetWantSelectedTasks() { return m_taskSel.GetWantSelectedTasks(); }
	BOOL GetWantSelectedSubtasks() { return m_taskSel.GetWantSelectedSubtasks(); }
	BOOL GetWantCompletedTasks() { return m_taskSel.GetWantCompletedTasks(); }
	BOOL GetWantInCompleteTasks() { return m_taskSel.GetWantInCompleteTasks(); }

protected:
// Dialog Data
	//{{AFX_DATA(CExportDlg)
	CComboBox	m_cbFormat;
	CFileEdit	m_eExportPath;
	int		m_nExportOption;
	CString	m_sExportPath;
	BOOL	m_bExportOneFile;
	CString	m_sPathLabel;
	//}}AFX_DATA
	CTaskSelectionDlg m_taskSel;
	BOOL m_bSingleTaskList; 
	CString m_sFolderPath, m_sFilePath;
	const CImportExportMgr& m_mgrImportExport;
	static int s_nFormatOption; // persists just for the current session

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTasklistoptions();
	afx_msg void OnSelchangeFormatoptions();
	afx_msg void OnExportonefile();
	afx_msg void OnChangeExportpath();
	afx_msg LRESULT OnChangeTaskSelOption(WPARAM wp, LPARAM lp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableOK();

	void ReplaceExtension(CString& sPathName, int nFormat);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__2F5B4FD1_E968_464E_9734_AC995DB13B35__INCLUDED_)
