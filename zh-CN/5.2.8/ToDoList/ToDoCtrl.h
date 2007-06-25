#if !defined(AFX_TODOCTRL_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TODOCTRL_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToDoCtrl.h : header file
//

#include "TaskListDropTarget.h"
#include "todoctrldata.h"
#include "taskfile.h"
#include "tdcstruct.h"
#include "tdlprioritycombobox.h"
#include "tdlriskcombobox.h"
#include "recurringtaskedit.h"

#include "..\shared\runtimedlg.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\orderedtreectrl.h"
#include "..\shared\fileedit.h"
#include "..\shared\sysimagelist.h"
#include "..\shared\urlricheditctrl.h"
#include "..\shared\colorcombobox.h"
#include "..\shared\autocombobox.h"
#include "..\shared\maskedit.h"
#include "..\shared\timeedit.h"
#include "..\shared\TreeDragDropHelper.h"
#include "..\shared\wndPrompt.h"
#include "..\shared\contentmgr.h"
#include "..\shared\encheckcombobox.h"
//#include "..\shared\endatetimectrl.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrl dialog

// predeclarations
class CXmlItem;
class CTaskFile;
class CDeferWndMove;
class CDlgUnits;
struct CTRLITEM;
struct TDCCOLUMN;
class CSpellCheckDlg;

