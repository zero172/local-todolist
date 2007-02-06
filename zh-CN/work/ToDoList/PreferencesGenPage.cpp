// PreferencesGenPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesGenPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesGenPage property page

IMPLEMENT_DYNCREATE(CPreferencesGenPage, CPropertyPage)

CPreferencesGenPage::CPreferencesGenPage() : CPropertyPage(CPreferencesGenPage::IDD), m_hkGlobal(TRUE)
{
	//{{AFX_DATA_INIT(CPreferencesGenPage)
	m_bAddFilesToMRU = FALSE;
	m_bEnableTDLExtension = FALSE;
	m_bDontAutoSaveUnsaved = FALSE;
	m_bAutoCheckForUpdates = FALSE;
	m_bEscapeMinimizes = FALSE;
	//}}AFX_DATA_INIT

	// load settings
	m_bAlwaysOnTop = AfxGetApp()->GetProfileInt("Preferences", "AlwaysOnTop", FALSE);
	m_bUseSysTray = AfxGetApp()->GetProfileInt("Preferences", "UseSysTray", FALSE);
	m_bAutoSaveOnSysTray = AfxGetApp()->GetProfileInt("Preferences", "AutoSave", TRUE);
	m_bConfirmDelete = AfxGetApp()->GetProfileInt("Preferences", "ConfirmDelete", TRUE);
	m_bConfirmSaveOnExit = AfxGetApp()->GetProfileInt("Preferences", "ConfirmSaveOnExit", TRUE);
	m_bMultiInstance = AfxGetApp()->GetProfileInt("Preferences", "MultiInstance", FALSE);
	m_bShowOnStartup = AfxGetApp()->GetProfileInt("Preferences", "ShowOnStartup", TRUE);
	m_nSysTrayOption = AfxGetApp()->GetProfileInt("Preferences", "SysTrayOption", STO_ONCLOSE);
	m_bToggleTrayVisibility = AfxGetApp()->GetProfileInt("Preferences", "ToggleTrayVisibility", FALSE);
	m_dwGlobalHotkey = AfxGetApp()->GetProfileInt("Preferences", "GlobalHotkey", 0);
	m_bSpecifyGlobalHotkey = (m_dwGlobalHotkey ? 1 : 0);
	m_bAddFilesToMRU = AfxGetApp()->GetProfileInt("Preferences", "AddFilesToMRU", TRUE);
	m_bEnableTDLExtension = AfxGetApp()->GetProfileInt("Preferences", "EnableTDLExtension", TRUE);
	m_bDontAutoSaveUnsaved = AfxGetApp()->GetProfileInt("Preferences", "DontAutoSaveUnsaved", FALSE);
	m_bAutoCheckForUpdates = AfxGetApp()->GetProfileInt("Preferences", "AutoCheckForUpdates", FALSE);
	m_bEscapeMinimizes = AfxGetApp()->GetProfileInt("Preferences", "EscapeMinimizes", TRUE);
//	m_b = AfxGetApp()->GetProfileInt("Preferences", "", TRUE);
}

CPreferencesGenPage::~CPreferencesGenPage()
{
}

void CPreferencesGenPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesGenPage)
	DDX_Control(pDX, IDC_GLOBALHOTKEY, m_hkGlobal);
	DDX_Check(pDX, IDC_MULTIINSTANCE, m_bMultiInstance);
	DDX_Check(pDX, IDC_SHOWONSTARTUP, m_bShowOnStartup);
	DDX_CBIndex(pDX, IDC_SYSTRAYOPTION, m_nSysTrayOption);
	DDX_Check(pDX, IDC_TOGGLETRAYVISIBILITY, m_bToggleTrayVisibility);
	DDX_Check(pDX, IDC_SPECIFYGLOBALHOTKEY, m_bSpecifyGlobalHotkey);
	DDX_Check(pDX, IDC_ADDFILESTOMRU, m_bAddFilesToMRU);
	DDX_Check(pDX, IDC_ENABLETDLEXTENSION, m_bEnableTDLExtension);
	DDX_Check(pDX, IDC_DONTAUTOSAVEUNSAVED, m_bDontAutoSaveUnsaved);
	DDX_Check(pDX, IDC_CHECKFORUPDATES, m_bAutoCheckForUpdates);
	DDX_Check(pDX, IDC_ESCAPEMINIMIZES, m_bEscapeMinimizes);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_ALWAYSONTOP, m_bAlwaysOnTop);
	DDX_Check(pDX, IDC_USESYSTRAY, m_bUseSysTray);
	DDX_Check(pDX, IDC_AUTOSAVEONSYSTRAY, m_bAutoSaveOnSysTray);
	DDX_Check(pDX, IDC_CONFIRMDELETE, m_bConfirmDelete);
	DDX_Check(pDX, IDC_CONFIRMSAVEONEXIT, m_bConfirmSaveOnExit);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		WORD wVKeyCode = 0, wModifiers = 0;
		m_hkGlobal.GetHotKey(wVKeyCode, wModifiers);
		m_dwGlobalHotkey = MAKELONG(wVKeyCode, wModifiers);
	}
	else if (m_bSpecifyGlobalHotkey)
		m_hkGlobal.SetHotKey(m_dwGlobalHotkey);
}

