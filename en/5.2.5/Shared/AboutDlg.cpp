// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AboutDlg.h"
#include "stringres.h"
#include "enstring.h"
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

enum // ctrl IDs
{
	IDC_APPICON = 100,
	IDC_APPNAME, // 101
	IDC_DIVIDER, // 102
	IDC_APPDESCRIPTION, // 103
	IDC_COPYRIGHT // 104
};

CAboutDlg::CAboutDlg(UINT nAppIconID, AB_STYLE nStyle, 
					 LPCTSTR szAppName, LPCTSTR szAppDescription, LPCTSTR szCopyright, 
                     int nAppLines, int nDescLines, int nCopyrightLines) 
					: CRuntimeDlg(), 
						m_sAppName(szAppName), 
						m_sAppDescription(szAppDescription), 
						m_sCopyright(szCopyright),
						m_nStyle(nStyle)
{
	SetBordersDLU(5);

    // icon
	AddRCControl("ICON", "", "", SS_ICON, 0, 5,5,20,20, IDC_APPICON);

    // variable height items
    const UINT ITEMIDS[] = { IDC_APPNAME, IDC_APPDESCRIPTION, IDC_COPYRIGHT };
    const UINT NUMLINES[] =  { max(nAppLines, 0), max(nDescLines, 0), max(nCopyrightLines, 0) };
    const UINT NUMITEMS = sizeof(ITEMIDS) / sizeof(UINT);

    int nTop = 7;

    for (int nItem = 0; nItem < 3; nItem++)
    {
        int nHeight = (NUMLINES[nItem] * 8); // 8 dlu / line

		// special case: copyright
		if (nItem == 2 && m_nStyle == ABS_EDITCOPYRIGHT)
			AddRCControl("CONTROL", "RICHEDIT", "", ES_READONLY | ES_MULTILINE | WS_VSCROLL, 0, 36, nTop, 167, nHeight, ITEMIDS[nItem]);
		else
			AddRCControl("LTEXT", "", "", SS_NOPREFIX, 0, 36, nTop, 167, nHeight, ITEMIDS[nItem]);
 
        nTop += nHeight;

        if (nHeight && nItem < (NUMITEMS - 1))
            nTop += 6;
    }

    // divider and ok button
    AddRCControl("CONTROL", "static", "", SS_ETCHEDHORZ, 0, 7, nTop + 4, 196, 1, IDC_DIVIDER);
	AddRCControl("DEFPUSHBUTTON", "", BTN_OK, WS_GROUP | BS_DEFPUSHBUTTON, 0, 79, nTop + 10, 50, 14, IDOK);

	m_hIcon = AfxGetApp()->LoadIcon(nAppIconID);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_APPICON, m_stIcon);
	DDX_Text(pDX, IDC_APPNAME, m_sAppName);
	DDX_Text(pDX, IDC_APPDESCRIPTION, m_sAppDescription);
	DDX_Text(pDX, IDC_COPYRIGHT, m_sCopyright);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_APPNAME, m_stAppName);
	DDX_Control(pDX, IDC_APPDESCRIPTION, m_stAppDescription);

	if (m_nStyle == ABS_HTMLCOPYRIGHT)
		DDX_Control(pDX, IDC_COPYRIGHT, m_stCopyright);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

int CAboutDlg::DoModal()
{
	return CRuntimeDlg::DoModal(ABOUT_TITLE, WS_VISIBLE | WS_POPUP | WS_BORDER | WS_CAPTION | DS_CENTER);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CRuntimeDlg::OnInitDialog();
	
	if (m_hIcon)
		((CStatic*)GetDlgItem(IDC_APPICON))->SetIcon(m_hIcon);

	CWnd* pCopyright = GetDlgItem(IDC_COPYRIGHT);

	if (pCopyright)
	{
		pCopyright->ModifyStyle(WS_BORDER, 0);
		pCopyright->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		pCopyright->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	m_stAppName.SetBkColor(GetSysColor(COLOR_3DFACE));
	m_stAppDescription.SetBkColor(GetSysColor(COLOR_3DFACE));
	m_stCopyright.SetBkColor(GetSysColor(COLOR_3DFACE));

	GetDlgItem(IDOK)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