class CToDoCtrl : public CRuntimeDlg, protected CDialogHelper
{
// Construction
public:
	CToDoCtrl(CContentMgr& mgr, int nDefaultContent = 0);
	virtual ~CToDoCtrl();

	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bVisible = TRUE);

	TDC_FILE Save(LPCTSTR szFilePath = NULL, BOOL bCheckforLaterChanges = TRUE);
	TDC_FILE Save(CTaskFile& tasks/*out*/, LPCTSTR szFilePath = NULL, BOOL bCheckforLaterChanges = TRUE);
	TDC_FILE Load(LPCTSTR szFilePath, LPCTSTR szArchivePath = NULL, 
					TDC_ARCHIVE nRemove = TDC_REMOVEALL, BOOL bRemoveFlagged = TRUE);
	BOOL DelayLoad(LPCTSTR szFilePath, COleDateTime& dtEarliestDue);

	BOOL Import(LPCTSTR szFilePath, BOOL bAppend);

	int ArchiveDoneTasks(LPCTSTR szFilePath, TDC_ARCHIVE nFlags, BOOL bRemoveFlagged); // returns the number of tasks removed

	void EnableEncryption(BOOL bEnable = TRUE);
	BOOL IsEncrypted() const { return (!m_sPassword.IsEmpty()); }
	static BOOL CanEncrypt(); // returns true only if the required encrytion capabilities are present
	CString GetPassword() const { return m_sPassword; }
	BOOL VerifyPassword(LPCTSTR szExplanation = NULL) const;

	BOOL CheckIn();
	BOOL CheckOut();
	BOOL CheckOut(CString& sCheckedOutTo);
	BOOL IsCheckedOut() const { return m_bCheckedOut; }
	BOOL IsSourceControlled() const { return m_bSourceControlled; }

	void Flush(); // called to end current editing actions
	BOOL IsModified() const { return m_bModified && !IsReadOnly(); }
	void SetModified(BOOL bMod = TRUE);

	inline CString GetFilePath() const { return m_sLastSavePath; }
	inline void ClearFilePath() { m_sLastSavePath.Empty(); }
	inline CString GetProjectName() const { return m_sProjectName; }
	void SetProjectName(LPCTSTR szProjectName);
	CString GetFriendlyProjectName(int nUntitledIndex = -1) const;
	inline void SetFilePath(LPCTSTR szPath) { m_sLastSavePath = szPath; }

	BOOL DeleteAllTasks();
	void NewList();

	int GetTasklist(CTaskFile& tasklist, DWORD dwFlags = 0) const;
	int GetTasks(CTaskFile& tasks, TDC_FILTER nFilter, DWORD dwFlags = 0, LPCTSTR szAllocatedTo = NULL) const;
	int GetSelectedTasks(CTaskFile& tasks, DWORD dwFlags = 0) const;
	BOOL SetTasks(const CTaskFile& tasks, BOOL bAppend);

	void SetReadonly(BOOL bReadOnly) { SetStyle(TDCS_READONLY, bReadOnly); }
	BOOL IsReadOnly() const { return HasStyle(TDCS_READONLY); }

	void SetStyles(const CTDCStyles& styles);
	void SetStyle(TDC_STYLE nStyle, BOOL bOn = TRUE); // one style at a time only 
	BOOL HasStyle(TDC_STYLE nStyle) const; // one style at a time only 

	void SetVisibleColumns(const CTDCColumnArray& aColumns);
	int GetVisibleColumns(CTDCColumnArray& aColumns) const;
	BOOL IsColumnShowing(TDC_COLUMN nColumn) const;

	BOOL SetPriorityColors(const CDWordArray& aColors); // must have 12 elements
	COLORREF GetPriorityColor(int nPriority) const;
    void SetGridlineColor(COLORREF color) { m_tree.SetGridlineColor(color); }
	void SetTaskCompletedColor(COLORREF color);
	void SetCategoryColors(const CTDCColorMap& colors);
	BOOL GetCategoryColor(LPCTSTR szCategory, COLORREF& color) const;
	void SetDueTaskColors(COLORREF crDue, COLORREF crDueToday);
	void GetDueTaskColors(COLORREF& crDue, COLORREF& crDueToday) { crDue = m_crDue; crDueToday = m_crDueToday; }

	// these return the full list of items in each droplist
	int GetAllocToNames(CStringArray& aNames) const { return m_cbAllocTo.GetItems(aNames); }
	int GetAllocByNames(CStringArray& aNames) const { return m_cbAllocBy.GetItems(aNames); }
	int GetCategoryNames(CStringArray& aNames) const { return m_cbCategory.GetItems(aNames); }
	int GetStatusNames(CStringArray& aNames) const { return m_cbStatus.GetItems(aNames); }

	void SetDefaultAllocToNames(const CStringArray& aNames);
	void SetDefaultAllocByNames(const CStringArray& aNames);
	void SetDefaultCategoryNames(const CStringArray& aNames);
	void SetDefaultStatusNames(const CStringArray& aNames);

	HTREEITEM NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere = TDC_INSERTATTOPOFSELTASKPARENT, 
						BOOL bSelect = TRUE, BOOL bEditText = TRUE);

	BOOL SplitSelectedTask(int nNumSubtasks = 2);
	BOOL CanSplitSelectedTask();

	DWORD GetSelectedTaskID() const { return m_data.GetTaskID(GetSelectedItem()); } 
	TODOITEM* GetSelectedTask() const { return m_data.GetTask(GetSelectedItem()); }
	virtual BOOL HasTask(DWORD dwTaskID);
	virtual BOOL SelectTask(DWORD dwTaskID);
	virtual BOOL IsTaskDone(DWORD dwTaskID) const { return m_data.IsTaskDone(dwTaskID); }

	BOOL DeleteSelectedTask();
	BOOL EditSelectedTask(BOOL bTaskIsNew = FALSE); 
	void SpellcheckSelectedTask(BOOL bTitle); // else comments
	BOOL CanSpellcheckSelectedTaskComments();
	
	COleDateTime GetSelectedTaskDate(TDC_DATE nDate) const;
	BOOL SetSelectedTaskDate(TDC_DATE nDate, const COleDateTime& date);
	BOOL SetSelectedTaskDone(BOOL bDone = TRUE);
	BOOL IsSelectedTaskDone() const;
	BOOL IsSelectedTaskDue() const;
	BOOL OffsetSelectedTaskDate(TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks);

	COleDateTime GetEarliestDueDate() const { return m_data.GetEarliestDueDate(); } // entire tasklist

	COLORREF GetSelectedTaskColor() const; // -1 on no item selected
	CString GetSelectedTaskComments() const;
	CString GetSelectedTaskCustomComments() const;
	CString GetSelectedTaskTitle() const;
	double GetSelectedTaskTimeEstimate(int& nUnits) const;
	double GetSelectedTaskTimeSpent(int& nUnits) const;
