// PreferencesFilePage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesexportPage.h"

#include "..\shared\Misc.h"
#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT ID_RESETCHARSET = 1;
const UINT RESETBTN = 0xc4;

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage property page

IMPLEMENT_DYNCREATE(CPreferencesExportPage, CPreferencesPageBase)

CPreferencesExportPage::CPreferencesExportPage() : 
		CPreferencesPageBase(CPreferencesExportPage::IDD)
{
//	m_psp.dwFlags &= ~PSP_HASHELP;

	//{{AFX_DATA_INIT(CPreferencesExportPage)
	//}}AFX_DATA_INIT

	// add a 'reset' button to the charset field
	m_eCharset.InsertButton(0, ID_RESETCHARSET, RESETBTN, 
								CEnString(IDS_PEP_RESETCHARSET), 
								DEF_BTNWIDTH + 4, "Wingdings");
}

CPreferencesExportPage::~CPreferencesExportPage()
{
}

void CPreferencesExportPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesExportPage)
	DDX_Control(pDX, IDC_HTMLFONTSIZE, m_cbFontSize);
	DDX_Check(pDX, IDC_EXPORTPARENTTITLECOMMENTSONLY, m_bExportParentTitleCommentsOnly);
	DDX_Check(pDX, IDC_EXPORTSPACEFORNOTES, m_bExportSpaceForNotes);
	DDX_Text(pDX, IDC_CHARSET, m_sHtmlCharSet);
	DDX_Radio(pDX, IDC_TABTEXTINDENT, m_bUseSpaceIndents);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TABWIDTHS, m_cbIndent);
	DDX_Control(pDX, IDC_FONTLIST, m_cbFonts);
	DDX_Check(pDX, IDC_PREVIEWEXPORT, m_bPreviewExport);
	DDX_Check(pDX, IDC_EXPORTVISIBLEONLY, m_bExportVisibleOnly);
	DDX_Control(pDX, IDC_CHARSET, m_eCharset);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		m_sHtmlFont = m_cbFonts.GetSelectedFont();

		CString sSize;
		m_cbFontSize.GetLBText(m_cbFontSize.GetCurSel(), sSize);
		m_nHtmlFontSize = atoi(sSize);

		CString sIndent;
		m_cbIndent.GetLBText(m_cbIndent.GetCurSel(), sIndent);
		m_nTextIndent = atoi(sIndent);
	}
	else
	{
		m_cbFonts.SetSelectedFont(m_sHtmlFont);

		CString sSize;
		sSize.Format("%d", m_nHtmlFontSize);

		if (CB_ERR == m_cbFontSize.SelectString(-1, sSize))
		{
			m_nHtmlFontSize = 3;
			m_cbFontSize.SelectString(-1, "3");
		}

		m_cbFontSize.GetLBText(m_cbFontSize.GetCurSel(), sSize);
		m_nHtmlFontSize = atoi(sSize);

		CString sIndent;
		sIndent.Format("%d", m_nTextIndent);

		if (CB_ERR == m_cbIndent.SelectString(-1, sIndent))
		{
			m_nTextIndent = 2;
			m_cbIndent.SelectString(-1, "2");
		}
	}
}


BEGIN_MESSAGE_MAP(CPreferencesExportPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesExportPage)
	ON_BN_CLICKED(IDC_TABTEXTINDENT, OnChangeTextIndentType)
	ON_BN_CLICKED(IDC_SPACETEXTINDENT, OnChangeTextIndentType)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnEEBtnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage message handlers

BOOL CPreferencesExportPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();
	
//	m_mgrGroupLines.AddGroupLine(IDC_EXPORTGROUP, *this); 
	m_cbIndent.EnableWindow(m_bUseSpaceIndents);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesExportPage::OnChangeTextIndentType() 
{
	UpdateData();

	m_cbIndent.EnableWindow(m_bUseSpaceIndents);
}

CString CPreferencesExportPage::GetHtmlCharSet() const
{
	if (m_sHtmlCharSet.IsEmpty())
		return "Windows - " + Misc::GetDefCharset();

	// else
	return m_sHtmlCharSet;
}


void CPreferencesExportPage::LoadPreferences(const CPreferencesStorage& prefs)
{
	m_sHtmlFont = prefs.GetProfileString("Preferences", "HTMLFont", "Verdana");
	m_bPreviewExport = prefs.GetProfileInt("Preferences", "PreviewSaveAs", TRUE);
	m_nHtmlFontSize = prefs.GetProfileInt("Preferences", "HtmlFontSize", 2);
	m_nTextIndent = prefs.GetProfileInt("Preferences", "TextIndent", 2);
	m_bExportVisibleOnly = prefs.GetProfileInt("Preferences", "ExportVisibleOnly", FALSE);
	m_bExportParentTitleCommentsOnly = prefs.GetProfileInt("Preferences", "ExportParentTitleCommentsOnly", FALSE);
	m_bExportSpaceForNotes = prefs.GetProfileInt("Preferences", "ExportSpaceForNotes", FALSE);
	m_bUseSpaceIndents = prefs.GetProfileInt("Preferences", "UseSpaceIndents", TRUE);

	// default code page
	CString sDefCharset = "Windows-" + Misc::GetDefCharset();

	m_sHtmlCharSet = prefs.GetProfileString("Preferences", "HtmlCharSet", sDefCharset);

//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);
}

void CPreferencesExportPage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	prefs.WriteProfileString("Preferences", "HTMLFont", m_sHtmlFont);
	prefs.WriteProfileInt("Preferences", "HtmlFontSize", m_nHtmlFontSize);
	prefs.WriteProfileInt("Preferences", "PreviewSaveAs", m_bPreviewExport);
	prefs.WriteProfileInt("Preferences", "TextIndent", m_nTextIndent);
	prefs.WriteProfileInt("Preferences", "ExportVisibleOnly", m_bExportVisibleOnly);
	prefs.WriteProfileInt("Preferences", "ExportParentTitleCommentsOnly", m_bExportParentTitleCommentsOnly);
	prefs.WriteProfileInt("Preferences", "ExportSpaceForNotes", m_bExportSpaceForNotes);
	prefs.WriteProfileString("Preferences", "HtmlCharSet", m_sHtmlCharSet);
	prefs.WriteProfileInt("Preferences", "UseSpaceIndents", m_bUseSpaceIndents);
//	prefs.WriteProfileInt("Preferences", "", m_b);
}

LRESULT CPreferencesExportPage::OnEEBtnClick(WPARAM wp, LPARAM lp)
{
	if (wp == IDC_CHARSET && lp == ID_RESETCHARSET)
	{
		m_sHtmlCharSet = "Windows-" + Misc::GetDefCharset();
		UpdateData(FALSE);
	}

	return 0L;
}