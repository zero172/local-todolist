// PreferencesUIPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesUIPage.h"

#include "..\shared\misc.h"
#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage property page

IMPLEMENT_DYNCREATE(CPreferencesUIPage, CPropertyPage)

CPreferencesUIPage::CPreferencesUIPage(const CContentMgr* pMgr) : 
	CPropertyPage(CPreferencesUIPage::IDD), m_pContentMgr(pMgr), m_bNotifyCommentsFormatChange(FALSE) 
{
	//{{AFX_DATA_INIT(CPreferencesUIPage)
	m_bMultiSelCategoryFilter = FALSE;
	m_bAutoReFilter = FALSE;
	m_bRestoreTasklistFilters = FALSE;
	m_bMultiSelAllocToFilter = FALSE;
	//}}AFX_DATA_INIT

	// load settings
	m_bShowCtrlsAsColumns = AfxGetApp()->GetProfileInt("Preferences", "ShowCtrlsAsColumns", FALSE);
	m_bShowEditMenuAsColumns = AfxGetApp()->GetProfileInt("Preferences", "ShowEditMenuAsColumns", FALSE);
	m_bShowCommentsAlways = AfxGetApp()->GetProfileInt("Preferences", "ShowCommentsAlways", FALSE);
	m_bAutoReposCtrls = AfxGetApp()->GetProfileInt("Preferences", "AutoReposCtrls", TRUE);
	m_bSpecifyToolbarImage = AfxGetApp()->GetProfileInt("Preferences", "SpecifyToolbarImage", FALSE);
	m_bSharedCommentsHeight = AfxGetApp()->GetProfileInt("Preferences", "SharedCommentsHeight", TRUE);
	m_bAutoHideTabbar = AfxGetApp()->GetProfileInt("Preferences", "AutoHideTabbar", TRUE);
	m_bStackTabbarItems = AfxGetApp()->GetProfileInt("Preferences", "StackTabbarItems", FALSE);
	m_bRightAlignLabels = AfxGetApp()->GetProfileInt("Preferences", "RightAlignLabels", TRUE);
	m_bFocusTreeOnEnter = AfxGetApp()->GetProfileInt("Preferences", "FocusTreeOnEnter", FALSE);
	m_bLargeToolbarIcons = AfxGetApp()->GetProfileInt("Preferences", "LargeToolbarIcons", TRUE);
	m_nNewTaskPos = AfxGetApp()->GetProfileInt("Preferences", "NewTaskPos", PUIP_BELOW);
	m_nNewSubtaskPos = AfxGetApp()->GetProfileInt("Preferences", "NewSubtaskPos", PUIP_BOTTOM);
	m_bKeepTabsOrdered = AfxGetApp()->GetProfileInt("Preferences", "KeepTabsOrdered", FALSE);
	m_bShowTasklistCloseButton = AfxGetApp()->GetProfileInt("Preferences", "ShowTasklistCloseButton", TRUE);
	m_bEnableCtrlMBtnClose = AfxGetApp()->GetProfileInt("Preferences", "EnableCtrlMBtnClose", TRUE);
	m_bEnableHeaderSorting = AfxGetApp()->GetProfileInt("Preferences", "EnableHeaderSorting", TRUE);
	m_bAutoReSort = AfxGetApp()->GetProfileInt("Preferences", "AutoReSort", FALSE);
	m_bSortVisibleOnly = AfxGetApp()->GetProfileInt("Preferences", "SortVisibleOnly", FALSE);
	m_bSortDoneTasksAtBottom = AfxGetApp()->GetProfileInt("Preferences", "SortDoneTasksAtBottom", TRUE);
	m_bRTLComments = AfxGetApp()->GetProfileInt("Preferences", "RTLComments", FALSE);
	m_nCommentsPos = AfxGetApp()->GetProfileInt("Preferences", "VertComments", PUIP_RIGHTCOMMENTS);
	m_bMultiSelCategoryFilter = AfxGetApp()->GetProfileInt("Preferences", "MultiSelCategoryFilter", TRUE);
	m_bAutoReFilter = AfxGetApp()->GetProfileInt("Preferences", "AutoReFilter", FALSE);
	m_bRestoreTasklistFilters = AfxGetApp()->GetProfileInt("Preferences", "RestoreTasklistFilters", FALSE);
	m_bMultiSelAllocToFilter = AfxGetApp()->GetProfileInt("Preferences", "MultiSelAllocToFilter", TRUE);
//	m_b = AfxGetApp()->GetProfileInt("Preferences", "", FALSE);

	// comments format
	if (m_pContentMgr)
	{
		// get the index via the guid if there is one
		GUID idFormat;
		CString sDefaultCommentsFormatID = AfxGetApp()->GetProfileString("Preferences", "DefaultCommentsFormatID");

		if (Misc::GuidFromString(sDefaultCommentsFormatID, idFormat))
			m_nDefaultCommentsFormat = m_pContentMgr->FindContent(idFormat);
		else
			m_nDefaultCommentsFormat = AfxGetApp()->GetProfileInt("Preferences", "DefaultCommentsFormat", 1);

		if (m_nDefaultCommentsFormat < 0 || m_nDefaultCommentsFormat >= m_pContentMgr->GetNumContent())
			m_nDefaultCommentsFormat = 0;
	}
}

