#if !defined(AFX_TDCSTRUCT_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TDCSTRUCT_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// tdlutil.h : header file
//

#include "tdcenum.h"
#include "taskfile.h"

#include "..\shared\TreeSelectionHelper.h"
#include "..\shared\TreeCtrlHelper.h"
#include "..\shared\misc.h"
#include "..\shared\datehelper.h"

typedef CMap<TDC_STYLE, TDC_STYLE, BOOL, BOOL&> CTDCStyles;
typedef CMap<CString, LPCTSTR, COLORREF, COLORREF&> CTDCColorMap;

struct TDIRECURRENCE
{
//  nRegularity       dwSpecific1        dwSpecific2

//	RTER_DAILY        every 'n' days     --- (0)
//	RTER_WEEKLY       every 'n' weeks    weekdays (TDIW_...)
//	RTER_MONTHLY      every 'n' months   day of month (1-31)
//	RTER_YEARLY       month (1-12)       day of month (1-31)

	TDIRECURRENCE() : nRegularity(TDIR_ONCE), dwSpecific1(1), dwSpecific2(0), bRecalcFromDue(TRUE) {}

	BOOL operator==(const TDIRECURRENCE& tr) const
	{
		return (tr.nRegularity == nRegularity && tr.dwSpecific1 == dwSpecific1 &&
				tr.dwSpecific2 == dwSpecific2 && tr.bRecalcFromDue == bRecalcFromDue);
	}

	BOOL operator!=(const TDIRECURRENCE& tr) const
	{
		return !(*this == tr);
	}

	BOOL GetNextOccurence(const COleDateTime& dtFrom, COleDateTime& dtNext) const
	{
		if (dtFrom.GetStatus() != COleDateTime::valid || dtFrom.m_dt == 0.0)
			return FALSE;

		if (nRegularity == TDIR_ONCE)
			return FALSE;

		dtNext = dtFrom;

		switch (nRegularity)		
		{
		case TDIR_DAILY:
			if ((int)dwSpecific1 <= 0)
				return FALSE;

			// add number of days specified by dwSpecific1
			CDateHelper::OffsetDate(dtNext, (int)dwSpecific1, DHU_DAYS);
			break;

		case TDIR_WEEKLY:
			{
				if ((int)dwSpecific1 <= 0/* || !dwSpecific2*/)
					return FALSE;

				// if no weekdays have been set we just add the specified number of weeks
				if (!dwSpecific2)
					CDateHelper::OffsetDate(dtNext, (int)dwSpecific1, DHU_WEEKS);

				else // go looking for the next specified weekday
				{
					// first add any weeks greater than one
					CDateHelper::OffsetDate(dtNext, (int)(dwSpecific1 - 1), DHU_WEEKS);

					// then look for the next weekday *after* this one

					// build a 2 week weekday array so we don't have to deal with 
					// wrapping around
					UINT nWeekdays[14] = 
					{
						(dwSpecific2 & TDIW_SUNDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_MONDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_TUESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_WEDNESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_THURSDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_FRIDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_SATURDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_SUNDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_MONDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_TUESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_WEDNESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_THURSDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_FRIDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_SATURDAY) ? 1 : 0,
					};

					dtNext += 1.0; // always next day at least

					int nStart = dtFrom.GetDayOfWeek();
					int nEnd = nStart + 7; // week later

					for (int nWeekday = nStart; nWeekday < nEnd; nWeekday++)
					{
						if (nWeekdays[nWeekday] != 0)
							break;
						else
							dtNext += 1.0; 
					}
				}
			}
			break;

		case TDIR_MONTHLY:
			{
				if ((int)dwSpecific1 <= 0 || (dwSpecific2 < 1 || dwSpecific2 > 31))
					return FALSE;
				
				// add number of months specified by dwSpecific1
				CDateHelper::OffsetDate(dtNext, (int)dwSpecific1, DHU_MONTHS);

				// then enforce the day
				SYSTEMTIME st;
				dtNext.GetAsSystemTime(st);
				st.wDay = (WORD)dwSpecific2;

				dtNext = COleDateTime(st);
			}
			break;

		case TDIR_YEARLY:
			{
				if ((dwSpecific1 < 1 || dwSpecific1 > 12) || (dwSpecific2 < 1 || dwSpecific2 > 31))
					return FALSE;
				
				// add a year 
				CDateHelper::OffsetDate(dtNext, 1, DHU_YEARS);

				// then enforce the month and the day
				SYSTEMTIME st;
				dtNext.GetAsSystemTime(st);
				st.wMonth = (WORD)dwSpecific1;
				st.wDay = (WORD)dwSpecific2;

				dtNext = COleDateTime(st);
			}
			break;

		default:
			return FALSE;
		}

		return TRUE;
	}

