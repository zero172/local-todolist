// TDLTransformDialog.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "TDLTransformDialog.h"

#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLTransformDialog dialog


CTDLTransformDialog::CTDLTransformDialog(LPCTSTR szTitle, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TRANSFORM_DIALOG, pParent), m_taskSel("Transform"),
		m_sTitle(szTitle), m_eStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER))

{
	//{{AFX_DATA_INIT(CTDLTransformDialog)
	//}}AFX_DATA_INIT
	// see what we had last time
	m_sStylesheet = AfxGetApp()->GetProfileString("Transform", "Stylesheet");
	m_bDate = AfxGetApp()->GetProfileInt("Transform", "WantDate", TRUE);
}


void CTDLTransformDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLTransformDialog)
	DDX_Control(pDX, IDC_STYLESHEET, m_eStylesheet);
	DDX_Text(pDX, IDC_STYLESHEET, m_sStylesheet);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TRANSFORMTITLE, m_sTitle);
	DDX_Check(pDX, IDC_TRANSFORMDATE, m_bDate);
}


BEGIN_MESSAGE_MAP(CTDLTransformDialog, CDialog)
	//{{AFX_MSG_MAP(CTDLTransformDialog)
	ON_EN_CHANGE(IDC_STYLESHEET, OnChangeStylesheet)
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_USESTYLESHEET, OnUsestylesheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLTransformDialog message handlers

void CTDLTransformDialog::OnOK() 
{
	CDialog::OnOK();

	AfxGetApp()->WriteProfileString("Transform", "Stylesheet", m_sStylesheet);
}


BOOL CTDLTransformDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    VERIFY(m_taskSel.Create(IDC_FRAME, this));
	GetDlgItem(IDOK)->EnableWindow(GetFileAttributes(m_sStylesheet) != 0xffffffff);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLTransformDialog::OnChangeStylesheet() 
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(GetFileAttributes(m_sStylesheet) != 0xffffffff);
}
