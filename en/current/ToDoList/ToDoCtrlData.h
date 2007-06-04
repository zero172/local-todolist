// ToDoCtrlData.h: interface for the CToDoCtrlData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODOCTRLDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
#define AFX_TODOCTRLDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tdcstruct.h"
#include "tdcenum.h"
#include <afxtempl.h>
#include "..\3rdparty\fixalloc.h" 

class TODOITEM
{
public:
	TODOITEM(LPCTSTR szTitle, LPCTSTR szComments = NULL); 
	TODOITEM(); 
	TODOITEM(const TODOITEM& tdi); 
	
	BOOL HasLastMod() const;
	BOOL HasCreation() const;
	BOOL HasStart() const;
	BOOL HasDue() const;
	BOOL IsDone() const;
	
	void ClearStart();
	void ClearDue();
	void ClearDone();
	
	BOOL IsDue() const;
	BOOL IsDue(const COleDateTime& dateDueBy) const;
	
	void SetModified();
	void ResetCalcs(); 

	CString GetFirstCategory() const;
	CString GetFirstAllocTo() const;

	BOOL GetNextOccurence(COleDateTime& dtNext) const;

	//  ------------------------------------------
	
	CString sTitle;
	CString sComments, sCustomComments;
	COLORREF color;
	COleDateTime dateStart, dateDue, dateDone, dateCreated;
	int nPriority;
	double dCost;
	CStringArray aAllocTo;
	CString sAllocBy;
	CString sStatus;
	CStringArray aCategories;
	CString sCreatedBy;
	int nPercentDone;
	CString sFileRefPath;
	double dTimeEstimate, dTimeSpent;
	int nTimeEstUnits, nTimeSpentUnits;
	COleDateTime tLastMod;
	BOOL bFlagged;
	int nRisk;
	CString sExternalID;
	CString sDependency;
	TDIRECURRENCE trRecurrence;
	CString sVersion;
	
	// cached calculations for drawing optimization
	// mutable so that they can be updated in const methods
	mutable int nCalcPriority;
	mutable int nCalcPercent;
	mutable int nCalcRisk;
	mutable double dCalcTimeEstimate, dCalcTimeSpent;
	mutable double dCalcCost;
	mutable COleDateTime dateEarliestDue;
	mutable BOOL bGoodAsDone, bDue;
	mutable int nSubtasksCount, nSubtasksDone;
	
	DECLARE_FIXED_ALLOC(TODOITEM);
};

typedef CMap<DWORD, DWORD, TODOITEM*, TODOITEM*&> CTDIMap;
typedef CMap<DWORD, DWORD, HTREEITEM, HTREEITEM&> CHTIMap;

class CToDoCtrlData  
{
public:
	CToDoCtrlData(CTreeCtrl& tree, const CWordArray& aStyles);
	virtual ~CToDoCtrlData();
	
	inline UINT GetTaskCount() const { return m_mapTDItems.GetCount(); }
	inline DWORD GetTaskID(HTREEITEM hti) const { return hti ? m_tree.GetItemData(hti) : 0; }
	
	HTREEITEM GetItem(DWORD dwID) const;
	
	TODOITEM* NewTask(const TODOITEM* pTDIRef = NULL);
	TODOITEM* GetTask(DWORD dwID) const;
	TODOITEM* GetTask(HTREEITEM hti) const;
	void DeleteTask(DWORD dwID);
	void DeleteAllTasks(BOOL bIncTree = TRUE);
	void AddTask(DWORD dwID, TODOITEM* pTDI);

	double GetEarliestDueDate() const;
	
	void Sort(TDC_SORTBY nBy, BOOL bAscending, HTREEITEM htiRoot = NULL);
	void SetSortDueTodayHigh(BOOL bHigh);
	
