// PreferencesMultiUserPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesMultiUserPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesMultiUserPage property page

IMPLEMENT_DYNCREATE(CPreferencesMultiUserPage, CPropertyPage)

CPreferencesMultiUserPage::CPreferencesMultiUserPage() : CPropertyPage(CPreferencesMultiUserPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesMultiUserPage)
	m_bFormatXmlOutput = FALSE;
	//}}AFX_DATA_INIT
	m_bEnableSourceControl = AfxGetApp()->GetProfileInt("Preferences", "EnableSourceControl", FALSE);
	m_bSourceControlLanOnly = AfxGetApp()->GetProfileInt("Preferences", "SourceControlLanOnly", TRUE);
	m_bPromptReloadOnWritable = AfxGetApp()->GetProfileInt("Preferences", "PromptReloadOnWritable", TRUE);
	m_bAutoCheckOut = AfxGetApp()->GetProfileInt("Preferences", "AutoCheckOut", FALSE);
	m_bPromptReloadOnTimestamp = AfxGetApp()->GetProfileInt("Preferences", "PromptReloadOnTimestamp", TRUE);
	m_bCheckoutOnCheckin = AfxGetApp()->GetProfileInt("Preferences", "CheckoutOnCheckin", FALSE);
	m_nReadonlyReloadOption = AfxGetApp()->GetProfileInt("Preferences", "ReadonlyReloadOption", RO_ASK) - 1;
	m_nTimestampReloadOption = AfxGetApp()->GetProfileInt("Preferences", "TimestampReloadOption", RO_ASK) - 1;
	m_bCheckinOnClose = AfxGetApp()->GetProfileInt("Preferences", "CheckinOnClose", TRUE);
	m_nRemoteFileCheckFreq = AfxGetApp()->GetProfileInt("Preferences", "RemoteFileCheckFrequency", 30);
	m_nCheckinNoEditTime = AfxGetApp()->GetProfileInt("Preferences", "CheckinNoEditTime", 10);
	m_bCheckinNoChange = AfxGetApp()->GetProfileInt("Preferences", "CheckinNoEdit", FALSE);
	m_bFormatXmlOutput = AfxGetApp()->GetProfileInt("Preferences", "FormatXmlOutput", FALSE);
}

CPreferencesMultiUserPage::~CPreferencesMultiUserPage()
{
}

void CPreferencesMultiUserPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesMultiUserPage)
	DDX_Control(pDX, IDC_NOCHANGETIME, m_cbNoEditTime);
	DDX_Check(pDX, IDC_CHECKINONNOEDIT, m_bCheckinNoChange);
	DDX_Check(pDX, IDC_FORMATXML, m_bFormatXmlOutput);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_REMOTEFILECHECK, m_cbRemoteFileCheck);
	DDX_Check(pDX, IDC_PROMPTRELOADONWRITABLE, m_bPromptReloadOnWritable);
	DDX_Check(pDX, IDC_PROMPTRELOADONCHANGE, m_bPromptReloadOnTimestamp);
	DDX_Check(pDX, IDC_ENABLESOURCECONTROL, m_bEnableSourceControl);
	DDX_Check(pDX, IDC_SOURCECONTROLLANONLY, m_bSourceControlLanOnly);
	DDX_Check(pDX, IDC_AUTOCHECKOUT, m_bAutoCheckOut);
	DDX_Check(pDX, IDC_CHECKOUTONCHECKIN, m_bCheckoutOnCheckin);
	DDX_CBIndex(pDX, IDC_READONLYRELOADOPTION, m_nReadonlyReloadOption);
	DDX_CBIndex(pDX, IDC_TIMESTAMPRELOADOPTION, m_nTimestampReloadOption);
	DDX_Check(pDX, IDC_CHECKINONCLOSE, m_bCheckinOnClose);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		CString sFreq;
		m_cbRemoteFileCheck.GetLBText(m_cbRemoteFileCheck.GetCurSel(), sFreq);
		m_nRemoteFileCheckFreq = atoi(sFreq);

		m_cbNoEditTime.GetLBText(m_cbNoEditTime.GetCurSel(), sFreq);
		m_nCheckinNoEditTime = atoi(sFreq);
	}
	else
	{
		CString sFreq;
		sFreq.Format("%d", m_nRemoteFileCheckFreq);

		if (CB_ERR == m_cbRemoteFileCheck.SelectString(-1, sFreq))
		{
			m_nRemoteFileCheckFreq = 30;
			m_cbRemoteFileCheck.SelectString(-1, "30");
		}

		sFreq.Format("%d", m_nCheckinNoEditTime);

		if (CB_ERR == m_cbNoEditTime.SelectString(-1, sFreq))
		{
			m_nCheckinNoEditTime = 0;
			m_cbRemoteFileCheck.SelectString(-1, "10");
		}
	}
}


