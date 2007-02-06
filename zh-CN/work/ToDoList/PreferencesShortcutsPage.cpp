// PrefererencesShortcutsPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesShortcutsPage.h"

#include "..\shared\winclasses.h"
#include "..\shared\wclassdefines.h"
#include "..\shared\enstring.h"
#include "..\shared\holdredraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesShortcutsPage property page

CPreferencesShortcutsPage::CPreferencesShortcutsPage(CShortcutManager* pMgr, UINT nMenuID, BOOL bIgnoreGrayedItems) 
	: CPropertyPage(CPreferencesShortcutsPage::IDD), 
	m_nMenuID(nMenuID), m_pShortcutMgr(pMgr), m_bIgnoreGrayedItems(bIgnoreGrayedItems)
{
	//{{AFX_DATA_INIT(CPreferencesShortcutsPage)
	m_sOtherCmdID = _T("");
	//}}AFX_DATA_INIT
}

CPreferencesShortcutsPage::~CPreferencesShortcutsPage()
{
}

void CPreferencesShortcutsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesShortcutsPage)
	DDX_Control(pDX, IDC_CURHOTKEY, m_hkCur);
	DDX_Control(pDX, IDC_COMMANDS, m_tcCommands);
	DDX_Control(pDX, IDC_NEWHOTKEY, m_hkNew);
	DDX_Text(pDX, IDC_INUSE, m_sOtherCmdID);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesShortcutsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesShortcutsPage)
	ON_BN_CLICKED(IDC_ASSIGNSHORTCUT, OnAssignshortcut)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, IDC_COMMANDS, OnSelchangedShortcuts)
	ON_EN_CHANGE(IDC_NEWHOTKEY, OnChangeShortcut)
	ON_REGISTERED_MESSAGE(WM_NCG_DRAWITEM, OnGutterDrawItem)
	ON_REGISTERED_MESSAGE(WM_NCG_RECALCCOLWIDTH, OnGutterRecalcColWidth)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_COMMANDS, OnTreeCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesShortcutsPage message handlers

