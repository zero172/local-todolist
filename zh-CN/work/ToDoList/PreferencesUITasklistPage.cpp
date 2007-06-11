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
	CPropertyPage(CPreferencesUITasklistPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesUITasklistPage)
	m_bShowParentsAsFolders = FALSE;
	m_bDisplayFirstCommentLine = FALSE;
	m_nMaxInfoTipCommentsLength = 0;
	m_bLimitInfoTipCommentsLength = FALSE;
	m_bAutoFocusTasklist = FALSE;
	m_bShowSubtaskCompletion = FALSE;
	//}}AFX_DATA_INIT

	// column visibility
	int nIndex = sizeof(COLUMNS) / sizeof(TDLB_COLUMN);
	
	while (nIndex--)
	{
		TDLB_COLUMN nColumn = COLUMNS[nIndex];

		CString sKey;
		sKey.Format("Col%d", nColumn);

		BOOL bDefault = m_lbColumnVisibility.IsColumnVisible(nColumn);
		BOOL bVisible = AfxGetApp()->GetProfileInt("Preferences\\ColumnVisibility", sKey, bDefault);

		m_lbColumnVisibility.SetColumnVisible(nColumn, bVisible);
	}

	// prefs
	m_bShowInfoTips = AfxGetApp()->GetProfileInt("Preferences", "ShowInfoTips", TRUE);
	m_bShowComments = AfxGetApp()->GetProfileInt("Preferences", "ShowComments", TRUE);
	m_bDisplayFirstCommentLine = AfxGetApp()->GetProfileInt("Preferences", "DisplayFirstCommentLine", TRUE);
	m_bStrikethroughDone = AfxGetApp()->GetProfileInt("Preferences", "StrikethroughDone", TRUE);
	m_bShowPathInHeader = AfxGetApp()->GetProfileInt("Preferences", "ShowPathInHeader", TRUE);
	m_bFullRowSelection = AfxGetApp()->GetProfileInt("Preferences", "FullRowSelection", FALSE);
	m_bTreeCheckboxes = AfxGetApp()->GetProfileInt("Preferences", "TreeCheckboxes", TRUE);
	m_bUseISOForDates = AfxGetApp()->GetProfileInt("Preferences", "DisplayDatesInISO", FALSE);
	m_bShowWeekdayInDates = AfxGetApp()->GetProfileInt("Preferences", "ShowWeekdayInDates", FALSE);
	m_bShowParentsAsFolders = AfxGetApp()->GetProfileInt("Preferences", "ShowParentsAsFolders", FALSE);
	m_nMaxInfoTipCommentsLength = AfxGetApp()->GetProfileInt("Preferences", "MaxInfoTipCommentsLength", 100);
	m_bLimitInfoTipCommentsLength = AfxGetApp()->GetProfileInt("Preferences", "LimitInfoTipCommentsLength", FALSE);
	m_bHidePercentForDoneTasks = AfxGetApp()->GetProfileInt("Preferences", "HidePercentForDoneTasks", TRUE);
	m_bHideStartDueForDoneTasks = AfxGetApp()->GetProfileInt("Preferences", "HideStartDueForDoneTasks", TRUE);
	m_bHideZeroTimeCost = AfxGetApp()->GetProfileInt("Preferences", "HideZeroTimeEst", TRUE);
	m_bShowPercentAsProgressbar = AfxGetApp()->GetProfileInt("Preferences", "ShowPercentAsProgressbar", FALSE);
	m_bRoundTimeFractions = AfxGetApp()->GetProfileInt("Preferences", "RoundTimeFractions", FALSE);
	m_bShowNonFilesAsText = AfxGetApp()->GetProfileInt("Preferences", "ShowNonFilesAsText", FALSE);
	m_bUseHMSTimeFormat = AfxGetApp()->GetProfileInt("Preferences", "UseHMSTimeFormat", FALSE);
	m_bAutoFocusTasklist = AfxGetApp()->GetProfileInt("Preferences", "AutoFocusTasklist", FALSE);
	m_bShowSubtaskCompletion = AfxGetApp()->GetProfileInt("Preferences", "ShowSubtaskCompletion", FALSE);
}