	// Gets
	CString GetTaskTitle(DWORD dwID) const;
	COleDateTime GetTaskDate(DWORD dwID, TDC_DATE nDate) const;
	BOOL IsTaskDone(DWORD dwID) const;
	COLORREF GetTaskColor(DWORD dwID) const; // -1 on no item selected
	CString GetTaskComments(DWORD dwID) const;
	CString GetTaskCustomComments(DWORD dwID) const;
	int GetTaskPercent(DWORD dwID, BOOL bCheckIfDone) const;
	double GetTaskTimeEstimate(DWORD dwID, int& nUnits) const;
	double GetTaskTimeSpent(DWORD dwID, int& nUnits) const;
	double GetTaskCost(DWORD dwID) const;
	int GetTaskAllocTo(DWORD dwID, CStringArray& aAllocTo) const;
	CString GetTaskAllocBy(DWORD dwID) const;
	CString GetTaskCreatedBy(DWORD dwID) const;
	CString GetTaskStatus(DWORD dwID) const;
	int GetTaskCategories(DWORD dwID, CStringArray& aCategories) const;
	CString GetTaskDependency(DWORD dwID) const;
	CString GetTaskFileRef(DWORD dwID) const;
	CString GetTaskExtID(DWORD dwID) const;
	int GetTaskPriority(DWORD dwID) const;
	int GetTaskRisk(DWORD dwID) const;
	BOOL IsTaskFlagged(DWORD dwID) const;
	BOOL GetTaskRecurrence(DWORD dwID, TDIRECURRENCE& tr) const;
	BOOL GetTaskNextOccurrence(DWORD dwID, COleDateTime& dtNext) const;
	CString GetTaskVersion(DWORD dwID) const;

	COLORREF GetTaskColor(HTREEITEM hti) const;
	double GetTaskTimeEstimate(HTREEITEM hti, int& nUnits) const;
	double GetTaskTimeSpent(HTREEITEM hti, int& nUnits) const;
	int GetTaskPercent(HTREEITEM hti) const;
	int GetTaskAllocTo(HTREEITEM hti, CStringArray& aAllocTo) const;
	CString GetTaskAllocBy(HTREEITEM hti) const;
	CString GetTaskStatus(HTREEITEM hti) const;
	int GetTaskCategories(HTREEITEM hti, CStringArray& aCategories) const;
	CString GetTaskExtID(HTREEITEM hti) const;
	int GetTaskPriority(HTREEITEM hti) const;
	int GetTaskRisk(HTREEITEM hti) const;
	BOOL DeleteTask(HTREEITEM hti);
	int GetItemPos(HTREEITEM hti, HTREEITEM htiSearch) const; // returns 0 if not found
	BOOL IsParentTaskDone(HTREEITEM hti) const; // this is recursive
	int AreChildTasksDone(HTREEITEM hti) const; // 1=yes, 0=no, -1=no children
	BOOL IsTaskFlagged(HTREEITEM hti) const;
	double GetTaskCost(HTREEITEM hti) const;
	COleDateTime GetTaskDate(HTREEITEM hti, TDC_DATE nDate) const;
	BOOL GetTaskRecurrence(HTREEITEM hti, TDIRECURRENCE& tr) const;
	CString GetTaskVersion(HTREEITEM hti) const;
	
	int GetTaskDependents(DWORD dwTaskID, CDWordArray& aDependents) const;
	int GetTaskDependencies(DWORD dwTaskID, CDWordArray& aDepends) const;
	BOOL HasCircularDependencies(DWORD dwTaskID) const;

	BOOL IsTaskDue(HTREEITEM hti, BOOL bToday = FALSE) const;
	double GetEarliestDueDate(HTREEITEM hti, BOOL bCheckChildren) const;
	int GetHighestPriority(HTREEITEM hti, BOOL bIncludeDue = TRUE) const;
	int GetHighestRisk(HTREEITEM hti) const;
	int CalcPercentDone(HTREEITEM hti) const;
	double CalcCost(HTREEITEM hti) const;
	double CalcTimeEstimate(HTREEITEM hti, int nUnits) const;
	double CalcTimeSpent(HTREEITEM hti, int nUnits) const;
	BOOL GetSubtaskTotals(HTREEITEM hti, int& nSubtasksTotal, int& nSubtasksDone) const;
	CString GetTaskPath(HTREEITEM hti, int nMaxElementLen = -1) const; // nMaxElementLen relates to each element of the path

