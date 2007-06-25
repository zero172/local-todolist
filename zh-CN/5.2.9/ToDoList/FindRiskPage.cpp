// FindRiskPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindRiskPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindRiskPage property page

enum { THISRISK, MORETHAN, LESSTHAN, BETWEEN };

IMPLEMENT_DYNCREATE(CFindRiskPage, CPropertyPage)

CFindRiskPage::CFindRiskPage() : CPropertyPage(CFindRiskPage::IDD)
{
	//{{AFX_DATA_INIT(CFindRiskPage)
	m_nRiskOption = MORETHAN;
	m_nMoreThanRisk = 0;
	m_nLessThanRisk = 0;
	m_nStartRisk = 0;
	m_nEndRisk = 0;
	m_nThisRisk = 0;
	//}}AFX_DATA_INIT

}

CFindRiskPage::~CFindRiskPage()
{
}

void CFindRiskPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindRiskPage)
	DDX_Radio(pDX, IDC_THIS, m_nRiskOption);
	DDX_CBRisk(pDX, IDC_MORETHANRISK, m_nMoreThanRisk);
	DDX_CBRisk(pDX, IDC_LESSTHANRISK, m_nLessThanRisk);
	DDX_CBRisk(pDX, IDC_RISKSTART, m_nStartRisk);
	DDX_CBRisk(pDX, IDC_RISKEND, m_nEndRisk);
	DDX_CBRisk(pDX, IDC_THISRISK, m_nThisRisk);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MORETHANRISK, m_cbMoreThanRisk);
	DDX_Control(pDX, IDC_LESSTHANRISK, m_cbLessThanRisk);
	DDX_Control(pDX, IDC_RISKSTART, m_cbStartRisk);
	DDX_Control(pDX, IDC_RISKEND, m_cbEndRisk);
	DDX_Control(pDX, IDC_THISRISK, m_cbThisRisk);
}


BEGIN_MESSAGE_MAP(CFindRiskPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindRiskPage)
	ON_BN_CLICKED(IDC_MORETHAN, OnChangeOption)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LESSTHAN, OnChangeOption)
	ON_BN_CLICKED(IDC_BETWEEN, OnChangeOption)
	ON_BN_CLICKED(IDC_THIS, OnChangeOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindRiskPage message handlers

BOOL CFindRiskPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString sRegKey = GetRegKey();
	
	m_nRiskOption = AfxGetApp()->GetProfileInt(sRegKey, "LastNumOption", m_nRiskOption);
	m_nThisRisk = AfxGetApp()->GetProfileInt(sRegKey, "LastThisRisk", m_nThisRisk);
	m_nMoreThanRisk = AfxGetApp()->GetProfileInt(sRegKey, "LastMoreThanRisk", m_nMoreThanRisk);
	m_nLessThanRisk = AfxGetApp()->GetProfileInt(sRegKey, "LastLessThanRisk", m_nLessThanRisk);
	m_nStartRisk = AfxGetApp()->GetProfileInt(sRegKey, "LastStartRisk", m_nStartRisk);
	m_nEndRisk = AfxGetApp()->GetProfileInt(sRegKey, "LastEndRisk", m_nEndRisk);

	UpdateData(FALSE);
	
	EnableControls(m_nRiskOption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindRiskPage::GetRange(int& nFrom, int& nTo)
{
	if (GetSafeHwnd())
		UpdateData();
	
	switch (m_nRiskOption)
	{
	case THISRISK:
		nFrom = m_nThisRisk;
		nTo = nFrom;
		break;
		
	case MORETHAN:
		nFrom = m_nMoreThanRisk + 1;
		nTo = INT_MAX;
		break;
		
	case LESSTHAN:
		nFrom = INT_MIN;
		nTo = m_nLessThanRisk - 1;
		break;
		
	case BETWEEN:
		nFrom = m_nStartRisk;
		nTo = m_nEndRisk;
		break;
	}
}

void CFindRiskPage::OnChangeOption() 
{
	UpdateData();
	EnableControls(m_nRiskOption);
}

void CFindRiskPage::EnableControls(int nOption)
{
	GetDlgItem(IDC_THISRISK)->EnableWindow(nOption == THISRISK);
	GetDlgItem(IDC_MORETHANRISK)->EnableWindow(nOption == MORETHAN);
	GetDlgItem(IDC_LESSTHANRISK)->EnableWindow(nOption == LESSTHAN);
	GetDlgItem(IDC_RISKSTART)->EnableWindow(nOption == BETWEEN);
	GetDlgItem(IDC_RISKEND)->EnableWindow(nOption == BETWEEN);
}

CString CFindRiskPage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

void CFindRiskPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();

	CString sRegKey = GetRegKey();
	
	AfxGetApp()->WriteProfileInt(sRegKey, "LastNumOption", m_nRiskOption);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastThisRisk", m_nThisRisk);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastMoreThanRisk", m_nMoreThanRisk);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastLessThanRisk", m_nLessThanRisk);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastStartRisk", m_nStartRisk);
	AfxGetApp()->WriteProfileInt(sRegKey, "LastEndRisk", m_nEndRisk);
}
