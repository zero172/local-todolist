#if !defined(AFX_FINDTASKDLG_H__BDCCE0F4_B91B_47AD_AE27_1DEB4BA7EA1C__INCLUDED_)
#define AFX_FINDTASKDLG_H__BDCCE0F4_B91B_47AD_AE27_1DEB4BA7EA1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTaskDlg.h : header file
//

#include "findtextpage.h"
#include "finddatepage.h"
#include "findnumpage.h"
#include "findblankpage.h"
#include "findprioritypage.h"
#include "findriskpage.h"
#include "findcategorypage.h"

#include "..\shared\PropertyPageHost.h"
#include "..\shared\dockmanager.h"
#include "..\shared\entoolbar.h"
#include "..\shared\toolbarhelper.h"
#include "..\shared\tabbedcombobox.h"

#include "..\3rdparty\lockableheaderctrl.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CFindTaskDlg dialog

const UINT WM_FTD_FIND = ::RegisterWindowMessage("WM_FTD_FIND");
const UINT WM_FTD_SELECTRESULT = ::RegisterWindowMessage("WM_FTD_SELECTRESULT");
const UINT WM_FTD_SELECTALL = ::RegisterWindowMessage("WM_FTD_SELECTALL");
const UINT WM_FTD_CLOSE = ::RegisterWindowMessage("WM_FTD_CLOSE");

struct FTDRESULT
{
	DWORD dwItemData;
	int nTaskList;
	BOOL bDone;
};

typedef CArray<FTDRESULT, FTDRESULT&> CFTDResultsArray;

enum FIND_WHAT
{
	FW_NOTHING = -1,
	FW_TITLECOMMENTS,
	FW_PRIORITY,
	FW_PERCENTDONE,
	FW_ALLOCTO,
	FW_ALLOCBY,
	FW_STATUS,
	FW_CATEGORY,
	FW_TIMEEST,
	FW_TIMESPENT,
	FW_STARTDATE,
	FW_DUEDATE,
	FW_DONEDATE,
	FW_TASKID,
	FW_FLAG,
	FW_CREATIONDATE,
	FW_CREATEDBY,
	FW_LASTMOD,
	FW_RISK,
	FW_EXTERNALID,
	FW_COST,
	FW_VERSION,
};

class CFindTaskDlg : public CDialog
{
// Construction
public:
	CFindTaskDlg(CWnd* pParent = NULL);   // standard constructor
	~CFindTaskDlg();

	BOOL Initialize(CWnd* pParent, BOOL bDockable = TRUE);
	BOOL Show(BOOL bShow = TRUE);
	void RefreshSearch();
	
	void SetPriorityColors(const CDWordArray& aColors);
	void SetCategories(const CStringArray& aCats);

	void AddResult(LPCTSTR szTask, LPCTSTR szMatch, LPCTSTR szPath, BOOL bDone, DWORD dwItemData, int nTaskList);

	FIND_WHAT GetFindWhat();

	BOOL GetRange(COleDateTime& dateFrom, COleDateTime& dateTo);
	BOOL GetRange(int& nFrom, int& nTo);
	BOOL GetRange(double& dFrom, double& dTo);

	int GetCategories(CStringArray& aCats);
	BOOL GetMatchAllCategories();
	
	CString GetText();
	BOOL GetMatchCase();
	BOOL GetMatchWholeWord();

	BOOL GetSearchAllTasklists();
	BOOL GetIncludeDone();
	BOOL GetSearchResults();

	int GetResultCount() const; // all tasklists
	int GetResultCount(int nTaskList) const;
	POSITION GetFirstResultPos() const;
	BOOL GetNextResult(POSITION& pos, FTDRESULT& res) const;
	int GetAllResults(CFTDResultsArray& results) const;

	void DeleteResults(int nTaskList);

	BOOL GetAutoSelectSingles();

protected:
	int DoModal() { return -1; } // not for public use

// Dialog Data
	//{{AFX_DATA(CFindTaskDlg)
	enum { IDD = IDD_FIND_DIALOG };
	CTabbedComboBox	m_cbFindOptions;
	CListCtrl	m_lcResults;
	int		m_bAllTasklists;
	CString	m_sResultsLabel;
	BOOL	m_bIncludeDone;
	BOOL	m_bMatchCase;
	BOOL	m_bMatchWholeWord;
	int		m_nSelFindIndex;
	FIND_WHAT	m_nFindOption;
	BOOL	m_bAutoSelectSingles;
	BOOL	m_bSearchResults;
	//}}AFX_DATA
	CMap<DWORD, DWORD, FTDRESULT, FTDRESULT&> m_mapResults;
	CPropertyPageHost m_host;
	CFindTextPage m_pageTitleComments;
	CFindPriorityPage  m_pagePriority;
	CFindRiskPage  m_pageRisk;
	CFindNumPage  m_pagePercent;
	CFindNumPage  m_pageTimeEst;
	CFindNumPage  m_pageTimeSpent;
	CFindNumPage  m_pageTaskID;
	CFindNumPage  m_pageCost;
	CFindDatePage m_pageCreationDate;
	CFindDatePage m_pageStartDate;
	CFindDatePage m_pageDueDate;
	CFindDatePage m_pageDoneDate;
	CFindDatePage m_pageLastMod;
	CFindTextPage m_pageAllocTo;
	CFindTextPage m_pageAllocBy;
	CFindTextPage m_pageCreatedBy;
	CFindTextPage m_pageStatus;
	CFindCategoryPage m_pageCategory;
	CFindTextPage m_pageExtID;
	CFindTextPage m_pageVersion;
	CFindBlankPage m_pageFlag;
	CDockManager m_dockMgr;
	BOOL m_bDockable;
	CEnToolBar m_toolbar;
	CToolbarHelper m_tbHelper;
	CLockableHeaderCtrl m_hdrResults;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTaskDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnCancel() { ShowWindow(SW_HIDE); }

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindTaskDlg)
	afx_msg void OnFind();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemActivated(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeFindoption();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelectall();
	afx_msg void OnDockright();
	afx_msg void OnUpdateDockright(CCmdUI* pCmdUI);
	afx_msg void OnDockleft();
	afx_msg void OnUpdateDockleft(CCmdUI* pCmdUI);
	afx_msg void OnUndock();
	afx_msg void OnUpdateUndock(CCmdUI* pCmdUI);
	afx_msg void OnDockbelow();
	afx_msg void OnUpdateDockbelow(CCmdUI* pCmdUI);
	afx_msg void OnSearchresults();
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	void SaveSettings();
	void ResizeDlg(int cx = 0, int cy = 0);
	void InitFieldColumn();
	void LoadSettings();
	CSize GetMinDockedSize(DM_POS nPos);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTASKDLG_H__BDCCE0F4_B91B_47AD_AE27_1DEB4BA7EA1C__INCLUDED_)
