// PreferencesFilePage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesexportPage.h"

#include "..\shared\Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage property page

IMPLEMENT_DYNCREATE(CPreferencesExportPage, CPropertyPage)

CPreferencesExportPage::CPreferencesExportPage() : 
		CPropertyPage(CPreferencesExportPage::IDD)
{
//	m_psp.dwFlags &= ~PSP_HASHELP;

	//{{AFX_DATA_INIT(CPreferencesExportPage)
	m_bExportParentTitleCommentsOnly = FALSE;
	m_bExportSpaceForNotes = FALSE;
	//}}AFX_DATA_INIT
	m_sHtmlFont = AfxGetApp()->GetProfileString("Preferences", "HTMLFont", "Verdana");
	m_bPreviewExport = AfxGetApp()->GetProfileInt("Preferences", "PreviewSaveAs", TRUE);
	m_nHtmlFontSize = AfxGetApp()->GetProfileInt("Preferences", "HtmlFontSize", 2);
	m_nTextIndent = AfxGetApp()->GetProfileInt("Preferences", "TextIndent", 2);
	m_bExportVisibleOnly = AfxGetApp()->GetProfileInt("Preferences", "ExportVisibleOnly", FALSE);
	m_bExportParentTitleCommentsOnly = AfxGetApp()->GetProfileInt("Preferences", "ExportParentTitleCommentsOnly", FALSE);
	m_bExportSpaceForNotes = AfxGetApp()->GetProfileInt("Preferences", "ExportSpaceForNotes", FALSE);
	m_bUseSpaceIndents = AfxGetApp()->GetProfileInt("Preferences", "UseSpaceIndents", TRUE);

	// default code page
	CString sDefCharset = "Windows-" + Misc::GetDefCharset();

	m_sHtmlCharSet = AfxGetApp()->GetProfileString("Preferences", "HtmlCharSet", sDefCharset);

//	m_b = AfxGetApp()->GetProfileInt("Preferences", "", FALSE);
}

CPreferencesExportPage::~CPreferencesExportPage()
{
}

void CPreferencesExportPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
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


BEGIN_MESSAGE_MAP(CPreferencesExportPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesExportPage)
	ON_BN_CLICKED(IDC_TABTEXTINDENT, OnChangeTextIndentType)
	ON_BN_CLICKED(IDC_SPACETEXTINDENT, OnChangeTextIndentType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage message handlers

void CPreferencesExportPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileString("Preferences", "HTMLFont", m_sHtmlFont);
	AfxGetApp()->WriteProfileInt("Preferences", "HtmlFontSize", m_nHtmlFontSize);
	AfxGetApp()->WriteProfileInt("Preferences", "PreviewSaveAs", m_bPreviewExport);
	AfxGetApp()->WriteProfileInt("Preferences", "TextIndent", m_nTextIndent);
	AfxGetApp()->WriteProfileInt("Preferences", "ExportVisibleOnly", m_bExportVisibleOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "ExportParentTitleCommentsOnly", m_bExportParentTitleCommentsOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "ExportSpaceForNotes", m_bExportSpaceForNotes);
	AfxGetApp()->WriteProfileString("Preferences", "HtmlCharSet", m_sHtmlCharSet);
	AfxGetApp()->WriteProfileInt("Preferences", "UseSpaceIndents", m_bUseSpaceIndents);
//	AfxGetApp()->WriteProfileInt("Preferences", "", m_b);
}

BOOL CPreferencesExportPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
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
