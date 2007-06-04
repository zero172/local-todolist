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

IMPLEMENT_DYNCREATE(CPreferencesTaskDefPage, CPropertyPage)

CPreferencesTaskDefPage::CPreferencesTaskDefPage() : 
	CPropertyPage(CPreferencesTaskDefPage::IDD),
	m_cbAllocByList(TRUE),
	m_cbAllocToList(TRUE),
	m_cbCategoryList(TRUE),
	m_cbStatusList(TRUE)
{
	//{{AFX_DATA_INIT(CPreferencesTaskDefPage)
	m_sDefCreatedBy = _T("");
	m_dDefCost = 0.0;
	//}}AFX_DATA_INIT
	m_nSelAttribUse = -1;

	// load settings
	m_nDefPriority = AfxGetApp()->GetProfileInt("Preferences", "DefaultPriority", FT_NOPRIORITY); 
	m_nDefRisk = AfxGetApp()->GetProfileInt("Preferences", "DefaultRisk", FT_NORISK); 
	m_sDefAllocTo = AfxGetApp()->GetProfileString("Preferences", "DefaultAllocTo");
	m_sDefAllocBy = AfxGetApp()->GetProfileString("Preferences", "DefaultAllocBy");
	m_sDefStatus = AfxGetApp()->GetProfileString("Preferences", "DefaultStatus");
	m_sDefCategory = AfxGetApp()->GetProfileString("Preferences", "DefaultCategory");
	m_sDefCreatedBy = AfxGetApp()->GetProfileString("Preferences", "DefaultCreatedBy");
	m_crDef = AfxGetApp()->GetProfileInt("Preferences", "DefaultColor", 0);
	m_bUseParentAttributes = AfxGetApp()->GetProfileInt("Preferences", "UseParentAttributes", TRUE);
	m_bUseCreationForDefStartDate = AfxGetApp()->GetProfileInt("Preferences", "UseCreationForDefStartDate", TRUE);

	// backwards compatibility
	int nTimeEst = AfxGetApp()->GetProfileInt("Preferences", "DefaultTimeEstimate", -1);

	if (nTimeEst != -1) // key exists
		m_dDefTimeEst = (double)nTimeEst;
	else
		m_dDefTimeEst = Misc::Atof(AfxGetApp()->GetProfileString("Preferences", "DefaultTimeEstimate", "0"));
	
	m_eTimeEst.SetUnits(AfxGetApp()->GetProfileInt("Preferences", "DefaultTimeEstUnits", THU_HOURS));

	m_dDefCost = Misc::Atof(AfxGetApp()->GetProfileString("Preferences", "DefaultCost", "0"));

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

	int nIndex = m_aAttribPrefs.GetSize();
	
	while (nIndex--)
	{
		CString sKey;
		sKey.Format("Attrib%d", m_aAttribPrefs[nIndex].nAttrib);
		
		m_aAttribPrefs[nIndex].bUse = AfxGetApp()->GetProfileInt("Preferences\\AttribUse", sKey, FALSE);
	}

	m_eCost.SetMask(".0123456789", ME_LOCALIZEDECIMAL);
}

CPreferencesTaskDefPage::~CPreferencesTaskDefPage()
{
}

void CPreferencesTaskDefPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
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
	DDX_Control(pDX, IDC_DEFAULTATTRIBUTES, m_lbAttribUse);
	DDX_CBPriority(pDX, IDC_DEFAULTPRIORITY, m_nDefPriority);
	DDX_CBRisk(pDX, IDC_DEFAULTRISK, m_nDefRisk);
	DDX_Text(pDX, IDC_DEFAULTTIMEEST, m_dDefTimeEst);
	DDX_Text(pDX, IDC_DEFAULTALLOCTO, m_sDefAllocTo);
	DDX_LBIndex(pDX, IDC_DEFAULTATTRIBUTES, m_nSelAttribUse);
	DDX_Text(pDX, IDC_DEFAULTALLOCBY, m_sDefAllocBy);
	DDX_Text(pDX, IDC_DEFAULTSTATUS, m_sDefStatus);
	DDX_Text(pDX, IDC_DEFAULTCATEGORY, m_sDefCategory);
	DDX_Check(pDX, IDC_USEPARENTATTRIB, m_bUseParentAttributes);
	DDX_Check(pDX, IDC_USECREATIONFORDEFSTARTDATE, m_bUseCreationForDefStartDate);
}


