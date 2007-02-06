#if !defined(AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
#define AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesFilePage.h : header file
//

#include "..\shared\fontcombobox.h"
#include "..\shared\fileedit.h"
#include "..\shared\groupline.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFilePage dialog

enum
{
	PFP_INVALID = -1,
	PFP_DUETODAY,
	PFP_DUETOMORROW,
	PFP_DUETHISWEEK,
	PFP_DUENEXTWEEK,
	PFP_DUETHISMONTH,
	PFP_DUENEXTMONTH,
};

class CPreferencesFilePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreferencesFilePage)

// Construction
public:
	CPreferencesFilePage();
	~CPreferencesFilePage();

	BOOL GetAutoArchive() const { return m_bAutoArchive; }
	BOOL GetNotifyReadOnly() const { return m_bNotifyReadOnly; }
	BOOL GetAutoHtmlExport() const { return m_bAutoHtmlExport; }
	int GetAutoSaveFrequency() const { return m_nAutoSaveFrequency; }
	BOOL GetRemoveArchivedTasks() const { return m_bRemoveArchivedTasks; }
	BOOL GetRemoveOnlyOnAbsoluteCompletion() const { return m_bRemoveOnlyOnAbsoluteCompletion; }
	CString GetAutoExportFolderPath() const;
	int GetNotifyDueByOnLoad() const { return m_bNotifyDueOnLoad ? m_nNotifyDueByOnLoad : PFP_INVALID; }
	int GetNotifyDueByOnSwitch() const { return m_bNotifyDueOnSwitch ? m_nNotifyDueByOnSwitch : PFP_INVALID; }
	BOOL GetDisplayDueTasksInHtml() const { return m_bDisplayDueTasksInHtml; }
	BOOL GetRefreshFindOnLoad() const { return m_bRefreshFindOnLoad; }
	BOOL GetDueTaskTitlesOnly() const { return m_bDueTaskTitlesOnly; }
	CString GetDueTaskStylesheet() const { return m_sDueTasksStylesheet; }
	CString GetSaveExportStylesheet() const { return m_sSaveExportStylesheet; }
	CString GetDueTaskPerson() const { return m_bOnlyShowDueTasksForPerson ? m_sDueTaskPerson : ""; }
	BOOL GetWarnAddDeleteArchive() const { return m_bWarnAddDeleteArchive; }
	BOOL GetAutoSaveUnsaved() const { return !m_bDontAutoSaveUnsaved; }
	BOOL GetDontRemoveFlagged() const { return m_bDontRemoveFlagged; }
	BOOL GetExpandTasksOnLoad() const { return m_bExpandTasks; }
//	BOOL Get() const { return m_b; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesFilePage)
	enum { IDD = IDD_PREFFILE_PAGE };
	CFileEdit	m_eSaveExportStylesheet;
	CFileEdit	m_eDueTaskStylesheet;
	CComboBox	m_cbFontSize;
	CFileEdit	m_eExportFolderPath;
	BOOL	m_bExportToFolder;
	CString	m_sExportFolderPath;
	BOOL	m_bDisplayDueTasksInHtml;
	BOOL	m_bRefreshFindOnLoad;
	BOOL	m_bDueTaskTitlesOnly;
	CString	m_sSaveExportStylesheet;
	CString	m_sDueTasksStylesheet;
	BOOL	m_bUseStylesheetForSaveExport;
	BOOL	m_bUseStyleSheetForDueTasks;
	BOOL	m_bOnlyShowDueTasksForPerson;
	CString	m_sDueTaskPerson;
	BOOL	m_bWarnAddDeleteArchive;
	BOOL	m_bDontAutoSaveUnsaved;
	BOOL	m_bDontRemoveFlagged;
	BOOL	m_bExpandTasks;
	//}}AFX_DATA
	BOOL	m_bNotifyDueOnLoad, m_bNotifyDueOnSwitch;
	int		m_nNotifyDueByOnLoad, m_nNotifyDueByOnSwitch;
	BOOL	m_bAutoArchive;
	BOOL	m_bNotifyReadOnly;
	CComboBox	m_cbAutoSave;
	BOOL	m_bRemoveArchivedTasks;
	BOOL	m_bRemoveOnlyOnAbsoluteCompletion;
	int		m_nAutoSaveFrequency;
	BOOL	m_bAutoSave;
	BOOL	m_bAutoHtmlExport;
	CGroupLineManager m_mgrGroupLines;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesFilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesFilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnExporttofolder();
	afx_msg void OnAutohtmlexport();
	afx_msg void OnNotifyDueOnLoad();
	afx_msg void OnDisplayduetasksinhtml();
	afx_msg void OnUsestylesheetfordueitems();
	afx_msg void OnUsestylesheetforsave();
	afx_msg void OnOnlyshowduetaskforperson();
	//}}AFX_MSG
	afx_msg void OnNotifyDueOnSwitch();
	afx_msg void OnRemovearchiveditems();
	afx_msg void OnAutosave();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
