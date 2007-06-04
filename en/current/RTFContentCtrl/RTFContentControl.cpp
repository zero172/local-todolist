// RTFContentControl.cpp : implementation file
//

#include "stdafx.h"
#include "RTFContentCtrl.h"
#include "RTFContentControl.h"

#include "..\shared\itasklist.h"
#include "..\shared\enfiledialog.h"
#include "..\shared\autoflag.h"
#include "..\todolist\tdcmsg.h"
#include "..\shared\richedithelper.h"
#include "..\todolist\tdlschemadef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTFContentControl

CRTFContentControl::CRTFContentControl() : m_bAllowNotify(TRUE)
{
	// add custom protocol to comments field for linking to task IDs
	GetRichEditCtrl().AddProtocol(TDL_PROTOCOL, TRUE);
}

CRTFContentControl::~CRTFContentControl()
{
}


BEGIN_MESSAGE_MAP(CRTFContentControl, CRulerRichEditCtrl)
	//{{AFX_MSG_MAP(CRTFContentControl)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(RTF_CONTROL, OnChangeText)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_STYLECHANGING()
	ON_REGISTERED_MESSAGE(WM_UREN_CUSTOMURL, OnCustomUrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTFContentControl message handlers

void CRTFContentControl::OnChangeText() 
{
	if (m_bAllowNotify && !GetRichEditCtrl().IsIMEComposing())
		GetParent()->SendMessage(WM_TDCN_COMMENTSCHANGE);
}

LRESULT CRTFContentControl::OnSetFont(WPARAM wp, LPARAM lp)
{
	// richedit2.0 sends a EN_CHANGE notification if it contains
	// text when it receives a font change.
	// to us though this is a bogus change so we prevent a notification
	// being sent
	CAutoFlag af(m_bAllowNotify, FALSE);

	return CRulerRichEditCtrl::OnSetFont(wp, lp);
}

// ICustomControl implementation
int CRTFContentControl::GetContent(unsigned char* pContent) const
{
	return GetContent(this, pContent);
}

// hack to get round GetRTF not being const
int CRTFContentControl::GetContent(const CRTFContentControl* pCtrl, unsigned char* pContent)
{
	int nLen = 0;
	
	if (pContent)
	{
		CString sContent;
		
		// cast away constness
		sContent = ((CRTFContentControl*)pCtrl)->GetRTF();
		nLen = sContent.GetLength();
		
		if (pContent)
			CopyMemory(pContent, (LPCTSTR)sContent, nLen);
	}
	else
		nLen = ((CRTFContentControl*)pCtrl)->GetRTFLength();
	
	return nLen;
}

bool CRTFContentControl::SetContent(unsigned char* pContent, int nLength)
{
	LPCTSTR RTFTAG = "{\\rtf";
	const int LENTAG = strlen(RTFTAG);

	// content must begin with rtf tag or be empty
	if (nLength && (nLength < LENTAG || strncmp((const char*)pContent, RTFTAG, LENTAG)))
		return false;

	CAutoFlag af(m_bAllowNotify, FALSE);
	CString sContent((LPCSTR)pContent, nLength);

	SetRTF(sContent);
	CRichEditHelper::ClearUndo(GetRichEditCtrl());

	GetRichEditCtrl().ParseAndFormatText(TRUE);
	
	return true; 
}

int CRTFContentControl::GetTextContent(char* szContent, int nLength) const
{
	if (!szContent)
		return GetWindowTextLength();

	// else
	if (nLength == -1)
		nLength = lstrlen(szContent);
	
	GetWindowText(szContent, nLength);
	return nLength;
}

bool CRTFContentControl::SetTextContent(const char* szContent)
{
	CAutoFlag af(m_bAllowNotify, TRUE);
	SendMessage(WM_SETTEXT, 0, (LPARAM)szContent);
	return true; 
}

HWND CRTFContentControl::GetHwnd() const
{
	return GetSafeHwnd();
}

bool CRTFContentControl::HasTypeID() const
{
	return true;
}

bool CRTFContentControl::GetTypeID(GUID& id) const
{
	id = RTF_TYPEID;
	return true;
}

void CRTFContentControl::SetReadOnly(bool bReadOnly)
{
	CRulerRichEditCtrl::SetReadOnly((BOOL)bReadOnly);
}

void CRTFContentControl::Release()
{
	DestroyWindow();
	delete this;
}

void CRTFContentControl::EnableMenuItem(CMenu* pMenu, UINT nCmdID, BOOL bEnable)
{
	pMenu->EnableMenuItem(nCmdID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
}

void CRTFContentControl::CheckMenuItem(CMenu* pMenu, UINT nCmdID, BOOL bCheck)
{
	pMenu->CheckMenuItem(nCmdID, MF_BYCOMMAND | (bCheck ? MF_CHECKED  : MF_UNCHECKED));
}

void CRTFContentControl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd == &GetRichEditCtrl())
	{
		// prepare a simple edit menu
		CMenu menu;

		if (menu.LoadMenu(IDR_POPUP))
		{
			CMenu* pPopup = menu.GetSubMenu(0);

			if (pPopup)
			{
				CRulerRichEdit& re = GetRichEditCtrl();

				BOOL bCanEdit = re.IsWindowEnabled() && !(re.GetStyle() & ES_READONLY);
				BOOL bHasText = re.GetTextLength();

				EnableMenuItem(pPopup, ID_EDIT_UNDO, bCanEdit && re.CanUndo());
				EnableMenuItem(pPopup, ID_EDIT_REDO, bCanEdit && re.SendMessage(EM_CANREDO));

				CHARRANGE cr;
				re.GetSel(cr);

				BOOL bHasSel = (cr.cpMax - cr.cpMin);

				CharFormat	cf;
				cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR;
				re.GetSelectionCharFormat(cf);
				
				CheckMenuItem(pPopup, BUTTON_BOLD, (cf.dwEffects & CFE_BOLD));
				CheckMenuItem(pPopup, BUTTON_ITALIC, (cf.dwEffects & CFE_ITALIC));
				CheckMenuItem(pPopup, BUTTON_UNDERLINE, (cf.dwEffects & CFE_UNDERLINE));
				CheckMenuItem(pPopup, BUTTON_STRIKETHRU, (cf.dwEffects & CFE_STRIKEOUT));

				CheckMenuItem(pPopup, ID_EDIT_SUPERSCRIPT, (cf.dwEffects & CFE_SUPERSCRIPT));
				CheckMenuItem(pPopup, ID_EDIT_SUBSCRIPT, (cf.dwEffects & CFE_SUBSCRIPT));

				EnableMenuItem(pPopup, BUTTON_BOLD, bCanEdit);
				EnableMenuItem(pPopup, BUTTON_ITALIC, bCanEdit);
				EnableMenuItem(pPopup, BUTTON_UNDERLINE, bCanEdit);
				EnableMenuItem(pPopup, BUTTON_STRIKETHRU, bCanEdit);

				EnableMenuItem(pPopup, ID_EDIT_SUPERSCRIPT, bCanEdit);
				EnableMenuItem(pPopup, ID_EDIT_SUBSCRIPT, bCanEdit);

				EnableMenuItem(pPopup, ID_EDIT_CUT, bCanEdit && bHasSel);
				EnableMenuItem(pPopup, ID_EDIT_COPY, bHasSel);
				EnableMenuItem(pPopup, ID_EDIT_PASTE, bCanEdit && CanPaste());
				EnableMenuItem(pPopup, ID_EDIT_PASTESIMPLE, bCanEdit && CanPaste());
				EnableMenuItem(pPopup, ID_EDIT_DELETE, bCanEdit && bHasSel);
				EnableMenuItem(pPopup, ID_EDIT_SELECT_ALL, bHasText);

				EnableMenuItem(pPopup, ID_EDIT_PASTEASREF, bCanEdit && !IsClipboardEmpty());
				
				int nUrl = re.GetContextUrl();
				EnableMenuItem(pPopup, ID_EDIT_OPENURL, nUrl != -1);

				if (nUrl != -1)
				{
					CString sText, sMenu;
					pPopup->GetMenuString(ID_EDIT_OPENURL, sMenu, MF_BYCOMMAND);
					
					sText.Format("%s: %s", sMenu, re.GetUrl(nUrl, TRUE));
					pPopup->ModifyMenu(ID_EDIT_OPENURL, MF_BYCOMMAND, ID_EDIT_OPENURL, sText);
				}

				EnableMenuItem(pPopup, ID_EDIT_FILEBROWSE, bCanEdit);
				EnableMenuItem(pPopup, ID_EDIT_INSERTDATESTAMP, bCanEdit);

				CheckMenuItem(pPopup, ID_EDIT_SHOWTOOLBAR, IsToolbarVisible());
				CheckMenuItem(pPopup, ID_EDIT_SHOWRULER, IsRulerVisible());
				CheckMenuItem(pPopup, ID_EDIT_WORDWRAP, HasWordWrap());

				// check pos
				if (point.x == -1 && point.y == -1)
				{
					point = GetCaretPos();
					::ClientToScreen(m_rtf, &point);
				}

				UINT nCmdID = ::TrackPopupMenu(*pPopup, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON, 
												point.x, point.y, 0, *this, NULL);

				switch (nCmdID)
				{
				case BUTTON_BOLD:
				case BUTTON_ITALIC:
				case BUTTON_UNDERLINE:
				case BUTTON_STRIKETHRU:
					SendMessage(WM_COMMAND, nCmdID);
					break;

				case ID_EDIT_SUPERSCRIPT:
					DoSuperscript();
					break;

				case ID_EDIT_SUBSCRIPT:
					DoSubscript();
					break;

				case ID_EDIT_UNDO:
					CTextDocument(re).Undo();
					break;

				case ID_EDIT_REDO:
					re.SendMessage(EM_REDO);
					break;

				case ID_EDIT_CUT:
					re.Cut();
					break;

				case ID_EDIT_COPY:
					re.Copy();
					break;

				case ID_EDIT_PASTE:
					re.Paste();
					break;

				case ID_EDIT_PASTESIMPLE:
					re.Paste(TRUE); // TRUE ==  simple
					break;

				case ID_EDIT_PASTEASREF:
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
							re.ReplaceSel(sRefs, TRUE);
						}
					}
					break;
		
				case ID_EDIT_DELETE:
					re.ReplaceSel("");
					break;

				case ID_EDIT_SELECT_ALL:
					re.SetSel(0, -1);
					break;
		
				case ID_EDIT_OPENURL:
					if (nUrl != -1)
						re.GoToUrl(nUrl);
					break;
					
				case ID_EDIT_FILEBROWSE:
					{
						CString sFile;
						
						if (nUrl != -1)
						{
							sFile = re.GetUrl(nUrl, TRUE);

							if (GetFileAttributes(sFile) == 0xffffffff)
								sFile.Empty();
						}
									
						CEnFileDialog dialog(TRUE, NULL, sFile);
						dialog.m_ofn.lpstrTitle = "Select File";
						
						if (dialog.DoModal() == IDOK)
							re.PathReplaceSel(dialog.GetPathName(), TRUE);
					}
					break;
		
				case ID_EDIT_INSERTDATESTAMP:
					{
						COleDateTime date = COleDateTime::GetCurrentTime();
						re.ReplaceSel(date.Format(), TRUE);
					}
					break;

				case ID_EDIT_SHOWTOOLBAR:
					ShowToolbar(!IsToolbarVisible());
					break;

				case ID_EDIT_SHOWRULER:
					ShowRuler(!IsRulerVisible());
					break;

				case ID_EDIT_WORDWRAP:
					SetWordWrap(!HasWordWrap());
					break;
				}
			}
		}
	}
}

