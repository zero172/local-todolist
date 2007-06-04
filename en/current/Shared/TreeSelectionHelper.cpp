// TreeSelectionHelper.cpp: implementation of the CTreeSelectionHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeSelectionHelper.h"
#include "TreectrlHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeSelectionHelper::CTreeSelectionHelper(CTreeCtrl& tree) : 
	m_tree(tree), m_nCurSelection(0), m_htiAnchor(NULL)
{

}

CTreeSelectionHelper::~CTreeSelectionHelper()
{

}

BOOL CTreeSelectionHelper::SetItem(HTREEITEM hti, int nState, BOOL bRedraw)
{
	if (!hti)
		return FALSE;

	POSITION pos = m_lstSelection.Find(hti);

	switch (nState)
	{
	case 1: // add
		if (!pos)
		{
			m_lstSelection.AddTail(hti);

			if (bRedraw)
				InvalidateItem(hti);

			return TRUE;
		}
		break;

	case 0: // remove
		if (pos)
		{
			m_lstSelection.RemoveAt(pos);

			if (bRedraw)
				InvalidateItem(hti);

			return TRUE;
		}
		break;

	case -1: // toggle
		if (!pos)
			m_lstSelection.AddTail(hti);
		else
			m_lstSelection.RemoveAt(pos);

		if (bRedraw)
			InvalidateItem(hti);

		return TRUE;
	}

	return FALSE;
}

void CTreeSelectionHelper::ClearHistory()
{
   m_aHistory.RemoveAll();
   m_nCurSelection = 0;
}

void CTreeSelectionHelper::RemoveItemFromHistory(HTREEITEM hti)
{
   int nHistory = m_aHistory.GetSize();

   while (nHistory--)
   {
      CHTIList& list = m_aHistory[nHistory];
		POSITION pos = list.GetHeadPosition();

		while (pos)
      {
         POSITION posCur = pos; // cache this
         HTREEITEM htiHist = list.GetNext(pos);

         if (htiHist == hti)
         {
            list.RemoveAt(posCur);
            break; // aItems are unique in list
         }
      }

      // if the selection list is not empty remove it too
      if (list.GetCount() == 0)
	  {
         m_aHistory.RemoveAt(nHistory);

		 // make sure m_nCurSelection remains valid
		 if (nHistory <= m_nCurSelection)
			 m_nCurSelection = max(0, m_nCurSelection - 1);
	  }
   }
}

void CTreeSelectionHelper::InvalidateItem(HTREEITEM hti)
{
	CRect rItem;

	if (m_tree.GetItemRect(hti, rItem, FALSE))
		m_tree.InvalidateRect(rItem, FALSE);	
}

BOOL CTreeSelectionHelper::SetItems(HTREEITEM htiFrom, HTREEITEM htiTo, int nState, BOOL bRedraw)
{
	if (!htiFrom || !htiTo)
		return FALSE;

	CTreeCtrlHelper tch(m_tree);

	int nDirection = tch.FindItem(htiTo, htiFrom);

	// if (htiFrom != htiTo) and nDirection is zero then htiTo could not be found
	if (htiFrom != htiTo && !nDirection)
		return FALSE;

	// if htiTo is above htiFrom then switch aItems so we can use a single loop
	if (nDirection == -1)
	{
		HTREEITEM htiTemp = htiFrom;
		htiFrom = htiTo;
		htiTo = htiTemp;
	}

	BOOL bRes = FALSE;
	HTREEITEM htiNext = htiFrom;

	while (htiNext) 
	{
		bRes |= SetItem(htiNext, nState, bRedraw);

		if (htiNext != htiTo)
			htiNext = tch.GetNextVisibleItem(htiNext, TRUE);
		else
			break;
	}

	return bRes;
}

BOOL CTreeSelectionHelper::AddItems(HTREEITEM htiFrom, HTREEITEM htiTo, BOOL bRedraw)
{
	return SetItems(htiFrom, htiTo, 1, bRedraw);
}

BOOL CTreeSelectionHelper::ToggleItems(HTREEITEM htiFrom, HTREEITEM htiTo, BOOL bRedraw)
{
	return SetItems(htiFrom, htiTo, -1, bRedraw);
}