CPreferencesUITasklistPage::~CPreferencesUITasklistPage()
{
}

void CPreferencesUITasklistPage::DoDataExchange(CDataExchange* pDX)
{ 
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesUITasklistPage)
	DDX_Check(pDX, IDC_USEISODATEFORMAT, m_bUseISOForDates);
	DDX_Check(pDX, IDC_SHOWWEEKDAYINDATES, m_bShowWeekdayInDates);
	DDX_Check(pDX, IDC_SHOWPARENTSASFOLDERS, m_bShowParentsAsFolders);
	DDX_Check(pDX, IDC_DISPLAYFIRSTCOMMENTLINE, m_bDisplayFirstCommentLine);
	DDX_Check(pDX, IDC_LIMITINFOTIPCOMMENTS, m_bLimitInfoTipCommentsLength);
	DDX_Check(pDX, IDC_AUTOFOCUSTASKLIST, m_bAutoFocusTasklist);
	DDX_Check(pDX, IDC_SHOWSUBTASKCOMPLETION, m_bShowSubtaskCompletion);
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


BEGIN_MESSAGE_MAP(CPreferencesUITasklistPage, CPropertyPage)
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
	CPropertyPage::OnInitDialog();

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

/*
BOOL CPreferencesUITasklistPage::GetShowColumn(TDLB_COLUMN nColumn) const
{ 
	return m_lbColumnVisibility.IsColumnVisible(nColumn);
}
*/

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
	int nIndex = sizeof(COLUMNS) / sizeof(TDLB_COLUMN);
	
	while (nIndex--)
	{
		TDLB_COLUMN nColumn = COLUMNS[nIndex];

		CString sKey;
		sKey.Format("Col%d", nColumn);

		AfxGetApp()->WriteProfileInt("Preferences\\ColumnVisibility", sKey, 
									m_lbColumnVisibility.IsColumnVisible(nColumn));
	}
}

void CPreferencesUITasklistPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// column visibility
	SaveColumns();

	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "ShowInfoTips", m_bShowInfoTips);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowComments", m_bShowComments);
	AfxGetApp()->WriteProfileInt("Preferences", "DisplayFirstCommentLine", m_bDisplayFirstCommentLine);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowPercentColumn", m_bShowPercentColumn);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowPriorityColumn", m_bShowPriorityColumn);
	AfxGetApp()->WriteProfileInt("Preferences", "StrikethroughDone", m_bStrikethroughDone);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowPathInHeader", m_bShowPathInHeader);
	AfxGetApp()->WriteProfileInt("Preferences", "FullRowSelection", m_bFullRowSelection);
	AfxGetApp()->WriteProfileInt("Preferences", "TreeCheckboxes", m_bTreeCheckboxes);
	AfxGetApp()->WriteProfileInt("Preferences", "DisplayDatesInISO", m_bUseISOForDates);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowWeekdayInDates", m_bShowWeekdayInDates);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowParentsAsFolders", m_bShowParentsAsFolders);
	AfxGetApp()->WriteProfileInt("Preferences", "MaxInfoTipCommentsLength", max(m_nMaxInfoTipCommentsLength, 0));
	AfxGetApp()->WriteProfileInt("Preferences", "LimitInfoTipCommentsLength", m_bLimitInfoTipCommentsLength);
	AfxGetApp()->WriteProfileInt("Preferences", "HidePercentForDoneTasks", m_bHidePercentForDoneTasks);
	AfxGetApp()->WriteProfileInt("Preferences", "HideStartDueForDoneTasks", m_bHideStartDueForDoneTasks);
	AfxGetApp()->WriteProfileInt("Preferences", "HideZeroTimeEst", m_bHideZeroTimeCost);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowPercentAsProgressbar", m_bShowPercentAsProgressbar);
	AfxGetApp()->WriteProfileInt("Preferences", "RoundTimeFractions", m_bRoundTimeFractions);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowNonFilesAsText", m_bShowNonFilesAsText);
	AfxGetApp()->WriteProfileInt("Preferences", "UseHMSTimeFormat", m_bUseHMSTimeFormat);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoFocusTasklist", m_bAutoFocusTasklist);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowSubtaskCompletion", m_bShowSubtaskCompletion);
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