	CString GetLongestVisibleExternalID(HTREEITEM hti) const;
	CString GetLongestVisibleCategory(HTREEITEM hti) const;
	CString GetLongestVisibleAllocTo(HTREEITEM hti) const;

	CString GetLongestVisibleExternalID() const;
	CString GetLongestVisibleCategory() const;
	CString GetLongestVisibleAllocTo() const;
	
	CString GetLongestVisibleExternalID(HTREEITEM hti, const TODOITEM* pTDI) const;
	CString GetLongestVisibleCategory(HTREEITEM hti, const TODOITEM* pTDI) const;
	CString GetLongestVisibleAllocTo(HTREEITEM hti, const TODOITEM* pTDI) const;

	BOOL IsTaskDue(HTREEITEM hti, const TODOITEM* tdi, BOOL bToday = FALSE) const;
	double GetEarliestDueDate(HTREEITEM hti, const TODOITEM* pTDI, BOOL bCheckChildren) const;
	int GetHighestPriority(HTREEITEM hti, const TODOITEM* pTDI, BOOL bIncludeDue = TRUE) const;
	int GetHighestRisk(HTREEITEM hti, const TODOITEM* pTDI) const;
	double CalcCost(HTREEITEM hti, const TODOITEM* pTDI) const;
	double CalcTimeEstimate(HTREEITEM hti, const TODOITEM* pTDI, int nUnits) const;
	double CalcTimeSpent(HTREEITEM hti, const TODOITEM* pTDI, int nUnits) const;
	int CalcPercentDone(HTREEITEM hti, const TODOITEM* pTDI) const;
	int CalcPercentFromTime(HTREEITEM hti, const TODOITEM* pTDI) const; // spent / estimate
	BOOL GetSubtaskTotals(HTREEITEM hti, const TODOITEM* pTDI, 
							int& nSubtasksTotal, int& nSubtasksDone) const;
	
	// Sets. 0 = failed, 1 = success, -1 = success (no change)
	int SetTaskDate(DWORD dwID, TDC_DATE nDate, const COleDateTime& date);
	int OffsetTaskDate(DWORD dwID, TDC_DATE nDate, int nAmount, int nUnits);
	int SetTaskDone(DWORD dwID, BOOL bDone, int& nPrevPercent);
	int SetTaskColor(DWORD dwID, COLORREF color);
	int ClearTaskColor(DWORD dwID) { SetTaskColor(dwID, (COLORREF)-1); }
	int SetTaskComments(DWORD dwID, LPCTSTR szComments);
	int SetTaskCustomComments(DWORD dwID, const CString& sComments); // can't be LPCTSTR cos not NULL terminated
	int SetTaskPercent(DWORD dwID, int nPercent);
	int SetTaskTimeEstimate(DWORD dwID, const double& dTime, int nUnits = TDITU_HOURS);
	int SetTaskTimeSpent(DWORD dwID, const double& dTime, int nUnits = TDITU_HOURS);
	int SetTaskCost(DWORD dwID, const double& dCost);
	int SetTaskAllocTo(DWORD dwID, const CStringArray& aAllocTo);
	int SetTaskAllocBy(DWORD dwID, LPCTSTR szAllocBy);
	int SetTaskStatus(DWORD dwID, LPCTSTR szStatus);
	int SetTaskCategories(DWORD dwID, const CStringArray& aCategories);
	int SetTaskDependency(DWORD dwID, LPCTSTR szDepends);
	int SetTaskExtID(DWORD dwID, LPCTSTR szID);
	int SetTaskFileRef(DWORD dwID, LPCTSTR szFilePath);
	int SetTaskPriority(DWORD dwID, int nPriority); // 0-10 (10 is highest)
	int SetTaskRisk(DWORD dwID, int nRisk); // 0-10 (10 is highest)
	int SetTaskTitle(DWORD dwID, LPCTSTR szTitle);
	int SetTaskAttributeAsParent(HTREEITEM hti, TDC_ATTRIBUTE nAttrib);
	int SetTaskFlag(DWORD dwID, BOOL bFlagged);
	int SetTaskRecurrence(DWORD dwID, const TDIRECURRENCE& tr);
	int SetTaskVersion(DWORD dwID, LPCTSTR szVersion);

