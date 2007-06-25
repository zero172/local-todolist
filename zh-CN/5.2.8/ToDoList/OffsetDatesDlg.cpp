// OffsetDatesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "OffsetDatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COffsetDatesDlg dialog


COffsetDatesDlg::COffsetDatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetDatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetDatesDlg)
	m_bOffsetSubtasks = FALSE;
	//}}AFX_DATA_INIT

	// restore state
	m_bOffsetStartDate = AfxGetApp()->GetProfileInt("OffsetDates", "StartDate", FALSE);
	m_bOffsetDueDate = AfxGetApp()->GetProfileInt("OffsetDates", "DueDate", FALSE);
	m_bOffsetDoneDate = AfxGetApp()->GetProfileInt("OffsetDates", "DoneDate", FALSE);
	m_bForward = AfxGetApp()->GetProfileInt("OffsetDates", "Forward", 1);
	m_nOffsetBy = AfxGetApp()->GetProfileInt("OffsetDates", "Amount", 1);
	m_nOffsetByUnits = AfxGetApp()->GetProfileInt("OffsetDates", "AmountUnits", 0);
	m_bOffsetSubtasks = AfxGetApp()->GetProfileInt("OffsetDates", "Subtasks", 1);
}


void COffsetDatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetDatesDlg)
	DDX_Check(pDX, IDC_OFFSETSTARTDATE, m_bOffsetStartDate);
	DDX_Check(pDX, IDC_OFFSETDUEDATE, m_bOffsetDueDate);
	DDX_Check(pDX, IDC_OFFSETDONEDATE, m_bOffsetDoneDate);
	DDX_CBIndex(pDX, IDC_DIRECTION, m_bForward);
	DDX_Text(pDX, IDC_BY, m_nOffsetBy);
	DDX_CBIndex(pDX, IDC_BYUNITS, m_nOffsetByUnits);
	DDX_Check(pDX, IDC_OFFSETSUBTASKS, m_bOffsetSubtasks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetDatesDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetDatesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetDatesDlg message handlers

DWORD COffsetDatesDlg::GetOffsetWhat() const
{
	DWORD dwWhat = 0;

	if (m_bOffsetStartDate)
		dwWhat |= ODD_STARTDATE;

	if (m_bOffsetDueDate)
		dwWhat |= ODD_DUEDATE;

	if (m_bOffsetDoneDate)
		dwWhat |= ODD_DONEDATE;

	return dwWhat;
}

int COffsetDatesDlg::GetOffsetAmount(ODD_UNITS& nUnits) const
{
	nUnits = (ODD_UNITS)m_nOffsetByUnits;

	return (m_bForward ? m_nOffsetBy : -m_nOffsetBy);
}

void COffsetDatesDlg::OnOK()
{
	CDialog::OnOK();

	// save state
	AfxGetApp()->WriteProfileInt("OffsetDates", "StartDate", m_bOffsetStartDate);
	AfxGetApp()->WriteProfileInt("OffsetDates", "DueDate", m_bOffsetDueDate);
	AfxGetApp()->WriteProfileInt("OffsetDates", "DoneDate", m_bOffsetDoneDate);
	AfxGetApp()->WriteProfileInt("OffsetDates", "Forward", m_bForward);
	AfxGetApp()->WriteProfileInt("OffsetDates", "Amount", m_nOffsetBy);
	AfxGetApp()->WriteProfileInt("OffsetDates", "AmountUnits", m_nOffsetByUnits);
	AfxGetApp()->WriteProfileInt("OffsetDates", "Subtasks", m_bOffsetSubtasks);
}

