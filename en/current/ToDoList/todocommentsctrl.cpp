// tdlcommentsctrl.cpp : implementation file
//

#include "stdafx.h"
#include "todocommentsctrl.h"
#include "tdcmsg.h"
#include "tdlschemadef.h"
#include "resource.h"

#include "..\shared\toolbarhelper.h"
#include "..\shared\filedialogex.h"
#include "..\shared\spellcheckdlg.h"
#include "..\shared\ITaskList.h"
#include "..\shared\wclassdefines.h"
#include "..\shared\autoflag.h"
#include "..\shared\richedithelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDoCommentsCtrl

CToDoCommentsCtrl::CToDoCommentsCtrl() : m_bAllowNotify(TRUE), m_bWordWrap(FALSE)
{
	// add custom protocol to comments field for linking to task IDs
	AddProtocol(TDL_PROTOCOL, TRUE);
}

CToDoCommentsCtrl::~CToDoCommentsCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// IContentCtrl

int CToDoCommentsCtrl::GetTextContent(char* szContent, int nLength) const
{
	if (!szContent)
		return GetWindowTextLength();

	// else
	if (nLength == -1)
		nLength = lstrlen(szContent) + 1; // inluding null

	GetWindowText(szContent, nLength);
	
	return nLength;
}

bool CToDoCommentsCtrl::SetTextContent(const char* szContent) 
{ 
	SendMessage(WM_SETTEXT, 0, (LPARAM)szContent);
	return true; 
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CToDoCommentsCtrl, CUrlRichEditCtrl)
	//{{AFX_MSG_MAP(CToDoCommentsCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_COMMENTS_CUT, ID_COMMENTS_REDO, OnCommentsMenuCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COMMENTS_CUT, ID_COMMENTS_REDO, OnUpdateCommentsMenuCmd)
	ON_NOTIFY_RANGE(TTN_NEEDTEXT, 0, 0xffff, OnNeedTooltip)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChangeText)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoCommentsCtrl message handlers

BOOL CToDoCommentsCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!AfxInitRichEdit())
		return FALSE;

	dwStyle |= ES_AUTOHSCROLL | WS_HSCROLL;

	CWnd* pWnd = this;
	return pWnd->Create(WC_RICHEDIT20, NULL, dwStyle, rect, pParentWnd, nID);
}

LRESULT CToDoCommentsCtrl::OnSetFont(WPARAM wp, LPARAM lp)
{
	// richedit2.0 sends a EN_CHANGE notification if it contains
	// text when it receives a font change.
	// to us though this is a bogus change so we prevent a notification
	// being sent
	CAutoFlag af(m_bAllowNotify, FALSE);

	return CUrlRichEditCtrl::OnSetFont(wp, lp);
}

BOOL CToDoCommentsCtrl::OnChangeText() 
{
	CUrlRichEditCtrl::OnChangeText();

	if (m_bAllowNotify)
		GetParent()->SendMessage(WM_TDCN_COMMENTSCHANGE);
	
	return FALSE;
}

void CToDoCommentsCtrl::SetReadOnly(bool bReadOnly)
{
	CUrlRichEditCtrl::SetReadOnly(bReadOnly);
}

void CToDoCommentsCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd == this)
	{
		// build the context menu ourselves
		// note: we could use the edit menu from user32.dll but because
		// we're adding items too, the languages would appear odd
		CMenu menu;
		
		if (menu.LoadMenu(IDR_MISC))
		{
			CMenu* pPopup = menu.GetSubMenu(4);

			if (pPopup)
			{
				CToolbarHelper::PrepareMenuItems(pPopup, this);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
			}
		}
	}
}

