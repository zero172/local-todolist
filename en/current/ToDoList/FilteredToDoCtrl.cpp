// FilteredToDoCtrl.cpp: implementation of the CFilteredToDoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilteredToDoCtrl.h"
#include "resource.h"

#include "..\shared\holdredraw.h"
#include "..\shared\datehelper.h"
#include "..\shared\enstring.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilteredToDoCtrl::CFilteredToDoCtrl(CContentMgr& mgr, const CONTENTFORMAT& cfDefault) :
	CToDoCtrl(mgr, cfDefault), m_nHiddenCount(0)
{
}

CFilteredToDoCtrl::~CFilteredToDoCtrl()
{

}

int CFilteredToDoCtrl::GetAllTasks(CTaskFile& tasks) const
{
	if (!m_nHiddenCount) // nothing to merge in
		return CToDoCtrl::GetAllTasks(tasks);

	// else	merge back in any tasks that were previously removed
	// to make sure that changes to the visible tasks (esp structure)
	// are not overwritten by the hidden tasks we need to merge
	// from the visible tasks -> hidden tasks.
	CTaskFile tasksSrc;

	if (CToDoCtrl::GetAllTasks(tasksSrc))
	{
		tasks.Copy(m_tasksHidden);
		tasks.Merge(tasksSrc, TRUE, TRUE);
	}
	else // nothing showing so just copy hidden
		tasks.Copy(m_tasksHidden);

	return tasks.GetTaskCount();
}

HTREEITEM CFilteredToDoCtrl::SetAllTasks(const CTaskFile& tasks)
{
	HTREEITEM hti = NULL;

	if (HasFilter())
	{
		int nTaskCount = tasks.GetTaskCount();

		// this will contain the visible
		CTaskFile tasksShow;
		tasksShow.Copy(tasks);
		
		// this will contain the non-visible tasks. ie mutually exclusive
		m_tasksHidden.Copy(tasks);

		FilterTasks(m_tasksHidden, NULL, tasksShow, NULL);

		// work out how many are hidden
		m_nHiddenCount = nTaskCount - tasksShow.GetTaskCount();

		TRACE ("CFilteredToDoCtrl::SetAllTasks(%d showing, %d hidden)\n", 
				tasksShow.GetTaskCount(), m_tasksHidden.GetTaskCount());

		m_mgrPrompts.SetPrompt(m_tree, CEnString(IDS_TDC_FILTEREDTASKLISTPROMPT), TVM_GETCOUNT);

		hti = CToDoCtrl::SetAllTasks(tasksShow);
	}
	else
	{
		m_mgrPrompts.SetPrompt(m_tree, CEnString(IDS_TDC_TASKLISTPROMPT), TVM_GETCOUNT);

		m_tasksHidden.Reset();
		m_nHiddenCount = 0;
	
		hti = CToDoCtrl::SetAllTasks(tasks);
	}

	m_tree.Invalidate();
	return hti;
}

FILTER_TYPE CFilteredToDoCtrl::GetFilter(FTDCFILTER& filter) const
{
	filter = m_filter;
	return m_filter.nFilter;
}

void CFilteredToDoCtrl::SetFilter(const FTDCFILTER& filter)
{
	if (m_filter == filter)
		return; // no change

	m_filter = filter;
	RefreshFilter();
}
	
void CFilteredToDoCtrl::ClearFilter()
{
	if (HasFilter())
	{
		FTDCFILTER filter; // empty filter
		SetFilter(filter);
	}
}

UINT CFilteredToDoCtrl::GetTaskCount(UINT* pVisible) const
{
	int nVisible = CToDoCtrl::GetTaskCount();
	
	if (pVisible)
		*pVisible = nVisible;

	return (nVisible + m_nHiddenCount);
}

void CFilteredToDoCtrl::RefreshFilter() 
{
	CHoldRedraw hr(m_tree);
	
	CWaitCursor cursor;
	CTaskFile tasks;

	if (GetAllTasks(tasks))
	{
		// save and restore focus
		HWND hwndFocus = ::GetFocus();

		if (CToDoCtrl::GetTaskCount())
			SaveExpandedState();

		InitDueDate();
		
		HTREEITEM htiFirst = SetAllTasks(tasks);
		m_data.ResetCachedCalculations();

		// restore last expanded state
		HTREEITEM htiSel = LoadExpandedState(FALSE);
		
		if (!htiSel)
			htiSel = htiFirst;
		
		if (htiSel)
		{
			SelectItem(htiSel);
			m_tree.EnsureVisible(htiSel);
		}

		if (hwndFocus != ::GetFocus())
			::SetFocus(hwndFocus);

		// redo last sort
		if (GetSortBy() != TDC_SORTDISABLED)
		{
			m_bModSinceLastSort = TRUE; // so the sort won't switch directions
			Sort(GetSortBy());
		}
	}
}

