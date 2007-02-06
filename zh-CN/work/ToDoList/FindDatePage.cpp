// FindDatePage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindDatePage.h"

#include <float.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindDatePage property page

enum { THISDATE, AFTER, BEFORE, BETWEEN };

IMPLEMENT_DYNCREATE(CFindDatePage, CPropertyPage)

CFindDatePage::CFindDatePage() : CPropertyPage(CFindDatePage::IDD)
{
	//{{AFX_DATA_INIT(CFindDatePage)
	m_nDateOption = 0;
	//}}AFX_DATA_INIT
	m_dateAfter = floor(COleDateTime::GetCurrentTime());
	m_dateBefore = floor(COleDateTime::GetCurrentTime());
	m_dateStart = floor(COleDateTime::GetCurrentTime());
	m_dateEnd = floor(COleDateTime::GetCurrentTime());
	m_dateThis = floor(COleDateTime::GetCurrentTime());
}

CFindDatePage::~CFindDatePage()
{
}

void CFindDatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDatePage)
	DDX_DateTimeCtrl(pDX, IDC_AFTERDATE, m_dateAfter);
	DDX_DateTimeCtrl(pDX, IDC_BEFOREDATE, m_dateBefore);
	DDX_DateTimeCtrl(pDX, IDC_BEGINDATE, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_ENDDATE, m_dateEnd);
	DDX_Radio(pDX, IDC_THIS, m_nDateOption);
	DDX_DateTimeCtrl(pDX, IDC_THISDATE, m_dateThis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDatePage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindDatePage)
	ON_BN_CLICKED(IDC_AFTER, OnChangeOption)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BEFORE, OnChangeOption)
	ON_BN_CLICKED(IDC_BETWEEN, OnChangeOption)
	ON_BN_CLICKED(IDC_THIS, OnChangeOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDatePage message handlers

BOOL CFindDatePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CString sRegKey = GetRegKey();
	
	m_nDateOption = AfxGetApp()->GetProfileInt(sRegKey, "LastDateOption", m_nDateOption);
	m_dateThis.m_dt = (double)AfxGetApp()->GetProfileInt(sRegKey, "LastThisDate", (int)m_dateThis.m_dt);
	m_dateAfter.m_dt = (double)AfxGetApp()->GetProfileInt(sRegKey, "LastAfterDate", (int)m_dateAfter.m_dt);
	m_dateBefore.m_dt = (double)AfxGetApp()->GetProfileInt(sRegKey, "LastBeforeDate", (int)m_dateBefore.m_dt);
	m_dateStart.m_dt = (double)AfxGetApp()->GetProfileInt(sRegKey, "LastStartDate", (int)m_dateStart.m_dt);
	m_dateEnd.m_dt = (double)AfxGetApp()->GetProfileInt(sRegKey, "LastEndDate", (int)m_dateEnd.m_dt);

	UpdateData(FALSE);

	EnableControls(m_nDateOption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindDatePage::GetDateRange(COleDateTime& dateFrom, COleDateTime& dateTo)
{
	if (GetSafeHwnd())
		UpdateData();

	switch (m_nDateOption)
	{
	case THISDATE:
		dateFrom = floor(m_dateThis.m_dt);
		dateTo.m_dt = dateFrom.m_dt + 0.999999;
		break;

	case AFTER:
		dateFrom.m_dt = floor(m_dateAfter.m_dt) + 1;
		dateTo.m_dt = FLT_MAX;
		break;
		
	case BEFORE:
		dateFrom.m_dt = 0;
		dateTo = floor(m_dateBefore.m_dt) - 0.000001; // ie just before midnight
		break;
		
	case BETWEEN:
		dateFrom = floor(m_dateStart.m_dt);
		dateTo = floor(m_dateEnd.m_dt);
		break;
	}
}

void CFindDatePage::OnChangeOption() 
{
	UpdateData();
	EnableControls(m_nDateOption);
}

void CFindDatePage::EnableControls(int nOption)
{
	GetDlgItem(IDC_THISDATE)->EnableWindow(nOption == THISDATE);
	GetDlgItem(IDC_AFTERDATE)->EnableWindow(nOption == AFTER);
	GetDlgItem(IDC_BEFOREDATE)->EnableWindow(nOption == BEFORE);
	GetDlgItem(IDC_BEGINDATE)->EnableWindow(nOption == BETWEEN);
	GetDlgItem(IDC_ENDDATE)->EnableWindow(nOption == BETWEEN);
}

CString CFindDatePage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

void CFindDatePage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	CString sRegKey = GetRegKey();
	
	AfxGetApp()->WriteProfileInt(sRegKey, "LastDateOption", m_nDateOption);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastThisDate", (int)m_dateThis.m_dt);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastAfterDate", (int)m_dateAfter.m_dt);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastBeforeDate", (int)m_dateBefore.m_dt);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastStartDate", (int)m_dateStart.m_dt);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastEndDate", (int)m_dateEnd.m_dt);
}
