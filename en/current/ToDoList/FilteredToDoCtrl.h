// FilteredToDoCtrl.h: interface for the CFilteredToDoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTEREDTODOCTRL_H__356A6EB9_C7EC_4395_8716_623AFF4A269B__INCLUDED_)
#define AFX_FILTEREDTODOCTRL_H__356A6EB9_C7EC_4395_8716_623AFF4A269B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToDoCtrl.h"
#include "..\shared\misc.h"

class CFilteredToDoCtrl : public CToDoCtrl  
{
public:
	CFilteredToDoCtrl(CContentMgr& mgr, const CONTENTFORMAT& cfDefault);
	virtual ~CFilteredToDoCtrl();

	void SetFilter(const FTDCFILTER& filter);
	FILTER_TYPE GetFilter(FTDCFILTER& filter) const;
	void RefreshFilter();
	void ClearFilter();
	BOOL HasFilter() const { return m_filter.IsSet(); }

	UINT GetTaskCount(UINT* pVisible = 0) const;
	BOOL DeleteSelectedTask() { return CToDoCtrl::DeleteSelectedTask(); }

	HTREEITEM NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere = TDC_INSERTATTOPOFSELTASKPARENT, 
					BOOL bSelect = TRUE, BOOL bEditText = TRUE);

	void SetModified(BOOL bMod = TRUE) { CToDoCtrl::SetModified(bMod); }
	BOOL IsTaskDone(DWORD dwTaskID) const;
	BOOL HasTask(DWORD dwTaskID);
	BOOL SelectTask(DWORD dwTaskID);

protected:
	CTaskFile m_tasksHidden;
	FTDCFILTER m_filter;
	COleDateTime m_dateDue;
	UINT m_nHiddenCount;
	
protected:
	virtual int GetAllTasks(CTaskFile& tasks) const;
	virtual HTREEITEM SetAllTasks(const CTaskFile& tasks);

	void FilterTasks(CTaskFile& tasksHide, HTASKITEM htHide, CTaskFile& tasksShow, HTASKITEM htShow);
	FTMATCHRESULT MatchFilter(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchCategory(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchStatus(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchAllocTo(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchAllocBy(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchPriority(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchRisk(const CTaskFile& tasks, HTASKITEM ht) const;
	BOOL MatchDueDate(const CTaskFile& tasks, HTASKITEM ht) const;
	void InitDueDate();
	BOOL ModNeedsRefilter(TDC_ATTRIBUTE nModType);

	BOOL IsTaskDone(const CTaskFile& tasks, HTASKITEM ht, BOOL bCheckParents = TRUE) const; // checks parents
	virtual BOOL DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel = FALSE);
	virtual BOOL SelectedTasksHaveChildren() const;
	virtual void SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib);
	virtual TODOITEM* NewTask();
};

#endif // !defined(AFX_FILTEREDTODOCTRL_H__356A6EB9_C7EC_4395_8716_623AFF4A269B__INCLUDED_)