BOOL CPreferencesShortcutsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_tcCommands.AddGutterColumn(PSP_SHORTCUTCOLUMNID, CEnString(IDS_PSP_SHORTCUT));

	m_tcCommands.SetGutterColumnHeaderTitle(NCG_CLIENTCOLUMNID, CEnString(IDS_PSP_MENUITEM));
	m_tcCommands.ShowGutterPosColumn(FALSE);
	m_tcCommands.SetGridlineColor(OTC_GRIDCOLOR);
	m_tcCommands.EnableGutterColumnHeaderClicking(PSP_SHORTCUTCOLUMNID, FALSE);
	m_tcCommands.EnableGutterColumnHeaderClicking(NCG_CLIENTCOLUMNID, FALSE);
	
	if (m_nMenuID && m_pShortcutMgr)
	{
		m_tcCommands.SendMessage(WM_NULL);
		m_tcCommands.SendMessage(WM_SETREDRAW, FALSE);

		CMenu menu;
		HTREEITEM htiFirst = NULL;

		if (menu.LoadMenu(m_nMenuID))
		{
			for (int nPos = 0; nPos < (int)menu.GetMenuItemCount(); nPos++)
			{
				HTREEITEM hti = AddMenuItem(NULL, &menu, nPos);

				if (!htiFirst)
					htiFirst = hti;

				m_tcCommands.Expand(hti, TVE_EXPAND);
			}
		}

		if (htiFirst)
			m_tcCommands.EnsureVisible(htiFirst);

		m_tcCommands.SendMessage(WM_SETREDRAW, TRUE);

		// init hotkey controls
		// note: we no longer pass in m_pShortcutMgr->GetInvalidComb() because
		// the hotkey control does a less than perfect job of respecting these
		m_hkNew.SetRules(0, 0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CPreferencesShortcutsPage::AddMenuItem(HTREEITEM htiParent, const CMenu* pMenu, int nPos)
{
	CString sItem;
	pMenu->GetMenuString(nPos, sItem, MF_BYPOSITION);

	// remove '&'
	sItem.Replace("&&", "~~~");
	sItem.Replace("&", "");
	sItem.Replace("~~~", "&");
	
	// remove everything after '\t'
	int nTab = sItem.Find('\t');
	
	if (nTab >= 0)
		sItem = sItem.Left(nTab);
	
	if (!sItem.IsEmpty())
	{
		// don't add disabled items if req
		if (m_bIgnoreGrayedItems && (pMenu->GetMenuState(nPos, MF_BYPOSITION) & MF_GRAYED))
			return NULL;

		HTREEITEM hti = m_tcCommands.InsertItem(sItem, htiParent ? htiParent : TVI_ROOT);

		if (hti)
		{
			UINT nCmdID = pMenu->GetMenuItemID(nPos);
			
			if (nCmdID == (UINT)-1) // submenu
			{
				// make top level items bold
				if (!htiParent)
					m_tcCommands.SetItemState(hti, TVIS_BOLD, TVIS_BOLD);

				CMenu* pSubMenu = pMenu->GetSubMenu(nPos);

				if (pSubMenu)
				{
					for (int nSubPos = 0; nSubPos < (int)pSubMenu->GetMenuItemCount(); nSubPos++)
						m_tcCommands.Expand(AddMenuItem(hti, pSubMenu, nSubPos), TVE_EXPAND);
				}
			}
			else
			{
				DWORD dwShortcut = m_pShortcutMgr->GetShortcut(nCmdID);

				if (dwShortcut)
				{
					m_mapID2Shortcut[nCmdID] = dwShortcut;
					m_mapShortcut2HTI[dwShortcut] = hti;
				}

				m_tcCommands.SetItemData(hti, nCmdID);
			}

			return hti;
		}
	}

	return NULL;
}

void CPreferencesShortcutsPage::OnSelchangedShortcuts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	UINT nCmdID = (UINT)pNMTreeView->itemNew.lParam;
	DWORD dwShortcut = 0;
	
	m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

	WORD wVKeyCode = LOWORD(dwShortcut);
	WORD wModifiers = HIWORD(dwShortcut);

	m_hkCur.SetHotKey(wVKeyCode, wModifiers);
	m_hkNew.SetHotKey(wVKeyCode, wModifiers);

	BOOL bCanHaveShortcut = !m_tcCommands.ItemHasChildren(pNMTreeView->itemNew.hItem);

	m_hkNew.EnableWindow(bCanHaveShortcut);
	GetDlgItem(IDC_ASSIGNSHORTCUT)->EnableWindow(bCanHaveShortcut);
	GetDlgItem(IDC_CURLABEL)->EnableWindow(bCanHaveShortcut);
	GetDlgItem(IDC_NEWLABEL)->EnableWindow(bCanHaveShortcut);

	m_sOtherCmdID.Empty();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CPreferencesShortcutsPage::OnOK()
{
	// copy all the changes to m_pShortcutMgr
	POSITION pos = m_mapID2Shortcut.GetStartPosition();

	while (pos)
	{
		UINT nCmdID = 0;
		DWORD dwShortcut = 0;

		m_mapID2Shortcut.GetNextAssoc(pos, nCmdID, dwShortcut);
		ASSERT (nCmdID);

		m_pShortcutMgr->SetShortcut(nCmdID, dwShortcut);
	}

	m_pShortcutMgr->SaveSettings();
}


void CPreferencesShortcutsPage::OnAssignshortcut() 
{
	HTREEITEM htiSel = m_tcCommands.GetSelectedItem();

	if (!htiSel)
		return;
	
	UINT nCmdID = m_tcCommands.GetItemData(htiSel);

	if (nCmdID)
	{
		// remove any shortcut currently assigned to nCmdID
		DWORD dwPrevSC = 0;

		if (m_mapID2Shortcut.Lookup(nCmdID, dwPrevSC))
			m_mapShortcut2HTI.RemoveKey(dwPrevSC);

		WORD wVKeyCode = 0, wModifiers = 0;
		m_hkNew.GetHotKey(wVKeyCode, wModifiers);

		DWORD dwShortcut = MAKELONG(wVKeyCode, wModifiers);
		
		// handle special case where user is explicitly deleting a 
		// shortcut
		if (!dwShortcut)
			dwShortcut = NO_SHORTCUT;
		// else if anyone has this shortcut we must remove their mapping first
		else
		{
			HTREEITEM htiOther = NULL;

			if (m_mapShortcut2HTI.Lookup(dwShortcut, htiOther) && htiOther)
			{
				UINT nOtherCmdID = m_tcCommands.GetItemData(htiOther);

				if (nOtherCmdID)
					m_mapID2Shortcut.RemoveKey(nOtherCmdID);
			}
		}

		// update maps
		m_mapID2Shortcut[nCmdID] = dwShortcut;

		if (dwShortcut != NO_SHORTCUT)
		{
			m_mapShortcut2HTI[dwShortcut] = htiSel;
			m_hkCur.SetHotKey(LOWORD(dwShortcut), HIWORD(dwShortcut));
		}

		m_sOtherCmdID.Empty();

		m_tcCommands.RecalcGutter();
		m_tcCommands.RedrawGutter();

		UpdateData(FALSE);
	}
}

void CPreferencesShortcutsPage::OnChangeShortcut()
{
	HTREEITEM htiSel = m_tcCommands.GetSelectedItem();

	if (!htiSel)
		return;

	UINT nCmdID = m_tcCommands.GetItemData(htiSel);
	
	WORD wVKeyCode = 0, wModifiers = 0;
	m_hkNew.GetHotKey(wVKeyCode, wModifiers);
	
	// validate modifiers but only if a 'main' key has been pressed
	if (wVKeyCode)
	{
		WORD wValidModifiers = m_pShortcutMgr->ValidateModifiers(wModifiers, wVKeyCode);

		if (wValidModifiers != wModifiers)
		{
			wModifiers = wValidModifiers;
			m_hkNew.SetHotKey(wVKeyCode, wModifiers);
		}
	}

	DWORD dwShortcut = MAKELONG(wVKeyCode, wModifiers);
	
	// if anyone has this shortcut we show who it is
	HTREEITEM htiOther = NULL;
	m_mapShortcut2HTI.Lookup(dwShortcut, htiOther);
	
	if (htiOther && m_tcCommands.GetItemData(htiOther) != nCmdID)
		m_sOtherCmdID.Format(IDS_PSP_CURRENTLYASSIGNED, m_tcCommands.GetItemText(htiOther));
	else
		m_sOtherCmdID.Empty();

	UpdateData(FALSE);
}

LRESULT CPreferencesShortcutsPage::OnGutterDrawItem(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGDRAWITEM* pNCGDI = (NCGDRAWITEM*)lParam;

	if (pNCGDI->nColID == PSP_SHORTCUTCOLUMNID)
	{
		CRect rItem(pNCGDI->rItem);
		HTREEITEM hti = (HTREEITEM)pNCGDI->dwItem;

		if (m_tcCommands.ItemHasChildren(hti))
		{
			pNCGDI->pDC->FillSolidRect(rItem, ::GetSysColor(COLOR_3DHIGHLIGHT));
			pNCGDI->pDC->FillSolidRect(rItem.right - 1, rItem.top, 1, rItem.Height(), OTC_GRIDCOLOR);
		}
		else
		{
			BOOL bSelected = pNCGDI->bSelected;
			BOOL bFocused = m_tcCommands.HasFocus();
			COLORREF crTextColor = GetSysColor((bSelected && bFocused) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);
			
			// bkcolor
			if (bSelected)
			{
				if (bFocused)
					pNCGDI->pDC->FillSolidRect(rItem, GetSysColor(COLOR_HIGHLIGHT));
				else
					pNCGDI->pDC->FillSolidRect(rItem, GetSysColor(COLOR_3DFACE));
			}
			
			UINT nCmdID = m_tcCommands.GetItemData(hti);
			DWORD dwShortcut = 0;
		
			m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

			if (dwShortcut)
			{
				rItem.left += 3;
				
				pNCGDI->pDC->SetTextColor(crTextColor);
				pNCGDI->pDC->SetBkMode(TRANSPARENT);
				pNCGDI->pDC->DrawText(m_pShortcutMgr->GetShortcutText(dwShortcut), rItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

				rItem.left -= 3;
			}
			
			// vertical divider
			if (!bSelected)
				pNCGDI->pDC->FillSolidRect(rItem.right - 1, rItem.top, 1, rItem.Height(), OTC_GRIDCOLOR);
		}

		rItem.top = rItem.bottom - 1;
		pNCGDI->pDC->FillSolidRect(rItem, OTC_GRIDCOLOR);
	
		return TRUE; // we handled it
	}

	return FALSE;
}

LRESULT CPreferencesShortcutsPage::OnGutterRecalcColWidth(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGRECALCCOLUMN* pNCRC = (NCGRECALCCOLUMN*)lParam;

	if (pNCRC->nColID == PSP_SHORTCUTCOLUMNID)
	{
		int nLongest = 0;
		HTREEITEM hti = m_tcCommands.GetNextItem(TVI_ROOT, TVGN_CHILD);

		while (hti)
		{
			int nWidth = GetLongestShortcutText(hti, pNCRC->pDC);
			nLongest = max(nLongest, nWidth);

			hti = m_tcCommands.GetNextItem(hti, TVGN_NEXT);
		}

		if (nLongest)
			nLongest += 6; // some padding

		pNCRC->nWidth = max(40, nLongest);

		return TRUE; // we handled it
	}

	return FALSE;
}

int CPreferencesShortcutsPage::GetLongestShortcutText(HTREEITEM hti, CDC* pDC)
{
	int nLongest = 0;
	int nCmdID = m_tcCommands.GetItemData(hti);

	if (nCmdID)
	{
		DWORD dwShortcut = 0;
		m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

		if (dwShortcut)
		{
			CString sShortcut = m_pShortcutMgr->GetShortcutText(dwShortcut);
			nLongest = sShortcut.IsEmpty() ? 0 : pDC->GetTextExtent(sShortcut).cx;
		}
	}

	HTREEITEM htiChild = m_tcCommands.GetChildItem(hti);

	while (htiChild)
	{
		int nWidth = GetLongestShortcutText(htiChild, pDC);
		nLongest = max(nLongest, nWidth);

		htiChild = m_tcCommands.GetNextItem(htiChild, TVGN_NEXT);
	}

	return nLongest;
}

void CPreferencesShortcutsPage::OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCUSTOMDRAW* pNMCD = (NMCUSTOMDRAW*)pNMHDR;

	if (pNMCD->dwDrawStage == CDDS_PREPAINT)
		*pResult |= CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;	
		
	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		NMTVCUSTOMDRAW* pTVCD = (NMTVCUSTOMDRAW*)pNMCD;

		// set colors
		HTREEITEM hti = (HTREEITEM)pTVCD->nmcd.dwItemSpec;

		if (m_tcCommands.ItemHasChildren(hti))
		{
			CRect rItem;
			m_tcCommands.GetItemRect(hti, rItem, FALSE);

			CDC* pDC = CDC::FromHandle(pNMCD->hdc);
			pDC->FillSolidRect(rItem, GetSysColor(COLOR_3DHIGHLIGHT));
			pDC->SetBkMode(TRANSPARENT);

			if (pNMCD->uItemState & CDIS_SELECTED)
			{
				pTVCD->clrTextBk = GetSysColor(COLOR_3DSHADOW);
				pTVCD->clrText = 0;//GetSysColor(COLOR_3DDKSHADOW);
			}
			else
			{
				pTVCD->clrText = GetSysColor(COLOR_3DSHADOW);
			}

			*pResult |= CDRF_DODEFAULT;
		}
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPOSTPAINT)
	{
		CDC* pDC = CDC::FromHandle(pNMCD->hdc);
		NMTVCUSTOMDRAW* pTVCD = (NMTVCUSTOMDRAW*)pNMCD;

		CRect rItem(pNMCD->rc);
		rItem.top = rItem.bottom - 1;

		// if the bottom of the text coincides with the bottom of the 
		// item and we have the then take care not to draw over the focus rect
		if (pNMCD->uItemState & CDIS_FOCUS)
		{
			HTREEITEM hti = (HTREEITEM)pTVCD->nmcd.dwItemSpec;

			CRect rText;
			m_tcCommands.GetItemRect(hti, rText, TRUE);

			if (rText.bottom == rItem.bottom)
			{
				pDC->FillSolidRect(rItem.left, rItem.bottom - 1, rText.left - rItem.left, 1, OTC_GRIDCOLOR);
				pDC->FillSolidRect(rText.right, rItem.bottom - 1, rItem.right - rText.right, 1, OTC_GRIDCOLOR);
			}
			else
				pDC->FillSolidRect(rItem, OTC_GRIDCOLOR);
		}
		else
			pDC->FillSolidRect(rItem, OTC_GRIDCOLOR);

		*pResult |= CDRF_DODEFAULT;
	}
}


BOOL CPreferencesShortcutsPage::PreTranslateMessage(MSG* pMsg) 
{
	// special handling for hotkeys
	if (CWinClasses::IsClass(pMsg->hwnd, WC_HOTKEY))
		return FALSE;

	// handle delete key when tree has the focus
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_DELETE:
			{
				if (GetFocus() == &m_tcCommands)
				{
					HTREEITEM htiSel = m_tcCommands.GetSelectedItem();

					if (htiSel)
					{
						UINT nCmdID = m_tcCommands.GetItemData(htiSel);

						if (nCmdID)
						{
							DWORD dwShortcut = 0;

							if (m_mapID2Shortcut.Lookup(nCmdID, dwShortcut))
								m_mapShortcut2HTI.RemoveKey(dwShortcut);

							m_mapID2Shortcut[nCmdID] = 0;
						}
					}

					m_hkCur.SetHotKey(0, 0);
					m_hkNew.SetHotKey(0, 0);
					m_sOtherCmdID.Empty();

					m_tcCommands.RecalcGutter();
					m_tcCommands.RedrawGutter();
				}
			}
			break;
		}
		break;
	}

	
	return CPropertyPage::PreTranslateMessage(pMsg);
}
