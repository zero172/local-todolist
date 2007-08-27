// TreeDragDropHelper.cpp: implementation of the CTreeDragDropHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeDragDropHelper.h"
#include "holdredraw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

const int MAXIMAGEWIDTH = 200;

class CDragDropTreeData : public CDragDropData
{
public:
	CDragDropTreeData(const CTreeSelectionHelper& selection) :
		m_tree(selection.TreeCtrl()), m_ptDrawOffset(0)
	{
		selection.CopySelection(m_selection);
	}

protected:
	virtual CSize OnGetDragSize(CDC& /*dc*/)
	{
		CRect rDrag(0, 0, 0, 0), rItem;

		// iterate the current selection accumulating their rects
		POSITION pos = m_selection.GetHeadPosition();

		while (pos)
		{
			HTREEITEM hti = m_selection.GetNext(pos);

			if (m_tree.GetItemRect(hti, rItem, TRUE))
				rDrag |= rItem;
		}

		// save this for when we draw
		m_ptDrawOffset = rDrag.TopLeft();

		rDrag.right = min(rDrag.right, rDrag.left + MAXIMAGEWIDTH);

		return rDrag.Size();
	}
	
	virtual void OnDrawData(CDC& dc, CRect& rc, COLORREF& crMask)
	{
		crMask = 1;
		dc.FillSolidRect(rc, crMask);

		dc.SetBkMode(OPAQUE);
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	
		// iterate the current selection accumulating their rects
		POSITION pos = m_selection.GetHeadPosition();

		while (pos)
		{
			HTREEITEM hti = m_selection.GetNext(pos);
			CRect rItem;

			if (m_tree.GetItemRect(hti, rItem, TRUE))
			{
				rItem -= m_ptDrawOffset;
				rItem += rc.TopLeft();
				rItem.IntersectRect(rc, rItem);

				dc.FillSolidRect(rItem, ::GetSysColor(COLOR_HIGHLIGHT));
				rItem.DeflateRect(2, 1);
				dc.DrawText(m_tree.GetItemText(hti), rItem, DT_LEFT | DT_END_ELLIPSIS);
			}
		}
	}
	
	virtual void* OnGetData() { return NULL; }

protected:
	CHTIList m_selection;
	const CTreeCtrl& m_tree;
	CPoint m_ptDrawOffset;
};

//////////////////////////////////////////////////////////////////////

// helper for copying
struct TDDHCOPY
{
	TDDHCOPY() { hti = NULL; dwItemData = 0; }
	TDDHCOPY(const TDDHCOPY& htc) { *this = htc; }
	
	HTREEITEM hti;
	DWORD dwItemData;
	CArray<TDDHCOPY, TDDHCOPY&> childItems;
	
	const TDDHCOPY& operator=(const TDDHCOPY& htc)
	{
		hti = htc.hti;
		dwItemData = htc.dwItemData;
		childItems.Copy(htc.childItems);
		
		return *this;
	}
	
	void Clear()
	{
		dwItemData = 0;
		hti = NULL;
		childItems.RemoveAll();
	}
	