BOOL CRTFContentControl::IsClipboardEmpty() const 
{ 
	// try for any clipboard first
	ITaskList* pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, FALSE);
	ITaskList4* pClip4 = NULL;

	if (S_OK == pClipboard->QueryInterface(IID_TASKLIST4, (void**)&pClip4))
		return (pClipboard->GetFirstTask() == NULL);

	// else try for 'our' clipboard only
	return (!GetParent()->SendMessage(WM_TDCM_HASCLIPBOARD, 0, TRUE)); 
}

BOOL CRTFContentControl::CanPaste()
{
	static CLIPFORMAT formats[] = 
	{ 
		(CLIPFORMAT)::RegisterClipboardFormat(CF_RTF),
		(CLIPFORMAT)::RegisterClipboardFormat(CF_RETEXTOBJ), 
		(CLIPFORMAT)::RegisterClipboardFormat(_T("Embedded Object")),
		(CLIPFORMAT)::RegisterClipboardFormat(_T("Rich Text Format")),
		CF_BITMAP,
		CF_TEXT,
		CF_DIB,
		CF_UNICODETEXT
	};
	const long formats_count = sizeof(formats) / sizeof(CLIPFORMAT);
	
	CUrlRichEditCtrl& re = GetRichEditCtrl();
	
	BOOL bCanPaste = FALSE;
	
	for( long i=0;  i<formats_count;  ++i )
		bCanPaste |= re.CanPaste( formats[i] );
	
	return bCanPaste;
}