void CToDoCommentsCtrl::OnCommentsMenuCmd(UINT nCmdID)
{
	switch (nCmdID)
	{
	case ID_COMMENTS_UNDO:
		CTextDocument(GetSafeHwnd()).Undo();
		break;

	case ID_COMMENTS_REDO:
		SendMessage(EM_REDO);
//		CTextDocument(GetSafeHwnd()).Redo();
		break;

	case ID_COMMENTS_CUT:
		Cut();
		break;
		
	case ID_COMMENTS_COPY:
		Copy();
		break;
		
	case ID_COMMENTS_PASTE:
		Paste();
		break;
		
	case ID_COMMENTS_PASTEASREF:
		{
			// try to get the clipboard for any tasklist
			ITaskList* pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, FALSE);

			// verify that we can get the corresponding filename
			CString sFileName;
			ITaskList4* pClip4 = NULL;

			if (S_OK == pClipboard->QueryInterface(IID_TASKLIST4, (void**)&pClip4))
			{
				sFileName = pClip4->GetAttribute(TDL_FILENAME);
				sFileName.Replace(" ", "%20");
			}
			else // get the clipboard for just this tasklist
				pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, TRUE);

			if (pClipboard && pClipboard->GetFirstTask())
			{
				// build single string containing each top level item as a different ref
				CString sRefs, sRef;
				HTASKITEM hClip = pClipboard->GetFirstTask();
				
				while (hClip)
				{
					if (sFileName.IsEmpty())
						sRef.Format(" %s%d", TDL_PROTOCOL, pClipboard->GetTaskID(hClip));
					else
						sRef.Format(" %s%s?%d", TDL_PROTOCOL, sFileName, pClipboard->GetTaskID(hClip));

					sRefs += sRef;
					
					hClip = pClipboard->GetNextTask(hClip);
				}

				sRefs += ' ';
				ReplaceSel(sRefs, TRUE);
			}
		}
		break;
		
	case ID_COMMENTS_SELECTALL:
		SetSel(0, -1);
		break;
		
	case ID_COMMENTS_OPENURL:
		if (m_nContextUrl != -1)
			GoToUrl(m_nContextUrl);
		
		// reset
		m_nContextUrl = -1;
		break;
		
	case ID_COMMENTS_FILEBROWSE:
		{
			CString sFile;
			
			if (m_nContextUrl != -1)
			{
				sFile = GetUrl(m_nContextUrl, TRUE);

				if (GetFileAttributes(sFile) == 0xffffffff)
					sFile.Empty();
			}
						
			CFileDialogEx dialog(TRUE, NULL, sFile);
			dialog.m_ofn.lpstrTitle = "Select File";
			
			if (dialog.DoModal() == IDOK)
				PathReplaceSel(dialog.GetPathName(), TRUE);
			
			// reset
			m_nContextUrl = -1;
		}
		break;
		
//	case ID_COMMENTS_DATESTAMP_UTC:
//		break;
		
	case ID_COMMENTS_DATESTAMP_LOCAL:
		{
			COleDateTime date = COleDateTime::GetCurrentTime();
			ReplaceSel(date.Format(), TRUE);
		}
		break;
		
	case ID_COMMENTS_SPELLCHECK:
		if (GetTextLength())
		{
			CString sComments;
			GetWindowText(sComments);
			CSpellCheckDlg dialog(NULL, sComments);
			
			if (dialog.DoModal() == IDOK)
				SetWindowText(dialog.GetCorrectedText());
		}
		break;

	case ID_COMMENTS_WORDWRAP:
		SetWordWrap(!m_bWordWrap);
		break;
	}
}

void CToDoCommentsCtrl::SetWordWrap(BOOL bWrap)
{
	SetTargetDevice(NULL, bWrap ? 0 : 1);
	m_bWordWrap = bWrap;
}

