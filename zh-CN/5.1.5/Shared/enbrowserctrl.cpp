// enbrowserctrl.cpp : implementation file
//

#include "stdafx.h"
#include "enbrowserctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { EBC_NONE, EBC_PRINT, EBC_PRINTPREVIEW };

/////////////////////////////////////////////////////////////////////////////
// CEnBrowserCtrl

CEnBrowserCtrl::CEnBrowserCtrl() : m_nAction(EBC_NONE)
{
}

CEnBrowserCtrl::~CEnBrowserCtrl()
{
}


BEGIN_MESSAGE_MAP(CEnBrowserCtrl, CWebBrowserCtrl)
	//{{AFX_MSG_MAP(CEnBrowserCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CEnBrowserCtrl, CWebBrowserCtrl)
    //{{AFX_EVENTSINK_MAP(CBrowserDlg)
	//}}AFX_EVENTSINK_MAP
//	ON_EVENT_REFLECT(CWebBrowser, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT_REFLECT(CEnBrowserCtrl, 252 /* NavigateComplete2 */, OnNavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnBrowserCtrl message handlers

void CEnBrowserCtrl::Print(LPCTSTR szFile)
{
	if (!szFile || !*szFile)
		ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
	else
	{
		COleVariant vFlags(0L), vFrame(""), vData(""), vHeaders("");
			
		// navigate to print file
		m_nAction = EBC_PRINT;
		Navigate(szFile, vFlags, vFrame, vData, vHeaders);
	}
}

void CEnBrowserCtrl::PrintPreview(LPCTSTR szFile)
{
	if (!szFile || !*szFile)
		ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
	else
	{
		COleVariant vFlags(0L), vFrame(""), vData(""), vHeaders("");
			
		// navigate to print file
		m_nAction = EBC_PRINTPREVIEW;
		Navigate(szFile, vFlags, vFrame, vData, vHeaders);
	}
}

void CEnBrowserCtrl::OnNavigateComplete2(LPDISPATCH /*pDisp*/, VARIANT FAR* URL) 
{
	CString sURL;
	_bstr_t sTemp;
	
	sTemp = *URL;
	sURL = CString(LPCTSTR(sTemp));
	
	switch (m_nAction)
	{
	case EBC_PRINT:
		SafeExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
		break;
		
	case EBC_PRINTPREVIEW:
		SafeExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
		break;
	}

	m_nAction = EBC_NONE;
}

BOOL CEnBrowserCtrl::SafeExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut) 
{
	try
	{
		ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
	}
	catch (CException* /*e*/)
	{
		return FALSE;
	}

	return TRUE;
}