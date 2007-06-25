// checkcombobox.cpp : implementation file
//

#include "stdafx.h"
#include "checkcombobox.h"
#include "misc.h"
#include "dlgunits.h"
#include "themed.h"
#include "autoflag.h"
#include "misc.h"
#include "dialoghelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckComboBox

CCheckComboBox::CCheckComboBox(BOOL bAllowDelete, BOOL bCaseSensitive)
	: CAutoComboBox(bAllowDelete, bCaseSensitive)
{
	m_bItemHeightSet = FALSE;
	m_bDrawing = TRUE;
}

CCheckComboBox::~CCheckComboBox()
{
}


BEGIN_MESSAGE_MAP(CCheckComboBox, CAutoComboBox)
	//{{AFX_MSG_MAP(CCheckComboBox)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnDropdownCloseUp)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnDropdownCloseUp)
	ON_CONTROL(LBN_SELCHANGE, 1000, OnLBSelChange)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnEditKillFocus)
	ON_EN_KILLFOCUS(1001, OnEditKillFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckComboBox message handlers

void CCheckComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HDC hdc = lpDrawItemStruct->hDC;

	CRect rcBitmap = lpDrawItemStruct->rcItem;
	CRect rcText   = lpDrawItemStruct->rcItem;
	int nItem = (int)lpDrawItemStruct->itemID;

	CString sText;

	// 0 - No check, 1 - Empty check, 2 - Checked
	int nCheck = 0;

	// Check if we are drawing the static portion of the combobox
	if (nItem < 0) 
   {
		// Get the text
		sText = m_sText;

		// Don't draw any boxes on this item
		nCheck = 0;
	}
	else // Otherwise it is one of the items
	{
		GetLBText(lpDrawItemStruct->itemID, sText);
		nCheck = 1 + (GetItemData(lpDrawItemStruct->itemID) != 0);

		TEXTMETRIC metrics;
		GetTextMetrics(hdc, &metrics);

		rcBitmap.left    = 0;
		rcBitmap.right   = rcBitmap.left + metrics.tmHeight + metrics.tmExternalLeading + 6;
		rcBitmap.top    += 1;
		rcBitmap.bottom -= 1;

		rcText.left = rcBitmap.right;
	}

	if (nCheck > 0) 
	{
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

		UINT nState = DFCS_BUTTONCHECK;

		if (nCheck > 1)
			nState |= DFCS_CHECKED;

		// Draw the checkmark using DrawFrameControl
		CThemed::DrawFrameControl((CWnd*)this, CDC::FromHandle(hdc), rcBitmap, DFC_BUTTON, nState);
	}
	
	if (nCheck > 0 || IsType(CBS_DROPDOWNLIST))
		DrawItemText(hdc, nItem, rcText, sText, lpDrawItemStruct->itemState);
}

void CCheckComboBox::RefreshMaxDropWidth()
{
   int nMaxWidth = CDialogHelper::CalcMaxTextWidth(*this, 0, TRUE) + 18;
   SetDroppedWidth(nMaxWidth);
}

void CCheckComboBox::OnEditKillFocus()
{
	if (!GetDroppedState())
		ParseText(TRUE);
}

