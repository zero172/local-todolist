// WelcomeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "WelcomeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWelcomeDialog dialog

CWelcomeDialog::CWelcomeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWelcomeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWelcomeDialog)
	m_bUseIniFile = 0;
	//}}AFX_DATA_INIT
	m_stText.SetBkColor(GetSysColor(COLOR_3DFACE));
	m_sText.LoadString(IDS_WELCOME);
}


void CWelcomeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomeDialog)
	DDX_Control(pDX, IDC_TEXT, m_stText);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_CBIndex(pDX, IDC_PREFCHOICE, m_bUseIniFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWelcomeDialog, CDialog)
	//{{AFX_MSG_MAP(CWelcomeDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcomeDialog message handlers
