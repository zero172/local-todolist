// autocombobox.cpp : implementation file
//

#include "stdafx.h"
#include "autocombobox.h"
#include "holdredraw.h"
#include "dialoghelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

void AFXAPI DDX_AutoCBString(CDataExchange* pDX, int nIDC, CString& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);

	if (pDX->m_bSaveAndValidate)
	{
		::DDX_CBString(pDX, nIDC, value);
	}
	else
	{
		// try exact first
		int nIndex = (int)::SendMessage(hWndCtrl, CB_FINDSTRINGEXACT, (WPARAM)-1,
										(LPARAM)(LPCTSTR)value);

		// then partial
		if (nIndex == CB_ERR)
		{
			nIndex = ::SendMessage(hWndCtrl, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)value);

			// if there's a match, check its not partial
			if (nIndex != CB_ERR)
			{
				CString sItem;
				int nLen = ::SendMessage(hWndCtrl, CB_GETLBTEXTLEN, nIndex, 0);
				::SendMessage(hWndCtrl, CB_GETLBTEXT, nIndex, (LPARAM)(LPCTSTR)sItem.GetBuffer(nLen + 1));
				sItem.ReleaseBuffer();

				if (value.CompareNoCase(sItem))
					nIndex = CB_ERR;
			}
		}

		if (nIndex == CB_ERR)
		{
			if (value.IsEmpty())
				::SendMessage(hWndCtrl, CB_SETCURSEL, (WPARAM)-1, 0L);
			else
				::SendMessage(hWndCtrl, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
		}
		else
		{
			// select it
			SendMessage(hWndCtrl, CB_SETCURSEL, nIndex, 0L);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox

CAutoComboBox::CAutoComboBox(BOOL bAllowDelete, BOOL bCaseSensitive) : 
	m_bAllowDelete(bAllowDelete), m_hwndListbox(NULL), 
	m_bCaseSensitive(bCaseSensitive)
{
}

CAutoComboBox::~CAutoComboBox()
{
}


BEGIN_MESSAGE_MAP(CAutoComboBox, CComboBox)
	//{{AFX_MSG_MAP(CAutoComboBox)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
//	ON_WM_KILLFOCUS()
//	ON_CONTROL_REFLECT_EX(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT_EX(CBN_SELENDOK, OnSelEndOK)
	ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, OnSelChange)
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnCloseUp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox message handlers

BOOL CAutoComboBox::OnCloseUp()
{
	// notify parent of (possible) selection change
	NotifyParent(CBN_SELCHANGE);
	
	return FALSE; // pass to parent
}

BOOL CAutoComboBox::OnSelEndOK()
{
	OnSelChange();
	HandleReturnKey();
	return FALSE; // continue routing
}

BOOL CAutoComboBox::OnSelChange()
{
	// make sure the edit control is up to date
	if (IsHooked())
		SetWindowText(GetSelectedItem());

	// eat notification if dropped down
	// ie we don't send selection notifications until the dropdown closes
	return GetDroppedState();
}

CString CAutoComboBox::GetSelectedItem() const
{
	CString sSel;
	int nSel = GetCurSel();

	if (nSel != CB_ERR)
		GetLBText(nSel, sSel);
	
	return sSel;
}

/*
BOOL CAutoComboBox::OnKillfocus() 
{
	HandleReturnKey();
	return FALSE; // continue routing
}
*/

int CAutoComboBox::FindStringExact(int nIndexStart, const CString& sItem, BOOL bCaseSensitive) const
{
	int nFind = nIndexStart; // default
	
	if (!sItem.IsEmpty())
	{
		// because more than one item might exist if were doing a case-sensitive
		// search we can't just stop if the first find doesn't exactly match
		// because there still may be further matches
		BOOL bContinue = TRUE;
		
		while (bContinue)
        {
			int nPrevFind = nFind;
			nFind = CComboBox::FindStringExact(nFind, sItem);
			
			// if no match then definitely done
			if (nFind <= nPrevFind)
			{
				nFind = -1;
				bContinue = FALSE;
			}
			else if (!bCaseSensitive)
				bContinue = FALSE;
			else
			{
				// else if (bCaseSensitive)
				ASSERT (nFind != CB_ERR);
				ASSERT (bCaseSensitive);
				
				// test for real exactness because FindStringExact is not case sensitive
				CString sFind;
				GetLBText(nFind, sFind);
				
				bContinue = !(sItem == sFind); // differ in case
			}
        }
	}
	else // special case: look for empty item
	{
		nFind = GetCount();

		while (nFind--)
		{
			CString sLBItem;
			GetLBText(nFind, sLBItem);

			if (sLBItem.IsEmpty())
				break;
		}
	}
	
	return nFind;
}

int CAutoComboBox::AddUniqueItem(const CString& sItem)
{
	int nIndex = CB_ERR;
	
	if (!sItem.IsEmpty())
	{
		int nFind = FindStringExact(-1, sItem, m_bCaseSensitive);
		
		if (nFind == CB_ERR)
		{
			nIndex = CComboBox::AddString(sItem); // add at end
			
			if (nIndex != CB_ERR)
				RefreshMaxDropWidth();
		}
		else // reinsert as text may have changed
			nIndex = InsertUniqueItem(nFind, sItem);
	}
	
	return nIndex;
}

int CAutoComboBox::InsertUniqueItem(int nIndex, const CString& sItem)
{
	if (!sItem.IsEmpty())
	{
		int nFind = FindStringExact(-1, sItem, m_bCaseSensitive);
		
		if (nFind != CB_ERR)
		{
			CString sLBItem;
			GetLBText(nFind, sLBItem);
			
			if (nIndex != nFind || sItem != sLBItem)
			{
				// save selection so we can restore it
				int nSel = GetCurSel();
				CString sSelItem;
				
				if (nSel != CB_ERR)
					GetLBText(nSel, sSelItem);
				
				DeleteString(nFind); // remove original
				nIndex = CComboBox::InsertString(nIndex, sItem); // re-insert
				
				if (nIndex != CB_ERR)
					RefreshMaxDropWidth();
				
				// restore selection
				if (nSel != CB_ERR)
					SelectString(-1, sSelItem);
				
				return nIndex;
			}
		}
		else
		{
			nIndex = CComboBox::InsertString(nIndex, sItem); // re-insert
			
			if (nIndex != CB_ERR)
				RefreshMaxDropWidth();
			
			return nIndex;
		}
	}
	
	return CB_ERR; // invalid item
}

void CAutoComboBox::RefreshMaxDropWidth()
{
   CDialogHelper::RefreshMaxDropWidth(*this);
}

void CAutoComboBox::OnSize(UINT nType, int cx, int cy) 
{
	CWnd* pEdit = GetDlgItem(1001);

	// if the edit control does not have the focus then hide the selection
	if (pEdit && pEdit != GetFocus())
	{
		CHoldRedraw hr(*pEdit);
		CComboBox::OnSize(nType, cx, cy);
	
		pEdit->SendMessage(EM_SETSEL, (UINT)-1, 0);
	}
	else
		CComboBox::OnSize(nType, cx, cy);
}

int CAutoComboBox::AddUniqueItems(const CStringArray& aItems)
{
    int nItem = aItems.GetSize(), nCount = 0;

    while (nItem--)
    {
        if (AddUniqueItem(aItems[nItem]) != CB_ERR)
            nCount++;
    }

    return nCount ? 0 : CB_ERR;
}

int CAutoComboBox::AddUniqueItems(const CAutoComboBox& cbItems)
{
    CStringArray aItems;

    if (cbItems.GetItems(aItems))
        return AddUniqueItems(aItems);

    // else
    return 0;
}

int CAutoComboBox::GetItems(CStringArray& aItems) const
{
    int nItem = GetCount();

    aItems.RemoveAll();
    aItems.SetSize(nItem);

    while (nItem--)
    {
        CString sItem;
		GetLBText(nItem, sItem);

        aItems.SetAt(nItem, sItem); // maintain order
    }

    return aItems.GetSize();
}

// messages destined for the edit control
LRESULT CAutoComboBox::WindowProc(HWND /*hRealWnd*/, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		// <Ctrl> + <Delete>
		if (m_bAllowDelete && wp == VK_DELETE && GetDroppedState() && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			if (DeleteSelectedLBItem())
			{
				// eat message else it'll go to the edit window
				return 0L;
			}
		}
		// <Return>
		else if (wp == VK_RETURN)
		{
			if (GetDroppedState())
				ShowDropDown(FALSE);

			// can be simple or not dropped
//			if (IsSimpleCombo() || !GetDroppedState())
			HandleReturnKey();
			
			return 0L;
		}
		else if (wp == VK_DOWN && !GetDroppedState())
		{
			ShowDropDown();
			return 0L; // eat
		}
		break;
		
	case WM_GETDLGCODE:
		// special handling of the return key
		if (lp)
		{
			LPMSG pMsg = (LPMSG)lp;

			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
			{
				// can be simple or not dropped
				if (IsSimpleCombo() || !GetDroppedState())
					return DLGC_WANTALLKEYS;
			}
		}
		break;

	case WM_KILLFOCUS:
		HandleReturnKey();
		break;
	}
	
	return CSubclassWnd::Default();
}

