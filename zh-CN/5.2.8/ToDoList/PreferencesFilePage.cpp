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

IMPLEMENT_DYNCREATE(CPreferencesFilePage, CPropertyPage)

CPreferencesFilePage::CPreferencesFilePage() : 
		CPropertyPage(CPreferencesFilePage::IDD),
		m_eExportFolderPath(FES_FOLDERS | FES_COMBOSTYLEBTN),
		m_eSaveExportStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER)),
		m_eDueTaskStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER))
{
//	m_psp.dwFlags &= ~PSP_HASHELP;

	//{{AFX_DATA_INIT(CPreferencesFilePage)
	m_bExpandTasks = FALSE;
	m_bCheckForChangesBeforeSaving = FALSE;
	//}}AFX_DATA_INIT
	m_bNotifyDueOnLoad = AfxGetApp()->GetProfileInt("Preferences", "NotifyDue", FALSE);
	m_bNotifyDueOnSwitch = AfxGetApp()->GetProfileInt("Preferences", "NotifyDueOnSwitch", FALSE);
	m_bAutoArchive = AfxGetApp()->GetProfileInt("Preferences", "AutoArchive", FALSE);
	m_bNotifyReadOnly = AfxGetApp()->GetProfileInt("Preferences", "NotifyReadOnly", TRUE);
	m_bRemoveArchivedTasks = AfxGetApp()->GetProfileInt("Preferences", "RemoveArchivedTasks", TRUE);
	m_bRemoveOnlyOnAbsoluteCompletion = AfxGetApp()->GetProfileInt("Preferences", "RemoveOnlyOnAbsoluteCompletion", TRUE);
	m_nAutoSaveFrequency = AfxGetApp()->GetProfileInt("Preferences", "AutoSaveFrequency", 0);
	m_bAutoSave = (m_nAutoSaveFrequency > 0);
	m_bAutoHtmlExport = AfxGetApp()->GetProfileInt("Preferences", "AutoHtmlExport", FALSE);
	m_sExportFolderPath = AfxGetApp()->GetProfileString("Preferences", "ExportFolderPath", "");
	m_nNotifyDueByOnLoad = AfxGetApp()->GetProfileInt("Preferences", "NotifyDueBy", PFP_DUETODAY);
	m_nNotifyDueByOnSwitch = AfxGetApp()->GetProfileInt("Preferences", "NotifyDueByOnSwitch", PFP_DUETODAY);
	m_bDisplayDueTasksInHtml = AfxGetApp()->GetProfileInt("Preferences", "DisplayDueTasksInHtml", TRUE);
	m_bRefreshFindOnLoad = AfxGetApp()->GetProfileInt("Preferences", "RefreshFindOnLoad", FALSE);
	m_bDueTaskTitlesOnly = AfxGetApp()->GetProfileInt("Preferences", "DueTaskTitlesOnly", FALSE);
	m_sDueTasksStylesheet = AfxGetApp()->GetProfileString("Preferences", "DueTasksStylesheet", FALSE);
	m_sSaveExportStylesheet = AfxGetApp()->GetProfileString("Preferences", "SaveExportStylesheet");
	m_sDueTaskPerson = AfxGetApp()->GetProfileString("Preferences", "DueTaskPerson");
	m_bOnlyShowDueTasksForPerson = !m_sDueTaskPerson.IsEmpty();
	m_bWarnAddDeleteArchive = AfxGetApp()->GetProfileInt("Preferences", "WarnAddDeleteArchive", TRUE);
	m_bDontAutoSaveUnsaved = AfxGetApp()->GetProfileInt("Preferences", "DontAutoSaveUnsaved", FALSE);
	m_bDontRemoveFlagged = AfxGetApp()->GetProfileInt("Preferences", "DontRemoveFlagged", FALSE);
	m_bExpandTasks = AfxGetApp()->GetProfileInt("Preferences", "ExpandTasks", FALSE);
	m_bCheckForChangesBeforeSaving = AfxGetApp()->GetProfileInt("Preferences", "CheckForChangesBeforeSaving", TRUE);
//	m_b = AfxGetApp()->GetProfileInt("Preferences", "", FALSE);

	m_sExportFolderPath.TrimLeft();
	m_sExportFolderPath.TrimRight();

	m_bExportToFolder = !m_sExportFolderPath.IsEmpty();
	m_bUseStylesheetForSaveExport = !m_sSaveExportStylesheet.IsEmpty();
	m_bUseStyleSheetForDueTasks = !m_sDueTasksStylesheet.IsEmpty();
}