int CRTFContentControl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CAutoFlag af(m_bAllowNotify, FALSE);
	
	if (CRulerRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// restore toolbar and ruler state
	BOOL bShowToolbar = AfxGetApp()->GetProfileInt("Settings", "ShowToolbar", TRUE);
	BOOL bShowRuler = AfxGetApp()->GetProfileInt("Settings", "ShowRuler", TRUE);
	BOOL bWordWrap = AfxGetApp()->GetProfileInt("Settings", "WordWrap", TRUE);
	
	ShowToolbar(bShowToolbar);
	ShowRuler(bShowRuler);
	SetWordWrap(bWordWrap);

	GetRichEditCtrl().SetEventMask(GetRichEditCtrl().GetEventMask() | ENM_CHANGE);
	
	// set max edit length
	GetRichEditCtrl().LimitText(1024 * 1024 * 1024); // one gigabyte
	
	m_tbHelper.Initialize(&m_toolbar, this);
	
	return 0;
}

bool CRTFContentControl::ProcessMessage(MSG* pMsg) 
{
	// process editing shortcuts
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
			BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);

			if (bCtrl && !bAlt)
			{
				switch (pMsg->wParam)
				{
				case 'c': 
				case 'C':
					GetRichEditCtrl().Copy();
					return TRUE;

				case 'v':
				case 'V':
					GetRichEditCtrl().Paste();
					return TRUE;

				case 'x':
				case 'X':
					GetRichEditCtrl().Cut();
					return TRUE;

				case 'a':
				case 'A':
					GetRichEditCtrl().SetSel(0, -1);
					return TRUE;

				case 'b':
				case 'B':
					DoBold();
					return TRUE;

				case 'i':
				case 'I':
					DoItalic();
					return TRUE;

				case 'u':
				case 'U':
					DoUnderline();
					return TRUE;

				case 'l':
				case 'L':
					DoLeftAlign();
					return TRUE;

				case 'e':
				case 'E':
					DoCenterAlign();
					return TRUE;

				case 'r':
				case 'R':
					DoRightAlign();
					return TRUE;
				}
			}
		}
		break;
	}

	return false;
}

