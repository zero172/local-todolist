// PreferencesUIPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesUIPage.h"
#include "tdcenum.h"

#include "..\shared\misc.h"
#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage property page

IMPLEMENT_DYNCREATE(CPreferencesUIPage, CPreferencesPageBase)

CPreferencesUIPage::CPreferencesUIPage(const CContentMgr* pMgr) : 
	CPreferencesPageBase(CPreferencesUIPage::IDD), 
		m_pContentMgr(pMgr), 
		m_cbCommentsFmt(pMgr),
		m_nDefaultCommentsFormat(-1)
{
	//{{AFX_DATA_INIT(CPreferencesUIPage)
	m_bMultiSelCategoryFilter = FALSE;
	m_bAutoReFilter = FALSE;
	m_bRestoreTasklistFilters = FALSE;
	//}}AFX_DATA_INIT

}

CPreferencesUIPage::~CPreferencesUIPage()
{
}

void CPreferencesUIPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
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

BEGIN_MESSAGE_MAP(CPreferencesUIPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesUIPage)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMMENTSFORMAT, OnSelchangeCommentsformat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage message handlers

BOOL CPreferencesUIPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_TOOLBARGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SORTGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_TABBARGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_COMMENTSGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_FILTERGROUP, *this);

	m_cbCommentsFmt.SetCurSel(m_nDefaultCommentsFormat);
	GetDlgItem(IDC_COMMENTSFORMAT)->EnableWindow(m_nDefaultCommentsFormat != CB_ERR);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesUIPage::OnSelchangeCommentsformat() 
{
	m_cbCommentsFmt.GetSelectedFormat(m_cfDefault);
}

void CPreferencesUIPage::LoadPreferences(const CPreferencesStorage& prefs)
{
	// load settings
	m_bShowCtrlsAsColumns = prefs.GetProfileInt("Preferences", "ShowCtrlsAsColumns", FALSE);
	m_bShowEditMenuAsColumns = prefs.GetProfileInt("Preferences", "ShowEditMenuAsColumns", FALSE);
	m_bShowCommentsAlways = prefs.GetProfileInt("Preferences", "ShowCommentsAlways", FALSE);
	m_bAutoReposCtrls = prefs.GetProfileInt("Preferences", "AutoReposCtrls", TRUE);
	m_bSpecifyToolbarImage = prefs.GetProfileInt("Preferences", "SpecifyToolbarImage", FALSE);
	m_bSharedCommentsHeight = prefs.GetProfileInt("Preferences", "SharedCommentsHeight", TRUE);
	m_bAutoHideTabbar = prefs.GetProfileInt("Preferences", "AutoHideTabbar", TRUE);
	m_bStackTabbarItems = prefs.GetProfileInt("Preferences", "StackTabbarItems", FALSE);
	m_bRightAlignLabels = prefs.GetProfileInt("Preferences", "RightAlignLabels", TRUE);
	m_bFocusTreeOnEnter = prefs.GetProfileInt("Preferences", "FocusTreeOnEnter", FALSE);
	m_bLargeToolbarIcons = prefs.GetProfileInt("Preferences", "LargeToolbarIcons", TRUE);
	m_nNewTaskPos = prefs.GetProfileInt("Preferences", "NewTaskPos", PUIP_BELOW);
	m_nNewSubtaskPos = prefs.GetProfileInt("Preferences", "NewSubtaskPos", PUIP_BOTTOM);
	m_bKeepTabsOrdered = prefs.GetProfileInt("Preferences", "KeepTabsOrdered", FALSE);
	m_bShowTasklistCloseButton = prefs.GetProfileInt("Preferences", "ShowTasklistCloseButton", TRUE);
	m_bEnableCtrlMBtnClose = prefs.GetProfileInt("Preferences", "EnableCtrlMBtnClose", TRUE);
	m_bEnableHeaderSorting = prefs.GetProfileInt("Preferences", "EnableHeaderSorting", TRUE);
	m_bAutoReSort = prefs.GetProfileInt("Preferences", "AutoReSort", FALSE);
	m_bSortVisibleOnly = prefs.GetProfileInt("Preferences", "SortVisibleOnly", FALSE);
	m_bSortDoneTasksAtBottom = prefs.GetProfileInt("Preferences", "SortDoneTasksAtBottom", TRUE);
	m_bRTLComments = prefs.GetProfileInt("Preferences", "RTLComments", FALSE);
	m_nCommentsPos = prefs.GetProfileInt("Preferences", "VertComments", PUIP_RIGHTCOMMENTS);
	m_bMultiSelCategoryFilter = prefs.GetProfileInt("Preferences", "MultiSelCategoryFilter", TRUE);
	m_bAutoReFilter = prefs.GetProfileInt("Preferences", "AutoReFilter", FALSE);
	m_bRestoreTasklistFilters = prefs.GetProfileInt("Preferences", "RestoreTasklistFilters", FALSE);
	m_bMultiSelAllocToFilter = prefs.GetProfileInt("Preferences", "MultiSelAllocToFilter", TRUE);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);

	// comments format
	if (m_cbCommentsFmt.IsInitialized())
	{
		m_cfDefault = prefs.GetProfileString("Preferences", "DefaultCommentsFormatID");
		m_nDefaultCommentsFormat = m_cbCommentsFmt.SetSelectedFormat(m_cfDefault);

		// fallback
		if (m_nDefaultCommentsFormat == CB_ERR)
			m_nDefaultCommentsFormat = prefs.GetProfileInt("Preferences", "DefaultCommentsFormat", -1);

		if (m_nDefaultCommentsFormat == CB_ERR || m_nDefaultCommentsFormat >= m_cbCommentsFmt.GetCount())
		{
			ASSERT (m_cbCommentsFmt.GetCount());

			m_nDefaultCommentsFormat = 0;
		}

		m_cbCommentsFmt.SetCurSel(m_nDefaultCommentsFormat);
		m_cbCommentsFmt.GetSelectedFormat(m_cfDefault);
	}
}