void CCheckComboBox::DrawItemText(HDC hdc, int nItem, CRect rText, const CString& sText, UINT nState)
{
	UNREFERENCED_PARAMETER(nItem);

	if (nState & ODS_SELECTED) 
	{
		SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
		SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else 
	{
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
	}
	
	// Erase and draw
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rText, 0, 0, 0);
	
	rText.DeflateRect(2, 0);
	DrawText(hdc, sText, sText.GetLength(), &rText, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	rText.InflateRect(2, 0);
	
	if ((nState & (ODS_FOCUS|ODS_SELECTED)) == (ODS_FOCUS|ODS_SELECTED))
		DrawFocusRect(hdc, &rText);
}

void CCheckComboBox::CheckAll(BOOL bCheck)
{
	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
		SetCheck(i, bCheck);
}

void CCheckComboBox::RecalcText(BOOL bUpdate, BOOL bNotify)
{
	CString sText;
	
	// Get the list count
	int nCount  = GetCount();
				
	for (int i = 0; i < nCount; i++) 
	{
		if (GetCheck(i)) 
		{
			CString strItem;
			GetLBText(i, strItem);
			
			if (!sText.IsEmpty())
			{
				sText += Misc::GetListSeparator();
				sText += ' '; // and a nice space
			}
			
			sText += strItem;
		}
	}
		
	// update edit field if necessary
	if (bUpdate)
	{
		if (!IsType(CBS_DROPDOWNLIST))
		{
			GetDlgItem(1001)->SetWindowText(sText);

			// notify parent
			if (bNotify && m_sText != sText)
				NotifyParent(CBN_EDITCHANGE);
		}
		else
			SetWindowText(sText);
	}
	
	m_sText = sText;
}

int CCheckComboBox::SetCheck(int nIndex, BOOL bFlag)
{
	int nResult = SetItemData(nIndex, bFlag);
	
	if (nResult < 0)
		return nResult;
	
	// update text
	RecalcText();
	
	// Redraw the window
	CComboBox::Invalidate(FALSE);
	
	return nResult;
}

BOOL CCheckComboBox::GetCheck(int nIndex) const
{
	return GetItemData(nIndex);
}

void CCheckComboBox::ParseText(BOOL bNotify)
{
	if (IsType(CBS_DROPDOWNLIST))
		return;

	// cache what's currently checked (for notifying)
	CStringArray aPrevChecked;

	if (bNotify)
		GetChecked(aPrevChecked);
		
	CString sEditText;
	GetDlgItem(1001)->GetWindowText(sEditText);
	
	// clear existing checks first but don't update window
	int nCount = GetCount();
	
	for (int i = 0; i < nCount; i++)
		SetItemData(i, 0);
	
	// now parse the text and set the check states
	CStringArray aText;

	if (Misc::ParseIntoArray(sEditText, aText))
	{
		int nText = aText.GetSize();

		while (nText--)
		{
			int nIndex = FindStringExact(-1, aText[nText]);
			
			if (nIndex != CB_ERR)
				SetItemData(nIndex, 1);
		}
	}

	if (GetDroppedState())
		ScGetCWnd()->Invalidate();
	
	// Redraw the window
	CComboBox::Invalidate(FALSE);

	// notify parent?
	if (bNotify)
	{
		// see what's now checked
		CStringArray aChecked;
		GetChecked(aChecked);
		
		// if different notify parent
		if (!Misc::ArraysMatch(aPrevChecked, aChecked))
			NotifyParent(CBN_SELCHANGE);
	}
}

// this handles messages destined for the dropped listbox
LRESULT CCheckComboBox::ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) 
	{
	// Make the combobox always return -1 as the current selection for
	// combos without an edit potion when drawing. This
	// causes the lpDrawItemStruct->itemID in DrawItem() to be -1
	// when the always-visible-portion of the combo is drawn
	case LB_GETCURSEL: 
		if (IsType(CBS_DROPDOWNLIST) && m_bDrawing)
			return -1;
		break;
		
	case WM_CHAR: 
		if (wp == VK_SPACE) 
		{
			ASSERT (GetDroppedState());

			// Get the current selection
			INT nIndex = GetCurSel();
			
			CRect rcItem;
			::SendMessage(hRealWnd, LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem);
			::InvalidateRect(hRealWnd, rcItem, FALSE);
			
			// Invert the check mark
			SetCheck(nIndex, !GetCheck(nIndex));
			return 0;
		}
		else if (wp == VK_ESCAPE && GetDroppedState())
		{
			m_bDropCancelled = TRUE;
			ShowDropDown(FALSE);
		}
		break;

	case WM_LBUTTONDOWN: 
		{
			ASSERT (GetDroppedState());

			// check first for a click in the dropped list 
			CPoint pt(lp);
			CRect rList;

			ScGetCWnd()->GetClientRect(rList);

			if (rList.PtInRect(pt)) // yes, so check which item
			{
				int nItem = GetCount();

				while (nItem--)
				{
					CRect rItem;
					::SendMessage(hRealWnd, LB_GETITEMRECT, nItem, (LPARAM)(LPRECT)rItem);

					if (rItem.PtInRect(pt))
					{
						// toggle check state
						::InvalidateRect(hRealWnd, rItem, FALSE);
						SetCheck(nItem, !GetCheck(nItem));

						return 0;
					}
				}
			}
			else // check for click outside the combo's top level parent
			{
				CRect rParent;
				GetTopLevelParent()->GetWindowRect(rParent);
				ScGetCWnd()->ScreenToClient(rParent);

				m_bDropCancelled = (!rParent.PtInRect(pt));
			}
		}
		// Do the default handling now (such as close the popup
		// window when clicked outside)
		break;
		
	case WM_LBUTTONUP: 
		{
			// Don't do anything here. This causes the combobox popup
			// windows to remain open after a selection has been made
			if (IsType(CBS_SIMPLE))
				return 0;
			else
			{
				LRESULT lr = CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
				SetCheck(0, GetCheck(0));
				return lr;
			}
		}
		break;
	}

	return CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
}

int CCheckComboBox::GetCurSel() const
{
	CAutoFlag af(m_bDrawing, FALSE);

	return CComboBox::GetCurSel();
}

void CCheckComboBox::NotifyParent(UINT nIDNotify)
{
	CWnd* pParent = CWnd::GetParent();

	if (pParent)
	{
		UINT nID = CWnd::GetDlgCtrlID();
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nID, nIDNotify), (LPARAM)m_hWnd);
	}
}

BOOL CCheckComboBox::IsType(UINT nComboType)
{
	return ((CWnd::GetStyle() & 0xf) == nComboType);
}