BEGIN_MESSAGE_MAP(CPreferencesGenPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesGenPage)
	ON_BN_CLICKED(IDC_SPECIFYGLOBALHOTKEY, OnSpecifyglobalhotkey)
	ON_BN_CLICKED(IDC_MULTIINSTANCE, OnMultiinstance)
	ON_BN_CLICKED(IDC_CLEARMRU, OnClearMRU)
	ON_BN_CLICKED(IDC_AUTOSAVEONSYSTRAY, OnAutosaveonsystray)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USESYSTRAY, OnUseSystray)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesGenPage message handlers

BOOL CPreferencesGenPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_AUTOSAVEONSYSTRAY)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_DONTAUTOSAVEUNSAVED)->EnableWindow(m_bUseSysTray && m_bAutoSaveOnSysTray);
	GetDlgItem(IDC_SYSTRAYOPTION)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_TOGGLETRAYVISIBILITY)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_CONFIRMSAVEONEXIT)->EnableWindow(!m_bUseSysTray);
	GetDlgItem(IDC_MULTIINSTANCE)->EnableWindow(!m_bSpecifyGlobalHotkey);
	GetDlgItem(IDC_SPECIFYGLOBALHOTKEY)->EnableWindow(!m_bMultiInstance);
	GetDlgItem(IDC_GLOBALHOTKEY)->EnableWindow(!m_bMultiInstance && m_bSpecifyGlobalHotkey);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesGenPage::OnUseSystray() 
{
	UpdateData();

	GetDlgItem(IDC_AUTOSAVEONSYSTRAY)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_DONTAUTOSAVEUNSAVED)->EnableWindow(m_bUseSysTray && m_bAutoSaveOnSysTray);
	GetDlgItem(IDC_SYSTRAYOPTION)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_TOGGLETRAYVISIBILITY)->EnableWindow(m_bUseSysTray);

	GetDlgItem(IDC_CONFIRMSAVEONEXIT)->EnableWindow(!m_bUseSysTray); // mutually exclusive
}

void CPreferencesGenPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "AlwaysOnTop", m_bAlwaysOnTop);
	AfxGetApp()->WriteProfileInt("Preferences", "UseSysTray", m_bUseSysTray);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoSave", m_bAutoSaveOnSysTray);
	AfxGetApp()->WriteProfileInt("Preferences", "ConfirmDelete", m_bConfirmDelete);
	AfxGetApp()->WriteProfileInt("Preferences", "ConfirmSaveOnExit", m_bConfirmSaveOnExit);
	AfxGetApp()->WriteProfileInt("Preferences", "ShowOnStartup", m_bShowOnStartup);
	AfxGetApp()->WriteProfileInt("Preferences", "SysTrayOption", m_nSysTrayOption);
	AfxGetApp()->WriteProfileInt("Preferences", "ToggleTrayVisibility", m_bToggleTrayVisibility);
	AfxGetApp()->WriteProfileInt("Preferences", "MultiInstance", m_bMultiInstance);
	AfxGetApp()->WriteProfileInt("Preferences", "GlobalHotkey", (m_bSpecifyGlobalHotkey ? m_dwGlobalHotkey : 0));
	AfxGetApp()->WriteProfileInt("Preferences", "AddFilesToMRU", m_bAddFilesToMRU);
	AfxGetApp()->WriteProfileInt("Preferences", "EnableTDLExtension", m_bEnableTDLExtension);
	AfxGetApp()->WriteProfileInt("Preferences", "DontAutoSaveUnsaved", m_bDontAutoSaveUnsaved);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoCheckForUpdates", m_bAutoCheckForUpdates);
	AfxGetApp()->WriteProfileInt("Preferences", "EscapeMinimizes", m_bEscapeMinimizes);
//	AfxGetApp()->WriteProfileInt("Preferences", "", m_b);
}


void CPreferencesGenPage::OnSpecifyglobalhotkey() 
{
	UpdateData();
	
	GetDlgItem(IDC_GLOBALHOTKEY)->EnableWindow(m_bSpecifyGlobalHotkey);
	GetDlgItem(IDC_MULTIINSTANCE)->EnableWindow(!m_bSpecifyGlobalHotkey);
}

void CPreferencesGenPage::OnMultiinstance() 
{
	UpdateData();
	
	GetDlgItem(IDC_SPECIFYGLOBALHOTKEY)->EnableWindow(!m_bMultiInstance);
	GetDlgItem(IDC_GLOBALHOTKEY)->EnableWindow(!m_bMultiInstance && m_bSpecifyGlobalHotkey);
}

void CPreferencesGenPage::OnClearMRU() 
{
   AfxGetMainWnd()->SendMessage(WM_PGP_CLEARMRU);
}

void CPreferencesGenPage::OnAutosaveonsystray() 
{
	UpdateData();
	
	GetDlgItem(IDC_DONTAUTOSAVEUNSAVED)->EnableWindow(m_bUseSysTray && m_bAutoSaveOnSysTray);
}