CPreferencesUIPage::~CPreferencesUIPage()
{
}

void CPreferencesUIPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesUIPage)
	DDX_Check(pDX, IDC_SHOWCTRLSASCOLUMNS, m_bShowCtrlsAsColumns);
	DDX_Check(pDX, IDC_SHOWCOMMENTSALWAYS, m_bShowCommentsAlways);
	DDX_Check(pDX, IDC_AUTOREPOSCTRLS, m_bAutoReposCtrls);
	DDX_Check(pDX, IDC_SHAREDCOMMENTSHEIGHT, m_bSharedCommentsHeight);
	DDX_Check(pDX, IDC_AUTOHIDETABBAR, m_bAutoHideTabbar);
	DDX_Check(pDX, IDC_STACKTABBARITEMS, m_bStackTabbarItems);
	DDX_Check(pDX, IDC_RIGHTALIGNLABELS, m_bRightAlignLabels);
	DDX_Check(pDX, IDC_FOCUSTREEONENTER, m_bFocusTreeOnEnter);
	DDX_Check(pDX, IDC_LARGETOOLBARICONS, m_bLargeToolbarIcons);
	DDX_CBIndex(pDX, IDC_NEWTASKPOSITION, m_nNewTaskPos);
	DDX_CBIndex(pDX, IDC_NEWSUBTASKPOSITION, m_nNewSubtaskPos);
	DDX_Check(pDX, IDC_KEEPTABSORDERED, m_bKeepTabsOrdered);
	DDX_Check(pDX, IDC_SHOWTASKLISTCLOSEBTN, m_bShowTasklistCloseButton);
	DDX_Check(pDX, IDC_ENABLECTRLMBTNCLOSE, m_bEnableCtrlMBtnClose);
	DDX_Check(pDX, IDC_SHOWEDITMENUASCOLUMNS, m_bShowEditMenuAsColumns);
	DDX_Check(pDX, IDC_MULTISELCATFILTER, m_bMultiSelCategoryFilter);
	DDX_Check(pDX, IDC_AUTOREFILTER, m_bAutoReFilter);
	DDX_Check(pDX, IDC_RESTORETASKLISTFILTERS, m_bRestoreTasklistFilters);
	DDX_CBIndex(pDX, IDC_COMMENTPOS, m_nCommentsPos);
	DDX_Check(pDX, IDC_MULTISELALLOCTOFILTER, m_bMultiSelAllocToFilter);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMMENTSFORMAT, m_cbCommentsFmt);
	DDX_CBIndex(pDX, IDC_COMMENTSFORMAT, m_nDefaultCommentsFormat);
	DDX_Check(pDX, IDC_AUTORESORT, m_bAutoReSort);
	DDX_Check(pDX, IDC_SORTVISIBLEONLY, m_bSortVisibleOnly);
	DDX_Check(pDX, IDC_SORTDONETASKSATBOTTOM, m_bSortDoneTasksAtBottom);
	DDX_Check(pDX, IDC_ENABLEHEADERSORTING, m_bEnableHeaderSorting);
//	DDX_Check(pDX, IDC_RTLCOMMENTS, m_bRTLComments);
}

BEGIN_MESSAGE_MAP(CPreferencesUIPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesUIPage)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMMENTSFORMAT, OnSelchangeCommentsformat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage message handlers

