// FindNumPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindNumPage.h"
#include "..\shared\misc.h"

#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindNumPage property page

enum { THISNUMBER, MORETHAN, LESSTHAN, BETWEEN };

const LPCTSTR MASKNEGINT = "0123456789-";
const LPCTSTR MASKPOSINT = "0123456789";
const LPCTSTR MASKNEGFLT = ".0123456789-";
const LPCTSTR MASKPOSFLT = ".0123456789";

IMPLEMENT_DYNCREATE(CFindNumPage, CPropertyPage)

CFindNumPage::CFindNumPage(BOOL bInteger, BOOL bAllowNegative) : 
	CPropertyPage(CFindNumPage::IDD), m_bAllowNegative(bAllowNegative)
{
	//{{AFX_DATA_INIT(CFindNumPage)
	m_nNumberOption = MORETHAN;
	m_sMoreThanValue = _T("");
	m_sLessThanValue = _T("");
	m_sStartValue = _T("");
	m_sEndValue = _T("");
	m_sThisValue = _T("");
	//}}AFX_DATA_INIT

	if (bInteger)
	{
		if (bAllowNegative)
		{
			m_eThis.SetMask(MASKNEGINT);
			m_eMoreThan.SetMask(MASKNEGINT);
			m_eLessThan.SetMask(MASKNEGINT);
			m_eStart.SetMask(MASKNEGINT);
			m_eEnd.SetMask(MASKNEGINT);
		}
		else
		{
			m_eThis.SetMask(MASKPOSINT);
			m_eMoreThan.SetMask(MASKPOSINT);
			m_eLessThan.SetMask(MASKPOSINT);
			m_eStart.SetMask(MASKPOSINT);
			m_eEnd.SetMask(MASKPOSINT);
		}
	}
	else // float
	{
		if (bAllowNegative)
		{
			m_eThis.SetMask(MASKNEGFLT, ME_LOCALIZEDECIMAL);
			m_eMoreThan.SetMask(MASKNEGFLT, ME_LOCALIZEDECIMAL);
			m_eLessThan.SetMask(MASKNEGFLT, ME_LOCALIZEDECIMAL);
			m_eStart.SetMask(MASKNEGFLT, ME_LOCALIZEDECIMAL);
			m_eEnd.SetMask(MASKNEGFLT, ME_LOCALIZEDECIMAL);
		}
		else
		{
			m_eThis.SetMask(MASKPOSFLT, ME_LOCALIZEDECIMAL);
			m_eMoreThan.SetMask(MASKPOSFLT, ME_LOCALIZEDECIMAL);
			m_eLessThan.SetMask(MASKPOSFLT, ME_LOCALIZEDECIMAL);
			m_eStart.SetMask(MASKPOSFLT, ME_LOCALIZEDECIMAL);
			m_eEnd.SetMask(MASKPOSFLT, ME_LOCALIZEDECIMAL);
		}
	}
}

CFindNumPage::~CFindNumPage()
{
}

void CFindNumPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindNumPage)
	DDX_Control(pDX, IDC_THISNUMBER, m_eThis);
	DDX_Control(pDX, IDC_NUMBEREND, m_eEnd);
	DDX_Control(pDX, IDC_NUMBERSTART, m_eStart);
	DDX_Control(pDX, IDC_LESSTHANNUMBER, m_eLessThan);
	DDX_Control(pDX, IDC_MORETHANNUMBER, m_eMoreThan);
	DDX_Radio(pDX, IDC_THIS, m_nNumberOption);
	DDX_Text(pDX, IDC_MORETHANNUMBER, m_sMoreThanValue);
	DDX_Text(pDX, IDC_LESSTHANNUMBER, m_sLessThanValue);
	DDX_Text(pDX, IDC_NUMBERSTART, m_sStartValue);
	DDX_Text(pDX, IDC_NUMBEREND, m_sEndValue);
	DDX_Text(pDX, IDC_THISNUMBER, m_sThisValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindNumPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindNumPage)
	ON_BN_CLICKED(IDC_MORETHAN, OnChangeOption)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LESSTHAN, OnChangeOption)
	ON_BN_CLICKED(IDC_BETWEEN, OnChangeOption)
	ON_BN_CLICKED(IDC_THIS, OnChangeOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindNumPage message handlers

BOOL CFindNumPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString sRegKey = GetRegKey();
	
	m_nNumberOption = AfxGetApp()->GetProfileInt(sRegKey, "LastNumOption", m_nNumberOption);
	m_sThisValue = AfxGetApp()->GetProfileString(sRegKey, "LastThisValue");
	m_sMoreThanValue = AfxGetApp()->GetProfileString(sRegKey, "LastMoreThanValue");
	m_sLessThanValue = AfxGetApp()->GetProfileString(sRegKey, "LastLessThanValue");
	m_sStartValue = AfxGetApp()->GetProfileString(sRegKey, "LastStartValue");
	m_sEndValue = AfxGetApp()->GetProfileString(sRegKey, "LastEndValue");

	UpdateData(FALSE);
	
	EnableControls(m_nNumberOption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindNumPage::GetNumberRange(int& nFrom, int& nTo)
{
	if (GetSafeHwnd())
		UpdateData();

	switch (m_nNumberOption)
	{
	case THISNUMBER:
		nFrom = atoi(m_sThisValue);
		nTo = nFrom;
		break;
		
	case MORETHAN:
		nFrom = atoi(m_sMoreThanValue) + 1;
		nTo = INT_MAX;
		break;
		
	case LESSTHAN:
		nFrom = INT_MIN;
		nTo = atoi(m_sLessThanValue) - 1;
		break;
		
	case BETWEEN:
		nFrom = atoi(m_sStartValue);
		nTo = atoi(m_sEndValue);
		break;
	}

	if (!m_bAllowNegative)
	{
		nFrom = max(nFrom, 0);
		nTo = max(nTo, 0);
	}
}

void CFindNumPage::GetNumberRange(double& dFrom, double& dTo)
{
	if (GetSafeHwnd())
		UpdateData();

	switch (m_nNumberOption)
	{
	case THISNUMBER:
		dFrom = Misc::Atof(m_sThisValue);
		dTo = dFrom;
		break;
		
	case MORETHAN:
		dFrom = Misc::Atof(m_sMoreThanValue) + 0.0000001;
		dTo = DBL_MAX;
		break;
		
	case LESSTHAN:
		dFrom = DBL_MIN;
		dTo = Misc::Atof(m_sLessThanValue) - 0.0000001;
		break;
		
	case BETWEEN:
		dFrom = Misc::Atof(m_sStartValue);
		dTo = Misc::Atof(m_sEndValue);
		break;
	}

	if (!m_bAllowNegative)
	{
		dFrom = max(dFrom, 0);
		dTo = max(dTo, 0);
	}
}

void CFindNumPage::OnChangeOption() 
{
	UpdateData();
	EnableControls(m_nNumberOption);
}

void CFindNumPage::EnableControls(int nOption)
{
	GetDlgItem(IDC_THISNUMBER)->EnableWindow(nOption == THISNUMBER);
	GetDlgItem(IDC_MORETHANNUMBER)->EnableWindow(nOption == MORETHAN);
	GetDlgItem(IDC_LESSTHANNUMBER)->EnableWindow(nOption == LESSTHAN);
	GetDlgItem(IDC_NUMBERSTART)->EnableWindow(nOption == BETWEEN);
	GetDlgItem(IDC_NUMBEREND)->EnableWindow(nOption == BETWEEN);
}

CString CFindNumPage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

void CFindNumPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();

	CString sRegKey = GetRegKey();
	
	AfxGetApp()->WriteProfileInt(sRegKey, "LastNumOption", m_nNumberOption);
	AfxGetApp()->WriteProfileString(sRegKey, "LastThisValue", m_sThisValue);
	AfxGetApp()->WriteProfileString(sRegKey, "LastMoreThanValue", m_sMoreThanValue);
	AfxGetApp()->WriteProfileString(sRegKey, "LastLessThanValue", m_sLessThanValue);
	AfxGetApp()->WriteProfileString(sRegKey, "LastStartValue", m_sStartValue);
	AfxGetApp()->WriteProfileString(sRegKey, "LastEndValue", m_sEndValue);
}
