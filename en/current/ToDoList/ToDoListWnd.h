// ToDoListWnd.h : header file
//

#if !defined(AFX_TODOLISTWND_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_)
#define AFX_TODOLISTWND_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "todoctrl.h"
#include "filteredtodoctrl.h"
#include "preferencesdlg.h"
#include "findtaskDlg.h"
#include "todoctrlmgr.h"
#include "TDImportExportMgr.h"
#include "TDLContentMgr.h"
#include "filterbar.h"

#include "..\shared\trayicon.h"
#include "..\shared\toolbarhelper.h"
#include "..\shared\filemisc.h"
#include "..\shared\enstatic.h"
#include "..\shared\ShortcutManager.h"
#include "..\shared\driveinfo.h"
#include "..\shared\entoolbar.h"
#include "..\shared\tabctrlex.h"
#include "..\shared\enrecentfilelist.h"
#include "..\shared\menuex.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\tabbedcombobox.h"
#include "..\shared\checkcombobox.h"
#include "..\shared\deferWndMove.h"
#include "..\shared\colorcombobox.h"
#include "..\shared\enBrowserctrl.h"
#include "..\3rdparty\menuiconmgr.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoListWnd dialog

const UINT WM_TDL_SHOWWINDOW = ::RegisterWindowMessage("WM_TDL_SHOWWINDOW");
const UINT WM_TDL_GETVERSION = ::RegisterWindowMessage("WM_TDL_GETVERSION");
const UINT WM_TDL_ISCLOSING = ::RegisterWindowMessage("WM_TDL_ISCLOSING");
const UINT WM_TDL_REFRESHPREFS = ::RegisterWindowMessage("WM_TDL_REFRESHPREFS");
const UINT WM_TDL_RESTORE = ::RegisterWindowMessage("WM_TDL_RESTORE");

// WM_COPYDATA 
enum 
{
	OPENTASKLIST,   // data is char*
	ADDNEWTASK,     // data is char*
	SETCOMMENTS,    // data is char*
	SELECTTASK,     // data is DWORD
};

// creation flags
enum
{
	TLD_FORCEVISIBLE		= 0x01,
    TLD_PASSWORDPROMPTING	= 0x02,
};

enum FIND_WHAT;

class CToDoListWnd : public CFrameWnd, public CDialogHelper
{
	// Construction
public:
	CToDoListWnd(); // standard constructor
	~CToDoListWnd();
	
