// PreferencesFilePage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesFilePage.h"

#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFilePage property page

IMPLEMENT_DYNCREATE(CPreferencesFilePage, CPreferencesPageBase)

CPreferencesFilePage::CPreferencesFilePage() : 
		CPreferencesPageBase(CPreferencesFilePage::IDD),
		m_eExportFolderPath(FES_FOLDERS | FES_COMBOSTYLEBTN),
		m_eSaveExportStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER)),
		m_eDueTaskStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER))
{
//	m_psp.dwFlags &= ~PSP_HASHELP;

	//{{AFX_DATA_INIT(CPreferencesFilePage)
	m_bAutoSaveOnSwitchTasklist = FALSE;
	m_bAutoSaveOnSwitchApp = FALSE;
	//}}AFX_DATA_INIT
}

CPreferencesFilePage::~CPreferencesFilePage()
{
}

void CPreferencesFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesFilePage)
	DDX_Control(pDX, IDC_SAVEEXPORTSTYLESHEET, m_eSaveExportStylesheet);
	DDX_Control(pDX, IDC_DUETASKSTYLESHEET, m_eDueTaskStylesheet);
	DDX_Control(pDX, IDC_EXPORTFOLDER, m_eExportFolderPath);
	DDX_Check(pDX, IDC_EXPORTTOFOLDER, m_bExportToFolder);
	DDX_Text(pDX, IDC_EXPORTFOLDER, m_sExportFolderPath);
	DDX_Check(pDX, IDC_DISPLAYDUETASKSINHTML, m_bDisplayDueTasksInHtml);
	DDX_Check(pDX, IDC_REFRESHFINDTASKS, m_bRefreshFindOnLoad);
	DDX_Check(pDX, IDC_DISPLAYDUETASKTITLESONLY, m_bDueTaskTitlesOnly);
	DDX_Text(pDX, IDC_SAVEEXPORTSTYLESHEET, m_sSaveExportStylesheet);
	DDX_Text(pDX, IDC_DUETASKSTYLESHEET, m_sDueTasksStylesheet);
	DDX_Check(pDX, IDC_USESTYLESHEETFORSAVE, m_bUseStylesheetForSaveExport);
	DDX_Check(pDX, IDC_USESTYLESHEETFORDUEITEMS, m_bUseStyleSheetForDueTasks);
	DDX_Check(pDX, IDC_ONLYSHOWDUETASKFORPERSON, m_bOnlyShowDueTasksForPerson);
	DDX_Text(pDX, IDC_DUETASKPERSON, m_sDueTaskPerson);
	DDX_Check(pDX, IDC_WARNADDDELARCHIVE, m_bWarnAddDeleteArchive);
	DDX_Check(pDX, IDC_DONTREMOVEFLAGGED, m_bDontRemoveFlagged);
	DDX_Check(pDX, IDC_EXPANDTASKS, m_bExpandTasks);
	DDX_Check(pDX, IDC_CHECKFORCHANGESBEFOESAVING, m_bCheckForChangesBeforeSaving);
	DDX_Check(pDX, IDC_AUTOSAVEONSWITCHTASKLIST, m_bAutoSaveOnSwitchTasklist);
	DDX_Check(pDX, IDC_AUTOSAVEONSWITCHAPP, m_bAutoSaveOnSwitchApp);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_NOTIFYDUEBYONLOAD, m_nNotifyDueByOnLoad);
	DDX_CBIndex(pDX, IDC_NOTIFYDUEBYONSWITCH, m_nNotifyDueByOnSwitch);
	DDX_Check(pDX, IDC_NOTIFYDUEONLOAD, m_bNotifyDueOnLoad);
	DDX_Check(pDX, IDC_NOTIFYDUEONSWITCH, m_bNotifyDueOnSwitch);
	DDX_Check(pDX, IDC_NOTIFYREADONLY, m_bNotifyReadOnly);
	DDX_Check(pDX, IDC_AUTOARCHIVE, m_bAutoArchive);
	DDX_Control(pDX, IDC_AUTOSAVEFREQUENCY, m_cbAutoSave);
	DDX_Check(pDX, IDC_AUTOHTMLEXPORT, m_bAutoHtmlExport);
	DDX_Check(pDX, IDC_REMOVEARCHIVEDITEMS, m_bRemoveArchivedTasks);
	DDX_Check(pDX, IDC_REMOVEONLYONABSCOMPLETION, m_bRemoveOnlyOnAbsoluteCompletion);
	DDX_Check(pDX, IDC_AUTOSAVE, m_bAutoSave);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		if (m_bAutoSave)
		{
			CString sFreq;
			m_cbAutoSave.GetLBText(m_cbAutoSave.GetCurSel(), sFreq);
			m_nAutoSaveFrequency = atoi(sFreq);
		}
		else
			m_nAutoSaveFrequency = 0;
	}
	else
	{
		if (m_bAutoSave)
		{
			CString sFreq;
			sFreq.Format("%d", m_nAutoSaveFrequency);

			if (CB_ERR == m_cbAutoSave.SelectString(-1, sFreq))
			{
				m_nAutoSaveFrequency = 5;
				m_cbAutoSave.SelectString(-1, "5");
			}
		}
		else
			m_cbAutoSave.SetCurSel(2);
	}
}