//	CString GetSelectedTaskAllocTo() const;
	int GetSelectedTaskAllocTo(CStringArray& aAllocTo) const;
	CString GetSelectedTaskAllocBy() const;
	CString GetSelectedTaskStatus() const;
	int GetSelectedTaskCategories(CStringArray& aCats) const;
	CString GetSelectedTaskFileRef() const;
	CString GetSelectedTaskExtID() const;
	int GetSelectedTaskPercent() const;
	CString GetSelectedTaskDependency() const;
	int GetSelectedTaskPriority() const;
	int GetSelectedTaskRisk() const;
	double GetSelectedTaskCost() const;
	BOOL IsSelectedTaskFlagged() const;
	BOOL GetSelectedTaskRecurrence(TDIRECURRENCE& tr) const;
	CString GetSelectedTaskVersion() const;
	BOOL GetCustomCommentsType(GUID& idType) const;

	double CalcSelectedTaskTimeEstimate(int nUnits = TDITU_HOURS) const;
	double CalcSelectedTaskTimeSpent(int nUnits = TDITU_HOURS) const;
	double CalcSelectedTaskCost() const;

	BOOL SetSelectedTaskColor(COLORREF color);
	BOOL ClearSelectedTaskColor() { return SetSelectedTaskColor((COLORREF)-1); }
	BOOL SetSelectedTaskTitle(LPCTSTR szTitle);
	BOOL SetSelectedTaskPercentDone(int nPercent);
	BOOL SetSelectedTaskTimeEstimate(const double& dHours, int nUnits = TDITU_HOURS);
	BOOL SetSelectedTaskTimeSpent(const double& dHours, int nUnits = TDITU_HOURS);
	BOOL SetSelectedTaskAllocTo(const CStringArray& aAllocTo);