void CRTFContentControl::OnDestroy() 
{
	CRulerRichEditCtrl::OnDestroy();
	
	// save toolbar and ruler state
	AfxGetApp()->WriteProfileInt("Settings", "ShowToolbar", IsToolbarVisible());
	AfxGetApp()->WriteProfileInt("Settings", "ShowRuler", IsRulerVisible());
	AfxGetApp()->WriteProfileInt("Settings", "WordWrap", HasWordWrap());
}

void CRTFContentControl::OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	if (nStyleType == GWL_EXSTYLE && (lpStyleStruct->styleNew & WS_EX_CLIENTEDGE))
		lpStyleStruct->styleNew &= ~WS_EX_CLIENTEDGE;

	CRulerRichEditCtrl::OnStyleChanging(nStyleType, lpStyleStruct);
}

LRESULT CRTFContentControl::OnCustomUrl(WPARAM wp, LPARAM lp)
{
	UNREFERENCED_PARAMETER(wp);
	ASSERT (wp == RTF_CONTROL);

	CString sUrl((LPCTSTR)lp);
	int nFind = sUrl.Find(TDL_PROTOCOL);

	if (nFind != -1)
		return GetParent()->SendMessage(WM_TDCM_TASKLINK, 0, lp);

	return 0;

}

BOOL CRTFContentControl::PreTranslateMessage(MSG* pMsg) 
{
	
	return CRulerRichEditCtrl::PreTranslateMessage(pMsg);
}