BEGIN_MESSAGE_MAP(CPreferencesMultiUserPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesMultiUserPage)
	ON_BN_CLICKED(IDC_CHECKINONNOEDIT, OnCheckinonnoedit)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ENABLESOURCECONTROL, OnEnablesourcecontrol)
	ON_BN_CLICKED(IDC_PROMPTRELOADONWRITABLE, OnPromptreloadonwritable)
	ON_BN_CLICKED(IDC_PROMPTRELOADONCHANGE, OnPromptreloadontimestamp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesMultiUserPage message handlers

BOOL CPreferencesMultiUserPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	GetDlgItem(IDC_SOURCECONTROLLANONLY)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_AUTOCHECKOUT)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_CHECKOUTONCHECKIN)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_CHECKINONCLOSE)->EnableWindow(m_bEnableSourceControl);
	
	GetDlgItem(IDC_READONLYRELOADOPTION)->EnableWindow(m_bPromptReloadOnWritable);
	GetDlgItem(IDC_TIMESTAMPRELOADOPTION)->EnableWindow(m_bPromptReloadOnTimestamp);

	GetDlgItem(IDC_CHECKINONNOEDIT)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_NOCHANGETIME)->EnableWindow(m_bEnableSourceControl && m_bCheckinNoChange);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesMultiUserPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "PromptReloadOnWritable", m_bPromptReloadOnWritable);
	AfxGetApp()->WriteProfileInt("Preferences", "PromptReloadOnTimestamp", m_bPromptReloadOnTimestamp);
	AfxGetApp()->WriteProfileInt("Preferences", "EnableSourceControl", m_bEnableSourceControl);
	AfxGetApp()->WriteProfileInt("Preferences", "SourceControlLanOnly", m_bSourceControlLanOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoCheckOut", m_bAutoCheckOut);
	AfxGetApp()->WriteProfileInt("Preferences", "CheckoutOnCheckin", m_bCheckoutOnCheckin);
	AfxGetApp()->WriteProfileInt("Preferences", "ReadonlyReloadOption", m_nReadonlyReloadOption + 1);
	AfxGetApp()->WriteProfileInt("Preferences", "TimestampReloadOption", m_nTimestampReloadOption + 1);
	AfxGetApp()->WriteProfileInt("Preferences", "CheckinOnClose", m_bCheckinOnClose);
	AfxGetApp()->WriteProfileInt("Preferences", "RemoteFileCheckFrequency", m_nRemoteFileCheckFreq);
	AfxGetApp()->WriteProfileInt("Preferences", "CheckinNoEditTime", m_nCheckinNoEditTime);
	AfxGetApp()->WriteProfileInt("Preferences", "CheckinNoEdit", m_bCheckinNoChange);
	AfxGetApp()->WriteProfileInt("Preferences", "FormatXmlOutput", m_bFormatXmlOutput);
//	AfxGetApp()->WriteProfileInt("Preferences", "", m_b);
}

void CPreferencesMultiUserPage::OnEnablesourcecontrol() 
{
	UpdateData();

	GetDlgItem(IDC_SOURCECONTROLLANONLY)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_AUTOCHECKOUT)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_CHECKOUTONCHECKIN)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_CHECKINONCLOSE)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_CHECKINONNOEDIT)->EnableWindow(m_bEnableSourceControl);
	GetDlgItem(IDC_NOCHANGETIME)->EnableWindow(m_bEnableSourceControl && m_bCheckinNoChange);
}

int CPreferencesMultiUserPage::GetReadonlyReloadOption() const
{ 
	if (!m_bPromptReloadOnWritable)
		return RO_NO;
	else
		return m_nReadonlyReloadOption + 1; 
}

int CPreferencesMultiUserPage::GetTimestampReloadOption() const 
{ 
	if (!m_bPromptReloadOnTimestamp)
		return RO_NO;
	else
		return m_nTimestampReloadOption + 1; 
}

void CPreferencesMultiUserPage::OnPromptreloadonwritable() 
{
	UpdateData();
	GetDlgItem(IDC_READONLYRELOADOPTION)->EnableWindow(m_bPromptReloadOnWritable);
}

void CPreferencesMultiUserPage::OnPromptreloadontimestamp() 
{
	UpdateData();
	GetDlgItem(IDC_TIMESTAMPRELOADOPTION)->EnableWindow(m_bPromptReloadOnTimestamp);
}

void CPreferencesMultiUserPage::OnCheckinonnoedit() 
{
	UpdateData();
	GetDlgItem(IDC_NOCHANGETIME)->EnableWindow(m_bEnableSourceControl && m_bCheckinNoChange);
}