BEGIN_MESSAGE_MAP(CPreferencesFilePage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesFilePage)
	ON_BN_CLICKED(IDC_EXPORTTOFOLDER, OnExporttofolder)
	ON_BN_CLICKED(IDC_AUTOHTMLEXPORT, OnAutohtmlexport)
	ON_BN_CLICKED(IDC_NOTIFYDUEONLOAD, OnNotifyDueOnLoad)
	ON_BN_CLICKED(IDC_DISPLAYDUETASKSINHTML, OnDisplayduetasksinhtml)
	ON_BN_CLICKED(IDC_USESTYLESHEETFORDUEITEMS, OnUsestylesheetfordueitems)
	ON_BN_CLICKED(IDC_USESTYLESHEETFORSAVE, OnUsestylesheetforsave)
	ON_BN_CLICKED(IDC_ONLYSHOWDUETASKFORPERSON, OnOnlyshowduetaskforperson)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NOTIFYDUEONSWITCH, OnNotifyDueOnSwitch)
	ON_BN_CLICKED(IDC_REMOVEARCHIVEDITEMS, OnRemovearchiveditems)
	ON_BN_CLICKED(IDC_AUTOSAVE, OnAutosave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFilePage message handlers

BOOL CPreferencesFilePage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_LOADGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_ARCHIVEGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SWITCHGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_DUEGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SAVEGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_PRINTGROUP, *this);

	m_eExportFolderPath.SetFolderPrompt(CEnString(IDS_PFP_SELECTFOLDER));
	
	GetDlgItem(IDC_REMOVEONLYONABSCOMPLETION)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_DONTREMOVEFLAGGED)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);
	GetDlgItem(IDC_EXPORTTOFOLDER)->EnableWindow(m_bAutoHtmlExport);
	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoHtmlExport && m_bExportToFolder);
	GetDlgItem(IDC_NOTIFYDUEBYONLOAD)->EnableWindow(m_bNotifyDueOnLoad);
	GetDlgItem(IDC_NOTIFYDUEBYONSWITCH)->EnableWindow(m_bNotifyDueOnSwitch);
	GetDlgItem(IDC_USESTYLESHEETFORDUEITEMS)->EnableWindow(m_bDisplayDueTasksInHtml);
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoHtmlExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoHtmlExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_DUETASKPERSON)->EnableWindow(m_bOnlyShowDueTasksForPerson);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesFilePage::OnRemovearchiveditems() 
{
	UpdateData();

	GetDlgItem(IDC_REMOVEONLYONABSCOMPLETION)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_DONTREMOVEFLAGGED)->EnableWindow(m_bRemoveArchivedTasks);
}

void CPreferencesFilePage::OnAutosave() 
{
	UpdateData();

	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);

	if (m_bAutoSave && !m_nAutoSaveFrequency)
	{
		m_nAutoSaveFrequency = 5;
		m_cbAutoSave.SetCurSel(2);
	}
}

void CPreferencesFilePage::OnExporttofolder() 
{
	UpdateData();

	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoHtmlExport && m_bExportToFolder);
}