void CFilteredToDoCtrl::FilterTasks(CTaskFile& tasksHide, HTASKITEM htHide, 
									CTaskFile& tasksShow, HTASKITEM htShow) 
{
	// iterate both lists at the same time
	HTASKITEM htchildHide = tasksHide.GetFirstTask(htHide);
	HTASKITEM htchildShow = tasksShow.GetFirstTask(htShow);

	while (htchildHide)
	{
		ASSERT (htchildShow);
		DWORD dwID = tasksShow.GetTaskID(htchildShow);

		// Note: because we will be hiding tasks from the underlying CToDoCtrl
		// we become responsible for validating m_dwNextUniqueID against the
		// actual IDs of the tasks
		m_dwNextUniqueID = max(m_dwNextUniqueID, dwID + 1); 		
		
		// save in case we later need to delete
		HTASKITEM htCacheHide = htchildHide;
		HTASKITEM htCacheShow = htchildShow;
		
		// if it has children then leave it in place but process children
		BOOL bHasChildren = (tasksShow.GetFirstTask(htchildShow) != NULL);
		
		// get next first in case we delete 
		htchildHide = tasksHide.GetNextTask(htchildHide);
		htchildShow = tasksShow.GetNextTask(htchildShow);
		
		if (bHasChildren)
		{
			// we need to handle done tasks in a special way if we have
			// the TDCS_TREATSUBCOMPLETEDASDONE style set because trying
			// to figure out whether to delete the parent after all the 
			// children have been deleted is too tricky. so if we're only
			// interested in incomplete tasks and we are sure that this
			// task is complete then we can deal with it right away
			if (m_filter.nFilter == FT_NOTDONE && HasStyle(TDCS_TREATSUBCOMPLETEDASDONE) &&
				IsTaskDone(tasksShow, htCacheShow))
			{
				tasksShow.DeleteTask(htCacheShow);
			}
			else // we go through all the tests
			{
				FilterTasks(tasksHide, htCacheHide, tasksShow, htCacheShow);
				
				// if the task previouly had children but now they've all 
				// be filtered out, decide whether the parent task needs to be 
				// deleted too
				BOOL bDelete = FALSE;
				
				if (bHasChildren && !tasksShow.GetFirstTask(htCacheShow))
				{
					bDelete = (MatchFilter(tasksShow, htCacheShow) != FTDC_MATCH);
				}

				if (bDelete)
					tasksShow.DeleteTask(htCacheShow);
				else
				{
					// task will exist in both lists so clear attributes from hidden
					// task to ensure that attributes are correctly merged back 
					// in from shown item (which may have been edited).
					// BUT make sure we don't delete the ID else the merge
					// will go horribly wrong
					tasksHide.DeleteTaskAttributes(htCacheHide);
					tasksHide.SetTaskID(htCacheHide, dwID); 
				}
			}
		}
		else // has no children so check for removal
		{
			// if the current item matches the filter then we delete
			// it from the 'hide' tasks because we will be showing
			// it, else we delete from the 'show' tasks because we
			// will be hiding it
			FTMATCHRESULT nMatch = MatchFilter(tasksHide, htCacheHide);
			
			if (nMatch == FTDC_MATCH)
				tasksHide.DeleteTask(htCacheHide);
			else
				tasksShow.DeleteTask(htCacheShow);
		}
	}
}