void CToDoCommentsCtrl::OnUpdateCommentsMenuCmd(CCmdUI* pCmdUI)
{
	BOOL bReadOnly = (GetStyle() & ES_READONLY);
	
	switch (pCmdUI->m_nID)
	{
	case ID_COMMENTS_UNDO:
		pCmdUI->Enable(CanUndo());
		break;
					
	case ID_COMMENTS_REDO:
		pCmdUI->Enable(SendMessage(EM_CANREDO));
		break;
					
	case ID_COMMENTS_CUT:
		if (!bReadOnly)
		{
			CHARRANGE crSel;
			GetSel(crSel);
			pCmdUI->Enable(crSel.cpMin != crSel.cpMax);
		}
		else
			pCmdUI->Enable(FALSE);
		break;
		
	case ID_COMMENTS_COPY:
		{
			CHARRANGE crSel;
			GetSel(crSel);
			pCmdUI->Enable(crSel.cpMin != crSel.cpMax);
		}
		break;
		
	case ID_COMMENTS_PASTE:
		pCmdUI->Enable(!bReadOnly && CanPaste());
		break;
		
	case ID_COMMENTS_PASTEASREF:
		pCmdUI->Enable(!bReadOnly && !IsClipboardEmpty());
		break;
		
	case ID_COMMENTS_SELECTALL:
	case ID_COMMENTS_SPELLCHECK:
		pCmdUI->Enable(GetTextLength());
		break;
		
	case ID_COMMENTS_OPENURL:
		// if pCmdUI->m_pMenu is NOT null then we need to set the menu
		// text to the url, else it's just MFC checking that the option
		// is stil enabled
		pCmdUI->Enable(m_nContextUrl != -1);

		if (m_nContextUrl != -1 && pCmdUI->m_pMenu)
		{
			CString sText, sMenu;
			pCmdUI->m_pMenu->GetMenuString(ID_COMMENTS_OPENURL, sMenu, 
				MF_BYCOMMAND);
			
			sText.Format("%s: %s", sMenu, GetUrl(m_nContextUrl, TRUE));
			pCmdUI->SetText(sText);
		}
		break;
		
	case ID_COMMENTS_FILEBROWSE:
		pCmdUI->Enable(!bReadOnly);
		break;
		
//	case ID_COMMENTS_DATESTAMP_UTC:
	case ID_COMMENTS_DATESTAMP_LOCAL:
		pCmdUI->Enable(!bReadOnly);
		break;

	case ID_COMMENTS_WORDWRAP:
		pCmdUI->SetCheck(m_bWordWrap ? 1 : 0);
		break;
	}
}


BOOL CToDoCommentsCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// bit of a hack but this is what we get just before the context
	// menu appears so we set the cursor back to the arrow
	if (nHitTest == HTCAPTION)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}
	
	return CUrlRichEditCtrl::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CToDoCommentsCtrl::IsClipboardEmpty() const 
{ 
	// try for any clipboard first
	ITaskList* pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, FALSE);
	ITaskList4* pClip4 = NULL;

	if (S_OK == pClipboard->QueryInterface(IID_TASKLIST4, (void**)&pClip4))
		return (pClipboard->GetFirstTask() == NULL);

	// else try for 'our' clipboard only
	return (!GetParent()->SendMessage(WM_TDCM_HASCLIPBOARD, 0, TRUE)); 
}

LRESULT CToDoCommentsCtrl::SendNotifyCustomUrl(LPCTSTR szUrl) const
{
	CString sUrl(szUrl);

	int nFind = sUrl.Find(TDL_PROTOCOL);

	if (nFind != -1)
		return GetParent()->SendMessage(WM_TDCM_TASKLINK, 0, (LPARAM)(LPCTSTR)sUrl);

	return 0;
}

void CToDoCommentsCtrl::PreSubclassWindow() 
{
	EnableToolTips();
	
	CUrlRichEditCtrl::PreSubclassWindow();
}

int CToDoCommentsCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	int nHit = MAKELONG(point.x, point.y);
	pTI->hwnd = m_hWnd;
	pTI->uId  = nHit;
	pTI->rect = CRect(CPoint(point.x-1,point.y-1),CSize(2,2));
	pTI->uFlags |= TTF_NOTBUTTON | TTF_ALWAYSTIP;
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	
	return nHit;
}

void CToDoCommentsCtrl::OnNeedTooltip(UINT /*id*/, NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;
/*	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	
	CPoint point(pNMHDR->idFrom);
	
	if (UrlHitTest(point) != -1)
		strcpy(pTTT->szText, "Ctrl+Click to open url");
*/
}

void CToDoCommentsCtrl::OnDestroy() 
{
	CUrlRichEditCtrl::OnDestroy();
	
	AfxGetApp()->WriteProfileInt("Settings", "WordWrap", m_bWordWrap);
}

int CToDoCommentsCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CUrlRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set max edit length
	LimitText(1024 * 1024 * 1024); // one gigabyte
	
	// we need to post the wordwrap initialization else the richedit
	// get very confused about something and doesn't repaint properly
	// when resizing
	m_bWordWrap = !AfxGetApp()->GetProfileInt("Settings", "WordWrap", TRUE);
	PostMessage(WM_COMMAND, ID_COMMENTS_WORDWRAP, (LPARAM)GetSafeHwnd());
	
	return 0;
}
