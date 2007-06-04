#if !defined(AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_)
#define AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesDlg.h : header file
//

#include "preferencesgenpage.h"
#include "preferencestaskpage.h"
#include "preferencestaskdefpage.h"
#include "preferencestoolpage.h"
#include "preferencesuipage.h"
#include "preferencesuitasklistpage.h"
#include "preferencesuitasklistcolorspage.h"
#include "preferencesshortcutspage.h"
#include "preferencesfilepage.h"
#include "preferencesexportpage.h"
#include "preferencesmultiuserpage.h"

#include "..\shared\propertypagehost.h"

// matches order of pages
enum 
{ 
	PREFPAGE_GEN, 
	PREFPAGE_MULTIUSER, 
	PREFPAGE_FILE, 
	PREFPAGE_UI, 
	PREFPAGE_UITASK, 
	PREFPAGE_TASK, 
	PREFPAGE_TASKDEF, 
	PREFPAGE_TOOL, 
	PREFPAGE_SHORTCUT 
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

class CPreferencesDlg : public CDialog
{
// Construction
public:
	CPreferencesDlg(CShortcutManager* pShortcutMgr = NULL, UINT nMenuID = 0, 
					const CContentMgr* pContentMgr = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreferencesDlg();

	int DoModal(int nInitPage = -1);

	BOOL GetAlwaysOnTop() const { return m_pageGen.GetAlwaysOnTop(); }
	BOOL GetUseSysTray() const { return m_pageGen.GetUseSysTray(); }
	BOOL GetAutoSaveOnSysTray() const { return m_pageGen.GetAutoSaveOnSysTray(); }
	BOOL GetConfirmDelete() const { return m_pageGen.GetConfirmDelete(); }
	BOOL GetConfirmSaveOnExit() const { return m_pageGen.GetConfirmSaveOnExit(); }
	BOOL GetShowOnStartup() const { return m_pageGen.GetShowOnStartup(); }
	int GetSysTrayOption() const { return m_pageGen.GetSysTrayOption(); }
	BOOL GetToggleTrayVisibility() const { return m_pageGen.GetToggleTrayVisibility(); }
	BOOL GetMultiInstance() const { return m_pageGen.GetMultiInstance(); }
	BOOL GetGlobalHotkey() const { return m_pageGen.GetGlobalHotkey(); }
	BOOL GetAddFilesToMRU() const { return m_pageGen.GetAddFilesToMRU(); }
	BOOL GetEnableTDLExtension() const { return m_pageGen.GetEnableTDLExtension(); }
	BOOL GetAutoSaveUnsavedOnMinimize() const { return m_pageGen.GetAutoSaveUnsavedOnMinimize(); }
	BOOL GetAutoCheckForUpdates() const { return m_pageGen.GetAutoCheckForUpdates(); }
	BOOL GetEscapeMinimizes() const { return m_pageGen.GetEscapeMinimizes(); }

	int GetReadonlyReloadOption() const { return m_pageMultiUser.GetReadonlyReloadOption(); }
	int GetTimestampReloadOption() const { return m_pageMultiUser.GetTimestampReloadOption(); }
	BOOL GetEnableSourceControl() const { return m_pageMultiUser.GetEnableSourceControl(); }
	BOOL GetSourceControlLanOnly() const { return m_pageMultiUser.GetSourceControlLanOnly(); }
	BOOL GetAutoCheckOut() const { return m_pageMultiUser.GetAutoCheckOut(); }
	BOOL GetCheckoutOnCheckin() const { return m_pageMultiUser.GetCheckoutOnCheckin(); }
	BOOL GetCheckinOnClose() const { return m_pageMultiUser.GetCheckinOnClose(); }
	UINT GetRemoteFileCheckFrequency() const { return m_pageMultiUser.GetRemoteFileCheckFrequency(); }
	UINT GetCheckinOnNoEditTime() const { return m_pageMultiUser.GetCheckinOnNoEditTime(); }
	BOOL GetFormatXmlOutput() const { return m_pageMultiUser.GetFormatXmlOutput(); }

	int GetNotifyDueByOnLoad() const { return m_pageFile.GetNotifyDueByOnLoad(); }
	int GetNotifyDueByOnSwitch() const { return m_pageFile.GetNotifyDueByOnSwitch(); }
	BOOL GetDisplayDueTasksInHtml() const { return m_pageFile.GetDisplayDueTasksInHtml(); }
	BOOL GetAutoArchive() const { return m_pageFile.GetAutoArchive(); }
	BOOL GetNotifyReadOnly() const { return m_pageFile.GetNotifyReadOnly(); }
	BOOL GetRemoveArchivedTasks() const { return m_pageFile.GetRemoveArchivedTasks(); }
	BOOL GetRemoveOnlyOnAbsoluteCompletion() const { return m_pageFile.GetRemoveOnlyOnAbsoluteCompletion(); }
	BOOL GetAutoHtmlExport() const { return m_pageFile.GetAutoHtmlExport(); }
	int GetAutoSaveFrequency() const { return m_pageFile.GetAutoSaveFrequency(); }
	CString GetAutoExportFolderPath() const { return m_pageFile.GetAutoExportFolderPath(); }
	BOOL GetRefreshFindOnLoad() const { return m_pageFile.GetRefreshFindOnLoad(); }
	BOOL GetDueTaskTitlesOnly() const { return m_pageFile.GetDueTaskTitlesOnly(); }
	CString GetDueTaskStylesheet() const { return m_pageFile.GetDueTaskStylesheet(); }
	CString GetSaveExportStylesheet() const { return m_pageFile.GetSaveExportStylesheet(); }
	CString GetDueTaskPerson() const { return m_pageFile.GetDueTaskPerson(); }
	BOOL GetWarnAddDeleteArchive() const { return m_pageFile.GetWarnAddDeleteArchive(); }
	BOOL GetAutoSaveUnsaved() const { return m_pageFile.GetAutoSaveUnsaved(); }
	BOOL GetDontRemoveFlagged() const { return m_pageFile.GetDontRemoveFlagged(); }
	BOOL GetExpandTasksOnLoad() const { return m_pageFile.GetExpandTasksOnLoad(); }

	CString GetHtmlFont() const { return m_pageExport.GetHtmlFont(); }
	int GetHtmlFontSize() const { return m_pageExport.GetHtmlFontSize(); }
	BOOL GetPreviewExport() const { return m_pageExport.GetPreviewExport(); }
	int GetTextIndent() const { return m_pageExport.GetTextIndent(); }
	BOOL GetExportParentTitleCommentsOnly() const { return m_pageExport.GetExportParentTitleCommentsOnly(); }
	BOOL GetExportSpaceForNotes() const { return m_pageExport.GetExportSpaceForNotes(); }
	BOOL GetExportVisibleColsOnly() const { return m_pageExport.GetExportVisibleColsOnly(); }
	CString GetHtmlCharSet() const { return m_pageExport.GetHtmlCharSet(); }

	int GetDefaultPriority() const { return m_pageTaskDef.GetDefaultPriority(); }
	int GetDefaultRisk() const { return m_pageTaskDef.GetDefaultRisk(); }
	CString GetDefaultAllocTo() const { return m_pageTaskDef.GetDefaultAllocTo(); }
	CString GetDefaultAllocBy() const { return m_pageTaskDef.GetDefaultAllocBy(); }
	CString GetDefaultStatus() const { return m_pageTaskDef.GetDefaultStatus(); }
	int GetDefaultCategories(CStringArray& aCats) const { return m_pageTaskDef.GetDefaultCategories(aCats); }
	CString GetDefaultCreatedBy() const { return m_pageTaskDef.GetDefaultCreatedBy(); }
	double GetDefaultTimeEst(int& nUnits) const { return m_pageTaskDef.GetDefaultTimeEst(nUnits); }
	COLORREF GetDefaultColor() const { return m_pageTaskDef.GetDefaultColor(); }
	BOOL GetAutoDefaultStartDate() const { return m_pageTaskDef.GetAutoDefaultStartDate(); }
	BOOL GetUseParentAttrib(PTP_ATTRIB nAttrib) const { return m_pageTaskDef.GetUseParentAttrib(nAttrib); }
	int GetCategoryList(CStringArray& aItems) const { return m_pageTaskDef.GetCategoryList(aItems); }
	int GetStatusList(CStringArray& aItems) const { return m_pageTaskDef.GetStatusList(aItems); }
	int GetAllocToList(CStringArray& aItems) const { return m_pageTaskDef.GetAllocToList(aItems); }
	int GetAllocByList(CStringArray& aItems) const { return m_pageTaskDef.GetAllocByList(aItems); }
	double GetDefaultCost() const { return m_pageTaskDef.GetDefaultCost(); }

	BOOL GetAveragePercentSubCompletion() const { return m_pageTask.GetAveragePercentSubCompletion(); }
	BOOL GetIncludeDoneInAverageCalc() const { return m_pageTask.GetIncludeDoneInAverageCalc(); }
	BOOL GetUseEarliestDueDate() const { return m_pageTask.GetUseEarliestDueDate(); }
	BOOL GetUsePercentDoneInTimeEst() const { return m_pageTask.GetUsePercentDoneInTimeEst(); }
	BOOL GetTreatSubCompletedAsDone() const { return m_pageTask.GetTreatSubCompletedAsDone(); }
	BOOL GetUseHighestPriority() const { return m_pageTask.GetUseHighestPriority(); }
	BOOL GetAutoCalcTimeEstimates() const { return m_pageTask.GetAutoCalcTimeEstimates(); }
	BOOL GetIncludeDoneInPriorityCalc() const { return m_pageTask.GetIncludeDoneInPriorityCalc(); }
	BOOL GetWeightPercentCompletionByTimeEst() const { return m_pageTask.GetWeightPercentCompletionByTimeEst(); }
	BOOL GetWeightPercentCompletionByPriority() const { return m_pageTask.GetWeightPercentCompletionByPriority(); }
	BOOL GetWeightPercentCompletionByNumSubtasks() const { return m_pageTask.GetWeightPercentCompletionByNumSubtasks(); }
	BOOL GetAutoCalcPercentDone() const { return m_pageTask.GetAutoCalcPercentDone(); }
	BOOL GetTrackActiveTasklistOnly() const { return m_pageTask.GetTrackActiveTasklistOnly(); }
	BOOL GetTrackSelectedTaskOnly() const { return m_pageTask.GetTrackSelectedTaskOnly(); }
	BOOL GetNoTrackOnScreenSaver() const { return m_pageTask.GetNoTrackOnScreenSaver(); }
	double GetHoursInOneDay() const { return m_pageTask.GetHoursInOneDay(); }
	double GetDaysInOneWeek() const { return m_pageTask.GetDaysInOneWeek(); }
	BOOL GetLogTimeTracking() const { return m_pageTask.GetLogTimeTracking(); }
	BOOL GetLogTaskTimeSeparately() const { return m_pageTask.GetLogTaskTimeSeparately(); }
	BOOL GetExclusiveTimeTracking() const { return m_pageTask.GetExclusiveTimeTracking(); }
	BOOL GetAllowParentTimeTracking() const { return m_pageTask.GetAllowParentTimeTracking(); }
	BOOL GetAutoAdjustDependents() const { return m_pageTask.GetAutoAdjustDependents(); }

	BOOL GetShowCtrlsAsColumns() const { return m_pageUI.GetShowCtrlsAsColumns(); }
	BOOL GetShowEditMenuAsColumns() const { return m_pageUI.GetShowEditMenuAsColumns(); }
	BOOL GetShowCommentsAlways() const { return m_pageUI.GetShowCommentsAlways(); }
	BOOL GetAutoReposCtrls() const { return m_pageUI.GetAutoReposCtrls(); }
	BOOL GetSharedCommentsHeight() const { return m_pageUI.GetSharedCommentsHeight(); }
	BOOL GetAutoHideTabbar() const { return m_pageUI.GetAutoHideTabbar(); }
	BOOL GetStackTabbarItems() const { return m_pageUI.GetStackTabbarItems(); }
	BOOL GetRightAlignLabels() const { return m_pageUI.GetRightAlignLabels(); }
	BOOL GetFocusTreeOnEnter() const { return m_pageUI.GetFocusTreeOnEnter(); }
	BOOL GetLargeToolbarIcons() const { return m_pageUI.GetLargeToolbarIcons(); }
	int GetNewTaskPos() const { return m_pageUI.GetNewTaskPos(); }
	int GetNewSubtaskPos() const { return m_pageUI.GetNewSubtaskPos(); }
	BOOL GetKeepTabsOrdered() const { return m_pageUI.GetKeepTabsOrdered(); }
	BOOL GetShowTasklistCloseButton() const { return m_pageUI.GetShowTasklistCloseButton(); }
	BOOL GetEnableCtrlMBtnClose() const { return m_pageUI.GetEnableCtrlMBtnClose(); }
	BOOL GetAutoReSort() const { return m_pageUI.GetAutoReSort(); }
	BOOL GetSortDoneTasksAtBottom() const { return m_pageUI.GetSortDoneTasksAtBottom(); }
	BOOL GetSortVisibleOnly() const { return m_pageUI.GetSortVisibleOnly(); }
	BOOL GetEnableHeaderSorting() const { return m_pageUI.GetEnableHeaderSorting(); }
	BOOL GetRTLComments() const { return m_pageUI.GetRTLComments(); }
	int GetCommentsPos() const { return m_pageUI.GetCommentsPos(); }
	int GetDefaultCommentsFormat() const { return m_pageUI.GetDefaultCommentsFormat(); }
	BOOL GetMultiSelCategoryFilter() const { return m_pageUI.GetMultiSelCategoryFilter(); }
	BOOL GetAutoReFilter() const { return m_pageUI.GetAutoReFilter(); }
	BOOL GetRestoreTasklistFilters() const { return m_pageUI.GetRestoreTasklistFilters(); }

	BOOL GetShowButtonsInTree() const { return m_pageUITasklist.GetShowButtonsInTree(); }
	BOOL GetShowInfoTips() const { return m_pageUITasklist.GetShowInfoTips(); }
	BOOL GetShowComments() const { return m_pageUITasklist.GetShowComments(); }
	BOOL GetShowColumn(TDLB_COLUMN nColumn) const { return m_pageUITasklist.GetShowColumn(nColumn); }
	BOOL GetShowPathInHeader() const { return m_pageUITasklist.GetShowPathInHeader(); }
	BOOL GetStrikethroughDone() const { return m_pageUITasklist.GetStrikethroughDone(); }
	BOOL GetFullRowSelection() const { return m_pageUITasklist.GetFullRowSelection(); }
	BOOL GetTreeCheckboxes() const { return m_pageUITasklist.GetTreeCheckboxes(); }
	BOOL GetDisplayDatesInISO() const { return m_pageUITasklist.GetDisplayDatesInISO(); }
	BOOL GetShowWeekdayInDates() const { return m_pageUITasklist.GetShowWeekdayInDates(); }
	BOOL GetShowParentsAsFolders() const { return m_pageUITasklist.GetShowParentsAsFolders(); }
	BOOL GetDisplayFirstCommentLine() const { return m_pageUITasklist.GetDisplayFirstCommentLine(); }
	int GetMaxInfoTipCommentsLength() const { return m_pageUITasklist.GetMaxInfoTipCommentsLength(); }
	BOOL GetHidePercentForDoneTasks() const { return m_pageUITasklist.GetHidePercentForDoneTasks(); }
	BOOL GetHideZeroTimeCost() const { return m_pageUITasklist.GetHideZeroTimeCost(); }
	BOOL GetHideStartDueForDoneTasks() const { return m_pageUITasklist.GetHideStartDueForDoneTasks(); }
	BOOL GetShowPercentAsProgressbar() const { return m_pageUITasklist.GetShowPercentAsProgressbar(); }
	BOOL GetRoundTimeFractions() const { return m_pageUITasklist.GetRoundTimeFractions(); }
	BOOL GetShowNonFilesAsText() const { return m_pageUITasklist.GetShowNonFilesAsText(); }
	BOOL GetUseHMSTimeFormat() const { return m_pageUITasklist.GetUseHMSTimeFormat(); }
	BOOL GetAutoFocusTasklist() const { return m_pageUITasklist.GetAutoFocusTasklist(); }
	BOOL GetShowSubtaskCompletion() const { return m_pageUITasklist.GetShowSubtaskCompletion(); }

	int GetTextColorOption() const { return m_pageUITasklistColors.GetTextColorOption(); }
	BOOL GetColorTaskBackground() const { return m_pageUITasklistColors.GetColorTaskBackground(); }
	BOOL GetCommentsUseTreeFont() const { return m_pageUITasklistColors.GetCommentsUseTreeFont(); }
	BOOL GetColorPriority() const { return m_pageUITasklistColors.GetColorPriority(); }
	int GetPriorityColors(CDWordArray& aColors) const { return m_pageUITasklistColors.GetPriorityColors(aColors); }
	BOOL GetTreeFont(CString& sFaceName, int& nPointSize) const { return m_pageUITasklistColors.GetTreeFont(sFaceName, nPointSize); }
	BOOL GetCommentsFont(CString& sFaceName, int& nPointSize) const { return m_pageUITasklistColors.GetCommentsFont(sFaceName, nPointSize); }
	COLORREF GetGridlineColor() const { return m_pageUITasklistColors.GetGridlineColor(); }
	COLORREF GetTaskDoneColor() const { return m_pageUITasklistColors.GetTaskDoneColor(); }
	COLORREF GetTaskDueColor() const { return m_pageUITasklistColors.GetTaskDueColor(); }
	COLORREF GetHidePriorityNumber() const { return m_pageUITasklistColors.GetHidePriorityNumber(); }
	COLORREF GetAlternateLineColor() const { return m_pageUITasklistColors.GetAlternateLineColor(); }
	int GetCategoryColors(CCatColorArray& aColors) const { return m_pageUITasklistColors.GetCategoryColors(aColors); }

	int GetUserTools(CUserToolArray& aTools) const { return m_pageTool.GetUserTools(aTools); }
	BOOL GetUserTool(int nTool, USERTOOL& tool) const { return m_pageTool.GetUserTool(nTool, tool); } 

//	BOOL Get() const { return m_b; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	enum { IDD = IDD_PREFERENCES };
	CTreeCtrl m_tcPages;
	//}}AFX_DATA
	int m_nInitPage;

	CPropertyPageHost m_pphost;
	CPreferencesGenPage m_pageGen;
	CPreferencesFilePage m_pageFile;
	CPreferencesExportPage m_pageExport;
	CPreferencesUIPage m_pageUI;
	CPreferencesUITasklistPage m_pageUITasklist;
	CPreferencesUITasklistColorsPage m_pageUITasklistColors;
	CPreferencesTaskPage m_pageTask;
	CPreferencesTaskDefPage m_pageTaskDef;
	CPreferencesToolPage m_pageTool;
	CPreferencesShortcutsPage m_pageShortcuts;
	CPreferencesMultiUserPage m_pageMultiUser;
	CMapPtrToPtr m_mapPP2HTI;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnHelp();
	afx_msg void OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void AddPage(CPropertyPage* pPage, UINT nIDPath);
	void SetActivePage(int nPage);
	CString GetItemPath(HTREEITEM hti);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_)