FTMATCHRESULT CFilteredToDoCtrl::MatchFilter(const CTaskFile& tasks, HTASKITEM ht) const
{
	FTMATCHRESULT nMatch = FTDC_NOMATCHSTATE;

	switch (m_filter.nFilter)
	{
	case FT_ALL:
		nMatch = FTDC_MATCH;
		break;

	case FT_DONE:
		if (IsTaskDone(tasks, ht))
			nMatch = FTDC_MATCH;
		break;

	case FT_NOTDONE:
		if (!IsTaskDone(tasks, ht))
			nMatch = FTDC_MATCH;
		break;

	default: // all the rest
		if (MatchDueDate(tasks, ht))
			nMatch = FTDC_MATCH;
		break;
	}

	// check other values if its a positive match
	if (nMatch == FTDC_MATCH)
	{
		if (!MatchCategory(tasks, ht))
			nMatch = FTDC_NOMATCHCATEGORY;

		else if (!MatchStatus(tasks, ht))
			nMatch = FTDC_NOMATCHSTATUS;

		else if (!MatchAllocTo(tasks, ht))
			nMatch = FTDC_NOMATCHALLOCTO;

		else if (!MatchAllocBy(tasks, ht))
			nMatch = FTDC_NOMATCHALLOCBY;

		else if (!MatchPriority(tasks, ht))
			nMatch = FTDC_NOMATCHPRIORITY;

		else if (!MatchRisk(tasks, ht))
			nMatch = FTDC_NOMATCHRISK;
	}

	return nMatch;
}

BOOL CFilteredToDoCtrl::MatchCategory(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (m_filter.aCategories.GetSize())
	{
		CStringArray aCats;
		tasks.GetTaskCategories(ht, aCats);

		return m_filter.MatchCategories(aCats);
	}

	// else implicit match
	return TRUE;
}

BOOL CFilteredToDoCtrl::MatchStatus(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (!m_filter.sStatus.IsEmpty())
		return (m_filter.sStatus.CompareNoCase(tasks.GetTaskStatus(ht)) == 0);

	// else match
	return TRUE;
}

BOOL CFilteredToDoCtrl::MatchAllocTo(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (m_filter.aAllocTo.GetSize())
	{
		CStringArray aAllocTo;
		tasks.GetTaskAllocatedTo(ht, aAllocTo);

		return m_filter.MatchAllocTo(aAllocTo);
	}

	// else match
	return TRUE;
}

BOOL CFilteredToDoCtrl::MatchAllocBy(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (!m_filter.sAllocBy.IsEmpty())
		return (m_filter.sAllocBy.CompareNoCase(tasks.GetTaskAllocatedBy(ht)) == 0);

	// else match
	return TRUE;
}

BOOL CFilteredToDoCtrl::MatchPriority(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (m_filter.nPriority != FT_ANYPRIORITY)
	{
		/*
		if (IsTaskDone(tasks, ht))
					return FALSE;
				
				else if (tasks.IsTaskDue(ht))
					return TRUE;
				// match exactly on 'none'
				else*/
		 if (m_filter.nPriority == FT_NOPRIORITY)
			return (tasks.GetTaskPriority(ht, FALSE) == FT_NOPRIORITY);
		else
			return (m_filter.nPriority <= tasks.GetTaskPriority(ht, FALSE));
	}

	// else match
	return TRUE;
}

BOOL CFilteredToDoCtrl::MatchRisk(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (m_filter.nRisk != FT_ANYRISK)
	{
		/*
		if (IsTaskDone(tasks, ht))
					return FALSE;
				
				// match exactly on 'none'
				else*/
		 if (m_filter.nRisk == FT_NORISK)
			return (tasks.GetTaskRisk(ht, FALSE) == FT_NORISK);
		else
			return (m_filter.nRisk <= tasks.GetTaskRisk(ht, FALSE));
	}

	// else match
	return TRUE;
}

BOOL CFilteredToDoCtrl::MatchDueDate(const CTaskFile& tasks, HTASKITEM ht) const
{
	if (m_dateDue.m_dt > 0)
	{
		if (IsTaskDone(tasks, ht))
			return FALSE;
		else
		{
			COleDateTime dateDue = tasks.GetTaskDueDateOle(ht);
			return (dateDue.m_dt > 0 && dateDue <= m_dateDue);
		}
	}

	// else match
	return TRUE;
}