//	BOOL SetSelectedTaskAllocTo(LPCTSTR szAllocTo);
	BOOL SetSelectedTaskAllocBy(LPCTSTR szAllocBy);
	BOOL SetSelectedTaskStatus(LPCTSTR szStatus);
	BOOL SetSelectedTaskCategories(const CStringArray& aCats);
	BOOL SetSelectedTaskPriority(int nPriority); // 0-10 (10 is highest)
	BOOL SetSelectedTaskRisk(int nRisk); // 0-10 (10 is highest)
	BOOL SetSelectedTaskFileRef(LPCTSTR szFilePath);
	BOOL SetSelectedTaskDependency(LPCTSTR szDepends);
	BOOL SetSelectedTaskExtID(LPCTSTR szID);
	BOOL SetSelectedTaskFlag(BOOL bFlagged);
	BOOL SetSelectedTaskCost(const double& dCost);
	BOOL SetSelectedTaskRecurrence(const TDIRECURRENCE& tr);
	BOOL SetSelectedTaskVersion(LPCTSTR szVersion);
	BOOL SetSelectedTaskComments(LPCTSTR szComments, const CString& sCustomComments);

	BOOL CanPasteText(); // into focused control
	BOOL PasteText(LPCTSTR szText); // into focused control

	BOOL IncrementSelectedTaskPercentDone(int nAmount); // +ve or -ve
	BOOL IncrementSelectedTaskPriority(int nAmount); // +ve or -ve

	BOOL GotoSelectedTaskFileRef();
	CString GetSelectedTaskPath();

	void PauseTimeTracking(BOOL bPause = TRUE) { m_bTimeTrackingPaused = bPause; }
	BOOL TimeTrackSelectedTask();
	BOOL CanTimeTrackSelectedTask() const;
	BOOL IsSelectedTaskBeingTimeTracked() const;
	BOOL IsActivelyTimeTracking() const; // this instant
	CString GetSelectedTaskTimeLogPath() const;
	void EndTimeTracking();

	BOOL SetSelectedTaskAttributeAsParent(TDC_ATTRIBUTE nAttrib);

	void SetDefaultTaskAttributes(LPCTSTR szTitle, LPCTSTR szComments, COLORREF color,
									const double& dTimeEst, int nTimeEstUnits, 
									const CStringArray& aAllocTo/*LPCTSTR szAllocTo*/,
									LPCTSTR szAllocBy, LPCTSTR szStatus, const CStringArray& aCats,
									int nPriority, int nRisk, BOOL bAutoStartDate, 
									LPCTSTR szCreatedBy, double dCost);

	void Sort(TDC_SORTBY nBy); // calling twice with the same param will toggle ascending attrib
	TDC_SORTBY GetSortBy() const { return m_nSortBy; }
	void Resort() { Sort(m_nSortBy); }

	// move functions
	BOOL MoveSelectedTask(TDDH_MOVE nDirection);
	BOOL CanMoveSelectedTask(TDDH_MOVE nDirection) const;

	void GotoTopLevelTask(TDC_GOTO nDirection); 
	BOOL CanGotoTopLevelTask(TDC_GOTO nDirection) const;

	BOOL CanExpandSelectedTask(BOOL bExpand) const;
	void ExpandSelectedTask(BOOL bExpand = TRUE);
	void ExpandAllTasks(BOOL bExpand = TRUE);

	// copy/paste functions
	BOOL CutSelectedTask();
	BOOL CopySelectedTask() const;
	void ClearCopiedItem() const;
	BOOL PasteOnSelectedTask();
	BOOL PasteAfterSelectedTask();
	BOOL CanPaste() const;
	static void ReleaseClipboard() { s_clipboard.tasks.Reset(); }

	void CToDoCtrl::ResetFileVersion(unsigned int nTo = 0) { m_nFileVersion = max(nTo, 0); }
	DWORD CToDoCtrl::GetFileVersion() const { return m_nFileVersion == 0 ? 1 : m_nFileVersion; }
    TDC_FILEFMT CompareFileFormat() const; // older, same, newer
	
	// tree related
	inline HTREEITEM GetSelectedItem() const;
	inline UINT GetTaskCount() const { return m_data.GetTaskCount(); }
	inline BOOL ItemHasChildren(HTREEITEM hti) const { return m_tree.ItemHasChildren(hti); }
	inline BOOL ItemHasParent(HTREEITEM hti) const { return (NULL != m_tree.GetParentItem(hti)); }
	inline BOOL TreeHasFocus() { return (GetFocus() == &m_tree); }
	inline CString GetItemText(HTREEITEM hti) const { return m_data.GetTaskTitle(m_tree.GetItemData(hti)); }
	inline int GetSelectedCount() const { return Selection().GetCount(); }
	inline BOOL IsItemSelected(HTREEITEM hti) const { return Selection().HasItem(hti); }
	inline BOOL HasSelection() const { return GetSelectedCount(); }
	inline CString GetItemPath(HTREEITEM hti, int nMaxElementLen = -1) const { return m_tree.TCH().GetItemPath(hti, nMaxElementLen); }
	inline void SetAlternateLineColor(COLORREF color) { m_tree.SetAlternateLineColor(color); }
	
	void SetFocusToTree();
	BOOL IsTreeLabelEditing();
	void SelectItem(HTREEITEM hti);
	void SelectAll();
	void DeselectAll(); // call externally only
	BOOL MultiSelectItems(const CDWordArray& aTasks, int nState = 1, BOOL bRedraw = TRUE);
	BOOL MultiSelectItem(HTREEITEM hti, int nState = 1, BOOL bRedraw = TRUE);
	BOOL MultiSelectItems(HTREEITEM htiFrom, HTREEITEM htiTo, int nState = 1, BOOL bRedraw = TRUE);

	BOOL CanSelectNextTasksInHistory() const { return Selection().HasNextSelection(); }
	void SelectNextTasksInHistory();
	BOOL CanSelectPrevTasksInHistory() const { return Selection().HasPrevSelection(); }
	void SelectPrevTasksInHistory();

	BOOL SetCheckImageList(HIMAGELIST hImageList); // setter responsible for deleting
	BOOL SetTreeFont(HFONT hFont); // setter responsible for deleting
	BOOL SetCommentsFont(HFONT hFont); // setter responsible for deleting

	int FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const;
	DWORD FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	BOOL TaskMatches(DWORD dwID, const SEARCHPARAMS& params, SEARCHRESULT& result) const;

	// misc
	int GetMinWidth();
	void Spellcheck();
	void SetMaxInfotipCommentsLength(int nLength) { m_nMaxInfotipCommentsLength = max(-1, nLength); } // -1 to switch off
	COleDateTime GetLastTaskModified() { return m_tLastTaskMod; }
	BOOL PtInHeader(CPoint ptScreen) const;

	static void ParseTaskLink(LPCTSTR szLink, DWORD& dwTaskID, CString& sFile);
	static BOOL IsReservedShortcut(DWORD dwShortcut);

