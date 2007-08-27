// PreferencesUITasklistPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesUITasklistPage.h"

#include "..\shared\colordef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TDLB_COLUMN COLUMNS[] = 
{
	TDLBC_PRIORITY, TDLBC_PERCENT, TDLBC_TIMEEST, TDLBC_STARTDATE, TDLBC_DUEDATE,
	TDLBC_DONEDATE, TDLBC_ALLOCTO, TDLBC_FILEREF, TDLBC_POSITION, TDLBC_ID, TDLBC_DONE, 
	TDLBC_ALLOCBY, TDLBC_STATUS, TDLBC_CATEGORY, TDLBC_TIMESPENT, TDLBC_TRACKTIME, 
	TDLBC_FLAG, TDLBC_CREATIONDATE, TDLBC_CREATEDBY, TDLBC_MODIFYDATE, TDLBC_RISK, 
	TDLBC_EXTERNALID, TDLBC_COST, TDLBC_DEPENDS, TDLBC_RECURRENCE, TDLBC_VERSION,
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistPage property page

CPreferencesUITasklistPage::CPreferencesUITasklistPage() : 
	CPreferencesPageBase(CPreferencesUITasklistPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesUITasklistPage)
	//}}AFX_DATA_INIT

}

CPreferencesUITasklistPage::~CPreferencesUITasklistPage()
{
}

void CPreferencesUITasklistPage::DoDataExchange(CDataExchange* pDX)
{ 
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesUITasklistPage)
	DDX_Check(pDX, IDC_USEISODATEFORMAT, m_bUseISOForDates);
	DDX_Check(pDX, IDC_SHOWWEEKDAYINDATES, m_bShowWeekdayInDates);
	DDX_Check(pDX, IDC_SHOWPARENTSASFOLDERS, m_bShowParentsAsFolders);
	DDX_Check(pDX, IDC_DISPLAYFIRSTCOMMENTLINE, m_bDisplayFirstCommentLine);
	DDX_Check(pDX, IDC_LIMITINFOTIPCOMMENTS, m_bLimitInfoTipCommentsLength);
	DDX_Check(pDX, IDC_AUTOFOCUSTASKLIST, m_bAutoFocusTasklist);
	DDX_Check(pDX, IDC_SHOWSUBTASKCOMPLETION, m_bShowSubtaskCompletion);
	DDX_Check(pDX, IDC_RIGHTSIDECOLUMNS, m_bShowColumnsOnRight);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DISPLAYPATHINHEADER, m_bShowPathInHeader);
	DDX_Check(pDX, IDC_STRIKETHROUGHDONE, m_bStrikethroughDone);
	DDX_Check(pDX, IDC_FULLROWSELECTION, m_bFullRowSelection);
	DDX_Check(pDX, IDC_TREECHECKBOXES, m_bTreeCheckboxes);
	DDX_Control(pDX, IDC_COLUMNVISIBILITY, m_lbColumnVisibility);
	DDX_Check(pDX, IDC_SHOWINFOTIPS, m_bShowInfoTips);
	DDX_Check(pDX, IDC_SHOWCOMMENTS, m_bShowComments);
	DDX_LBIndex(pDX, IDC_COLUMNVISIBILITY, m_nSelColumnVisibility);
	DDX_Text(pDX, IDC_INFOTIPCOMMENTSMAX, m_nMaxInfoTipCommentsLength);
	DDX_Check(pDX, IDC_HIDEUNDEFINEDTIMEST, m_bHideZeroTimeCost);
	DDX_Check(pDX, IDC_HIDESTARTDUEFORDONETASKS, m_bHideStartDueForDoneTasks);
	DDX_Check(pDX, IDC_ROUNDTIMEFRACTIONS, m_bRoundTimeFractions);
	DDX_Check(pDX, IDC_SHOWNONFILEREFSASTEXT, m_bShowNonFilesAsText);
	DDX_Check(pDX, IDC_USEHMSTIMEFORMAT, m_bUseHMSTimeFormat);
	DDX_Check(pDX, IDC_SHOWPERCENTPROGRESSBAR, m_bShowPercentAsProgressbar);
	DDX_Check(pDX, IDC_HIDEPERCENTFORDONETASKS, m_bHidePercentForDoneTasks);
}


BEGIN_MESSAGE_MAP(CPreferencesUITasklistPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesUITasklistPage)
	ON_BN_CLICKED(IDC_SHOWCOMMENTS, OnShowcomments)
	ON_BN_CLICKED(IDC_SHOWINFOTIPS, OnShowinfotips)
	ON_BN_CLICKED(IDC_LIMITINFOTIPCOMMENTS, OnLimitinfotipcomments)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistPage message handlers