	static int GetVersion();
	BOOL Create(DWORD dwFlags = 0, LPCTSTR szTDListPath = NULL);
	
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDoListWnd)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	virtual void OnOK() {}
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	// Implementation
protected:
	HICON m_hIcon;
	CImageList m_ilTabCtrl, m_ilToDoCtrl;
	CEnToolBar m_toolbar;
	CTrayIcon m_ti;
	CEnRecentFileList m_mruList;
	CTabCtrlEx m_tabCtrl;
	CShortcutManager m_mgrShortcuts;
	CTaskListDropTarget m_dropTarget;
	CPreferencesDlg* m_pPrefs;
	CFindTaskDlg m_findDlg;
	CTDImportExportMgr m_mgrImportExport;
	CToDoCtrlMgr m_mgrToDoCtrls;
	CFont m_fontTree, m_fontComments; // shared by all tasklists
	CMenuEx m_menubar;
	CTDLContentMgr m_mgrContent;
	CEnBrowserCtrl m_IE;
	CFilterBar m_filterBar;
	CEnStatic	m_statusBar;
	HWND m_hwndLastFocus;
	CMenuIconMgr m_mgrMenuIcons;

	struct TDCSTARTUP
	{
		TDCSTARTUP() : dwIDSel(0) {}
		void Clear() { sFilePath.Empty(); sNewTask.Empty(); sComments.Empty(); dwIDSel = 0; }

		CString	sFilePath; // tasklist to load
		DWORD dwIDSel; // task to select
		CString sNewTask, sComments; // task title/comments
	};
	TDCSTARTUP m_startupOptions;

	CDWordArray m_aPriorityColors;
	CFont m_fontMain;
	int m_nLastSelItem; // just for flicker-free todoctrl switching

	BOOL m_bVisible;
	BOOL m_bMaxTasklists;
	BOOL m_bShowFilterBar, m_bShowProjectName;
	BOOL m_bShowStatusBar, m_bShowToolbar;
	BOOL m_bInNewTask;
	BOOL m_bSaving;
	BOOL m_bInTimer;
	BOOL m_bClosing;
	BOOL m_bFindShowing;
	BOOL m_bQueryOpenAllow;
	BOOL m_bPasswordPrompting;
	BOOL m_bReloading; // on startup
	BOOL m_bStartHidden;

	// Generated message map functions
	//{{AFX_MSG(CToDoListWnd)
	afx_msg void OnViewExpandtask();
	afx_msg void OnUpdateViewExpandtask(CCmdUI* pCmdUI);
	afx_msg void OnViewCollapsetask();
	afx_msg void OnUpdateViewCollapsetask(CCmdUI* pCmdUI);
	afx_msg void OnViewCollapseall();
	afx_msg void OnViewExpandall();
	afx_msg void OnUpdateViewExpandall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewCollapseall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserExport1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserImport1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindow(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnNewtask();
	afx_msg void OnNewsubtask();
	afx_msg void OnUpdateNewsubtask(CCmdUI* pCmdUI);
	afx_msg void OnToolsTransformactivetasklist();
	afx_msg void OnViewSorttasklisttabs();
	afx_msg void OnUpdateViewSorttasklisttabs(CCmdUI* pCmdUI);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnEditInctaskpercentdone();
	afx_msg void OnUpdateEditInctaskpercentdone(CCmdUI* pCmdUI);
	afx_msg void OnEditDectaskpercentdone();
	afx_msg void OnUpdateEditDectaskpercentdone(CCmdUI* pCmdUI);
	afx_msg void OnEditDectaskpriority();
	afx_msg void OnUpdateEditDectaskpriority(CCmdUI* pCmdUI);
	afx_msg void OnEditInctaskpriority();
	afx_msg void OnUpdateEditInctaskpriority(CCmdUI* pCmdUI);
	afx_msg void OnEditFlagtask();
	afx_msg void OnUpdateEditFlagtask(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenarchive();
	afx_msg void OnUpdateFileOpenarchive(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnViewRefreshfilter();
	afx_msg void OnUpdateViewRefreshfilter(CCmdUI* pCmdUI);
	afx_msg void OnViewShowfilterbar();
	afx_msg void OnUpdateViewShowfilterbar(CCmdUI* pCmdUI);
	afx_msg void OnViewClearfilter();
	afx_msg void OnUpdateViewClearfilter(CCmdUI* pCmdUI);
	afx_msg void OnViewFilter();
	afx_msg void OnUpdateViewFilter(CCmdUI* pCmdUI);
	afx_msg void OnTabctrlPreferences();
	afx_msg void OnTasklistSelectColumns();
	afx_msg void OnViewProjectname();
	afx_msg void OnUpdateViewProjectname(CCmdUI* pCmdUI);
	afx_msg void OnEditOffsetdates();
	afx_msg void OnUpdateEditOffsetdates(CCmdUI* pCmdUI);
	afx_msg void OnPrintpreview();
	afx_msg void OnShowTimelogfile();
	afx_msg void OnUpdateShowTimelogfile(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryOpen();
	afx_msg void OnCopyTaskasRef();
	afx_msg void OnUpdateCopyTaskasRef(CCmdUI* pCmdUI);
	afx_msg void OnCopyTaskasDependency();
	afx_msg void OnUpdateCopyTaskasDependency(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnToolsCheckforupdates();
	afx_msg void OnShowKeyboardshortcuts();
	afx_msg void OnEditInsertdatetime();
	afx_msg void OnUpdateEditInsertdatetime(CCmdUI* pCmdUI);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	afx_msg LRESULT OnSelchangeFilter(WPARAM wp, LPARAM lp);
	afx_msg void OnWindow(UINT nCmdID);
	afx_msg void OnUserImport(UINT nCmdID);
	afx_msg void OnUserExport(UINT nCmdID);
	afx_msg void OnViewNext();
	afx_msg void OnUpdateViewNext(CCmdUI* pCmdUI);
	afx_msg void OnViewPrev();
	afx_msg void OnUpdateViewPrev(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnUpdateImport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtask(CCmdUI* pCmdUI);
	afx_msg void OnToolsCheckout();
	afx_msg void OnUpdateToolsCheckout(CCmdUI* pCmdUI);
	afx_msg void OnToolsCheckin();
	afx_msg void OnUpdateToolsCheckin(CCmdUI* pCmdUI);
	afx_msg void OnExport();
	afx_msg void OnUpdateExport(CCmdUI* pCmdUI);
	afx_msg void OnNexttopleveltask();
	afx_msg void OnPrevtopleveltask();
	afx_msg void OnUpdateNexttopleveltask(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrevtopleveltask(CCmdUI* pCmdUI);
	afx_msg void OnFindTasks();
	afx_msg void OnViewMovetasklistright();
	afx_msg void OnUpdateViewMovetasklistright(CCmdUI* pCmdUI);
	afx_msg void OnViewMovetasklistleft();
	afx_msg void OnUpdateViewMovetasklistleft(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnSort();
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);
	afx_msg void OnNewtaskAttop();
	afx_msg void OnUpdateNewtaskAttop(CCmdUI* pCmdUI);
	afx_msg void OnNewtaskAtbottom();
	afx_msg void OnUpdateNewtaskAtbottom(CCmdUI* pCmdUI);
	afx_msg void OnSpellcheckcomments();
	afx_msg void OnUpdateSpellcheckcomments(CCmdUI* pCmdUI);
	afx_msg void OnSpellchecktitle();
	afx_msg void OnUpdateSpellchecktitle(CCmdUI* pCmdUI);
	afx_msg void OnFileEncrypt();
	afx_msg void OnUpdateFileEncrypt(CCmdUI* pCmdUI);
	afx_msg void OnFileResetversion();
	afx_msg void OnUpdateFileResetversion(CCmdUI* pCmdUI);
	afx_msg void OnSpellcheckTasklist();
	afx_msg void OnUpdateSpellcheckTasklist(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteAfter();
	afx_msg void OnUpdateEditPasteAfter(CCmdUI* pCmdUI);
	afx_msg void OnEditTimeTrackTask();
	afx_msg void OnUpdateEditTimeTrackTask(CCmdUI* pCmdUI);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnViewNextSel();
	afx_msg void OnUpdateViewNextSel(CCmdUI* pCmdUI);
	afx_msg void OnViewPrevSel();
	afx_msg void OnUpdateViewPrevSel(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnToolsToggleCheckin();
	afx_msg void OnUpdateToolsToggleCheckin(CCmdUI* pCmdUI);
	afx_msg void OnSplitTaskIntoPieces(UINT nCmdID);
	afx_msg void OnUpdateSplitTaskIntoPieces(CCmdUI* pCmdUI);
	afx_msg void OnToolsShowtasksDue(UINT nCmdID);
	afx_msg void OnDeleteTask();
	afx_msg void OnDeleteAllTasks();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnNew();
	afx_msg void OnUpdateDeletetask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTaskcolor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTaskdone(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeletealltasks(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNew(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNewtaskAttopSelected();
	afx_msg void OnNewtaskAtbottomSelected();
	afx_msg void OnNewtaskAfterselectedtask();
	afx_msg void OnNewtaskBeforeselectedtask();
	afx_msg void OnNewsubtaskAtbottom();
	afx_msg void OnNewsubtaskAttop();
	afx_msg void OnEditTaskcolor();
	afx_msg void OnEditTaskdone();
	afx_msg void OnEditTasktext();
	afx_msg void OnMovetaskdown();
	afx_msg void OnUpdateMovetaskdown(CCmdUI* pCmdUI);
	afx_msg void OnMovetaskup();
	afx_msg void OnUpdateMovetaskup(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnTrayiconClose();
	afx_msg void OnUpdateNewsubtaskAtBottom(CCmdUI* pCmdUI);
	afx_msg void OnSaveas();
	afx_msg void OnUpdateSaveas(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTrayiconShow();
	afx_msg void OnTrayiconShowDueTasks(UINT nCmdID);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
	afx_msg void OnAbout();
	afx_msg void OnPreferences();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnEditCopy();
	afx_msg void OnEditPasteSub();
	afx_msg void OnEditCopyastext();
	afx_msg void OnEditCopyashtml();
	afx_msg void OnUpdateEditPasteSub(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskAttopSelected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskAtbottomSelected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskAfterselectedtask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskBeforeselectedtask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewsubtaskAttop(CCmdUI* pCmdUI);
	afx_msg void OnSimplemode();
	afx_msg void OnUpdateSimplemode(CCmdUI* pCmdUI);
	afx_msg void OnReload();
	afx_msg void OnUpdateReload(CCmdUI* pCmdUI);
	afx_msg void OnArchiveCompletedtasks();
	afx_msg void OnUpdateArchiveCompletedtasks(CCmdUI* pCmdUI);
	afx_msg void OnPrint();
	afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);
	afx_msg void OnMovetaskright();
	afx_msg void OnUpdateMovetaskright(CCmdUI* pCmdUI);
	afx_msg void OnMovetaskleft();
	afx_msg void OnUpdateMovetaskleft(CCmdUI* pCmdUI);
	afx_msg void OnSelchangeTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMBtnClickTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseTasklist();
	afx_msg void OnSaveall();
	afx_msg void OnUpdateSaveall(CCmdUI* pCmdUI);
	afx_msg void OnCloseall();
	afx_msg void OnUpdateCloseall(CCmdUI* pCmdUI);
	afx_msg void OnExit();
	afx_msg void OnUpdateExit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMovetask(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnImportTasklist();
	afx_msg void OnSortBy(UINT nCmdID);
	afx_msg void OnUpdateSortBy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTasktext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnTrayiconCreatetask();
	afx_msg void OnSetPriority(UINT nCmdUI);
	afx_msg void OnUpdateSetPriority(CCmdUI* pCmdUI);
	afx_msg void OnEditSetfileref();
	afx_msg void OnUpdateEditSetfileref(CCmdUI* pCmdUI);
	afx_msg void OnEditOpenfileref();
	afx_msg void OnUpdateEditOpenfileref(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserTool(CCmdUI* pCmdUI);
	afx_msg void OnUserTool(UINT nCmdID);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnTrayIconClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrayIconDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrayIconRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNeedUserTooltipText(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg LRESULT OnGetIcon(WPARAM bLargeIcon, LPARAM /*not used*/);
	afx_msg LRESULT OnToDoCtrlNotifySort(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyMod(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyMinWidthChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyTimeTrack(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlDoTaskLink(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoCtrlTaskIsDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoListShowWindow(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyListChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoListGetVersion(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoListIsClosing(WPARAM /*wp*/, LPARAM /*lp*/) { return m_bClosing; }
	afx_msg LRESULT OnToDoListRefreshPrefs(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoListRestore(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindDlgFind(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindSelectResult(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindSelectAll(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindDlgClose(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDropFile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPreferencesTestTool(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPreferencesClearMRU(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPostOnCreate(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnWebUpdateWizard(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnAddToolbarTools(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnHotkey(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPowerBroadcast(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnGetFont(WPARAM wp, LPARAM lp);

#ifdef _DEBUG
	afx_msg void OnDebugQueryEndSession() { SendMessage(WM_QUERYENDSESSION); }
#endif
	DECLARE_MESSAGE_MAP()
		
	// OnTimer helpers
	void OnTimerReadOnlyStatus(int nCtrl = -1);
	void OnTimerTimestampChange(int nCtrl = -1);
	void OnTimerAutoSave();
	void OnTimerCheckoutStatus(int nCtrl = -1);
	void OnTimerDueItems(int nCtrl = -1);
	void OnTimerTimeTracking();
	
	virtual void LoadSettings();
	virtual void SaveSettings();
	CRect OffsetCtrl(UINT uCtrlID, int cx = 0, int cy = 0);
	void RestorePosition();
	void RestoreVisibility();
	
	// tdc not const because we need to flush it first
	BOOL Export2Html(CFilteredToDoCtrl& tdc, LPCTSTR szFilePath, LPCTSTR szStylesheet = NULL) const;
	BOOL Export2Html(const CTaskFile& tasks, LPCTSTR szFilePath, LPCTSTR szStylesheet = NULL) const;

	TDC_FILE OpenTaskList(LPCTSTR szFilePath, BOOL bNotifyDueTasks = TRUE); // 0 = failed, 1 = success, -1 = cancelled
	TDC_FILE OpenTaskList(CFilteredToDoCtrl* pCtrl, LPCTSTR szFilePath = NULL, LPCTSTR szArchivePath = NULL, TDC_ARCHIVE nRemove = TDC_REMOVEALL);
	void ReloadTaskList(int nIndex, BOOL bNotifyDueTasks);
	BOOL NewTask(LPCTSTR szTitle, TDC_INSERTWHERE nInsertWhere, BOOL bSelect = TRUE, BOOL bEdit = TRUE);
	TDC_SORTBY GetSortBy(UINT nSortID);
	UINT GetSortID(TDC_SORTBY nSortBy);
	void CheckMinWidth();
	void MinimizeToTray();
	void Show(BOOL bAllowToggle);
	BOOL InitCheckboxImageList();
	BOOL DoDueTaskNotification(const CFilteredToDoCtrl* pCtrl, int nDueBy);
	BOOL DoDueTaskNotification(int nDueBy); // works on active tasklist
	void DoExit();
	void InitShortcutManager();
	void InitMenuIconManager();
	void PauseTimeTracking(BOOL bPause = TRUE);
	void SetTimer(UINT nTimerID, BOOL bOn);
	void EnsureVisible();
	void RefreshTabOrder();
	void UpdateGlobalHotkey();
	LPCTSTR GetFileFilter();
	LPCTSTR GetDefaultFileExt();
	void DoPreferences(int nInitPage = -1);
	void DoPrint(BOOL bPreview = FALSE);
	void HandleLoadTasklistError(TDC_FILE nErr, LPCTSTR szTasklist);
	void ShowColumn(TDC_COLUMN nColumn, CFilteredToDoCtrl& tdc, BOOL bShow, BOOL bUpdate);
	BOOL LoadMenubar();
	void CheckForUpdates(BOOL bManual);
	void UpdateCwd();
	void InitUIFont();

	enum { CT_ASHTML, CT_ASTEXT, CT_ASREF, CT_ASDEPENDS };
	void CopySelectedTasksToClipboard(int nAsFormat);

	void RefreshFilterControls();
	void RefreshFilterLabelAlignment();
	
	void ResizeDlg(int cx = 0, int cy = 0, BOOL bMaximized = FALSE);
	BOOL CalcToDoCtrlRect(CRect& rect, int cx = 0, int cy = 0, BOOL bMaximized = FALSE);
	int ReposTabBar(CDeferWndMove& dwm, const CPoint& ptOrg, int nWidth, BOOL bCalcOnly = FALSE);

	void PrepareEditMenu(CMenu* pMenu);

	int MapFindWhat(FIND_WHAT fw);
	void AddFindResult(const SEARCHRESULT& result, const SEARCHPARAMS& params, int nTaskList, LPCTSTR szTaskList);
	
	BOOL InitToolbar();
	void PrepareToolbar(int nOption);
	void SetToolbarOption(int nOption);
	void AppendTools2Toolbar(BOOL bAppend = TRUE);
	
	CFilteredToDoCtrl& GetToDoCtrl();
	CFilteredToDoCtrl& GetToDoCtrl(int nIndex);
	const CFilteredToDoCtrl& GetToDoCtrl() const;
	const CFilteredToDoCtrl& GetToDoCtrl(int nIndex) const;
	CFilteredToDoCtrl* NewToDoCtrl();
	BOOL CreateToDoCtrl(CFilteredToDoCtrl* pCtrl);
	int AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bResizeDlg = TRUE);
	inline int GetTDCCount() { return m_mgrToDoCtrls.GetCount(); }
	BOOL SelectToDoCtrl(LPCTSTR szFilePath, BOOL bCheckPassword);
	BOOL SelectToDoCtrl(int nIndex, BOOL bCheckPassword);
	int GetSelToDoCtrl() const;
	BOOL CreateNewTaskList(BOOL bAddDefTask);
	BOOL VerifyToDoCtrlPassword() const;
	BOOL VerifyToDoCtrlPassword(int nIndex) const;
	
	// caller must flush todoctrls if required before calling these
	BOOL CloseToDoCtrl(int nIndex);
	BOOL ConfirmSaveTaskList(int nIndex, BOOL bClosingTaskList, BOOL bClosingWindows);
	BOOL SaveTaskList(int nIndex, LPCTSTR szFilePath = NULL); // returns FALSE only if the user was prompted to save and cancelled
	BOOL SaveAll(BOOL bIncUnsaved, BOOL bClosingTaskLists, BOOL bClosingWindows, BOOL bFlush); // returns FALSE only if the user was prompted to save and cancelled
	
	void UpdateCaption();
	void UpdateStatusbar();
	void UpdateTabSwitchTooltip();
		
	void UpdateToDoCtrlPreferences(CFilteredToDoCtrl* pCtrl);
	void UpdateToDoCtrlPreferences();
	CPreferencesDlg& Prefs() const;
	void ResetPrefs();

	UINT MapNewTaskPos(int nPos, BOOL bSubtask);
	UINT GetNewTaskCmdID();
	UINT GetNewSubtaskCmdID();

	int MessageBox(UINT nIDText, UINT nIDCaption, UINT nType = MB_OK);
	int MessageBox(LPCTSTR szText, UINT nIDCaption, UINT nType = MB_OK);
	int MessageBox(LPCTSTR szText, LPCTSTR szCaption, UINT nType = MB_OK)
	{
		return CFrameWnd::MessageBox(szText, szCaption, nType);
	}

	int GetTasks(int nTDC, BOOL bWantCompleted, BOOL bWantIncomplete,
               BOOL bWantSelected, BOOL bWantSelSubtasks, BOOL bHtmlComments, 
			   BOOL bTransform, CTaskFile& tasks);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOLISTWND_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_)