BEGIN_MESSAGE_MAP(CPreferencesTaskDefPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesTaskDefPage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETDEFAULTCOLOR, OnSetdefaultcolor)
	ON_BN_CLICKED(IDC_USEPARENTATTRIB, OnUseparentattrib)
	ON_CLBN_CHKCHANGE(IDC_DEFAULTATTRIBUTES, OnAttribUseChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage message handlers

BOOL CPreferencesTaskDefPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_mgrGroupLines.AddGroupLine(IDC_DEFGROUP, *this);
	GetDlgItem(IDC_DEFAULTATTRIBUTES)->EnableWindow(m_bUseParentAttributes);
	
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
	RestoreComboList(m_cbCategoryList, "Preferences\\CategoryList");
	RestoreComboList(m_cbStatusList, "Preferences\\StatusList");
	RestoreComboList(m_cbAllocToList, "Preferences\\AllocToList");
	RestoreComboList(m_cbAllocByList, "Preferences\\AllocByList");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTaskDefPage::SetPriorityColors(const CDWordArray& aColors)
{
	m_cbDefPriority.SetColors(aColors);
}

void CPreferencesTaskDefPage::OnOK() 
{
	CPropertyPage::OnOK();

	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "DefaultPriority", m_nDefPriority);
	AfxGetApp()->WriteProfileInt("Preferences", "DefaultRisk", m_nDefRisk);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultAllocTo", m_sDefAllocTo);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultAllocBy", m_sDefAllocBy);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultStatus", m_sDefStatus);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultCategory", m_sDefCategory);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultCreatedBy", m_sDefCreatedBy);
	AfxGetApp()->WriteProfileInt("Preferences", "DefaultColor", m_crDef);
	AfxGetApp()->WriteProfileInt("Preferences", "UseParentAttributes", m_bUseParentAttributes);
	AfxGetApp()->WriteProfileInt("Preferences", "UseCreationForDefStartDate", m_bUseCreationForDefStartDate);

	CString sTimeEst;
	sTimeEst.Format("%f", m_dDefTimeEst);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultTimeEstimate", sTimeEst);
	AfxGetApp()->WriteProfileInt("Preferences", "DefaultTimeEstUnits", m_eTimeEst.GetUnits());

	CString sCost;
	sCost.Format("%f", m_dDefCost);
	AfxGetApp()->WriteProfileString("Preferences", "DefaultCost", sCost);
	
	// attribute usage
	int nIndex = m_aAttribPrefs.GetSize();

	while (nIndex--)
	{
		CString sKey;
		sKey.Format("Attrib%d", m_aAttribPrefs[nIndex].nAttrib);

		AfxGetApp()->WriteProfileInt("Preferences\\AttribUse", sKey, m_aAttribPrefs[nIndex].bUse);
	}

	// default string lists
	SaveComboList(m_cbCategoryList, "Preferences\\CategoryList");
	SaveComboList(m_cbStatusList, "Preferences\\StatusList");
	SaveComboList(m_cbAllocToList, "Preferences\\AllocToList");
	SaveComboList(m_cbAllocByList, "Preferences\\AllocByList");
}

void CPreferencesTaskDefPage::OnSetdefaultcolor() 
{
	m_crDef = m_btDefColor.GetColor();
}

void CPreferencesTaskDefPage::OnUseparentattrib() 
{
	UpdateData();

	GetDlgItem(IDC_DEFAULTATTRIBUTES)->EnableWindow(m_bUseParentAttributes);
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

void CPreferencesTaskDefPage::SaveComboList(const CAutoComboBox& combo, LPCTSTR szRegKey) const
{
	CStringArray aItems;
	int nCount = combo.GetItems(aItems);

	// items
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey;
		sItemKey.Format("Item%d", nItem);
		AfxGetApp()->WriteProfileString(szRegKey, sItemKey, aItems[nItem]);
	}

	// item count
	AfxGetApp()->WriteProfileInt(szRegKey, "ItemCount", nCount);
}

void CPreferencesTaskDefPage::RestoreComboList(CAutoComboBox& combo, LPCTSTR szRegKey)
{
	CStringArray aItems;

	if (GetComboListItems(szRegKey, aItems))
		combo.AddUniqueItems(aItems);
}

int CPreferencesTaskDefPage::GetCategoryList(CStringArray& aItems) const
{
	return GetComboListItems("Preferences\\CategoryList", aItems);
}

int CPreferencesTaskDefPage::GetStatusList(CStringArray& aItems) const
{
	return GetComboListItems("Preferences\\StatusList", aItems);
}

int CPreferencesTaskDefPage::GetAllocToList(CStringArray& aItems) const
{
	return GetComboListItems("Preferences\\AllocToList", aItems);
}

int CPreferencesTaskDefPage::GetAllocByList(CStringArray& aItems) const
{
	return GetComboListItems("Preferences\\AllocByList", aItems);
}

int CPreferencesTaskDefPage::GetComboListItems(LPCTSTR szRegKey, CStringArray& aItems) const
{
	aItems.RemoveAll();
	int nCount = AfxGetApp()->GetProfileInt(szRegKey, "ItemCount", 0);

	// items
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey, sItem;
		sItemKey.Format("Item%d", nItem);
		sItem = AfxGetApp()->GetProfileString(szRegKey, sItemKey);

		if (!sItem.IsEmpty())
			aItems.Add(sItem);
	}

	return aItems.GetSize();
}

HBRUSH CPreferencesTaskDefPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (pWnd->GetDlgCtrlID() == IDC_DEFLISTLABEL)
		pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
	
	return hbr;
}