CPreferencesFilePage::~CPreferencesFilePage()
{
}

void CPreferencesFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
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
	DDX_Check(pDX, IDC_DONTAUTOSAVEUNSAVED, m_bDontAutoSaveUnsaved);
	DDX_Check(pDX, IDC_DONTREMOVEFLAGGED, m_bDontRemoveFlagged);
	DDX_Check(pDX, IDC_EXPANDTASKS, m_bExpandTasks);
	DDX_Check(pDX, IDC_CHECKFORCHANGESBEFOESAVING, m_bCheckForChangesBeforeSaving);
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


BEGIN_MESSAGE_MAP(CPreferencesFilePage, CPropertyPage)
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
	CPropertyPage::OnInitDialog();

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
	GetDlgItem(IDC_DONTAUTOSAVEUNSAVED)->EnableWindow(m_bAutoSave);
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

void CPreferencesFilePage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "NotifyDue", m_bNotifyDueOnLoad);
	AfxGetApp()->WriteProfileInt("Preferences", "NotifyDueOnSwitch", m_bNotifyDueOnSwitch);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoArchive", m_bAutoArchive);
	AfxGetApp()->WriteProfileInt("Preferences", "NotifyReadOnly", m_bNotifyReadOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "RemoveArchivedTasks", m_bRemoveArchivedTasks);
	AfxGetApp()->WriteProfileInt("Preferences", "RemoveOnlyOnAbsoluteCompletion", m_bRemoveOnlyOnAbsoluteCompletion);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoHtmlExport", m_bAutoHtmlExport);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoSaveFrequency", m_nAutoSaveFrequency);
	AfxGetApp()->WriteProfileString("Preferences", "ExportFolderPath", (m_bAutoHtmlExport && m_bExportToFolder) ? m_sExportFolderPath : "");
	AfxGetApp()->WriteProfileInt("Preferences", "NotifyDueBy", m_nNotifyDueByOnLoad);
	AfxGetApp()->WriteProfileInt("Preferences", "NotifyDueByOnSwitch", m_nNotifyDueByOnSwitch);
	AfxGetApp()->WriteProfileInt("Preferences", "DisplayDueTasksInHtml", m_bDisplayDueTasksInHtml);
	AfxGetApp()->WriteProfileInt("Preferences", "RefreshFindOnLoad", m_bRefreshFindOnLoad);
	AfxGetApp()->WriteProfileInt("Preferences", "DueTaskTitlesOnly", m_bDueTaskTitlesOnly);
	AfxGetApp()->WriteProfileString("Preferences", "DueTasksStylesheet", m_bUseStyleSheetForDueTasks ? m_sDueTasksStylesheet : "");
	AfxGetApp()->WriteProfileString("Preferences", "SaveExportStylesheet", m_bUseStylesheetForSaveExport ? m_sSaveExportStylesheet : "");
	AfxGetApp()->WriteProfileString("Preferences", "DueTaskPerson", m_bOnlyShowDueTasksForPerson ? m_sDueTaskPerson : "");
	AfxGetApp()->WriteProfileInt("Preferences", "WarnAddDeleteArchive", m_bWarnAddDeleteArchive);
	AfxGetApp()->WriteProfileInt("Preferences", "DontAutoSaveUnsaved", m_bDontAutoSaveUnsaved);
	AfxGetApp()->WriteProfileInt("Preferences", "DontRemoveFlagged", m_bDontRemoveFlagged);
	AfxGetApp()->WriteProfileInt("Preferences", "ExpandTasks", m_bExpandTasks);
	AfxGetApp()->WriteProfileInt("Preferences", "CheckForChangesBeforeSaving", m_bCheckForChangesBeforeSaving);

//	AfxGetApp()->WriteProfileInt("Preferences", "", m_b);
}

void CPreferencesFilePage::OnAutosave() 
{
	UpdateData();

	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);
	GetDlgItem(IDC_DONTAUTOSAVEUNSAVED)->EnableWindow(m_bAutoSave);

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