	int OffsetTaskDate(HTREEITEM hti, TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks);
	
	int FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const;
	DWORD FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	BOOL TaskMatches(DWORD dwID, const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	
	void BuildHTIMap(CHTIMap& mapHTI, HTREEITEM htiRoot = NULL) const;
	void UpdateHTIMapEntry(CHTIMap& mapHTI, HTREEITEM hti) const;
	BOOL IsTaskFullyDone(HTREEITEM hti, const TODOITEM* pTDI, BOOL bCheckSiblings) const;
	
	void ResetCachedCalculations();
	
	BOOL IsTaskTimeTrackable(HTREEITEM hti) const;
	BOOL IsTaskDone(HTREEITEM hti, DWORD dwExtraCheck = TDCCHECKNONE) const;
	void ApplyLastChangeToSubtasks(HTREEITEM hti, const TODOITEM* pTDI, TDC_ATTRIBUTE nAttrib);
	
	int SaveTreeExpandedState(LPCTSTR szRegKey, HTREEITEM hti = NULL, int nStart = 0); 
	void LoadTreeExpandedState(LPCTSTR szRegKey); 
	
	//	void RefreshItemCheckState(HTREEITEM hti, int nDone = -1, BOOL bRefreshParents = FALSE);
	//	void RefreshAllCheckStates(HTREEITEM htiRoot = NULL);
	
	static int MapTimeUnits(const CString& sUnits);
	static CString MapTimeUnits(int nUnits);
	
protected:
	CTDIMap m_mapTDItems; // the real data
	CTreeCtrl& m_tree; // CToDoCtrl tree
	const CWordArray& m_aStyles; // CToDoCtrl styles
	BOOL m_bSortDueTodayHigh;
	
protected:
	DWORD FindFirstTask(HTREEITEM hti, const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	int FindTasks(HTREEITEM hti, const SEARCHPARAMS& params, CResultArray& aResults) const;
	BOOL TaskMatches(HTREEITEM hti, const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	
	// internal use only
	void SumPercentDone(HTREEITEM hti, const TODOITEM* pTDI,
		double& dTotalPercent, double& dTotalWeighting) const;
	
	static BOOL TaskMatches(const COleDateTime& date, const SEARCHPARAMS& params, SEARCHRESULT& result);
	static BOOL TaskMatches(const CString& sText, const SEARCHPARAMS& params, SEARCHRESULT& result);
	static BOOL TaskMatches(const double& dValue, const SEARCHPARAMS& params, SEARCHRESULT& result);
	static BOOL TaskMatches(int nValue, const SEARCHPARAMS& params, SEARCHRESULT& result);
	static BOOL TaskMatches(const CStringArray& aItems, const SEARCHPARAMS& params, SEARCHRESULT& result);
	
	inline BOOL HasStyle(int nStyle) const { return m_aStyles[nStyle] ? TRUE : FALSE; }
	
	// for sorting
	// sort helper structure
	class CToDoCtrlData;
	
	struct TDSORTSTRUCT
	{
		CToDoCtrlData* pData;
		const CHTIMap* pMapHTItems;
		TDC_SORTBY nSortBy;
		BOOL bAscending;
	};
	
	void Sort(HTREEITEM hti, const TDSORTSTRUCT& ss);
	int CompareTasks(TODOITEM* pTDI1, HTREEITEM hti1, TODOITEM* pTDI2, HTREEITEM hti2, 
					 TDC_SORTBY nSortBy, BOOL bAscending);

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
	static int Compare(const COleDateTime& date1, const COleDateTime& date2);
	static int Compare(const CString& sText1, const CString& sText2, BOOL bCheckEmpty = FALSE);
	static int Compare(int nNum1, int nNum2);
	static int Compare(const double& dNum1, const double& dNum2);
	
};

#endif // !defined(AFX_TODOCTRLDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