void CPreferencesFilePage::OnAutohtmlexport() 
{
	UpdateData();	

	GetDlgItem(IDC_EXPORTTOFOLDER)->EnableWindow(m_bAutoHtmlExport);
	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoHtmlExport && m_bExportToFolder);
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoHtmlExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoHtmlExport && m_bUseStylesheetForSaveExport);
}

CString CPreferencesFilePage::GetAutoExportFolderPath() const 
{ 
	if (m_bAutoHtmlExport && m_bExportToFolder)
		return m_sExportFolderPath;
	else
		return "";
}

void CPreferencesFilePage::OnNotifyDueOnLoad() 
{
	UpdateData();
	
	GetDlgItem(IDC_NOTIFYDUEBYONLOAD)->EnableWindow(m_bNotifyDueOnLoad);
}

void CPreferencesFilePage::OnNotifyDueOnSwitch() 
{
	UpdateData();
	
	GetDlgItem(IDC_NOTIFYDUEBYONSWITCH)->EnableWindow(m_bNotifyDueOnSwitch);
}

void CPreferencesFilePage::OnDisplayduetasksinhtml() 
{
	UpdateData();
	
	GetDlgItem(IDC_USESTYLESHEETFORDUEITEMS)->EnableWindow(m_bDisplayDueTasksInHtml);
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
}

void CPreferencesFilePage::OnUsestylesheetfordueitems() 
{
	UpdateData();
	
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
}

void CPreferencesFilePage::OnUsestylesheetforsave() 
{
	UpdateData();
	
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoHtmlExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoHtmlExport && m_bUseStylesheetForSaveExport);
}

void CPreferencesFilePage::OnOnlyshowduetaskforperson() 
{
	UpdateData();
	
	GetDlgItem(IDC_DUETASKPERSON)->EnableWindow(m_bOnlyShowDueTasksForPerson);
}

void CPreferencesFilePage::LoadPreferences(const CPreferencesStorage& prefs)
{
	m_bNotifyDueOnLoad = prefs.GetProfileInt("Preferences", "NotifyDue", FALSE);
	m_bNotifyDueOnSwitch = prefs.GetProfileInt("Preferences", "NotifyDueOnSwitch", FALSE);
	m_bAutoArchive = prefs.GetProfileInt("Preferences", "AutoArchive", FALSE);
	m_bNotifyReadOnly = prefs.GetProfileInt("Preferences", "NotifyReadOnly", TRUE);
	m_bRemoveArchivedTasks = prefs.GetProfileInt("Preferences", "RemoveArchivedTasks", TRUE);
	m_bRemoveOnlyOnAbsoluteCompletion = prefs.GetProfileInt("Preferences", "RemoveOnlyOnAbsoluteCompletion", TRUE);
	m_nAutoSaveFrequency = prefs.GetProfileInt("Preferences", "AutoSaveFrequency", 0);
	m_bAutoSave = (m_nAutoSaveFrequency > 0);
	m_bAutoHtmlExport = prefs.GetProfileInt("Preferences", "AutoHtmlExport", FALSE);
	m_sExportFolderPath = prefs.GetProfileString("Preferences", "ExportFolderPath", "");
	m_nNotifyDueByOnLoad = prefs.GetProfileInt("Preferences", "NotifyDueBy", PFP_DUETODAY);
	m_nNotifyDueByOnSwitch = prefs.GetProfileInt("Preferences", "NotifyDueByOnSwitch", PFP_DUETODAY);
	m_bDisplayDueTasksInHtml = prefs.GetProfileInt("Preferences", "DisplayDueTasksInHtml", TRUE);
	m_bRefreshFindOnLoad = prefs.GetProfileInt("Preferences", "RefreshFindOnLoad", FALSE);
	m_bDueTaskTitlesOnly = prefs.GetProfileInt("Preferences", "DueTaskTitlesOnly", FALSE);
	m_sDueTasksStylesheet = prefs.GetProfileString("Preferences", "DueTasksStylesheet", FALSE);
	m_sSaveExportStylesheet = prefs.GetProfileString("Preferences", "SaveExportStylesheet");
	m_sDueTaskPerson = prefs.GetProfileString("Preferences", "DueTaskPerson");
	m_bOnlyShowDueTasksForPerson = !m_sDueTaskPerson.IsEmpty();
	m_bWarnAddDeleteArchive = prefs.GetProfileInt("Preferences", "WarnAddDeleteArchive", TRUE);
	m_bDontRemoveFlagged = prefs.GetProfileInt("Preferences", "DontRemoveFlagged", FALSE);
	m_bExpandTasks = prefs.GetProfileInt("Preferences", "ExpandTasks", FALSE);
	m_bCheckForChangesBeforeSaving = AfxGetApp()->GetProfileInt("Preferences", "CheckForChangesBeforeSaving", TRUE);
	m_bAutoSaveOnSwitchTasklist = prefs.GetProfileInt("Preferences", "AutoSaveOnSwitchTasklist", FALSE);
	m_bAutoSaveOnSwitchApp = prefs.GetProfileInt("Preferences", "AutoSaveOnSwitchApp", FALSE);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);

	m_sExportFolderPath.TrimLeft();
	m_sExportFolderPath.TrimRight();

	m_bExportToFolder = !m_sExportFolderPath.IsEmpty();
	m_bUseStylesheetForSaveExport = !m_sSaveExportStylesheet.IsEmpty();
	m_bUseStyleSheetForDueTasks = !m_sDueTasksStylesheet.IsEmpty();
}