BOOL CPreferencesUIPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_TOOLBARGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SORTGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_TABBARGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_COMMENTSGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_FILTERGROUP, *this);

	// content types
	if (m_pContentMgr)
	{
		for (int nItem = 0; nItem < m_pContentMgr->GetNumContent(); nItem++)
		{
			CString sItem = m_pContentMgr->GetContentTypeDescription(nItem);
			m_cbCommentsFmt.AddString(sItem);
		}

		m_cbCommentsFmt.SetCurSel(m_nDefaultCommentsFormat);
	}
	
	GetDlgItem(IDC_COMMENTSFORMAT)->EnableWindow(m_pContentMgr != NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesUIPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "ShowCtrlsAsColumns", m_bShowCtrlsAsColumns);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowEditMenuAsColumns", m_bShowEditMenuAsColumns);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowCommentsAlways", m_bShowCommentsAlways);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoReposCtrls", m_bAutoReposCtrls);
	AfxGetApp()->WriteProfileInt("Preferences", "SpecifyToolbarImage", m_bSpecifyToolbarImage);
	AfxGetApp()->WriteProfileInt("Preferences", "SharedCommentsHeight", m_bSharedCommentsHeight);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoHideTabbar", m_bAutoHideTabbar);
	AfxGetApp()->WriteProfileInt("Preferences", "StackTabbarItems", m_bStackTabbarItems);
	AfxGetApp()->WriteProfileInt("Preferences", "RightAlignLabels", m_bRightAlignLabels);
	AfxGetApp()->WriteProfileInt("Preferences", "FocusTreeOnEnter", m_bFocusTreeOnEnter);
	AfxGetApp()->WriteProfileInt("Preferences", "LargeToolbarIcons", m_bLargeToolbarIcons);
	AfxGetApp()->WriteProfileInt("Preferences", "NewTaskPos", m_nNewTaskPos);
	AfxGetApp()->WriteProfileInt("Preferences", "NewSubtaskPos", m_nNewSubtaskPos);
	AfxGetApp()->WriteProfileInt("Preferences", "KeepTabsOrdered", m_bKeepTabsOrdered);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowTasklistCloseButton", m_bShowTasklistCloseButton);
	AfxGetApp()->WriteProfileInt("Preferences", "EnableCtrlMBtnClose", m_bEnableCtrlMBtnClose);
	AfxGetApp()->WriteProfileInt("Preferences", "EnableHeaderSorting", m_bEnableHeaderSorting);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoReSort", m_bAutoReSort);
	AfxGetApp()->WriteProfileInt("Preferences", "SortVisibleOnly", m_bSortVisibleOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "SortDoneTasksAtBottom", m_bSortDoneTasksAtBottom);
	AfxGetApp()->WriteProfileInt("Preferences", "RTLComments", m_bRTLComments);
	AfxGetApp()->WriteProfileInt("Preferences", "VertComments", m_nCommentsPos);
	AfxGetApp()->WriteProfileInt("Preferences", "MultiSelCategoryFilter", m_bMultiSelCategoryFilter);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoReFilter", m_bAutoReFilter);
	AfxGetApp()->WriteProfileInt("Preferences", "RestoreTasklistFilters", m_bRestoreTasklistFilters);
	AfxGetApp()->WriteProfileInt("Preferences", "MultiSelAllocToFilter", m_bMultiSelAllocToFilter);
//	AfxGetApp()->WriteProfileInt("Preferences", "", m_b);

	// comments format
	if (m_pContentMgr)
	{
		AfxGetApp()->WriteProfileInt("Preferences", "DefaultCommentsFormat", m_nDefaultCommentsFormat);
		AfxGetApp()->WriteProfileString("Preferences", "DefaultCommentsFormatID", ""); // clear

		GUID idFormat;

		if (m_pContentMgr->GetContentTypeID(m_nDefaultCommentsFormat, idFormat))
		{
			CString sIDFormat;

			if (!Misc::GuidIsNull(idFormat) && Misc::GuidToString(idFormat, sIDFormat))
				AfxGetApp()->WriteProfileString("Preferences", "DefaultCommentsFormatID", sIDFormat);
		}
	}
}

void CPreferencesUIPage::OnSelchangeCommentsformat() 
{
	// notify once only
	if (!m_bNotifyCommentsFormatChange)
		MessageBox(CEnString(IDS_COMMENTFORMATCHANGE), CEnString(IDS_COMMENTFORMATCHANGE_TITLE));

	m_bNotifyCommentsFormatChange = TRUE;
}
