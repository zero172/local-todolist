// PreferencesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesDlg.h"

#include "..\shared\winclasses.h"
#include "..\shared\wclassdefines.h"
#include "..\shared\enstring.h"
#include "..\shared\dialoghelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

// default priority colors
const COLORREF PRIORITYLOWCOLOR = RGB(30, 225, 0);
const COLORREF PRIORITYHIGHCOLOR = RGB(255, 0, 0);

const char PATHDELIM = '>';

CPreferencesDlg::CPreferencesDlg(CShortcutManager* pShortcutMgr, UINT nMenuID, 
								 const CContentMgr* pContentMgr, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PREFERENCES, pParent), 
	m_pageShortcuts(pShortcutMgr, nMenuID, FALSE), m_pageUI(pContentMgr)
{
}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDlg)
	DDX_Control(pDX, IDC_PAGES, m_tcPages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_HELP2, OnHelp)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PAGES, OnSelchangedPages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg message handlers

int CPreferencesDlg::DoModal(int nInitPage)
{
	m_nInitPage = nInitPage;

	return CDialog::DoModal();
}

BOOL CPreferencesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_tcPages.SetIndent(0);

	AddPage(&m_pageGen, IDS_PREF_GEN);
	AddPage(&m_pageMultiUser, IDS_PREF_MULTIUSER);
	AddPage(&m_pageFile, IDS_PREF_FILE);
	AddPage(&m_pageUI, IDS_PREF_UI);
	AddPage(&m_pageUITasklist, IDS_PREF_UITASK);
	AddPage(&m_pageUITasklistColors, IDS_PREF_UITASKCOLOR);
	AddPage(&m_pageTask, IDS_PREF_TASK);
	AddPage(&m_pageTaskDef, IDS_PREF_TASKDEF);
	AddPage(&m_pageExport, IDS_PREF_EXPORT);
	AddPage(&m_pageTool, IDS_PREF_TOOLS);
	AddPage(&m_pageShortcuts, IDS_PREF_SHORTCUT);

	CRect rPPHost;
	GetDlgItem(IDC_HOSTFRAME)->GetWindowRect(rPPHost);
	ScreenToClient(rPPHost);

	if (m_pphost.Create(rPPHost, this))
	{
		if (m_nInitPage < 0 || m_nInitPage >= m_pphost.GetPageCount())
			m_nInitPage = AfxGetApp()->GetProfileInt("Preferences", "StartPage", 0);

		SetActivePage(m_nInitPage);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesDlg::OnOK()
{
	CDialog::OnOK();

	m_pphost.OnOK();
}

void CPreferencesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	AfxGetApp()->WriteProfileInt("Preferences", "StartPage", m_pphost.GetActiveIndex());
}

BOOL CPreferencesDlg::PreTranslateMessage(MSG* pMsg) 
{
	// special handling for hotkeys
	if (CWinClasses::IsClass(pMsg->hwnd, WC_HOTKEY))
		return FALSE;

	// handle F1 help
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F1)
	{
		OnHelp();
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPreferencesDlg::OnHelp() 
{
	// who is the active page?
	int nSel = m_pphost.GetActiveIndex();
	CString sHelpPage = "pref.htm";

	switch (nSel)
	{
	case PREFPAGE_GEN:
		sHelpPage += "#gen";
		break;
		
	case PREFPAGE_MULTIUSER:
		sHelpPage += "#multiuser";
		break;
		
	case PREFPAGE_FILE:
		sHelpPage += "#file";
		break;
		
	case PREFPAGE_UI:
		sHelpPage += "#ui";
		break;
		
	case PREFPAGE_UITASK:
		sHelpPage += "#uitask";
		break;
		
	case PREFPAGE_TASK:
		sHelpPage += "#tasks";
		break;
		
	case PREFPAGE_TASKDEF:
		sHelpPage += "#taskdefs";
		break;
		
	case PREFPAGE_TOOL:
		sHelpPage += "#tools";
		break;
		
	case PREFPAGE_SHORTCUT:
		sHelpPage += "#shortcut";
		break;
	}

	AfxGetApp()->WinHelp((DWORD)(LPCTSTR)sHelpPage);
}

void CPreferencesDlg::AddPage(CPropertyPage* pPage, UINT nIDPath)
{
	CString sPath;
	sPath.LoadString(nIDPath);

	if (m_pphost.AddPage(pPage))
	{
		HTREEITEM htiParent = TVI_ROOT; // default
		int nFind = sPath.Find(PATHDELIM);

		while (nFind != -1)
		{
			CString sParent = sPath.Left(nFind);
			sPath = sPath.Mid(nFind + 1);

			// see if parent already exists
			HTREEITEM htiParentParent = htiParent;
			htiParent = m_tcPages.GetChildItem(htiParentParent);

			while (htiParent)
			{
				if (sParent.CompareNoCase(m_tcPages.GetItemText(htiParent)) == 0)
					break;

				htiParent = m_tcPages.GetNextItem(htiParent, TVGN_NEXT);
			}

			if (!htiParent)
			{
				htiParent = m_tcPages.InsertItem(sParent, htiParentParent);

				// embolden root items
				if (htiParentParent == TVI_ROOT)
					m_tcPages.SetItemState(htiParent, TVIS_BOLD, TVIS_BOLD);
			}

			nFind = sPath.Find(PATHDELIM);
		}

		HTREEITEM hti = m_tcPages.InsertItem(sPath, htiParent); // whatever's left
		m_tcPages.EnsureVisible(hti);

		// embolden root items
		if (htiParent == TVI_ROOT)
			m_tcPages.SetItemState(hti, TVIS_BOLD, TVIS_BOLD);

		// map both ways
		m_tcPages.SetItemData(hti, (DWORD)pPage);
		m_mapPP2HTI[(void*)pPage] = (void*)hti;
	}
}

void CPreferencesDlg::OnSelchangedPages(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	HTREEITEM htiSel = m_tcPages.GetSelectedItem();

	while (m_tcPages.ItemHasChildren(htiSel))
		htiSel = m_tcPages.GetChildItem(htiSel);

	CPropertyPage* pPage = (CPropertyPage*)m_tcPages.GetItemData(htiSel);
	ASSERT (pPage);

	if (pPage)
	{
		m_pphost.SetActivePage(pPage, FALSE);

		// special handling:
		// if we're activating the defaults page then update the
		// priority colors
		if (pPage == &m_pageTaskDef)
		{
			CDWordArray aColors;
			m_pageUITasklistColors.GetPriorityColors(aColors);
			m_pageTaskDef.SetPriorityColors(aColors);
		}

		// update caption
		CString sCaption;
		sCaption.Format(IDS_PREF_TITLE, GetItemPath(htiSel));
		SetWindowText(sCaption);
	}

	m_tcPages.SetFocus();
	
	*pResult = 0;
}

void CPreferencesDlg::SetActivePage(int nPage)
{
	CPropertyPage* pPage = m_pphost.GetPage(nPage);
	ASSERT (pPage);

	VERIFY (m_pphost.SetActivePage(pPage, FALSE));

	// synchronize tree
	HTREEITEM hti = NULL;

	if (m_mapPP2HTI.Lookup(pPage, (void*&)hti) && hti)
		m_tcPages.SelectItem(hti);
}

CString CPreferencesDlg::GetItemPath(HTREEITEM hti)
{
	CString sPath = m_tcPages.GetItemText(hti);
	hti = m_tcPages.GetParentItem(hti);

	while (hti)
	{
		sPath = m_tcPages.GetItemText(hti) + " > " + sPath;
		hti = m_tcPages.GetParentItem(hti);
	}

	return sPath;
}
