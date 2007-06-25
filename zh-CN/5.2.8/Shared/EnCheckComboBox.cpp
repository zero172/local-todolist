// TDLCategoryComboBox.cpp : implementation file
//

#include "stdafx.h"

#include "enCheckComboBox.h"
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnCheckComboBox

CEnCheckComboBox::CEnCheckComboBox(BOOL bMulti, UINT nIDNoneString, UINT nIDAnyString) : 
	CCheckComboBox(TRUE, FALSE), m_bMultiSel(bMulti), m_nIDNoneString(nIDNoneString), m_nIDAnyString(nIDAnyString)
{
}

CEnCheckComboBox::~CEnCheckComboBox()
{
}


BEGIN_MESSAGE_MAP(CEnCheckComboBox, CCheckComboBox)
//{{AFX_MSG_MAP(CEnCheckComboBox)
//}}AFX_MSG_MAP
ON_CONTROL(LBN_SELCHANGE, 1000, OnLBSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnCheckComboBox message handlers

BOOL CEnCheckComboBox::EnableMultiSelection(BOOL bEnable)
{
	if (bEnable == m_bMultiSel)
		return TRUE;
	
	m_bMultiSel = bEnable;

	// if changing from multi selection and only one item was 
	// selected then set that as the single selection else clear all
	if (!m_bMultiSel)
	{
		CStringArray aItems;

		if (CCheckComboBox::GetChecked(aItems) == 1)
		{
			SelectString(0, aItems[0]);
			m_sText == aItems[0];
		}
		else
		{
			CheckAll(FALSE);
			m_sText.Empty();
		}
	}
	else 
	{
		// set the current selection to whatever was singly selected
		// provided it's not blank		
		CheckAll(FALSE);

		int nSel = GetCurSel();

		if (nSel != CB_ERR)
		{
			CString sItem;
			GetLBText(nSel, sItem);

			if (!sItem.IsEmpty())
				SetCheck(nSel, TRUE);
		}
	}
	
	if (GetSafeHwnd())
		CComboBox::Invalidate();
	
	return TRUE;
}

void CEnCheckComboBox::PreSubclassWindow() 
{
	CCheckComboBox::PreSubclassWindow();
}


BOOL CEnCheckComboBox::GetCheck(int nIndex) const
{
	if (m_bMultiSel)
		return CCheckComboBox::GetCheck(nIndex);
	
	// else
	return (nIndex != CB_ERR && CComboBox::GetCurSel() == nIndex);
}

int CEnCheckComboBox::GetChecked(CStringArray& aItems) const
{
	if (m_bMultiSel)
		return CCheckComboBox::GetChecked(aItems);
	
	// else
	aItems.RemoveAll();
	
	if (CComboBox::GetCurSel() != CB_ERR)
	{
		CString sItem;
		GetLBText(CComboBox::GetCurSel(), sItem);

		// we don't add the blank item
		if (!sItem.IsEmpty())
			aItems.Add(sItem);
	}
	
	return aItems.GetSize();
}

void CEnCheckComboBox::SetChecked(const CStringArray& aItems)
{
	if (m_bMultiSel)
		CCheckComboBox::SetChecked(aItems);
	else
	{
		if (aItems.GetSize() == 0)
			SetCurSel(-1);
		else
			SelectString(0, aItems[0]);
	}
}

int CEnCheckComboBox::SetCheck(int nIndex, BOOL bCheck)
{
	if (m_bMultiSel)
		return CCheckComboBox::SetCheck(nIndex, bCheck);

	// else
	if (bCheck)
		SetCurSel(nIndex);
	
	else if (nIndex == GetCurSel())
		SetCurSel(-1);

	return nIndex;
}

void CEnCheckComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (m_bMultiSel) // sanity check
		CCheckComboBox::DrawItem(lpDrawItemStruct);
	else
	{
		CString sText(m_sText);
		
		if (lpDrawItemStruct->itemID != -1)
			GetLBText(lpDrawItemStruct->itemID, sText);
		
		DrawItemText(lpDrawItemStruct->hDC, 
					lpDrawItemStruct->itemID,
					lpDrawItemStruct->rcItem,
					sText, 
					lpDrawItemStruct->itemState);
	}
}

LRESULT CEnCheckComboBox::ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// we don't prevent the base class from hooking the droplist
	// if single selecting, we just bypass it
	if (m_bMultiSel) 
		return CCheckComboBox::ScWindowProc(hRealWnd, msg, wp, lp);
	
	// else
	return CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
}

LRESULT CEnCheckComboBox::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (m_bMultiSel)
		return CCheckComboBox::WindowProc(hRealWnd, msg, wp, lp);
	
	// else
	return CAutoComboBox::WindowProc(hRealWnd, msg, wp, lp);
}

void CEnCheckComboBox::OnLBSelChange()
{
	if (m_bMultiSel)
		CCheckComboBox::OnLBSelChange();
	
	// else
	CComboBox::Default();
}

void CEnCheckComboBox::DrawItemText(HDC hdc, int nItem, CRect rText, const CString& sText, UINT nState)
{
	CString sEnText(sText);
	CString sNone("<none>"), sAny("<any>");
	
	if (m_nIDNoneString)
		sNone.LoadString(m_nIDNoneString);
	
	if (m_nIDAnyString)
		sAny.LoadString(m_nIDAnyString);
	
	if (m_bMultiSel)
	{
		// if drawing the comma-delimited list and it includes a blank
		// item, append <none> to the text
		if (nItem == -1)
		{
			int nBlank = FindStringExact(0, "");

			if (nBlank != -1 && GetCheck(nBlank))
			{
				if (!sEnText.IsEmpty())
					sEnText += Misc::GetListSeparator();

				sEnText += sNone;
			}
			else if (sEnText.IsEmpty())
				sEnText = sAny;
		}
		else if (nItem != -1 && sEnText.IsEmpty())
			sEnText = sNone;
	}
	else
	{
		if (nItem == -1 || sEnText.IsEmpty())
			sEnText = sAny;
	}
	
	CCheckComboBox::DrawItemText(hdc, nItem, rText, sEnText, nState);
}