	BOOL IsValid() const { return (NULL != hti); }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeDragDropHelper* CTreeDragDropHelper::s_pTDDH = NULL;

enum
{
	DELAY_INTERVAL = 150, 
	SCROLL_INTERVAL = 100, 
	SCROLL_MARGIN = 20, 
	EXPAND_INTERVAL = 500,
};

const CPoint OUTERSPACE(-10000, -10000);

CTreeDragDropHelper::CTreeDragDropHelper(CTreeSelectionHelper& selection, 
	 CTreeCtrl& tree, BOOL bTreeUsesTextCallback, BOOL bTreeUsesImageCallback)
	: m_selection(selection), m_tree(tree), 
	  m_bTreeUsesTextCallback(bTreeUsesTextCallback), 
	  m_bTreeUsesImageCallback(bTreeUsesImageCallback),
	  m_htiDropTarget(NULL), m_htiDropAfter(NULL), m_bEnabled(FALSE), 
	  m_nScrollTimer(0), m_nExpandTimer(0)
{

}

CTreeDragDropHelper::~CTreeDragDropHelper()
{

}

BOOL CTreeDragDropHelper::Initialize(CWnd* pOwner, BOOL bEnabled, BOOL bAllowNcDrag)
{
	if (m_ddMgr.Install(pOwner, m_tree))
	{
		m_tree.ModifyStyle(0, TVS_DISABLEDRAGDROP);
		
		m_bEnabled = bEnabled;
		m_bAllowNcDrag = bAllowNcDrag;

		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CTreeDragDropHelper::AddTargetWnd(CWnd* pWnd)
{
	ASSERT(pWnd->GetSafeHwnd());

	if (pWnd->GetSafeHwnd())
	{
		m_ddMgr.AddWindow(pWnd->GetSafeHwnd(), DDW_TARGET);
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CTreeDragDropHelper::GetDropTarget(HTREEITEM& htiDrop, HTREEITEM& htiAfter)
{
	htiDrop = m_htiDropTarget ? m_htiDropTarget : TVI_ROOT;
	htiAfter = m_htiDropAfter;

	return (htiDrop || htiAfter);
}

void CTreeDragDropHelper::SetTimer(int nTimer, UINT nPeriod)
{
	switch (nTimer)
	{
	case TIMER_SCROLL:
		if (m_nScrollTimer)
		{
			::KillTimer(NULL, m_nScrollTimer);
			m_nScrollTimer = 0;
		}
			
		if (nPeriod)
			m_nScrollTimer = ::SetTimer(NULL, 0, nPeriod, TimerProc);
		break;
		
	case TIMER_EXPAND:
		if (m_nExpandTimer)
		{
			::KillTimer(NULL, m_nExpandTimer);
			m_nExpandTimer = 0;
		}
			
		if (nPeriod)
			m_nExpandTimer = ::SetTimer(NULL, 0, nPeriod, TimerProc);
		break;
	}

	s_pTDDH = (m_nExpandTimer || m_nScrollTimer) ? this : NULL;
}

UINT CTreeDragDropHelper::ProcessMessage(const MSG* pMsg) 
{ 
	DRAGDROPINFO* pDDI = (DRAGDROPINFO*)pMsg->lParam;

	if (pMsg->message == WM_DD_DRAGENTER)
		return OnDragEnter(pDDI);

	else if (pMsg->message == WM_DD_PREDRAGMOVE)
		return OnDragPreMove(pDDI);

	else if (pMsg->message == WM_DD_DRAGOVER)
		return OnDragOver(pDDI);

	else if (pMsg->message == WM_DD_DRAGDROP)
		return OnDragDrop(pDDI);

	else if (pMsg->message == WM_DD_DRAGABORT)
		return OnDragAbort();

	// else
	return m_ddMgr.ProcessMessage(pMsg, m_bAllowNcDrag);
}

BOOL CTreeDragDropHelper::OnDragEnter(DRAGDROPINFO* pDDI)
{
	if (!m_bEnabled || !m_selection.GetCount())
		return FALSE;

	// make sure we clicked on an item
	HTREEITEM htiHit = HitTest(pDDI->pt);

	if (!htiHit || !m_selection.HasItem(htiHit))
		return FALSE;

	// make sure we didn't click on an expansion button
	UINT nFlags = 0;
	m_tree.HitTest(pDDI->pt, &nFlags);

	if (nFlags & TVHT_ONITEMBUTTON)
		return FALSE;
	
	// make sure this has not initiated a label edit
	m_tree.SendMessage(TVM_ENDEDITLABELNOW, TRUE, 0);
	
	pDDI->pData = new CDragDropTreeData(m_selection);
	
	// reset droptarget
	m_htiDropTarget = m_htiDropAfter = NULL;
	
	m_dropPos.htiDrop = NULL;
	m_dropPos.nWhere = DD_ON;
	
	return TRUE;
}

BOOL CTreeDragDropHelper::OnDragPreMove(const DRAGDROPINFO* /*pDDI*/)
{
	if (!m_bEnabled || !m_selection.GetCount())
		return FALSE;
	
	m_tree.SetInsertMark(NULL);
	
	return TRUE;
}

UINT CTreeDragDropHelper::OnDragOver(const DRAGDROPINFO* pDDI)
{
	if (!m_bEnabled || !m_selection.GetCount())
		return 0;
	
	HTREEITEM htiDrop = NULL;
	
	if (pDDI->hwndTarget == m_tree.GetSafeHwnd())
	{
		htiDrop = HighlightDropTarget(pDDI->pt);
		
		// Set a timer if the cursor is at the top or bottom of the window,
		// or if it's over a collapsed item.
		CRect rect;
		GetClientRect(pDDI->hwndTarget, rect);

		int cy = rect.Height();
		
		if ((pDDI->pt.y >= 0 && pDDI->pt.y <= SCROLL_MARGIN) || 
			(pDDI->pt.y >= cy - SCROLL_MARGIN && pDDI->pt.y <= cy))
		{
			SetTimer(TIMER_SCROLL, DELAY_INTERVAL);
		}
		
		if (htiDrop != NULL && m_tree.ItemHasChildren(htiDrop) &&
			!(m_tree.GetItemState(htiDrop, TVIS_EXPANDED) & TVIS_EXPANDED))
		{
			SetTimer(TIMER_EXPAND, EXPAND_INTERVAL);
		}
	}
	else
		HighlightDropTarget(OUTERSPACE);
			
	if (htiDrop)
	{
		if (pDDI->bLeftDrag)
		{
			BOOL bCopy = (GetKeyState(VK_CONTROL) & 0x8000);
			return bCopy ? DD_DROPEFFECT_COPY : DD_DROPEFFECT_MOVE;
		}
		else
			return DD_DROPEFFECT_MOVE;
	}

	// else
	return DD_DROPEFFECT_NONE;
}

BOOL CTreeDragDropHelper::OnDragDrop(const DRAGDROPINFO* pDDI)
{
	TRACE ("CTreeDragDropHelper::OnDragDrop(enter)\n");

	if (!m_bEnabled || !m_selection.GetCount())
		return FALSE;
	
	if (pDDI->hwndTarget == m_tree.GetSafeHwnd() && m_dropPos.htiDrop)
	{
		TRACE ("CTreeDragDropHelper::OnDragDrop(%s, %s)\n", m_tree.GetItemText(m_dropPos.htiDrop),
					(m_dropPos.nWhere == DD_ON) ? "On" : (m_dropPos.nWhere == DD_ABOVE) ? "Above" : "Below");
		
		// figure out where to drop
		switch (m_dropPos.nWhere)
		{
		case DD_ON:
			m_htiDropTarget = m_dropPos.htiDrop;
			m_htiDropAfter = NULL; // indicates this is a drop ON
			break;
			
		case DD_ABOVE:
			{
				m_htiDropAfter = m_tree.GetPrevSiblingItem(m_dropPos.htiDrop);
				
				if (!m_htiDropAfter || m_htiDropAfter == m_dropPos.htiDrop)
					m_htiDropAfter = TVI_FIRST;
				
				m_htiDropTarget = m_tree.GetParentItem(m_dropPos.htiDrop);
			}
			break;
			
		case DD_BELOW:
			m_htiDropAfter = m_dropPos.htiDrop;
			m_htiDropTarget = m_tree.GetParentItem(m_dropPos.htiDrop);
			break;
		}
	}
	else
	{
		TRACE ("CTreeDragDropHelper::OnDragDrop(not on tree)\n");
		m_htiDropTarget = m_htiDropAfter = NULL;
	}
	
	SetTimer(TIMER_SCROLL, 0);
	SetTimer(TIMER_EXPAND, 0);
	
	m_tree.SetInsertMark(NULL);
	m_tree.SelectDropTarget(NULL);
	m_tree.Invalidate(FALSE);

	TRACE ("CTreeDragDropHelper::OnDragDrop(leave)\n");
	
	return (m_htiDropTarget || m_htiDropAfter);
}

BOOL CTreeDragDropHelper::OnDragAbort()
{
	if (!m_bEnabled || !m_selection.GetCount())
		return FALSE;
	
	// reset droptarget
	m_htiDropTarget = m_htiDropAfter = NULL;
	
	// cleanup
	m_tree.SetInsertMark(NULL);
	m_tree.SelectDropTarget(NULL);
	m_tree.Invalidate(FALSE);
	
	SetTimer(TIMER_SCROLL, 0);
	SetTimer(TIMER_EXPAND, 0);
	
	return TRUE;
}

HTREEITEM CTreeDragDropHelper::HitTest(CPoint point, DDWHERE& nWhere) const
{
	if (point == OUTERSPACE)
		return NULL;

	if (point.y < 0)
		return NULL;
	
	if (point.x < 0 && m_bAllowNcDrag)
	{
		// check if point is within entire window
		CRect rWindow;
		m_tree.GetWindowRect(rWindow);
		m_tree.ScreenToClient(rWindow);

		if (rWindow.PtInRect(point))
			point.x = 0;
		else // outside window
			return NULL;
	}

	UINT nFlags;
	HTREEITEM hItem = m_tree.HitTest(point, &nFlags);

	if (!hItem)
		return NULL;

	CRect rItem;

	if (m_tree.GetItemRect(hItem, rItem, FALSE))
	{
		int nMargin = (rItem.Height() + 3) / 4;

		if (point.y < rItem.top + nMargin) // top 1/4 == above the item
			nWhere = DD_ABOVE;

		else if (point.y > rItem.bottom - nMargin) // bottom 1/4 == below the item
			nWhere = DD_BELOW;
		else
			nWhere = DD_ON; // ie its on the item

		return hItem;
	}

	return NULL;
}

HTREEITEM CTreeDragDropHelper::HighlightDropTarget()
{
	CPoint point(::GetMessagePos());
	m_tree.ScreenToClient(&point);

	return HighlightDropTarget(point);
}

HTREEITEM CTreeDragDropHelper::HighlightDropTarget(CPoint point)
{
	// Find out which item (if any) the cursor is over.
	DDWHERE nWhere;
	HTREEITEM hItem = HitTest(point, nWhere);

	// don't do nuthin' if nuthin's changed
	if (hItem == m_dropPos.htiDrop && nWhere == m_dropPos.nWhere)
	{
		TRACE("CTreeDragDropHelper::HighlightDropTarget(nothing's changed)\n");
	}

	// drop item cannot be selected unless this is a copy
   // nor can it be a child of a selected item unless its a copy
	BOOL bCopy = (GetKeyState(VK_CONTROL) & 0x8000);

	if (!bCopy && (m_selection.HasItem(hItem) || m_selection.HasSelectedParent(hItem)))
			hItem = NULL;

	m_dropPos.htiDrop = hItem;
	m_dropPos.nWhere = nWhere;

	// Highlight the item, or unhighlight all items if the cursor isn't
	// over an item. 
	if (hItem)
	{
		if (nWhere == DD_ON)
		{
			m_tree.SetInsertMark(NULL);
			m_tree.SelectDropTarget(hItem);
		}
		else
		{
			m_tree.SelectDropTarget(NULL);
			m_tree.SetInsertMark(hItem, (nWhere == DD_BELOW));
		}
	}
	else
	{
		m_tree.SetInsertMark(NULL);
		m_tree.SelectDropTarget(NULL);
	}

	// Return the handle of the highlighted item.
	return hItem;
}

void CTreeDragDropHelper::OnTimer(UINT nIDEvent)
{
	TRACE ("CTreeDragDropHelper::OnTimer\n");

	if (nIDEvent == m_nScrollTimer)
	{
		// Reset the timer
		SetTimer(TIMER_SCROLL, SCROLL_INTERVAL);
			
		// Get the current cursor position and window height.
		CPoint point(::GetMessagePos());
		ScreenToClient(m_tree, &point);
			
		CRect rect;
		m_tree.GetClientRect(rect);
		int cy = rect.Height();

		HTREEITEM hFirstVisible = m_tree.GetFirstVisibleItem();
			
		// Scroll the window if the cursor is near the top or bottom.
		m_ddMgr.DragShowNolock(FALSE);

		if (point.y >= 0 && point.y <= SCROLL_MARGIN) 
		{
			m_tree.SelectDropTarget(NULL);
			m_tree.SetInsertMark(NULL);
			m_tree.SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), NULL);
		}
		else if (point.y >= cy - SCROLL_MARGIN && point.y <= cy) 
		{
			m_tree.SelectDropTarget(NULL);
			m_tree.SetInsertMark(NULL);
			m_tree.SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), NULL);
		}

		m_ddMgr.DragShowNolock(TRUE);
				
		// Kill the timer if the window did not scroll
		// redraw the drop target highlight regardless
		if (m_tree.GetFirstVisibleItem() == hFirstVisible)
			SetTimer(TIMER_SCROLL, 0);

		HighlightDropTarget();
	}
	else if (nIDEvent == m_nExpandTimer)
	{
		// If the cursor is hovering over a collapsed item, expand the tree.
		CPoint point(::GetMessagePos());
		m_tree.ScreenToClient(&point);
		
		DDWHERE nWhere;
		HTREEITEM hItem = HitTest(point, nWhere);
		
		if (hItem != NULL && nWhere == DD_ON && m_tree.ItemHasChildren(hItem) &&
			!(m_tree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED)) 
		{
			m_ddMgr.DragShowNolock(FALSE);

			m_tree.SelectDropTarget(NULL);
			m_tree.SetInsertMark(NULL);
			m_tree.Expand(hItem, TVE_EXPAND);

			m_ddMgr.DragShowNolock(TRUE);
			HighlightDropTarget();

			SetTimer(TIMER_EXPAND, 0); // kill the timer
			return;
		}
	}
}

VOID CALLBACK CTreeDragDropHelper::TimerProc(HWND /*hwnd*/, UINT /*uMsg*/, UINT idEvent, DWORD /*dwTime*/)
{
	if (s_pTDDH)
		s_pTDDH->OnTimer(idEvent); // pseudo message handler
}

HTREEITEM CTreeDragDropHelper::MoveTree(HTREEITEM hDest, HTREEITEM hSrc, DDWHERE nWhere)
{
	CHoldRedraw hr(m_tree);

	HTREEITEM htiNew = CopyTree(hDest, hSrc, nWhere);
	m_tree.DeleteItem(hSrc);

	return htiNew;
}

void CTreeDragDropHelper::BuildCopy(const HTREEITEM hti, TDDHCOPY* pCopy) const
{
	pCopy->hti = hti;
	pCopy->dwItemData = m_tree.GetItemData(hti);

	HTREEITEM htiChild = m_tree.GetChildItem(hti);

	while (htiChild)
	{
		TDDHCOPY htcChild;
		BuildCopy(htiChild, &htcChild);
		pCopy->childItems.Add(htcChild);

		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

HTREEITEM CTreeDragDropHelper::CopyTree(HTREEITEM hDest, const TDDHCOPY* pSrc, DDWHERE nWhere)
{
	HTREEITEM hSrc = pSrc->hti;

	// Get the attributes of item to be copied.
	int nImage = I_IMAGECALLBACK, nSelectedImage = I_IMAGECALLBACK;

	if (!m_bTreeUsesImageCallback)
	m_tree.GetItemImage(hSrc, nImage, nSelectedImage);

	CString sText = m_tree.GetItemText(hSrc);
	LPSTR szText = m_bTreeUsesTextCallback ? LPSTR_TEXTCALLBACK : (LPSTR)(LPCTSTR)sText;

	DWORD dwItemData = m_tree.GetItemData(hSrc);
	UINT uState = m_tree.GetItemState(hSrc, TVIS_BOLD | TVIS_STATEIMAGEMASK);
	UINT uMask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;

	HTREEITEM hParent = (nWhere != DD_ON) ? m_tree.GetParentItem(hDest) : hDest;
	HTREEITEM hAfter = NULL;
	
	if (nWhere == DD_ON)
		hAfter = TVI_LAST;
	
	else if (nWhere == DD_BELOW)
		hAfter = hDest;

	else // above
	{
		hAfter = m_tree.GetNextItem(hDest, TVGN_PREVIOUS);

		if (!hAfter)
			hAfter = TVI_FIRST;
	}

	// Create an exact copy of the item at the destination.
	HTREEITEM hNewItem = m_tree.InsertItem(uMask, szText, nImage, nSelectedImage, uState, 
								TVIS_BOLD | TVIS_STATEIMAGEMASK, dwItemData, hParent, hAfter);

	// copy children too
	if (pSrc->childItems.GetSize())
	{
		for (int nChild = 0; nChild < pSrc->childItems.GetSize(); nChild++)
		{
			const TDDHCOPY& htcChild = pSrc->childItems[nChild];
			CopyTree(hNewItem, &htcChild, DD_ON);
		}
	}

	// restore the expanded state
	if (m_tree.GetItemState(hSrc, TVIS_EXPANDED) & TVIS_EXPANDED)
		m_tree.Expand(hNewItem, TVE_EXPAND);
	else
		m_tree.Expand(hNewItem, TVE_COLLAPSE);

	return hNewItem;
}

HTREEITEM CTreeDragDropHelper::CopyTree(HTREEITEM hDest, HTREEITEM hSrc, DDWHERE nWhere)
{
	TDDHCOPY htcSrc;

	BuildCopy(hSrc, &htcSrc);
	return CopyTree(hDest, &htcSrc, nWhere);
}

BOOL CTreeDragDropHelper::CanMoveItem(HTREEITEM hti, TDDH_MOVE nDirection) const
{
	if (!hti)
		return FALSE;

	switch (nDirection)
	{
	case MOVE_DOWN:
		return (m_tree.GetNextItem(hti, TVGN_NEXT) != NULL);

	case MOVE_UP:
		return (m_tree.GetNextItem(hti, TVGN_PREVIOUS) != NULL);

	case MOVE_LEFT:
		{
			// must have a parent which will become its sibling
			// and not the root
			HTREEITEM htiParent = m_tree.GetParentItem(hti);
			return (htiParent && htiParent != TVI_ROOT);
		}
		break;

	case MOVE_RIGHT:
		// must have a prior sibling (which will become the parent)
		return CanMoveItem(hti, MOVE_UP);
	}

	return FALSE;
}

BOOL CTreeDragDropHelper::MoveSelection(TDDH_MOVE nDirection)
{
	switch (nDirection)
	{
	case MOVE_DOWN:
		return MoveSelectionDown();

	case MOVE_UP:
		return MoveSelectionUp();

	case MOVE_LEFT:
		return MoveSelectionLeft();

	case MOVE_RIGHT:
		return MoveSelectionRight();
	}

	return FALSE;
}

BOOL CTreeDragDropHelper::CanMoveSelection(TDDH_MOVE nDirection) const
{
	if (!m_selection.GetCount() || !m_selection.ItemsAreAllSiblings())
		return FALSE;

	// must be able to move all items
	POSITION pos = m_selection.GetFirstItemPos();
	
	while (pos)
	{
		HTREEITEM hti = m_selection.GetNextItem(pos);
		
		if (!CanMoveItem(hti, nDirection))
			return FALSE;
	}

	return TRUE;
}

BOOL CTreeDragDropHelper::MoveSelectionDown()
{
	if (CanMoveSelection(MOVE_DOWN))
	{
		// 0. save off the selection state so we can restore it after
		HTREEITEM htiTreeSel, htiAnchor;
		GetAnchorSel(htiAnchor, htiTreeSel);

		// 1. sort items
		m_selection.SortIfAllSiblings(FALSE);

		// 2. move items as siblings after the item following the last item
		CHTIList selection;
		m_selection.CopySelection(selection);
		m_selection.RemoveAll(TRUE, FALSE);

		HTREEITEM htiNext = m_tree.GetNextItem(selection.GetTail(), TVGN_NEXT);
		POSITION pos = selection.GetHeadPosition();

		while (pos)
		{
			HTREEITEM htiSel = selection.GetNext(pos);
			htiNext = CopyTree(htiNext, htiSel, DD_BELOW);

			UpdateAnchorSel(htiSel, htiNext, htiAnchor, htiTreeSel);

			m_tree.DeleteItem(htiSel);
			m_selection.AddItem(htiNext, FALSE);
		}
		
		// 5. restore tree state
		RestoreAnchorSel(htiAnchor, htiTreeSel);

		return TRUE;
	}

	return FALSE;
}

BOOL CTreeDragDropHelper::MoveSelectionUp()
{
	if (CanMoveSelection(MOVE_UP))
	{
		// 0. save off the selection state so we can restore it after
		HTREEITEM htiTreeSel, htiAnchor;
		GetAnchorSel(htiAnchor, htiTreeSel);

		// 1. sort items
		m_selection.SortIfAllSiblings(FALSE);

		// 2. move first item before its preceding item
		CHTIList selection;
		m_selection.CopySelection(selection);
		m_selection.RemoveAll(TRUE, FALSE);

		POSITION pos = selection.GetHeadPosition();
		HTREEITEM htiFirst = m_selection.GetNextItem(pos);
		HTREEITEM htiPrev = m_tree.GetNextItem(htiFirst, TVGN_PREVIOUS);

		HTREEITEM htiNew = CopyTree(htiPrev, htiFirst, DD_ABOVE);

		if (htiNew)
		{
			UpdateAnchorSel(htiFirst, htiNew, htiAnchor, htiTreeSel);

			m_tree.DeleteItem(htiFirst);
			m_selection.AddItem(htiNew, FALSE);

			// 3. copy rest of siblings after htiNew
			while (pos)
			{
				HTREEITEM htiSel = selection.GetNext(pos);
				htiNew = CopyTree(htiNew, htiSel, DD_BELOW);

				UpdateAnchorSel(htiSel, htiNew, htiAnchor, htiTreeSel);

				m_tree.DeleteItem(htiSel);
				m_selection.AddItem(htiNew, FALSE);
			}

			// 5. restore tree state
			RestoreAnchorSel(htiAnchor, htiTreeSel);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CTreeDragDropHelper::MoveSelectionLeft()
{
	if (CanMoveSelection(MOVE_LEFT))
	{
		// 0. save off the selection state so we can restore it after
		HTREEITEM htiTreeSel, htiAnchor;
		GetAnchorSel(htiAnchor, htiTreeSel);

		// 1. sort items
		m_selection.SortIfAllSiblings(FALSE);

		CHTIList selection;
		m_selection.CopySelection(selection);
		m_selection.RemoveAll(TRUE, FALSE);

		// 2. copy the items as siblings of the parent of the first item
		HTREEITEM htiPrev = m_tree.GetParentItem(selection.GetHead());
		POSITION pos = selection.GetHeadPosition();

		while (pos)
		{
			HTREEITEM htiSel = selection.GetNext(pos);
			htiPrev = CopyTree(htiPrev, htiSel, DD_BELOW);

			UpdateAnchorSel(htiSel, htiPrev, htiAnchor, htiTreeSel);

			m_tree.DeleteItem(htiSel);
			m_selection.AddItem(htiPrev, FALSE);
		}

		// 5. restore tree state
		RestoreAnchorSel(htiAnchor, htiTreeSel);
		
		return TRUE;
	}

	return FALSE;
}

BOOL CTreeDragDropHelper::MoveSelectionRight()
{
	if (CanMoveSelection(MOVE_RIGHT))
	{
		// 0. save off the selection state so we can restore it after
		HTREEITEM htiTreeSel, htiAnchor;
		GetAnchorSel(htiAnchor, htiTreeSel);

		// 1. sort items
		m_selection.SortIfAllSiblings(FALSE);

		CHTIList selection;
		m_selection.CopySelection(selection);
		m_selection.RemoveAll(TRUE, FALSE);

		// 2. copy the first item as a child of its immediate sibling
		POSITION pos = selection.GetHeadPosition();
		HTREEITEM htiFirst = m_selection.GetNextItem(pos);
		HTREEITEM htiParent = m_tree.GetNextItem(htiFirst, TVGN_PREVIOUS);

		HTREEITEM htiNew = CopyTree(htiParent, htiFirst, DD_ON);

		if (htiNew)
		{
			UpdateAnchorSel(htiFirst, htiNew, htiAnchor, htiTreeSel);

			m_tree.DeleteItem(htiFirst);
			m_selection.AddItem(htiNew, FALSE);

			// 3. then copy the rest of the selection as siblings of the first
			while (pos)
			{
				HTREEITEM htiSel = selection.GetNext(pos);
				htiNew = CopyTree(htiNew, htiSel, DD_BELOW);

				UpdateAnchorSel(htiSel, htiNew, htiAnchor, htiTreeSel);

				m_tree.DeleteItem(htiSel);
				m_selection.AddItem(htiNew, FALSE);
			}

			// 4. expand parent
			m_tree.Expand(htiParent, TVE_EXPAND);

			// 5. restore tree state
			RestoreAnchorSel(htiAnchor, htiTreeSel);

			return TRUE;
		}
	}

	return FALSE;
}

void CTreeDragDropHelper::GetAnchorSel(HTREEITEM& htiAnchor, HTREEITEM& htiTreeSel) const
{
	htiTreeSel = m_tree.GetSelectedItem();
	htiAnchor = m_selection.GetAnchor();
}

void CTreeDragDropHelper::UpdateAnchorSel(HTREEITEM htiPrev, HTREEITEM htiNew, 
										  HTREEITEM& htiAnchor, HTREEITEM& htiTreeSel) const
{
	if (htiTreeSel == htiPrev)
		htiTreeSel = htiNew;
	
	if (htiAnchor == htiPrev)
		htiAnchor = htiNew;
}

void CTreeDragDropHelper::RestoreAnchorSel(HTREEITEM htiAnchor, HTREEITEM htiTreeSel)
{
#ifdef _DEBUG
//	DWORD dwIDAnchor = m_tree.GetItemData(htiAnchor);
//	DWORD dwIDSel = m_tree.GetItemData(htiTreeSel);
#endif

	m_tree.SelectItem(htiTreeSel);
	m_selection.SetAnchor(htiAnchor);
}
