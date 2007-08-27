// PreferencesTaskDefPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesTaskDefPage.h"
#include "tdcenum.h"

#include "..\shared\enstring.h"
#include "..\shared\misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage property page

IMPLEMENT_DYNCREATE(CPreferencesTaskDefPage, CPreferencesPageBase)

CPreferencesTaskDefPage::CPreferencesTaskDefPage() : 
	CPreferencesPageBase(CPreferencesTaskDefPage::IDD),
	m_cbAllocByList(TRUE),
	m_cbAllocToList(TRUE),
	m_cbCategoryList(TRUE),
	m_cbStatusList(TRUE)
{
	//{{AFX_DATA_INIT(CPreferencesTaskDefPage)
	//}}AFX_DATA_INIT
	m_nSelAttribUse = -1;

	// attrib use
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_PRIORITY, PTPA_PRIORITY, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_RISK, PTPA_RISK, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_TIMEEST, PTPA_TIMEEST, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_ALLOCTO, PTPA_ALLOCTO, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_ALLOCBY, PTPA_ALLOCBY, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_STATUS, PTPA_STATUS, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_CATEGORY, PTPA_CATEGORY, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_PTDP_COLOR, PTPA_COLOR, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_PTDP_DUEDATE, PTPA_DUEDATE, -1)); 

	m_eCost.SetMask(".0123456789", ME_LOCALIZEDECIMAL);
}

CPreferencesTaskDefPage::~CPreferencesTaskDefPage()
{
}

void CPreferencesTaskDefPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskDefPage)
	DDX_Control(pDX, IDC_DEFAULTRISK, m_cbDefRisk);
	DDX_Control(pDX, IDC_DEFAULTPRIORITY, m_cbDefPriority);
	DDX_Control(pDX, IDC_ALLOCBYLIST, m_cbAllocByList);
	DDX_Control(pDX, IDC_ALLOCTOLIST, m_cbAllocToList);
	DDX_Control(pDX, IDC_STATUSLIST, m_cbStatusList);
	DDX_Control(pDX, IDC_CATEGORYLIST, m_cbCategoryList);
	DDX_Control(pDX, IDC_DEFAULTTIMEEST, m_eTimeEst);
	DDX_Text(pDX, IDC_DEFAULTCREATEDBY, m_sDefCreatedBy);
	DDX_Text(pDX, IDC_DEFAULTCOST, m_dDefCost);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DEFAULTCOST, m_eCost);
	DDX_Control(pDX, IDC_SETDEFAULTCOLOR, m_btDefColor);
	DDX_Control(pDX, IDC_INHERITATTRIBUTES, m_lbAttribUse);
	DDX_CBPriority(pDX, IDC_DEFAULTPRIORITY, m_nDefPriority);
	DDX_CBRisk(pDX, IDC_DEFAULTRISK, m_nDefRisk);
	DDX_Text(pDX, IDC_DEFAULTTIMEEST, m_dDefTimeEst);
	DDX_Text(pDX, IDC_DEFAULTALLOCTO, m_sDefAllocTo);
	DDX_LBIndex(pDX, IDC_INHERITATTRIBUTES, m_nSelAttribUse);
	DDX_Text(pDX, IDC_DEFAULTALLOCBY, m_sDefAllocBy);
	DDX_Text(pDX, IDC_DEFAULTSTATUS, m_sDefStatus);
	DDX_Text(pDX, IDC_DEFAULTCATEGORY, m_sDefCategory);
	DDX_Check(pDX, IDC_USEPARENTATTRIB, m_bUseParentAttributes);
	DDX_Check(pDX, IDC_USECREATIONFORDEFSTARTDATE, m_bUseCreationForDefStartDate);
}