	TDI_REGULARITY nRegularity;
	DWORD dwSpecific1;
	DWORD dwSpecific2;
	BOOL bRecalcFromDue; // else recalc from 'completed'
};

struct TDCFILTER
{
	TDCFILTER() : 
			nFilter(TDCF_ALL), dwFlags(TDCGT_ISODATES) {}

	TDCFILTER(TDC_FILTER filter, DWORD flags, double dueBy = 0) :
			nFilter(filter), dwFlags(flags), dateDueBy(dueBy) {}

	TDC_FILTER nFilter;
	COleDateTime dateDueBy;
	DWORD dwFlags;
	CString sAllocTo;
};

struct TDLSELECTION
{
	TDLSELECTION(CTreeCtrl& tree) : selection(tree), htiLastHandledLBtnDown(NULL), 
									tch(tree), nLBtnDownFlags(0), nNcLBtnDownColID(-1) {}

	CTreeSelectionHelper selection;
	CTreeCtrlHelper tch;
	HTREEITEM htiLastHandledLBtnDown;
	UINT nLBtnDownFlags;
	int nNcLBtnDownColID;

	BOOL HasIncompleteSubtasks()
	{
		POSITION pos = selection.GetFirstItemPos();

		while (pos)
		{
			HTREEITEM hti = selection.GetNextItem(pos);

			if (ItemHasIncompleteSubtasks(hti))
				return TRUE;
		}

		return FALSE;
	}
	
	BOOL ItemHasIncompleteSubtasks(HTREEITEM hti)
	{
		const CTreeCtrl& tree = selection.TreeCtrl();
		HTREEITEM htiChild = tree.GetChildItem(hti);

		while (htiChild)
		{
			if (tch.GetItemCheckState(htiChild) != TCHC_CHECKED || ItemHasIncompleteSubtasks(htiChild))
				return TRUE;
			
			htiChild = tree.GetNextItem(htiChild, TVGN_NEXT);
		}

		return FALSE;
	}
};

struct TDLCLIPBOARD
{
	TDLCLIPBOARD() : hwndToDoCtrl(NULL) {}
	
	CTaskFile tasks;
	HWND hwndToDoCtrl;
};

struct TDCCONTROL
{
	LPCTSTR szClass;
	UINT nIDCaption;
	DWORD dwStyle;
	DWORD dwExStyle; 
	int nX;
	int nY;
	int nCx;
	int nCy;
	UINT nID;
};

struct TDCCOLUMN
{
	TDC_COLUMN nColID;
	UINT nIDName;
	TDC_SORTBY nSortBy;
	UINT nAlignment;
	BOOL bClickable;
	LPCTSTR szFont;
	BOOL bSymbolFont;
	BOOL bSortAscending;
};

struct CTRLITEM
{
	UINT nCtrlID;
	UINT nLabelID;
	TDC_COLUMN nCol;
};

struct SEARCHPARAMS
{
	SEARCHPARAMS() : nFindWhat(0), dwFlags(0), nFrom(0), nTo(0), dFrom(0), dTo(0) {}
	int nFindWhat;
	DWORD dwFlags; // see above
	