void CAutoComboBox::HandleReturnKey()
{
	if (IsHooked())
	{
		CString sEdit;
		GetCWnd()->GetWindowText(sEdit);
		
		int nAdd = AddUniqueItem(sEdit);
		
		if (nAdd != CB_ERR)
			CComboBox::GetParent()->SendMessage(WM_ACB_ITEMADDED, 
												MAKEWPARAM(CWnd::GetDlgCtrlID(), nAdd),
												(LPARAM)(LPCTSTR)sEdit);
		else // send a possible selection change
		{
			SelectString(-1, sEdit);
			NotifyParent(CBN_SELCHANGE);
		}
	}
}

void CAutoComboBox::NotifyParent(UINT nIDNotify)
{
	CWnd* pParent = CWnd::GetParent();

	if (pParent)
	{
		UINT nID = CWnd::GetDlgCtrlID();
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nID, nIDNotify), (LPARAM)m_hWnd);
	}
}

BOOL CAutoComboBox::IsSimpleCombo()
{
	return ((CComboBox::GetStyle() & 0xf) == CBS_SIMPLE);
}

HBRUSH CAutoComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// save handle of listbox window
	if (m_bAllowDelete && !m_hwndListbox && nCtlColor == CTLCOLOR_LISTBOX)
		m_hwndListbox = pWnd->GetSafeHwnd();
	
	// and hook edit box
	if (nCtlColor == CTLCOLOR_EDIT && !IsHooked())
		HookWindow(pWnd->GetSafeHwnd());
	
	return hbr;
}

BOOL CAutoComboBox::DeleteSelectedLBItem()
{
	ASSERT(m_bAllowDelete);
	int nSelItem = ::SendMessage(m_hwndListbox, LB_GETCURSEL, 0, 0);
	
	if (nSelItem >= 0)
	{
		CString sCurItem, sSelItem;
		int nCurSel = GetCurSel();
		
		// save existing selection
		if (nCurSel != -1)
			GetWindowText(sCurItem);
		
		GetLBText(nSelItem, sSelItem); // need this for notifying parent
		::SendMessage(m_hwndListbox, LB_DELETESTRING, nSelItem, 0);
		
		// restore combo selection
		if (!sCurItem.IsEmpty())
			SelectString(0, sCurItem);
		
		// notify parent that we've been fiddling
		CComboBox::GetParent()->SendMessage(WM_ACB_ITEMDELETED, 
												MAKEWPARAM(CWnd::GetDlgCtrlID(), nSelItem), 
												(LPARAM)(LPCTSTR)sSelItem);

		return TRUE;
	}

	// else
	return FALSE;
}

void CAutoComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	HandleReturnKey();
}