int CCheckComboBox::AddUniqueItem(const CString& sItem)
{
	if (sItem.Find(Misc::GetListSeparator()) != -1)
	{
		CStringArray aText;
		
		Misc::ParseIntoArray(sItem, aText);
		return CAutoComboBox::AddUniqueItems(aText);
	}
	
	// else add single item and mark as selected
	int nIndex = CAutoComboBox::AddUniqueItem(sItem);

	if (nIndex != CB_ERR)
		SetItemData(nIndex, 1);

	return nIndex;
}

void CCheckComboBox::HandleReturnKey()
{
	ParseText(TRUE);

	// default handling
	CAutoComboBox::HandleReturnKey();
}

HBRUSH CCheckComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CAutoComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// hook list box
	if (nCtlColor == CTLCOLOR_LISTBOX && !ScIsHooked())
		ScHookWindow(pWnd->GetSafeHwnd());
	
	return hbr;
}

void CCheckComboBox::PreSubclassWindow() 
{
	if (!m_bItemHeightSet) 
	{
		m_bItemHeightSet = TRUE;
		
		CDlgUnits dlu(CComboBox::GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9)); 
	}
	
	CAutoComboBox::PreSubclassWindow();
}

int CCheckComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAutoComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_bItemHeightSet) 
	{
		m_bItemHeightSet = TRUE;
		
		CDlgUnits dlu(CComboBox::GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9)); 
	}
	
	return 0;
}

BOOL CCheckComboBox::OnEditchange() 
{
	// update check states
	if (GetDroppedState())
		ParseText();
	
	return FALSE; // pass to parent
}

BOOL CCheckComboBox::OnDropdownCloseUp() 
{
	static BOOL bDown = FALSE;
	static CStringArray aPrevChecked;
	
	if (bDown) // therefore we're closing
	{
		if (!m_bDropCancelled)
		{
			// if the checked items have changed then notify parent
			CStringArray aChecked;
			GetChecked(aChecked);

			if (!Misc::ArraysMatch(aPrevChecked, aChecked))
				NotifyParent(CBN_SELCHANGE);
		}
		else // restore prev checked
		{
			SetChecked(aPrevChecked);
		}
	}
	else // opening
	{
		ParseText();
		RecalcText(FALSE); // updates m_sText

		GetChecked(aPrevChecked);

		m_bDropCancelled = FALSE;
	}

	bDown = !bDown;
	
	return FALSE; // pass to parent
}

// this handles messages destined for the embedded edit field
LRESULT CCheckComboBox::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wp == VK_DOWN && !GetDroppedState())
		{
			ShowDropDown();
				
			// eat message else it'll go to the edit window
			return 0L;
		}
		else if (wp == VK_ESCAPE && GetDroppedState())
		{
			m_bDropCancelled = TRUE;
			ShowDropDown(FALSE);
		}
		break;

	case WM_CHAR:
		if (wp == VK_SPACE && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			if ((GetDroppedState() && IsType(CBS_DROPDOWN)) || IsType(CBS_SIMPLE))
			{
				ScGetCWnd()->SendMessage(msg, wp, lp);
				return 0;
			}
		}
		else if (wp == VK_RETURN)
		{
			HandleReturnKey();
		}
		break;
	}
	
	return CAutoComboBox::WindowProc(hRealWnd, msg, wp, lp);
}

void CCheckComboBox::OnLBSelChange()
{
	// do nothing
}

BOOL CCheckComboBox::DeleteSelectedLBItem()
{
	if (CAutoComboBox::DeleteSelectedLBItem())
	{
		RecalcText();
		return TRUE;
	}

	// else 
	return FALSE;
}

int CCheckComboBox::GetChecked(CStringArray& aItems) const
{
	aItems.RemoveAll();

	int nCount = GetCount();
	
	for (int i = 0; i < nCount; i++)
	{
		if (GetItemData(i))
		{
			CString sItem;
			GetLBText(i, sItem);

			aItems.Add(sItem);
		}
	}	

	return aItems.GetSize();
}

void CCheckComboBox::SetChecked(const CStringArray& aItems)
{
	// clear existing checks first but don't update window
	int nCount = GetCount();
	
	for (int i = 0; i < nCount; i++)
		SetItemData(i, 0);
	
	// now set the check states
	int nItem = aItems.GetSize(), nChecked = 0;
	
	while (nItem--)
	{
		int nIndex = FindStringExact(-1, aItems[nItem]);
		
		if (nIndex != CB_ERR)
		{
			SetItemData(nIndex, 1);
			nChecked++;
		}
	}
	RecalcText(TRUE, FALSE);

	if (GetDroppedState())
		ScGetCWnd()->Invalidate();

	// Redraw the window
	CComboBox::Invalidate(FALSE);
}

void CCheckComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_DOWN && !GetDroppedState())
	{
		ShowDropDown();
		return;
	}
	else if (nChar == VK_ESCAPE && GetDroppedState())
		m_bDropCancelled = TRUE;
		
	CAutoComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