protected:
	CDateTimeCtrl m_dateStart, m_dateDue, m_dateDone;
	CTDLPriorityComboBox m_cbPriority;
	CTDLRiskComboBox m_cbRisk;
	CEnEdit m_eExternalID, m_eDependency;
	CSpinButtonCtrl m_spinPercent;
	CMaskEdit m_ePercentDone, m_eCost;
	CAutoComboBox /*m_cbAllocTo, */m_cbAllocBy;
	CAutoComboBox m_cbStatus;
	CCheckComboBox m_cbCategory, m_cbAllocTo;
	CTimeEdit m_eTimeEstimate, m_eTimeSpent;
	CFileEdit m_eFileRef;
	CContentCtrl m_ctrlComments;
	COrderedTreeCtrl m_tree;
	CRecurringTaskEdit m_eRecurrence;
	CAutoComboBox m_cbVersion;

	HFONT m_fontTree, m_fontDone, m_fontComments, m_fontBold;
	HIMAGELIST m_hilDone;
	CSysImageList m_ilFileRef;
	CBrush m_brDue, m_brDueToday;
	COLORREF m_crDue, m_crDueToday;

	CToDoCtrlData m_data;

	TDC_SORTBY m_nSortBy;
	CWordArray m_aStyles;
	CDWordArray m_aPriorityColors;
	CString m_sXmlHeader;
	CTaskListDropTarget m_dtTree, m_dtFileRef;
	CString m_sLastSavePath;
	COLORREF m_crTaskDone;
	CString m_sMachineName; // for source control
	int m_nCommentsSize;
	CString m_sPassword;
	WORD m_wKeyPress;
	CTreeDragDropHelper m_treeDragDrop;
	HTREEITEM m_htiLastAdded, m_htiEditing;
	CTDCColorMap m_mapCatColors;
	int m_nMaxInfotipCommentsLength;
	TDLSELECTION m_selection;
	CWndPromptManager m_mgrPrompts;
	COleDateTime m_tLastTaskMod;
	CContentMgr& m_mgrContent;
	int m_nDefaultContent;

	CString m_sFileRefPath;
	CString m_sTextComments, m_CustomComments;
	CString /*m_sAllocTo, */m_sAllocBy;
	CString m_sStatus;
	CStringArray m_aCategory, m_aAllocTo;
	CString m_sProjectName;
	CString m_sExternalID, m_sDepends;
	double m_dTimeEstimate, m_dTimeSpent;
	double m_dCost;
	double m_dLogTime; // in hours
	int m_nPriority;
	int m_nRisk;
	int m_nPercentDone;
	int m_nTimeEstUnits, m_nTimeSpentUnits;
	GUID m_idCommentsType;
	TDIRECURRENCE m_tRecurrence;
	CString m_sVersion;

	DWORD m_dwNextUniqueID;
	DWORD m_nFileVersion;
	DWORD m_nFileFormat;
	DWORD m_dwVisibleColumns;
	DWORD m_dwTimeTrackTaskID;
	DWORD m_dwTickLast; // time tracking

	BOOL m_bModified;
	BOOL m_bSortAscending;
	BOOL m_bArchive;
	BOOL m_bModSinceLastSort;
	BOOL m_bCheckedOut; // intentionally not a style
	BOOL m_bSplitting; // dragging comments splitter
	BOOL m_bTimeTrackingPaused;
	BOOL m_bSourceControlled;

	static TDLCLIPBOARD s_clipboard;
	static int s_nCommentsSize; // TDCS_SHAREDCOMMENTSHEIGHT
	static TODOITEM s_tdDefault;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDoCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void PreSubclassWindow();
	
