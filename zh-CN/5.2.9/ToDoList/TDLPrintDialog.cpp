// TDLPrintDialog.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "TDLPrintDialog.h"

#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLPrintDialog dialog


CTDLPrintDialog::CTDLPrintDialog(LPCTSTR szTitle, BOOL bPreview, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PRINT_DIALOG, pParent), m_bPreview(bPreview), m_taskSel("Print"),
		m_sTitle(szTitle), m_eStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER))
{
	//{{AFX_DATA_INIT(CTDLPrintDialog)
	//}}AFX_DATA_INIT
	m_bDate = AfxGetApp()->GetProfileInt("Print", "WantDate", TRUE);
	m_sStylesheet = AfxGetApp()->GetProfileString("Print", "Stylesheet");

	// check whether user has set stylesheet in prefs
	if (m_sStylesheet.IsEmpty() || AfxGetApp()->GetProfileInt("Print", "DefaultStylesheet", FALSE))
	{
		CString sDefStylesheet = AfxGetApp()->GetProfileString("Preferences", "PrintStylesheet");

		if (!sDefStylesheet.IsEmpty())
			m_sStylesheet = sDefStylesheet;
	}

	m_bUseStylesheet = (!m_sStylesheet.IsEmpty());
}


void CTDLPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLPrintDialog)
	DDX_Control(pDX, IDC_STYLESHEET, m_eStylesheet);
	DDX_Text(pDX, IDC_STYLESHEET, m_sStylesheet);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PRINTTITLE, m_sTitle);
	DDX_Check(pDX, IDC_PRINTDATE, m_bDate);
	DDX_Check(pDX, IDC_USESTYLESHEET, m_bUseStylesheet);
}


BEGIN_MESSAGE_MAP(CTDLPrintDialog, CDialog)
	//{{AFX_MSG_MAP(CTDLPrintDialog)
	ON_EN_CHANGE(IDC_STYLESHEET, OnChangeStylesheet)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USESTYLESHEET, OnUsestylesheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLPrintDialog message handlers

void CTDLPrintDialog::OnOK() 
{
	CDialog::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileString("Print", "Stylesheet", m_bUseStylesheet ? m_sStylesheet : "");

	// we store whether this is the same as the default print stylesheet
	// so we can update as it does
	CString sDefStylesheet = AfxGetApp()->GetProfileString("Preferences", "PrintStylesheet");
	AfxGetApp()->WriteProfileInt("Print", "DefaultStylesheet", (m_sStylesheet.CompareNoCase(sDefStylesheet) == 0));
}


BOOL CTDLPrintDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    VERIFY(m_taskSel.Create(IDC_FRAME, this));

	GetDlgItem(IDOK)->EnableWindow(!m_bUseStylesheet ||
									GetFileAttributes(m_sStylesheet) != 0xffffffff);

	if (m_bPreview)
		SetWindowText(CEnString(IDS_PRINTDLG_PREVIEW_TITLE));
	else
		SetWindowText(CEnString(IDS_PRINTDLG_PRINT_TITLE));

	GetDlgItem(IDC_STYLESHEET)->EnableWindow(m_bUseStylesheet);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLPrintDialog::OnUsestylesheet() 
{
	UpdateData();

	GetDlgItem(IDC_STYLESHEET)->EnableWindow(m_bUseStylesheet);
	GetDlgItem(IDOK)->EnableWindow(!m_bUseStylesheet ||
									GetFileAttributes(m_sStylesheet) != 0xffffffff);
}

void CTDLPrintDialog::OnChangeStylesheet() 
{
	UpdateData();
	GetDlgItem(IDOK)->EnableWindow(!m_bUseStylesheet ||
									GetFileAttributes(m_sStylesheet) != 0xffffffff);
}
