// FindPriorityPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindPriorityPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindPriorityPage property page

enum { THISPRIORITY, MORETHAN, LESSTHAN, BETWEEN };

IMPLEMENT_DYNCREATE(CFindPriorityPage, CPropertyPage)

CFindPriorityPage::CFindPriorityPage() : CPropertyPage(CFindPriorityPage::IDD)
{
	//{{AFX_DATA_INIT(CFindPriorityPage)
	m_nPriorityOption = MORETHAN;
	m_nMoreThanPriority = 0;
	m_nLessThanPriority = 0;
	m_nStartPriority = 0;
	m_nEndPriority = 0;
	m_nThisPriority = 0;
	//}}AFX_DATA_INIT

}

CFindPriorityPage::~CFindPriorityPage()
{
}

void CFindPriorityPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindPriorityPage)
	DDX_Radio(pDX, IDC_THIS, m_nPriorityOption);
	//}}AFX_DATA_MAP
	DDX_CBPriority(pDX, IDC_MORETHANPRIORITY, m_nMoreThanPriority);
	DDX_CBPriority(pDX, IDC_LESSTHANPRIORITY, m_nLessThanPriority);
	DDX_CBPriority(pDX, IDC_PRIORITYSTART, m_nStartPriority);
	DDX_CBPriority(pDX, IDC_PRIORITYEND, m_nEndPriority);
	DDX_CBPriority(pDX, IDC_THISPRIORITY, m_nThisPriority);
	DDX_Control(pDX, IDC_MORETHANPRIORITY, m_cbMoreThanPriority);
	DDX_Control(pDX, IDC_LESSTHANPRIORITY, m_cbLessThanPriority);
	DDX_Control(pDX, IDC_PRIORITYSTART, m_cbStartPriority);
	DDX_Control(pDX, IDC_PRIORITYEND, m_cbEndPriority);
	DDX_Control(pDX, IDC_THISPRIORITY, m_cbThisPriority);
}


BEGIN_MESSAGE_MAP(CFindPriorityPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindPriorityPage)
	ON_BN_CLICKED(IDC_MORETHAN, OnChangeOption)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LESSTHAN, OnChangeOption)
	ON_BN_CLICKED(IDC_BETWEEN, OnChangeOption)
	ON_BN_CLICKED(IDC_THIS, OnChangeOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindPriorityPage message handlers

BOOL CFindPriorityPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString sRegKey = GetRegKey();
	
	m_nPriorityOption = AfxGetApp()->GetProfileInt(sRegKey, "LastNumOption", m_nPriorityOption);
	m_nThisPriority = AfxGetApp()->GetProfileInt(sRegKey, "LastThisPriority", m_nThisPriority);
	m_nMoreThanPriority = AfxGetApp()->GetProfileInt(sRegKey, "LastMoreThanPriority", m_nMoreThanPriority);
	m_nLessThanPriority = AfxGetApp()->GetProfileInt(sRegKey, "LastLessThanPriority", m_nLessThanPriority);
	m_nStartPriority = AfxGetApp()->GetProfileInt(sRegKey, "LastStartPriority", m_nStartPriority);
	m_nEndPriority = AfxGetApp()->GetProfileInt(sRegKey, "LastEndPriority", m_nEndPriority);

	UpdateData(FALSE);
	
	EnableControls(m_nPriorityOption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindPriorityPage::SetPriorityColors(const CDWordArray& aColors)
{
	m_cbThisPriority.SetColors(aColors);
	m_cbLessThanPriority.SetColors(aColors);
	m_cbMoreThanPriority.SetColors(aColors);
	m_cbEndPriority.SetColors(aColors);
	m_cbStartPriority.SetColors(aColors);
}

void CFindPriorityPage::GetRange(int& nFrom, int& nTo)
{
	if (GetSafeHwnd())
		UpdateData();

	switch (m_nPriorityOption)
	{
	case THISPRIORITY:
		nFrom = m_nThisPriority;
		nTo = nFrom;
		break;
		
	case MORETHAN:
		nFrom = m_nMoreThanPriority + 1;
		nTo = INT_MAX;
		break;
		
	case LESSTHAN:
		nFrom = INT_MIN;
		nTo = m_nLessThanPriority - 1;
		break;
		
	case BETWEEN:
		nFrom = m_nStartPriority;
		nTo = m_nEndPriority;
		break;
	}
}

void CFindPriorityPage::OnChangeOption() 
{
	UpdateData();
	EnableControls(m_nPriorityOption);
}

void CFindPriorityPage::EnableControls(int nOption)
{
	GetDlgItem(IDC_THISPRIORITY)->EnableWindow(nOption == THISPRIORITY);
	GetDlgItem(IDC_MORETHANPRIORITY)->EnableWindow(nOption == MORETHAN);
	GetDlgItem(IDC_LESSTHANPRIORITY)->EnableWindow(nOption == LESSTHAN);
	GetDlgItem(IDC_PRIORITYSTART)->EnableWindow(nOption == BETWEEN);
	GetDlgItem(IDC_PRIORITYEND)->EnableWindow(nOption == BETWEEN);
}

CString CFindPriorityPage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

void CFindPriorityPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();

	CString sRegKey = GetRegKey();
	
	AfxGetApp()->WriteProfileInt(sRegKey, "LastNumOption", m_nPriorityOption);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastThisPriority", m_nThisPriority);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastMoreThanPriority", m_nMoreThanPriority);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastLessThanPriority", m_nLessThanPriority);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastStartPriority", m_nStartPriority);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastEndPriority", m_nEndPriority);
}
