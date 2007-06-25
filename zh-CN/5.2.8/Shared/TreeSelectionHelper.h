// TreeSelectionHelper.h: interface for the CTreeSelectionHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREESELECTIONHELPER_H__098294B4_8B41_4369_8522_FE1637BA7EA1__INCLUDED_)
#define AFX_TREESELECTIONHELPER_H__098294B4_8B41_4369_8522_FE1637BA7EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class CHTIList : public CList<HTREEITEM, HTREEITEM>
{
public:
	CHTIList() {}
	CHTIList(const CHTIList& list) { Copy(list); }

	const CHTIList& CHTIList::operator=(const CHTIList& list)
	{
		Copy(list);
		return *this;
	}

	void Copy(const CHTIList& list)
	{
		RemoveAll();
		POSITION pos = list.GetHeadPosition();

		while (pos)
			AddTail(list.GetNext(pos));
	}
};

class CTreeSelectionHelper  
{
public:
	CTreeSelectionHelper(CTreeCtrl& tree);
	virtual ~CTreeSelectionHelper();

	void SetAnchor(HTREEITEM htiAnchor) { m_htiAnchor = htiAnchor; }
	HTREEITEM GetAnchor() { return m_htiAnchor; }

	// 1 = add, 0 = remove, -1 = toggle
	BOOL SetItems(HTREEITEM htiFrom, HTREEITEM htiTo, int nState, BOOL bRedraw = TRUE);
	BOOL SetItem(HTREEITEM hti, int nState, BOOL bRedraw = TRUE);

	inline BOOL AddItem(HTREEITEM hti, BOOL bRedraw = TRUE) { return SetItem(hti, 1, bRedraw); }
	inline BOOL ToggleItem(HTREEITEM hti, BOOL bRedraw = TRUE) { return SetItem(hti, -1, bRedraw); }
	BOOL RemoveItem(HTREEITEM hti, BOOL bRemoveFromHistory = FALSE, BOOL bRedraw = TRUE);

	BOOL RemoveAll(BOOL bRemoveFromHistory = FALSE, BOOL bRedraw = TRUE);
	BOOL AddItems(HTREEITEM htiFrom, HTREEITEM htiTo, BOOL bRedraw = TRUE);
	BOOL ToggleItems(HTREEITEM htiFrom, HTREEITEM htiTo, BOOL bRedraw = TRUE);
	BOOL AddAll(BOOL bRedraw = TRUE);

	inline HTREEITEM GetFirstItem() const { return GetCount() ? m_lstSelection.GetHead() : NULL; }
	inline HTREEITEM GetLastItem() const { return GetCount() ? m_lstSelection.GetTail() : NULL; }
	inline POSITION GetFirstItemPos() const { return m_lstSelection.GetHeadPosition(); }
	inline HTREEITEM GetNextItem(POSITION& pos) const { return m_lstSelection.GetNext(pos); }
	inline int GetCount() const { return m_lstSelection.GetCount(); }
	inline BOOL HasItem(HTREEITEM hti) const { return (m_lstSelection.Find(hti) != NULL); }

	inline void CopySelection(CHTIList& selection) const { selection = m_lstSelection; }

	BOOL ContainsAllItems() const;
	void InvalidateAll(BOOL bErase = TRUE);
	BOOL AnyItemsHaveChildren() const;

	BOOL ItemsAreAllParents() const;
	BOOL ItemsAreAllSiblings() const;

	void SortIfAllSiblings(BOOL bAscending);
	static int SortProc(const void* item1, const void* item2);

	// removes any items which are children of other items in the list
	void RemoveChildDuplicates() { RemoveChildDuplicates(m_lstSelection, m_tree); }
	static void RemoveChildDuplicates(CHTIList& selection, const CTreeCtrl& tree);

	// returns TRUE if any direct ancestor is selected
	BOOL HasSelectedParent(HTREEITEM hti) const;
	BOOL AnchorHasSelectedParent() const { return HasSelectedParent(m_htiAnchor); }

	inline BOOL IsItemExpanded(HTREEITEM hti) const
		{ return (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED); }

	// history
	BOOL NextSelection(BOOL bRedraw = TRUE);
	BOOL HasNextSelection() const;
	BOOL PrevSelection(BOOL bRedraw = TRUE);
	BOOL HasPrevSelection() const;
   void ClearHistory();
   void RemoveItemFromHistory(HTREEITEM hti);

	const CTreeCtrl& TreeCtrl() const { return m_tree; }

protected:
	CTreeCtrl& m_tree;
	CHTIList m_lstSelection;
	CArray<CHTIList, CHTIList&> m_aHistory;
	int m_nCurSelection;
	HTREEITEM m_htiAnchor;

protected:
	void InvalidateItem(HTREEITEM hti);
	static BOOL HasSelectedParent(HTREEITEM hti, const CHTIList& selection, const CTreeCtrl& tree);
	int GetItemPos(HTREEITEM hti);
	void AddAll(HTREEITEM hti);

	struct SORTITEM
	{
		HTREEITEM hti;
		int nPos;
	};

	typedef CArray<SORTITEM, SORTITEM&> CSortArray;

	int BuildSortArray(CSortArray& aItems);
};

#endif // !defined(AFX_TREESELECTIONHELPER_H__098294B4_8B41_4369_8522_FE1637BA7EA1__INCLUDED_)