void CFilteredToDoCtrl::InitDueDate()
{
	switch (m_filter.nFilter)
	{
	case FT_ALL:
	case FT_DONE:
	case FT_NOTDONE:
		m_dateDue.m_dt = 0;
		break;

	case FT_DUETODAY:
		m_dateDue = CDateHelper::GetDate(DHD_TODAY);
		break;

	case FT_DUETOMORROW:
		m_dateDue = CDateHelper::GetDate(DHD_TOMORROW);
		break;

	case FT_DUEENDTHISWEEK:
		m_dateDue = CDateHelper::GetDate(DHD_ENDTHISWEEK);
		break;

	case FT_DUEENDNEXTWEEK: 
		m_dateDue = CDateHelper::GetDate(DHD_ENDNEXTWEEK);
		break;

	case FT_DUEENDTHISMONTH:
		m_dateDue = CDateHelper::GetDate(DHD_ENDTHISMONTH);
		break;

	case FT_DUEENDNEXTMONTH:
		m_dateDue = CDateHelper::GetDate(DHD_ENDNEXTMONTH);
		break;

	case FT_DUEENDTHISYEAR:
		m_dateDue = CDateHelper::GetDate(DHD_ENDTHISYEAR);
		break;

	case FT_DUEENDNEXTYEAR:
		m_dateDue = CDateHelper::GetDate(DHD_ENDNEXTYEAR);
		break;

	default:
		ASSERT(0);
		break;
	}
}

BOOL CFilteredToDoCtrl::IsTaskDone(const CTaskFile& tasks, HTASKITEM ht, BOOL bCheckParents) const
{
	if (!ht)
		return FALSE;

	if (tasks.IsTaskDone(ht))
		return TRUE;

	// check parents
	if (bCheckParents)
	{
		HTASKITEM htParent = tasks.GetTaskParent(ht);
		
		while (htParent)
		{
			if (tasks.IsTaskDone(htParent))
				return TRUE;

			htParent = tasks.GetTaskParent(htParent);
		}
	}

	// check children if needs be
	if (HasStyle(TDCS_TREATSUBCOMPLETEDASDONE))
	{
		HTASKITEM htChild = tasks.GetFirstTask(ht);

		if (htChild)
		{
			while (htChild)
			{
				if (!IsTaskDone(tasks, htChild, FALSE))
					return FALSE;

				htChild = tasks.GetNextTask(htChild);
			}

			// if we got here then all children and
			// children's children, etc are done
			return TRUE;
		}
	}

	// else not done
	return FALSE;
}

BOOL CFilteredToDoCtrl::DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel)
{
	if (!HasFilter())
		return CToDoCtrl::DeleteSelectedTask(bWarnUser, bResetSel);

	// 1. take a copy of the selected task IDs that also exist
	// in m_tasksHidden because we will have to delete those too. 
	CDWordArray aTaskIDs;
	POSITION pos = Selection().GetFirstItemPos();

	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		DWORD dwTaskID = GetTaskID(hti);

		if (m_tasksHidden.FindTask(dwTaskID))
			aTaskIDs.Add(dwTaskID);
	}

	// 2. let the base class do the visible deletion
	if (CToDoCtrl::DeleteSelectedTask(bWarnUser, bResetSel))
	{
		// 3. then we delete the duplicates
		for (int nID = 0; nID < aTaskIDs.GetSize(); nID++)
		{
			DWORD dwTaskID = aTaskIDs[nID];
			HTASKITEM ht = m_tasksHidden.FindTask(dwTaskID);

			if (ht)
				m_tasksHidden.DeleteTask(ht);
		}

		// 4. all done
		return TRUE;
	}

	return FALSE; // user cancelled
}

BOOL CFilteredToDoCtrl::SelectedTasksHaveChildren() const
{
	if (CToDoCtrl::SelectedTasksHaveChildren())
		return TRUE;

	// else lookup selected tasks in m_tasksHidden and if
	// they exist check if they have children
	POSITION pos = Selection().GetFirstItemPos();

	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		DWORD dwTaskID = GetTaskID(hti);

		HTASKITEM ht = m_tasksHidden.FindTask(dwTaskID);

		if (ht && m_tasksHidden.GetFirstTask(ht))
			return TRUE;
	}

	// else
	return FALSE;
}