	CString sText;
	CStringArray aCategories;
	COleDateTime dateFrom, dateTo;
	int nFrom, nTo;
	double dFrom, dTo;
};

struct SEARCHRESULT
{
	SEARCHRESULT() : dwID(0), hti(NULL), nMatch(0), dMatch(0), bDone(0) {}
	DWORD dwID;
	HTREEITEM hti;
	CString sMatch;
	COleDateTime dateMatch;
	int nMatch;
	double dMatch;
	BOOL bDone;
};

typedef CArray<SEARCHRESULT, SEARCHRESULT&> CResultArray;

struct FTDCFILTER
{
	FTDCFILTER() : nFilter(FT_ALL), nPriority(-1), nRisk(-1), dwFlags(0) {}

	void operator=(const FTDCFILTER& filter)
	{
		nFilter = filter.nFilter;
		nPriority = filter.nPriority;
		nRisk = filter.nRisk;
		sAllocTo = filter.sAllocTo;
		sStatus = filter.sStatus;
		sAllocBy = filter.sAllocBy;
		aCategories.Copy(filter.aCategories);
		dwFlags = filter.dwFlags;
	}

	BOOL operator==(const FTDCFILTER& filter) const
	{
		return (filter.nFilter == nFilter && filter.nPriority == nPriority &&
				filter.nRisk == nRisk && filter.sAllocTo == sAllocTo &&
				filter.sStatus == sStatus && filter.sAllocBy == sAllocBy &&
				Misc::ArraysMatch(aCategories, filter.aCategories) &&
				dwFlags == filter.dwFlags);
	}

	BOOL operator!=(const FTDCFILTER& filter) const
	{
		return !(*this == filter);
	}
	
	BOOL IsSet() const
	{
		return (nFilter != FT_ALL || nPriority >= 0 || nRisk >= 0 ||
			aCategories.GetSize() || !sAllocTo.IsEmpty() ||
			!sStatus.IsEmpty() || !sAllocBy.IsEmpty()) ? 1 : 0;
	}

	BOOL HasFlag(DWORD dwFlag) const
	{
		return ((dwFlags & dwFlag) == dwFlag);
	}

	void SetFlag(DWORD dwFlag, BOOL bOn = TRUE)
	{
		if (bOn)
			dwFlags |= dwFlag;
		else
			dwFlags &= ~dwFlags;
	}

	BOOL MatchCategories(const CStringArray& aCats) const
	{
		if (HasFlag(FT_ANYCATEGORY))
		{
			// special case: if aCats is empty, test for empty
			// string in aCategories
			if (aCats.GetSize() == 0)
				return (Misc::Find(aCategories, "") != -1);
			else
				return Misc::MatchAny(aCategories, aCats);
		}

		// else exact match required
		// since we cannot filter against both having a category
		// and not having a category at the same time
		// we ignore the empty string unless that's all there is
		if (aCategories.GetSize() == 1 && aCategories[0].IsEmpty())
			return (aCats.GetSize() == 0);

		int nBlank = Misc::Find(aCategories, "");
		
		if (nBlank != -1)
		{
			CStringArray aCopyCats; // ;)
			aCopyCats.Copy(aCategories);
			
			while (nBlank != -1)
			{
				aCopyCats.RemoveAt(nBlank);
				nBlank = Misc::Find(aCopyCats, "");
			}
			
			// compare what's left
			return Misc::ArraysMatch(aCopyCats, aCats);
		}
		else // simple compare
			return Misc::ArraysMatch(aCategories, aCats);
	}

	void Reset()
	{
		*this = FTDCFILTER(); // empty filter
	}
	
	FILTER_TYPE nFilter;
	int nPriority, nRisk;
	CStringArray aCategories;
	CString sStatus;
	CString sAllocTo, sAllocBy;
	DWORD dwFlags;
};


#endif // AFX_TDCSTRUCT_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_