// Implementation
protected:
	void UpdateComments(BOOL bSaveAndValidate);

	// Generated message map functions
	//{{AFX_MSG(CToDoCtrl)
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTreeGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTaskDatechange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeChangeFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTreeDragDrop(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangePriority();
	afx_msg LRESULT OnChangeComments(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangePercent();
	afx_msg void OnChangeTimeEstimate();
	afx_msg void OnChangeTimeSpent();
//	afx_msg void OnEditChangeAllocTo();
	afx_msg void OnSelChangeAllocTo();
//	afx_msg void OnEditChangeAllocBy();
	afx_msg void OnSelChangeAllocBy();
//	afx_msg void OnEditChangeStatus();
	afx_msg void OnSelChangeStatus();
//	afx_msg void OnEditChangeVersion();
	afx_msg void OnSelChangeVersion();
//	afx_msg void OnEditChangeCategory();
	afx_msg void OnSelChangeCategory();
	afx_msg void OnChangeRisk();
	afx_msg void OnChangeProjectName();
	afx_msg void OnChangeCost();
	afx_msg void OnChangeDependency();
	afx_msg void OnChangeExternalID();
	afx_msg void OnChangeFileRefPath();
	afx_msg void OnChangeRecurrence();
	afx_msg LRESULT OnAutoComboSelChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterWantRedraw(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterDrawItem(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnGutterPostDrawItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterRecalcColWidth(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterNotifyHeaderClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterWidthChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetCursor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterNotifyItemClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterIsItemSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetSelectedCount(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEEBtnClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomUrl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTimeUnitsChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDropFileRef(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFileEditWantIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFileEditDisplayFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGotoFileRef();
	afx_msg LRESULT OnTreeRestoreFocusedItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTDCHasClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTDCGetClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTDCDoTaskLink(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAutoComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnRefreshPercentSpinVisibility(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

protected:
	inline TODOITEM* GetTask(DWORD dwUniqueID) const { return m_data.GetTask(dwUniqueID); }
	inline DWORD GetTaskID(HTREEITEM hti) const { return m_data.GetTaskID(hti); }

	CString FormatInfoTip(const HTREEITEM hti, const TODOITEM* tdi) const;
	void InvalidateSelectedItem();
	void InvalidateItem(HTREEITEM hti);
	void UpdateTask(TDC_ATTRIBUTE nAttrib);
	void UpdateControls(HTREEITEM hti = NULL);
	void UpdateSelectedTaskPath();
	CRect GetSplitterRect();
	void UpdateColumnHeaderClicking();
	BOOL WantExportColumn(TDC_COLUMN nColumn, BOOL bVisibleColsOnly) const;
	void SetCtrlDate(CDateTimeCtrl& ctrl, const COleDateTime& date);
	BOOL CreateContentControl();
	BOOL ModNeedsResort(TDC_ATTRIBUTE nModType);
	int GetMaxTaskCategoryWidth(CDC* pDC, BOOL bVisibleOnly = TRUE);
	void RefreshBoldStates(HTREEITEM hti = NULL);
	void IncrementTrackedTime();

	// internal version so we can tell how we've been called
	BOOL SetSelectedTaskComments(LPCTSTR szComments, const CString& sCustomComments, BOOL bInternal);

	// internal version so we can control the prompting for subtask completion
	BOOL SetSelectedTaskDate(TDC_DATE nDate, const COleDateTime& date, BOOL bPromptCompleteSubtasks);

	int SetTaskDone(HTREEITEM hti, BOOL bDone, int& nPrevPercent, BOOL bAndChildren);
	int SetTaskDoneDate(HTREEITEM hti, const COleDateTime& date, BOOL bAndChildren);
	BOOL SetSelectedTaskDone(BOOL bDone, BOOL bUpdatePercent);
	int CheckWantSubtasksCompleted();

	virtual void SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib);
	virtual TODOITEM* NewTask(); // overridable
	virtual BOOL DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel = FALSE);
	virtual BOOL SelectedTasksHaveChildren() const;
	virtual int GetAllTasks(CTaskFile& tasks) const;
	virtual HTREEITEM SetAllTasks(const CTaskFile& tasks);

	int ArchiveDoneTasks(LPCTSTR szFilePath, TDC_ARCHIVE nRemove, 
						BOOL bRemoveFlagged, BOOL bHoldRedraw); // returns the number of tasks removed

	BOOL IsSortable() const;

	void ProcessItemLButtonDown(HTREEITEM htiHit, int nHitFlags, UINT nColID);
	void ProcessItemLButtonUp(HTREEITEM htiHit, int nHitFlags, UINT nColID);

	BOOL CopyCurrentSelection(BOOL bCut) const;
	BOOL IsClipboardEmpty() const;
	inline CTreeSelectionHelper& Selection() { return m_selection.selection; }
	inline const CTreeSelectionHelper& Selection() const { return m_selection.selection; }

	void Resize(int cx = 0, int cy = 0);
	int VisibleCtrlCount();
	void ReposControl(int nCtrl, CDeferWndMove* pDWM, const CDlgUnits* pDLU, int nCol, 
						int nTop, int nClientRight);
	void ReposComments(int cx, int cy, CDeferWndMove* pDWM, CRect& rComments /*out*/);
	int GetMinNonCommentSize();

	void SetLabelAlignment(int nStyle);
	void InitEditPrompts();

	int AddChildren(HTREEITEM hti, CTaskFile& file, HTASKITEM hTask, const TDCFILTER& filter) const;
	BOOL AddItem(HTREEITEM hti, CTaskFile& file, HTASKITEM hParentTask, const TDCFILTER& filter, int nPos) const;

	HTREEITEM AddItem(const CTaskFile& file, HTASKITEM hTask, HTREEITEM htiParent = NULL, HTREEITEM htiAfter = TVI_LAST, TDC_RESETIDS nResetID = TDCR_NO);
	BOOL AddToItem(const CTaskFile& tasks, HTREEITEM htiDest, HTREEITEM htiDestAfter, TDC_RESETIDS nResetID);
	HTREEITEM InsertItem(LPCTSTR szText, HTREEITEM htiParent, HTREEITEM htiAfter, BOOL bSelect, BOOL bEdit/*, BOOL bSort*/);

	BOOL Load(const CTaskFile& file, LPCTSTR szArchivePath = NULL, 
				TDC_ARCHIVE nRemove = TDC_REMOVEALL, BOOL bRemoveFlagged = TRUE);
	BOOL CheckRestoreBackupFile(LPCTSTR szFilePath);

	TDCCOLUMN* GetColumn(UINT nColID); 
	TDCCOLUMN* GetColumn(TDC_SORTBY nSortBy);

	void SaveExpandedState(); // keyed by last filepath
	HTREEITEM LoadExpandedState(BOOL bResetSel = TRUE); // returns the previously selected item if any
	void SaveSplitPos();
	void LoadSplitPos();
	void SaveSortState(); // keyed by last filepath
	void LoadSortState(LPCTSTR szFilePath); // keyed by filepath

	void TimeTrackTask(HTREEITEM hti);
	BOOL IsActivelyTimeTracking(BOOL bCheckScreenSaver) const; // this instant

	BOOL SetTextChange(int nChange, CString& sItem, LPCTSTR szNewItem, TDC_ATTRIBUTE nAttrib, UINT nIDC);
	void DrawGutterItemText(CDC* pDC, const CString& sText, const CRect& rect, int nAlign, COLORREF crText);
	BOOL GetTaskTextColors(HTREEITEM hti, const TODOITEM* pTDI, BOOL bDone, COLORREF& crText, COLORREF& crBack);

	void RecalcSelectedTimeEstimate();
	BOOL SpellcheckItem(HTREEITEM hti, CSpellCheckDlg* pSpellChecker, BOOL bTitle, BOOL bNotifyNoErrors);
	BOOL SpellcheckItem(HTREEITEM hti, CSpellCheckDlg* pSpellChecker);
	BOOL CanSpellcheckComments();

	static void InitExportFilter(TDCFILTER& filter, TDC_FILTER nFilter, DWORD dwFlags, LPCTSTR szAllocatedTo);

	BOOL GotoFile(const CString& sUrl, BOOL bShellExecute);
	BOOL ShowTaskLink(DWORD dwTaskID, LPCTSTR szLink);
	BOOL SelectionHasIncompleteDependencies() const;
	void ShowTaskHasDependenciesError();

	void FixupTaskDependentsDates(DWORD dwTaskID);
	BOOL AdjustTaskDates(DWORD dwTaskID, DWORD dwDependencyID);
	

	enum TDI_STATE 
	{
		TDIS_NONE,
		TDIS_SELECTED,
		TDIS_SELECTEDNOTFOCUSED,
		TDIS_DROPHILITED,
		TDIS_SELECTEDDROPHILITED,
	};

	TDI_STATE GetItemState(HTREEITEM hti);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOCTRL_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
