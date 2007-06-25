// TreeCtrlHelper.h: interface for the CTreeCtrlHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREECTRLHELPER_H__F6652DBE_3770_4E1C_A246_057AD6AD16B7__INCLUDED_)
#define AFX_TREECTRLHELPER_H__F6652DBE_3770_4E1C_A246_057AD6AD16B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

typedef CMap<HTREEITEM, HTREEITEM, int, int&> CMapIndices;

enum TCH_EDGE { TCHE_TOP, TCHE_BOTTOM };
enum TCH_CHECK { TCHC_UNCHECKED, TCHC_CHECKED, TCHC_MIXED };

class CTreeCtrlHelper  
{
public:
	CTreeCtrlHelper(CTreeCtrl& tree);
	virtual ~CTreeCtrlHelper();

	BOOL HasFocus(BOOL bIncEditing = TRUE);
	BOOL IsItemExpanded(HTREEITEM hItem) const;
	void ExpandAll(BOOL bExpand = TRUE, HTREEITEM hti = NULL);
	void ExpandItem(HTREEITEM hti, BOOL bExpand = TRUE);

	void SetItemIntegral(HTREEITEM hti, int iIntegral);
	
	void SetItemChecked(HTREEITEM hti, BOOL bChecked); // 2 state
	void SetItemChecked(HTREEITEM hti, TCH_CHECK nChecked); // 3 state
	TCH_CHECK GetItemCheckState(HTREEITEM hti);

	inline void EndLabelEdit(BOOL bCancel) { SendMessage(m_tree, TVM_ENDEDITLABELNOW, bCancel, 0); }
	CString GetItemPath(HTREEITEM hti, int nMaxElementLen = -1) const; // nMaxElementLen relates to each element of the path

	void InvalidateItem(HTREEITEM hti, BOOL bChildren = TRUE);
	void GetClientRect(LPRECT lpRect, HTREEITEM htiFrom);

	int GetItemPos(HTREEITEM hti, HTREEITEM htiParent);
	int GetItemLevel(HTREEITEM hti);

	HTREEITEM FindItem(DWORD dwItemData, HTREEITEM htiStart) const;

	BOOL IsItemBold(HTREEITEM hti);
	void SetItemBold(HTREEITEM hti, BOOL bBold = TRUE);
	void SetTopLevelItemsBold(BOOL bBold = TRUE);

	void SetItemStateEx(HTREEITEM hti, UINT nState, UINT nMask, BOOL bChildren = FALSE);

	HTREEITEM GetTopLevelParentItem(HTREEITEM hti) const;
	HTREEITEM GetNextTopLevelItem(HTREEITEM hti, BOOL bNext = TRUE) const;

	// returns the top level item whose child is the first visible item (or itself)
	HTREEITEM GetFirstVisibleTopLevelItem(int& nPos); // return 0 if no items
	HTREEITEM GetTopLevelParentItem(HTREEITEM hti);

	int BuildVisibleIndexMap() { return BuildVisibleIndexMap(VIMap()); }
	BOOL ItemLineIsOdd(HTREEITEM hti) { return ItemLineIsOdd(VIMap(), hti); }

	int BuildVisibleIndexMap(CMapIndices& index);
	BOOL ItemLineIsOdd(CMapIndices& index, HTREEITEM hti);

	// return increments of item height
	void SetMinDistanceToEdge(HTREEITEM htiFrom, TCH_EDGE nToEdge, int nItems);
	int GetDistanceToEdge(HTREEITEM htiFrom, TCH_EDGE nToEdge);

	// get next/prev selectable items, NULL if none
	HTREEITEM GetNextPageVisibleItem(HTREEITEM hti) const;
	HTREEITEM GetPrevPageVisibleItem(HTREEITEM hti) const;
	HTREEITEM GetNextVisibleItem(HTREEITEM hti, BOOL bAllowChildren = TRUE) const;
	HTREEITEM GetPrevVisibleItem(HTREEITEM hti, BOOL bAllowChildren = TRUE) const;

	int FindItem(HTREEITEM htiFind, HTREEITEM htiStart); // return -1 for above, 1 for below, 0 if same
	BOOL IsFullyVisible(HTREEITEM hti) const;

	void EnsureVisibleEx(HTREEITEM hti, BOOL bVPartialOK = TRUE, BOOL bHPartialOK = TRUE);

protected:
	CTreeCtrl& m_tree;
	mutable CMapIndices* m_pVisibleIndices;

protected:
	void AddVisibleItemToIndex(CMapIndices& index, HTREEITEM hti);

	CMapIndices& VIMap();
	const CMapIndices& VIMap() const;
};

#endif // !defined(AFX_TREECTRLHELPER_H__F6652DBE_3770_4E1C_A246_057AD6AD16B7__INCLUDED_)
