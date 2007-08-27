// TDLSendTasksDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "TDLSendTasksDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLSendTasksDlg dialog


CTDLSendTasksDlg::CTDLSendTasksDlg(TD_SENDWHAT nWhat, TD_SENDAS nSendAs, CWnd* pParent /*=NULL*/)
	: CDialog(CTDLSendTasksDlg::IDD, pParent), m_nSendWhat(nWhat), m_nSendTasksAsOption(nSendAs)
{
	//{{AFX_DATA_INIT(CTDLSendTasksDlg)
	//}}AFX_DATA_INIT
}


void CTDLSendTasksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLSendTasksDlg)
	DDX_Radio(pDX, IDC_SENDACTIVETASKLIST, m_nSendWhat);
	DDX_CBIndex(pDX, IDC_SELTASKSSENDAS, m_nSendTasksAsOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTDLSendTasksDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLSendTasksDlg)
	ON_BN_CLICKED(IDC_SENDACTIVETASKLIST, OnChangeSendWhat)
	ON_BN_CLICKED(IDC_SENDSELECTEDTASKS, OnChangeSendWhat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLSendTasksDlg message handlers

BOOL CTDLSendTasksDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_SELTASKSSENDAS)->EnableWindow(m_nSendWhat == TDSW_SELECTEDTASKS);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLSendTasksDlg::OnChangeSendWhat() 
{
	UpdateData();

	GetDlgItem(IDC_SELTASKSSENDAS)->EnableWindow(m_nSendWhat == TDSW_SELECTEDTASKS);
}