BOOL CPreferencesUITasklistPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_COLUMNGROUP, *this);

	GetDlgItem(IDC_DISPLAYFIRSTCOMMENTLINE)->EnableWindow(m_bShowComments);
	GetDlgItem(IDC_LIMITINFOTIPCOMMENTS)->EnableWindow(m_bShowInfoTips);
	GetDlgItem(IDC_INFOTIPCOMMENTSMAX)->EnableWindow(m_bShowInfoTips && m_bLimitInfoTipCommentsLength);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CPreferencesUITasklistPage::GetMaxInfoTipCommentsLength() const
{
	if (m_bShowInfoTips)
		return m_bLimitInfoTipCommentsLength ? max(0, m_nMaxInfoTipCommentsLength) : -1;

	return -1;
}

void CPreferencesUITasklistPage::OnShowcomments() 
{
	UpdateData();

	GetDlgItem(IDC_DISPLAYFIRSTCOMMENTLINE)->EnableWindow(m_bShowComments);
}

void CPreferencesUITasklistPage::OnShowinfotips() 
{
	UpdateData();

	GetDlgItem(IDC_LIMITINFOTIPCOMMENTS)->EnableWindow(m_bShowInfoTips);
	GetDlgItem(IDC_INFOTIPCOMMENTSMAX)->EnableWindow(m_bShowInfoTips && m_bLimitInfoTipCommentsLength);
}

void CPreferencesUITasklistPage::OnLimitinfotipcomments() 
{
	UpdateData();

	GetDlgItem(IDC_INFOTIPCOMMENTSMAX)->EnableWindow(m_bShowInfoTips && m_bLimitInfoTipCommentsLength);
}

int CPreferencesUITasklistPage::GetVisibleColumns(CTDCColumnArray& aColumns) const
{
	return m_lbColumnVisibility.GetVisibleColumns(aColumns);
}

void CPreferencesUITasklistPage::SetVisibleColumns(const CTDCColumnArray& aColumns) 
{
	m_lbColumnVisibility.SetVisibleColumns(aColumns);
	SaveColumns();
}

void CPreferencesUITasklistPage::SaveColumns() const
{
	CPreferencesStorage prefs;
	int nIndex = sizeof(COLUMNS) / sizeof(TDLB_COLUMN);
	
	while (nIndex--)
	{
		TDLB_COLUMN nColumn = COLUMNS[nIndex];

		CString sKey;
		sKey.Format("Col%d", nColumn);

		prefs.WriteProfileInt("Preferences\\ColumnVisibility", sKey, 
									m_lbColumnVisibility.IsColumnVisible(nColumn));
	}
}

void CPreferencesUITasklistPage::LoadPreferences(const CPreferencesStorage& prefs)
{
	// column visibility
	int nIndex = sizeof(COLUMNS) / sizeof(TDLB_COLUMN);
	
	while (nIndex--)
	{
		TDLB_COLUMN nColumn = COLUMNS[nIndex];

		CString sKey;
		sKey.Format("Col%d", nColumn);

		BOOL bDefault = m_lbColumnVisibility.IsColumnVisible(nColumn);
		BOOL bVisible = prefs.GetProfileInt("Preferences\\ColumnVisibility", sKey, bDefault);

		m_lbColumnVisibility.SetColumnVisible(nColumn, bVisible);
	}

	// prefs
	m_bShowInfoTips = prefs.GetProfileInt("Preferences", "ShowInfoTips", TRUE);
	m_bShowComments = prefs.GetProfileInt("Preferences", "ShowComments", TRUE);
	m_bDisplayFirstCommentLine = prefs.GetProfileInt("Preferences", "DisplayFirstCommentLine", TRUE);
	m_bStrikethroughDone = prefs.GetProfileInt("Preferences", "StrikethroughDone", TRUE);
	m_bShowPathInHeader = prefs.GetProfileInt("Preferences", "ShowPathInHeader", TRUE);
	m_bFullRowSelection = prefs.GetProfileInt("Preferences", "FullRowSelection", FALSE);
	m_bTreeCheckboxes = prefs.GetProfileInt("Preferences", "TreeCheckboxes", TRUE);
	m_bUseISOForDates = prefs.GetProfileInt("Preferences", "DisplayDatesInISO", FALSE);
	m_bShowWeekdayInDates = prefs.GetProfileInt("Preferences", "ShowWeekdayInDates", FALSE);
	m_bShowParentsAsFolders = prefs.GetProfileInt("Preferences", "ShowParentsAsFolders", FALSE);
	m_nMaxInfoTipCommentsLength = prefs.GetProfileInt("Preferences", "MaxInfoTipCommentsLength", 100);
	m_bLimitInfoTipCommentsLength = prefs.GetProfileInt("Preferences", "LimitInfoTipCommentsLength", FALSE);
	m_bHidePercentForDoneTasks = prefs.GetProfileInt("Preferences", "HidePercentForDoneTasks", TRUE);
	m_bHideStartDueForDoneTasks = prefs.GetProfileInt("Preferences", "HideStartDueForDoneTasks", TRUE);
	m_bHideZeroTimeCost = prefs.GetProfileInt("Preferences", "HideZeroTimeEst", TRUE);
	m_bShowPercentAsProgressbar = prefs.GetProfileInt("Preferences", "ShowPercentAsProgressbar", FALSE);
	m_bRoundTimeFractions = prefs.GetProfileInt("Preferences", "RoundTimeFractions", FALSE);
	m_bShowNonFilesAsText = prefs.GetProfileInt("Preferences", "ShowNonFilesAsText", FALSE);
	m_bUseHMSTimeFormat = prefs.GetProfileInt("Preferences", "UseHMSTimeFormat", FALSE);
	m_bAutoFocusTasklist = prefs.GetProfileInt("Preferences", "AutoFocusTasklist", FALSE);
	m_bShowSubtaskCompletion = prefs.GetProfileInt("Preferences", "ShowSubtaskCompletion", FALSE);
	m_bShowColumnsOnRight = prefs.GetProfileInt("Preferences", "ShowColumnsOnRight", FALSE);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);
}