BEGIN_MESSAGE_MAP(CPreferencesTaskDefPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesTaskDefPage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETDEFAULTCOLOR, OnSetdefaultcolor)
	ON_BN_CLICKED(IDC_USEPARENTATTRIB, OnUseparentattrib)
	ON_CLBN_CHKCHANGE(IDC_INHERITATTRIBUTES, OnAttribUseChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage message handlers

BOOL CPreferencesTaskDefPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();
	
	m_mgrGroupLines.AddGroupLine(IDC_DEFGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_INHERITGROUP, *this);

	GetDlgItem(IDC_INHERITATTRIBUTES)->EnableWindow(m_bUseParentAttributes);
	
	m_btDefColor.SetColor(m_crDef);

	int nIndex = (int)m_aAttribPrefs.GetSize();
	
	while (nIndex--)
	{
		int nPos = m_lbAttribUse.InsertString(0, m_aAttribPrefs[nIndex].sName);
		m_lbAttribUse.SetCheck(nPos, m_aAttribPrefs[nIndex].bUse ? 1 : 0);
	}

	// init edit prompts()
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTALLOCTO, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTALLOCBY, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTSTATUS, *this, CEnString(IDS_PTDP_STATUSPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTCATEGORY, *this, CEnString(IDS_PTDP_CATEGORYPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTCREATEDBY, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_ALLOCTOLIST, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_ALLOCBYLIST, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_STATUSLIST, *this, CEnString(IDS_PTDP_STATUSPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_CATEGORYLIST, *this, CEnString(IDS_PTDP_CATEGORYPROMPT));

	// default string lists
	m_cbCategoryList.AddUniqueItems(m_aDefCats);
	m_cbStatusList.AddUniqueItems(m_aDefStatus);
	m_cbAllocToList.AddUniqueItems(m_aDefAllocTo);
	m_cbAllocByList.AddUniqueItems(m_aDefAllocBy);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTaskDefPage::SetPriorityColors(const CDWordArray& aColors)
{
	m_cbDefPriority.SetColors(aColors);
}

void CPreferencesTaskDefPage::OnOK() 
{
	CPreferencesPageBase::OnOK();

	// default string lists
	m_cbCategoryList.GetItems(m_aDefCats);
	m_cbStatusList.GetItems(m_aDefStatus);
	m_cbAllocToList.GetItems(m_aDefAllocTo);
	m_cbAllocByList.GetItems(m_aDefAllocBy);
}

void CPreferencesTaskDefPage::OnSetdefaultcolor() 
{
	m_crDef = m_btDefColor.GetColor();
}

void CPreferencesTaskDefPage::OnUseparentattrib() 
{
	UpdateData();

	GetDlgItem(IDC_INHERITATTRIBUTES)->EnableWindow(m_bUseParentAttributes);
}

void CPreferencesTaskDefPage::OnAttribUseChange()
{
	UpdateData();

	ASSERT (m_nSelAttribUse >= 0);
	
	if (m_nSelAttribUse >= 0)
		m_aAttribPrefs[m_nSelAttribUse].bUse = m_lbAttribUse.GetCheck(m_nSelAttribUse);
}

BOOL CPreferencesTaskDefPage::GetUseParentAttrib(PTP_ATTRIB nAttrib) const
{ 
	if (!m_bUseParentAttributes)
		return FALSE;
	
	int nIndex = (int)m_aAttribPrefs.GetSize();
	
	while (nIndex--)
	{
		if (m_aAttribPrefs[nIndex].nAttrib == nAttrib)
			return m_aAttribPrefs[nIndex].bUse; 
	}

	// else
	return FALSE;
}

int CPreferencesTaskDefPage::GetDefaultCategories(CStringArray& aCats) const
{
	return Misc::ParseIntoArray(m_sDefCategory, aCats);
}

int CPreferencesTaskDefPage::GetDefaultAllocTo(CStringArray& aAllocTo) const
{
	return Misc::ParseIntoArray(m_sDefAllocTo, aAllocTo);
}

double CPreferencesTaskDefPage::GetDefaultTimeEst(int& nUnits) const 
{ 
	nUnits = m_eTimeEst.GetUnits();

	return m_dDefTimeEst; 
}

int CPreferencesTaskDefPage::GetCategoryList(CStringArray& aItems) const
{
	aItems.Copy(m_aDefCats);
	return aItems.GetSize();
}

int CPreferencesTaskDefPage::GetStatusList(CStringArray& aItems) const
{
	aItems.Copy(m_aDefStatus);
	return aItems.GetSize();
}

int CPreferencesTaskDefPage::GetAllocToList(CStringArray& aItems) const
{
	aItems.Copy(m_aDefAllocTo);
	return aItems.GetSize();
}

int CPreferencesTaskDefPage::GetAllocByList(CStringArray& aItems) const
{
	aItems.Copy(m_aDefAllocBy);
	return aItems.GetSize();
}

HBRUSH CPreferencesTaskDefPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPreferencesPageBase::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (pWnd->GetDlgCtrlID() == IDC_DEFLISTLABEL)
		pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
	
	return hbr;
}

void CPreferencesTaskDefPage::LoadPreferences(const CPreferencesStorage& prefs)
{
	// load settings
	m_nDefPriority = prefs.GetProfileInt("Preferences", "DefaultPriority", 5); 
	m_nDefRisk = prefs.GetProfileInt("Preferences", "DefaultRisk", 0); 
	m_sDefAllocTo = prefs.GetProfileString("Preferences", "DefaultAllocTo");
	m_sDefAllocBy = prefs.GetProfileString("Preferences", "DefaultAllocBy");
	m_sDefStatus = prefs.GetProfileString("Preferences", "DefaultStatus");
	m_sDefCategory = prefs.GetProfileString("Preferences", "DefaultCategory");
	m_sDefCreatedBy = prefs.GetProfileString("Preferences", "DefaultCreatedBy");
	m_crDef = prefs.GetProfileInt("Preferences", "DefaultColor", 0);
	m_bUseParentAttributes = prefs.GetProfileInt("Preferences", "UseParentAttributes", TRUE);
	m_bUseCreationForDefStartDate = prefs.GetProfileInt("Preferences", "UseCreationForDefStartDate", TRUE);

	// backwards compatibility
	int nTimeEst = prefs.GetProfileInt("Preferences", "DefaultTimeEstimate", -1);

	if (nTimeEst != -1) // key exists
		m_dDefTimeEst = (double)nTimeEst;
	else
		m_dDefTimeEst = prefs.GetProfileDouble("Preferences", "DefaultTimeEstimate", 0);

   m_eTimeEst.SetUnits(prefs.GetProfileInt("Preferences", "DefaultTimeEstUnits", THU_HOURS));

   m_dDefCost = Misc::Atof(prefs.GetProfileString("Preferences", "DefaultCost", "0"));

   // attribute use
	int nIndex = m_aAttribPrefs.GetSize();
	
	while (nIndex--)
	{
		CString sKey;
		sKey.Format("Attrib%d", m_aAttribPrefs[nIndex].nAttrib);
		
		m_aAttribPrefs[nIndex].bUse = prefs.GetProfileInt("Preferences\\AttribUse", sKey, FALSE);
	}

	// combo lists
	prefs.GetArrayItems("Preferences\\CategoryList", m_aDefCats);
	prefs.GetArrayItems("Preferences\\StatusList", m_aDefStatus);
	prefs.GetArrayItems("Preferences\\AllocToList", m_aDefAllocTo);
	prefs.GetArrayItems("Preferences\\AllocByList", m_aDefAllocBy);
}

void CPreferencesTaskDefPage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "DefaultPriority", m_nDefPriority);
	prefs.WriteProfileInt("Preferences", "DefaultRisk", m_nDefRisk);
	prefs.WriteProfileString("Preferences", "DefaultAllocTo", m_sDefAllocTo);
	prefs.WriteProfileString("Preferences", "DefaultAllocBy", m_sDefAllocBy);
	prefs.WriteProfileString("Preferences", "DefaultStatus", m_sDefStatus);
	prefs.WriteProfileString("Preferences", "DefaultCategory", m_sDefCategory);
	prefs.WriteProfileString("Preferences", "DefaultCreatedBy", m_sDefCreatedBy);
	prefs.WriteProfileInt("Preferences", "DefaultColor", m_crDef);
	prefs.WriteProfileInt("Preferences", "UseParentAttributes", m_bUseParentAttributes);
	prefs.WriteProfileInt("Preferences", "UseCreationForDefStartDate", m_bUseCreationForDefStartDate);
	prefs.WriteProfileDouble("Preferences", "DefaultCost", m_dDefCost);
	prefs.WriteProfileDouble("Preferences", "DefaultTimeEstimate", m_dDefTimeEst);
	prefs.WriteProfileInt("Preferences", "DefaultTimeEstUnits", m_eTimeEst.GetUnits());
	
	// attribute usage
	int nIndex = m_aAttribPrefs.GetSize();

	while (nIndex--)
	{
		CString sKey;
		sKey.Format("Attrib%d", m_aAttribPrefs[nIndex].nAttrib);

		prefs.WriteProfileInt("Preferences\\AttribUse", sKey, m_aAttribPrefs[nIndex].bUse);
	}

	prefs.WriteArrayItems(m_aDefCats, "Preferences\\CategoryList");
	prefs.WriteArrayItems(m_aDefStatus, "Preferences\\StatusList");
	prefs.WriteArrayItems(m_aDefAllocTo, "Preferences\\AllocToList");
	prefs.WriteArrayItems(m_aDefAllocBy, "Preferences\\AllocByList");
}
