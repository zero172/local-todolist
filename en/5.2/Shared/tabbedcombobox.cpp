// tabbedcombobox.cpp : implementation file
//

#include "stdafx.h"
#include "..\ToDoList\todolist.h"
#include "tabbedcombobox.h"
#include "dlgunits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabbedComboBox

CTabbedComboBox::CTabbedComboBox(int nTabStops) : TABSTOPS(nTabStops), m_bResized(FALSE)
{
}

CTabbedComboBox::~CTabbedComboBox()
{
}


BEGIN_MESSAGE_MAP(CTabbedComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTabbedComboBox)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabbedComboBox message handlers

BOOL CTabbedComboBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
	
	return CComboBox::PreCreateWindow(cs);
}

void CTabbedComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC dc;
	
	if (!dc.Attach(lpDrawItemStruct->hDC))
		return;
	
	int nDC = dc.SaveDC();
	
	BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);
	COLORREF crBack = GetSysColor(IsWindowEnabled() ? (bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW) : COLOR_3DFACE);
	COLORREF crText = GetSysColor(IsWindowEnabled() ? (bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT) : COLOR_3DSHADOW);
	
	CRect rItem(lpDrawItemStruct->rcItem);
	
	dc.FillSolidRect(rItem, crBack);
	dc.SetTextColor(crText);
	
	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
	{
		dc.DrawFocusRect(rItem);
	}
	else if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			dc.DrawFocusRect(rItem);
	}
	
	// draw the item
	if (GetStyle() & CBS_HASSTRINGS)
	{
		if ((int)lpDrawItemStruct->itemID >= 0)	// Any item selected?
		{
			CDlgUnits dlu(GetParent());

			if (rItem.Height() <= dlu.ToPixelsY(9))
				rItem.DeflateRect(1, 1); // selected item
			else
				rItem.DeflateRect(2, 2); // listbox item
			
			CRect rColor(rItem);
			
			// draw text
			CRect rText(rItem);
			
			CString sText;
			GetLBText(lpDrawItemStruct->itemID, sText);

			// replace [\][t] with [\t]
			sText.Replace("\\t", "\t");
			
			if (!sText.IsEmpty())
				dc.TabbedTextOut(rText.left, rText.top, sText, 1, (int*)&TABSTOPS, rText.left);
		}
	}
	
	// Restore the DC state
	dc.RestoreDC(nDC);
	dc.Detach();
}

void CTabbedComboBox::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CComboBox::OnWindowPosChanged(lpwndpos);
}

int CTabbedComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_bResized)
	{
		m_bResized = TRUE;
		
		CDlgUnits dlu(GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9)); 
	}
	
	return 0;
}

void CTabbedComboBox::PreSubclassWindow() 
{
	if (!m_bResized)
	{
		m_bResized = TRUE;
		
		CDlgUnits dlu(GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9)); 
	}
	
	CComboBox::PreSubclassWindow();
}