BOOL CTreeSelectionHelper::RemoveItem(HTREEITEM hti, BOOL bRemoveFromHistory, BOOL bRedraw) 
{ 
   if (bRemoveFromHistory)
      RemoveItemFromHistory(hti);
   
   return SetItem(hti, 0, bRedraw); 
}

BOOL CTreeSelectionHelper::AddAll(BOOL bRedraw)
{
	// traverse all top level aItems adding each in turn
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		AddAll(hti);
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	// redraw
	if (bRedraw)
		m_tree.Invalidate();

	return GetCount();
}

void CTreeSelectionHelper::AddAll(HTREEITEM hti)
{
	AddItem(hti, FALSE); // no redraw

	// add children
	HTREEITEM htiChild = m_tree.GetChildItem(hti);

	while (htiChild)
	{
		AddAll(htiChild);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

BOOL CTreeSelectionHelper::RemoveAll(BOOL bRemoveFromHistory, BOOL bRedraw) 
{ 
	if (GetCount())
	{
		// flush the history stack
		if (m_nCurSelection < m_aHistory.GetSize())
			m_aHistory.RemoveAt(m_nCurSelection + 1, m_aHistory.GetSize() - m_nCurSelection - 1);

		else // add the current selection to the history
			m_aHistory.Add(m_lstSelection);

		// update the current selection
		m_nCurSelection++;
		
      // remove from elsewhere in history
      if (bRemoveFromHistory)
      {
         POSITION pos = GetFirstItemPos();

         while (pos)
            RemoveItemFromHistory(GetNextItem(pos));
      }
		
		if (bRedraw)
			InvalidateAll(FALSE);

		m_lstSelection.RemoveAll(); 
		return TRUE;
	}

	return FALSE;
}

void CTreeSelectionHelper::InvalidateAll(BOOL bErase)
{
	POSITION pos = GetFirstItemPos();
	CRect rItem;

	while (pos)
	{
		HTREEITEM hti = GetNextItem(pos);

		m_tree.GetItemRect(hti, rItem, FALSE);
		m_tree.InvalidateRect(rItem, bErase);
	}
}

BOOL CTreeSelectionHelper::AnyItemsHaveChildren() const
{
	POSITION pos = GetFirstItemPos();

	while (pos)
	{
		if (m_tree.ItemHasChildren(GetNextItem(pos)))
			return TRUE;
	}

	return FALSE;
}

BOOL CTreeSelectionHelper::ItemsAreAllParents() const
{
	POSITION pos = GetFirstItemPos();

	if (pos)
	{
		while (pos)
		{
			HTREEITEM hti = GetNextItem(pos);

			if (!m_tree.ItemHasChildren(hti))
				return FALSE; // not a parent
		}

		return TRUE; // all parents
	}

	return FALSE; // nothing selected
}

BOOL CTreeSelectionHelper::ItemsAreAllSiblings() const
{
	if (GetCount() < 2)
		return TRUE;

	POSITION pos = GetFirstItemPos();

	if (pos)
	{
		HTREEITEM hti = GetNextItem(pos);
		HTREEITEM htiParent = m_tree.GetParentItem(hti);

		while (pos)
		{
			hti = GetNextItem(pos);

			if (m_tree.GetParentItem(hti) != htiParent)
				return FALSE;
		}
	}

	return TRUE;
}

void CTreeSelectionHelper::SortIfAllSiblings(BOOL bAscending)
{
	if (ItemsAreAllSiblings())
	{
		// build an array of the current selection
		CSortArray aItems;

		if (!BuildSortArray(aItems))
			return;

		// sort that array
		qsort(aItems.GetData(), aItems.GetSize(), sizeof(SORTITEM), SortProc);

		// rebuild the selection
		RemoveAll(FALSE);

		if (bAscending)
		{
			for (int nItem = aItems.GetSize() - 1; nItem >= 0 ; nItem--)
			{
				const SORTITEM& si = aItems[nItem];
				AddItem(si.hti, FALSE);
			}
		}
		else
		{
			for (int nItem = 0; nItem < aItems.GetSize(); nItem++)
			{
				const SORTITEM& si = aItems[nItem];
				AddItem(si.hti, FALSE);
			}
		}
	}
}

int CTreeSelectionHelper::BuildSortArray(CSortArray& aItems)
{
	aItems.SetSize(GetCount());
	
	POSITION pos = GetFirstItemPos();
	int nItem = 0;
	
	while (pos)
	{
		HTREEITEM hti = GetNextItem(pos);

		int nPos = GetItemPos(hti);
		SORTITEM si = { hti, nPos };
		
		aItems.SetAt(nItem, si);
		nItem++;
	}

	return aItems.GetSize();
}

int CTreeSelectionHelper::SortProc(const void* item1, const void* item2)
{
	const SORTITEM* pItem1 = (const SORTITEM*)item1;
	const SORTITEM* pItem2 = (const SORTITEM*)item2;

	if (pItem1->nPos < pItem2->nPos)
		return -1;

	else if (pItem1->nPos > pItem2->nPos)
		return 1;
	else
		return 0;
}

int CTreeSelectionHelper::GetItemPos(HTREEITEM hti)
{
	HTREEITEM htiParent = m_tree.GetParentItem(hti);
	HTREEITEM htiChild = m_tree.GetChildItem(htiParent);
	int nPos = 0;

	while (htiChild)
	{
		if (hti == htiChild)
			return nPos;

		nPos++;
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}

	// not found ??
	return -1;
}

void CTreeSelectionHelper::RemoveChildDuplicates(CHTIList& selection, const CTreeCtrl& tree)
{
	POSITION pos = selection.GetHeadPosition();

	while (pos)
	{
		POSITION posChild = pos;
		HTREEITEM hti = selection.GetNext(pos);

		if (HasSelectedParent(hti, selection, tree))
			selection.RemoveAt(posChild);
	}
}

BOOL CTreeSelectionHelper::HasSelectedParent(HTREEITEM hti) const
{
	return HasSelectedParent(hti, m_lstSelection, m_tree);
}

BOOL CTreeSelectionHelper::HasSelectedParent(HTREEITEM hti, const CHTIList& selection, const CTreeCtrl& tree)
{
	HTREEITEM htiParent = tree.GetParentItem(hti);
	
	while (htiParent)
	{
		if (selection.Find(htiParent))
			return TRUE;
		
		// else
		htiParent = tree.GetParentItem(htiParent);
	}
	
	return FALSE; // not found
}

BOOL CTreeSelectionHelper::ContainsAllItems() const
{
	// traverse all top level aItems and check for inclusion
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		if (!HasItem(hti))
			return FALSE;

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return TRUE;
}

BOOL CTreeSelectionHelper::HasNextSelection() const
{
	return (m_nCurSelection < m_aHistory.GetSize() - 1);
}

BOOL CTreeSelectionHelper::NextSelection(BOOL bRedraw)
{
	if (HasNextSelection())
	{
		// invalidate current selection
		if (bRedraw)
			InvalidateAll(FALSE);

		// save current selection in history
		m_aHistory[m_nCurSelection] = m_lstSelection;

		// copy next selection
		m_nCurSelection++;
		m_lstSelection = m_aHistory[m_nCurSelection];

		// invalidate new selection
		if (bRedraw)
			InvalidateAll(FALSE);

		return TRUE;
	}

	return FALSE;
}

BOOL CTreeSelectionHelper::HasPrevSelection() const
{
	return m_nCurSelection && m_aHistory.GetSize();
}

BOOL CTreeSelectionHelper::PrevSelection(BOOL bRedraw)
{
	if (HasPrevSelection())
	{
		int nSizeHistory = m_aHistory.GetSize();

		// invalidate current selection
		if (bRedraw)
			InvalidateAll(FALSE);

		// save current selection in history
		if (m_nCurSelection < nSizeHistory)
			m_aHistory[m_nCurSelection] = m_lstSelection;
		else
			m_aHistory.Add(m_lstSelection);

		m_nCurSelection--;
		m_lstSelection = m_aHistory[m_nCurSelection];

		// invalidate new selection
		if (bRedraw)
			InvalidateAll(FALSE);

		return TRUE;
	}

	return FALSE;
}