TODOITEM* CFilteredToDoCtrl::NewTask()
{
	TODOITEM* pTDI = CToDoCtrl::NewTask();

	// fiddle with the default attributes so that the task 
	// will not be filtered out by the current filter
	if (HasFilter())
	{
		if (m_filter.nRisk != FT_ANYRISK)
			pTDI->nRisk = max(pTDI->nRisk, m_filter.nRisk);

		if (m_filter.nPriority != FT_ANYPRIORITY)
			pTDI->nPriority = max(pTDI->nPriority, m_filter.nPriority);

		if (!m_filter.sAllocBy.IsEmpty() && pTDI->sAllocBy.IsEmpty())
			pTDI->sAllocBy = m_filter.sAllocBy;

		if (!m_filter.MatchAllocTo(pTDI->aAllocTo))
		{
			// if any category will match then set it to the first
			if (m_filter.HasFlag(FT_ANYALLOCTO))
			{
				pTDI->aAllocTo.RemoveAll();

				if (m_filter.aAllocTo.GetSize())
					pTDI->aAllocTo.Add(m_filter.aAllocTo[0]);
			}
			else // set it to all the filter cats
				pTDI->aAllocTo.Copy(m_filter.aAllocTo);
		}

		if (!m_filter.sStatus.IsEmpty() && pTDI->sStatus.IsEmpty())
			pTDI->sStatus = m_filter.sStatus;

		if (!m_filter.MatchCategories(pTDI->aCategories))
		{
			// if any category will match then set it to the first
			if (m_filter.HasFlag(FT_ANYCATEGORY))
			{
				pTDI->aCategories.RemoveAll();

				if (m_filter.aCategories.GetSize())
					pTDI->aCategories.Add(m_filter.aCategories[0]);
			}
			else // set it to all the filter cats
				pTDI->aCategories.Copy(m_filter.aCategories);
		}

		// if the filter is FT_DONE then complete the task
		if (m_filter.nFilter == FT_DONE)
		{
			pTDI->dateDone = floor(COleDateTime::GetCurrentTime());
		}
		// else if due, set due today
		else if (m_filter.nFilter != FT_ALL && m_filter.nFilter != FT_NOTDONE)
		{
			pTDI->dateDue = floor(COleDateTime::GetCurrentTime());
		}
	}

	return pTDI;
}

HTREEITEM CFilteredToDoCtrl::NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere, 
									BOOL bSelect, BOOL bEditText)
{
	return CToDoCtrl::NewTask(szText, nWhere, bSelect, bEditText);
}

void CFilteredToDoCtrl::SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib)
{
	if (bMod && ModNeedsRefilter(nAttrib))
		RefreshFilter();

	CToDoCtrl::SetModified(bMod, nAttrib);
}

BOOL CFilteredToDoCtrl::ModNeedsRefilter(TDC_ATTRIBUTE nModType)
{
	if (HasStyle(TDCS_REFILTERONMODIFY))
	{
		switch (nModType)
		{
		case TDCA_PRIORITY:		return (m_filter.nPriority != -1);
		case TDCA_RISK:			return (m_filter.nRisk != -1);
			
		case TDCA_ALLOCBY:		return (!m_filter.sAllocBy.IsEmpty());
		case TDCA_STATUS:		return (!m_filter.sStatus.IsEmpty());

		case TDCA_CATEGORY:		return (m_filter.aCategories.GetSize());
		case TDCA_ALLOCTO:		return (m_filter.aAllocTo.GetSize());

		case TDCA_PERCENT:
		case TDCA_DONEDATE:		return (m_filter.nFilter == FT_DONE ||
										m_filter.nFilter == FT_NOTDONE);
			
		case TDCA_DUEDATE:		return (m_filter.nFilter != FT_DONE &&
										m_filter.nFilter != FT_NOTDONE &&
										m_filter.nFilter != FT_ALL);
		}
	}

	// all else
	return FALSE;
}

BOOL CFilteredToDoCtrl::IsTaskDone(DWORD dwTaskID) const
{
	// check task has not been filtered out
	if (m_data.GetTask(dwTaskID))
		return CToDoCtrl::IsTaskDone(dwTaskID);

	// else look it up in the filtered out tasks
	HTASKITEM ht = m_tasksHidden.FindTask(dwTaskID);

	if (ht)
		return m_tasksHidden.IsTaskDone(ht);

	// else
	return FALSE; // not found
}

BOOL CFilteredToDoCtrl::HasTask(DWORD dwTaskID)
{
	// check task has not been filtered out
	if (m_data.GetTask(dwTaskID))
		return TRUE;

	// else look it up in the filtered out tasks
	return (NULL != m_tasksHidden.FindTask(dwTaskID));
}

BOOL CFilteredToDoCtrl::SelectTask(DWORD dwTaskID)
{
	// check task has not been filtered out
	if (m_data.GetTask(dwTaskID))
		return CToDoCtrl::SelectTask(dwTaskID);

	// else look it up in the filtered out tasks
	if (m_tasksHidden.FindTask(dwTaskID))
	{
		// TODO
		return TRUE;
	}

	// else
	return FALSE;
}