void CPreferencesFilePage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "NotifyDue", m_bNotifyDueOnLoad);
	prefs.WriteProfileInt("Preferences", "NotifyDueOnSwitch", m_bNotifyDueOnSwitch);
	prefs.WriteProfileInt("Preferences", "AutoArchive", m_bAutoArchive);
	prefs.WriteProfileInt("Preferences", "NotifyReadOnly", m_bNotifyReadOnly);
	prefs.WriteProfileInt("Preferences", "RemoveArchivedTasks", m_bRemoveArchivedTasks);
	prefs.WriteProfileInt("Preferences", "RemoveOnlyOnAbsoluteCompletion", m_bRemoveOnlyOnAbsoluteCompletion);
	prefs.WriteProfileInt("Preferences", "AutoHtmlExport", m_bAutoHtmlExport);
	prefs.WriteProfileInt("Preferences", "AutoSaveFrequency", m_nAutoSaveFrequency);
	prefs.WriteProfileString("Preferences", "ExportFolderPath", (m_bAutoHtmlExport && m_bExportToFolder) ? m_sExportFolderPath : "");
	prefs.WriteProfileInt("Preferences", "NotifyDueBy", m_nNotifyDueByOnLoad);
	prefs.WriteProfileInt("Preferences", "NotifyDueByOnSwitch", m_nNotifyDueByOnSwitch);
	prefs.WriteProfileInt("Preferences", "DisplayDueTasksInHtml", m_bDisplayDueTasksInHtml);
	prefs.WriteProfileInt("Preferences", "RefreshFindOnLoad", m_bRefreshFindOnLoad);
	prefs.WriteProfileInt("Preferences", "DueTaskTitlesOnly", m_bDueTaskTitlesOnly);
	prefs.WriteProfileString("Preferences", "DueTasksStylesheet", m_bUseStyleSheetForDueTasks ? m_sDueTasksStylesheet : "");
	prefs.WriteProfileString("Preferences", "SaveExportStylesheet", m_bUseStylesheetForSaveExport ? m_sSaveExportStylesheet : "");
	prefs.WriteProfileString("Preferences", "DueTaskPerson", m_bOnlyShowDueTasksForPerson ? m_sDueTaskPerson : "");
	prefs.WriteProfileInt("Preferences", "WarnAddDeleteArchive", m_bWarnAddDeleteArchive);
	prefs.WriteProfileInt("Preferences", "DontRemoveFlagged", m_bDontRemoveFlagged);
	prefs.WriteProfileInt("Preferences", "ExpandTasks", m_bExpandTasks);
	prefs.WriteProfileInt("Preferences", "CheckForChangesBeforeSaving", m_bCheckForChangesBeforeSaving);
	prefs.WriteProfileInt("Preferences", "AutoSaveOnSwitchTasklist", m_bAutoSaveOnSwitchTasklist);
	prefs.WriteProfileInt("Preferences", "AutoSaveOnSwitchApp", m_bAutoSaveOnSwitchApp);

//	prefs.WriteProfileInt("Preferences", "", m_b);
}