void CPreferencesUIPage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "ShowCtrlsAsColumns", m_bShowCtrlsAsColumns);
	prefs.WriteProfileInt("Preferences", "ShowEditMenuAsColumns", m_bShowEditMenuAsColumns);
	prefs.WriteProfileInt("Preferences", "ShowCommentsAlways", m_bShowCommentsAlways);
	prefs.WriteProfileInt("Preferences", "AutoReposCtrls", m_bAutoReposCtrls);
	prefs.WriteProfileInt("Preferences", "SpecifyToolbarImage", m_bSpecifyToolbarImage);
	prefs.WriteProfileInt("Preferences", "SharedCommentsHeight", m_bSharedCommentsHeight);
	prefs.WriteProfileInt("Preferences", "AutoHideTabbar", m_bAutoHideTabbar);
	prefs.WriteProfileInt("Preferences", "StackTabbarItems", m_bStackTabbarItems);
	prefs.WriteProfileInt("Preferences", "RightAlignLabels", m_bRightAlignLabels);
	prefs.WriteProfileInt("Preferences", "FocusTreeOnEnter", m_bFocusTreeOnEnter);
	prefs.WriteProfileInt("Preferences", "LargeToolbarIcons", m_bLargeToolbarIcons);
	prefs.WriteProfileInt("Preferences", "NewTaskPos", m_nNewTaskPos);
	prefs.WriteProfileInt("Preferences", "NewSubtaskPos", m_nNewSubtaskPos);
	prefs.WriteProfileInt("Preferences", "KeepTabsOrdered", m_bKeepTabsOrdered);
	prefs.WriteProfileInt("Preferences", "ShowTasklistCloseButton", m_bShowTasklistCloseButton);
	prefs.WriteProfileInt("Preferences", "EnableCtrlMBtnClose", m_bEnableCtrlMBtnClose);
	prefs.WriteProfileInt("Preferences", "EnableHeaderSorting", m_bEnableHeaderSorting);
	prefs.WriteProfileInt("Preferences", "AutoReSort", m_bAutoReSort);
	prefs.WriteProfileInt("Preferences", "SortVisibleOnly", m_bSortVisibleOnly);
	prefs.WriteProfileInt("Preferences", "SortDoneTasksAtBottom", m_bSortDoneTasksAtBottom);
	prefs.WriteProfileInt("Preferences", "RTLComments", m_bRTLComments);
	prefs.WriteProfileInt("Preferences", "VertComments", m_nCommentsPos);
	prefs.WriteProfileInt("Preferences", "MultiSelCategoryFilter", m_bMultiSelCategoryFilter);
	prefs.WriteProfileInt("Preferences", "AutoReFilter", m_bAutoReFilter);
	prefs.WriteProfileInt("Preferences", "RestoreTasklistFilters", m_bRestoreTasklistFilters);
	prefs.WriteProfileInt("Preferences", "MultiSelAllocToFilter", m_bMultiSelAllocToFilter);
//	prefs.WriteProfileInt("Preferences", "", m_b);

	// comments format
	if (m_pContentMgr)
	{
		prefs.WriteProfileInt("Preferences", "DefaultCommentsFormat", m_nDefaultCommentsFormat);
		prefs.WriteProfileString("Preferences", "DefaultCommentsFormatID", m_cfDefault);
	}
}

DWORD CPreferencesUIPage::GetMultiSelFilterFlags() const
{
	DWORD dwFlags = 0;

	if (m_bMultiSelAllocToFilter)
		dwFlags |= FB_MULTISELALLOCTO;

	if (m_bMultiSelCategoryFilter)
		dwFlags |= FB_MULTISELCAT;

	return dwFlags;
}