void CPreferencesUITasklistPage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	// column visibility
	int nIndex = sizeof(COLUMNS) / sizeof(TDLB_COLUMN);
	
	while (nIndex--)
	{
		TDLB_COLUMN nColumn = COLUMNS[nIndex];

		CString sKey;
		sKey.Format("Col%d", nColumn);

		prefs.WriteProfileInt("Preferences\\ColumnVisibility", sKey, 
									m_lbColumnVisibility.IsColumnVisible(nColumn));
	}

	// save settings
	prefs.WriteProfileInt("Preferences", "ShowInfoTips", m_bShowInfoTips);
	prefs.WriteProfileInt("Preferences", "ShowComments", m_bShowComments);
	prefs.WriteProfileInt("Preferences", "DisplayFirstCommentLine", m_bDisplayFirstCommentLine);
	prefs.WriteProfileInt("Preferences", "ShowPercentColumn", m_bShowPercentColumn);
	prefs.WriteProfileInt("Preferences", "ShowPriorityColumn", m_bShowPriorityColumn);
	prefs.WriteProfileInt("Preferences", "StrikethroughDone", m_bStrikethroughDone);
	prefs.WriteProfileInt("Preferences", "ShowPathInHeader", m_bShowPathInHeader);
	prefs.WriteProfileInt("Preferences", "FullRowSelection", m_bFullRowSelection);
	prefs.WriteProfileInt("Preferences", "TreeCheckboxes", m_bTreeCheckboxes);
	prefs.WriteProfileInt("Preferences", "DisplayDatesInISO", m_bUseISOForDates);
	prefs.WriteProfileInt("Preferences", "ShowWeekdayInDates", m_bShowWeekdayInDates);
	prefs.WriteProfileInt("Preferences", "ShowParentsAsFolders", m_bShowParentsAsFolders);
	prefs.WriteProfileInt("Preferences", "MaxInfoTipCommentsLength", max(m_nMaxInfoTipCommentsLength, 0));
	prefs.WriteProfileInt("Preferences", "LimitInfoTipCommentsLength", m_bLimitInfoTipCommentsLength);
	prefs.WriteProfileInt("Preferences", "HidePercentForDoneTasks", m_bHidePercentForDoneTasks);
	prefs.WriteProfileInt("Preferences", "HideStartDueForDoneTasks", m_bHideStartDueForDoneTasks);
	prefs.WriteProfileInt("Preferences", "HideZeroTimeEst", m_bHideZeroTimeCost);
	prefs.WriteProfileInt("Preferences", "ShowPercentAsProgressbar", m_bShowPercentAsProgressbar);
	prefs.WriteProfileInt("Preferences", "RoundTimeFractions", m_bRoundTimeFractions);
	prefs.WriteProfileInt("Preferences", "ShowNonFilesAsText", m_bShowNonFilesAsText);
	prefs.WriteProfileInt("Preferences", "UseHMSTimeFormat", m_bUseHMSTimeFormat);
	prefs.WriteProfileInt("Preferences", "AutoFocusTasklist", m_bAutoFocusTasklist);
	prefs.WriteProfileInt("Preferences", "ShowSubtaskCompletion", m_bShowSubtaskCompletion);
	prefs.WriteProfileInt("Preferences", "ShowColumnsOnRight", m_bShowColumnsOnRight);
//	prefs.WriteProfileInt("Preferences", "", m_b);
}


