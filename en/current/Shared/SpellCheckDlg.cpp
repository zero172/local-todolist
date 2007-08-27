// SpellCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpellCheckDlg.h"
#include "ispellcheck.h"
#include "enfiledialog.h"
#include "stringres.h"
#include "filemisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpellCheckDlg dialog

enum // control IDs
{
	IDC_DICTIONARIES = 1001,
	IDC_BROWSE,
	IDC_SUGGESTIONS,
	IDC_TEXT,
	IDC_MISSPELTWORD,
	IDC_REPLACE,
	IDC_CONTINUE,
	IDC_RESTART,
	IDC_URL,
};

CSpellCheckDlg::CSpellCheckDlg(LPCTSTR szDictionaryPath, ISpellCheck* pSpellCheck, LPCTSTR szText, CWnd* /*pParent*/) : 
	m_pSpellChecker(NULL), 
	m_reSpellCheck(m_reText),
    m_sSelDictionary(szDictionaryPath), 
	m_stURL("http://wiki.services.openoffice.org/wiki/Dictionaries"),
	m_bMadeChanges(FALSE),
	m_ptTopLeft(-1, -1)
{
	InitDialog(pSpellCheck, szText);
}

void CSpellCheckDlg::InitDialog(ISpellCheck* pSpellCheck, LPCTSTR szText)
{
	AfxEnableControlContainer();
	AfxInitRichEdit();

	AddRCControl("LTEXT", "", SPELLCHECK_ACTIVEDICT, 0,0, 7,9,65,8, (UINT)IDC_STATIC);
	AddRCControl("COMBOBOX", "", "", CBS_DROPDOWNLIST | WS_TABSTOP, 0, 66, 8, 182,100, IDC_DICTIONARIES);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_BROWSE,WS_TABSTOP, 0,256,7,50,14,IDC_BROWSE);
	AddRCControl("LTEXT", "", SPELLCHECK_DOWNLOADMORE, WS_TABSTOP,0, 66,21,90,8,IDC_URL);
	AddRCControl("LTEXT", "", SPELLCHECK_CHECKING, 0,0, 7,30,49,8, (UINT)IDC_STATIC);
	AddRCControl("CONTROL", "RICHEDIT", "",ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL |/*WS_BORDER | */ ES_READONLY | WS_VSCROLL | WS_TABSTOP | WS_DISABLED,0, 7,40,242,68,IDC_TEXT);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_RESTART,WS_TABSTOP, 0,256,40,50,14,IDC_RESTART);
	AddRCControl("LTEXT", "", SPELLCHECK_REPLACE,0, 0,7,112,30,8, (UINT)IDC_STATIC);
	AddRCControl("LTEXT", "", "Static",0, 0,44,112,205,8,IDC_MISSPELTWORD);
	AddRCControl("LTEXT", "", SPELLCHECK_WITH,0, 0,7,124,18,8, (UINT)IDC_STATIC);
	AddRCControl("LISTBOX", "", "", LBS_SORT | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP | LBS_NOTIFY, 0,41,124,208,51, IDC_SUGGESTIONS);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_BTN_REPLACE,WS_TABSTOP, 0,256,124,50,14,IDC_REPLACE);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_NEXTWORD,WS_TABSTOP, 0,256,144,50,14,IDC_CONTINUE);
	AddRCControl("CONTROL", "static", "",SS_ETCHEDHORZ,0,7,182,299,1, (UINT)IDC_STATIC);

	if (!pSpellCheck)
	{
		AddRCControl("DEFPUSHBUTTON", "", BTN_OK, WS_TABSTOP, 0, 199,190,50,14,IDOK);
		AddRCControl("PUSHBUTTON", "", BTN_CANCEL, WS_TABSTOP, 0,256,190,50,14,IDCANCEL);

		SetText(szText);
	}
	else
	{
		AddRCControl("PUSHBUTTON", "", BTN_CLOSE, WS_TABSTOP, 0,256,190,50,14,IDCANCEL);

		SetSpellCheck(pSpellCheck);
	}

	// init spell checker dll path
#ifndef _DEBUG
	m_sEnginePath = AfxGetApp()->GetProfileString("SpellChecker", "EnginePath", "MySpellCheck.dll");
#else
	m_sEnginePath = "MySpellCheck.dll";
#endif

	if (!IsSpellCheckDll(m_sEnginePath))
	{
		CString sDllPath = FileMisc::GetModuleFileName(), sDrive, sFolder;

		FileMisc::SplitPath(sDllPath, &sDrive, &sFolder);
		FileMisc::MakePath(sDllPath, sDrive, sFolder, "*", ".dll");

		m_sEnginePath = sDllPath;
	}
}

CSpellCheckDlg::~CSpellCheckDlg()
{
	ReleaseSpellCheckerInterface(m_pSpellChecker);
}

void CSpellCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellCheckDlg)
	DDX_Control(pDX, IDC_SUGGESTIONS, m_lbSuggestions);
	DDX_Text(pDX, IDC_MISSPELTWORD, m_sMisspeltWord);
	DDX_LBString(pDX, IDC_SUGGESTIONS, m_sSuggestion);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_URL, m_stURL);
	DDX_Control(pDX, IDC_DICTIONARIES, m_cbDictionaries);

	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_Control(pDX, IDC_TEXT, m_reText);

	if (pDX->m_bSaveAndValidate)
	{
		int nCurSel = m_cbDictionaries.GetCurSel();

		if (nCurSel != CB_ERR)
			m_cbDictionaries.GetLBText(nCurSel, m_sSelDictionary);
		else
			m_sSelDictionary.Empty();
	}
	else
	{
		// make sure its in the list and move to top
		int nDic = m_cbDictionaries.FindStringExact(-1, m_sSelDictionary);

		if (nDic != CB_ERR)
			m_cbDictionaries.DeleteString(nDic);

		if (!m_sSelDictionary.IsEmpty())
		{
			m_cbDictionaries.InsertString(0, m_sSelDictionary);
			m_cbDictionaries.SetCurSel(0);
		}
	}
}

BEGIN_MESSAGE_MAP(CSpellCheckDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CSpellCheckDlg)
	ON_BN_CLICKED(IDC_REPLACE, OnReplace)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_LBN_SELCHANGE(IDC_SUGGESTIONS, OnSelchangeSuggestions)
	ON_BN_CLICKED(IDC_RESTART, OnRestart)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_DICTIONARIES, OnChangeDictionary)
	ON_LBN_DBLCLK(IDC_SUGGESTIONS, OnDblClkSuggestions)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellCheckDlg message handlers

int CSpellCheckDlg::DoModal(BOOL bEndOnNoErrors) 
{ 
	m_bEndOnNoErrors = bEndOnNoErrors;

	// turn off centering if we saved the last position
	DWORD dwFlags = RTD_DEFSTYLE;

	if (m_ptTopLeft.x != -1 || m_ptTopLeft.y != -1)
		dwFlags &= ~DS_CENTER;

	return CRuntimeDlg::DoModal(SPELLCHECK_TITLE, dwFlags); 
}

BOOL CSpellCheckDlg::InitDictionary(LPCTSTR szDicPath)
{
	CString sAffixPath(szDicPath);
	sAffixPath.MakeLower();
	sAffixPath.Replace(".dic", ".aff");

	ISpellChecker* pTemp = CreateSpellCheckerInterface(m_sEnginePath, sAffixPath, szDicPath);

	if (pTemp) // alls well
	{
		ReleaseSpellCheckerInterface(m_pSpellChecker);
		m_pSpellChecker = pTemp;
		pTemp = NULL;

		m_sSelDictionary = szDicPath;
		UpdateData(FALSE);

		// clear the current word and reset 
		HighlightWord(FALSE);
	}

	return (NULL != m_pSpellChecker);
}

void CSpellCheckDlg::SetText(LPCTSTR szText) 
{ 
	m_sText = szText; 
	m_pSpellCheck = &m_reSpellCheck;

	m_sMisspeltWord.Empty();
	m_sSuggestion.Empty();

	if (GetSafeHwnd())
	{
		m_reText.SetBackgroundColor(TRUE, 0);
		m_lbSuggestions.ResetContent();

		UpdateData(FALSE);
		StartChecking();
	}
}

void CSpellCheckDlg::SetSpellCheck(ISpellCheck* pSpellCheck) 
{ 
	m_sText.Empty(); 
	m_pSpellCheck = pSpellCheck;

	m_sMisspeltWord.Empty();
	m_sSuggestion.Empty();

	if (GetSafeHwnd())
	{
		m_reText.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
		m_lbSuggestions.ResetContent();

		UpdateData(FALSE);
		StartChecking();
	}
}

void CSpellCheckDlg::OnChangeDictionary()
{
	UpdateData();

	if (InitDictionary(m_sSelDictionary))
		StartChecking(CH_CURRENT);
}

void CSpellCheckDlg::OnBrowse()
{
	UpdateData();
	CEnFileDialog dialog(TRUE, "dic", m_sSelDictionary, OFN_PATHMUSTEXIST, SPELLCHECK_DICT_FILTER);
	
	dialog.m_ofn.lpstrTitle = SPELLCHECK_BROWSE_TITLE;
	
	if (dialog.DoModal() == IDOK)
	{
		if (InitDictionary(dialog.GetPathName()))
			StartChecking(CH_CURRENT);
	}
}

void CSpellCheckDlg::OnReplace() 
{
	if (!m_pSpellChecker)
		return;

	UpdateData();
	ASSERT (!m_sSuggestion.IsEmpty());

	m_pSpellCheck->ReplaceCurrentWord(m_sSuggestion);
	m_pSpellCheck->ClearSelection();
	m_bMadeChanges = TRUE;

	UpdateData();
	
	OnContinue();
}

void CSpellCheckDlg::OnContinue() 
{
	if (!m_pSpellChecker)
		return;

	StartChecking(CH_NEXT);
}

BOOL CSpellCheckDlg::FindNextWord(CString& sWord, CHECKFROM nFrom)
{
	switch (nFrom)
	{
	case CH_START:
		sWord = m_pSpellCheck->GetFirstWord();
		break;

	case CH_CURRENT:
		sWord = m_pSpellCheck->GetCurrentWord();
		break;

	case CH_NEXT:
		sWord = m_pSpellCheck->GetNextWord();
		break;
	}

	return (!sWord.IsEmpty());
}

BOOL CSpellCheckDlg::FindNextMisspeltWord(CString& sWord, CHECKFROM nFrom)
{
	while (TRUE)
	{
		if (!FindNextWord(sWord, nFrom))
			return FALSE;

		else if (IsWordMisspelt(sWord))
			return TRUE;

		// else continue
		nFrom = CH_NEXT;
	}

	// cant get here
	ASSERT (0);
	return FALSE;
}

BOOL CSpellCheckDlg::OnInitDialog() 
{
	CRuntimeDlg::OnInitDialog();

	m_ncBorder.Initialize(m_reText);
	m_bMadeChanges = FALSE;

	// reload dictionary list
	int nDicCount = AfxGetApp()->GetProfileInt("SpellChecker", "DictionaryCount", 0);

	for (int nDic = 0; nDic < nDicCount; nDic++)
	{
		CString sKey;
		sKey.Format("Dictionary%d", nDic);

		CString sPath = AfxGetApp()->GetProfileString("SpellChecker", sKey);

		if (GetFileAttributes(sPath) != 0xffffffff)
		{
			if (m_cbDictionaries.FindStringExact(-1, sPath) == CB_ERR)
				m_cbDictionaries.AddString(sPath);
		}
	}

	if (m_sSelDictionary.IsEmpty())
		m_sSelDictionary = AfxGetApp()->GetProfileString("SpellChecker", "ActiveDictionary");

	// check spell check engine is initialized
	BOOL bCancel = FALSE;

	if (!IsSpellCheckDll(m_sEnginePath))
	{
		bCancel = (IDCANCEL == AfxMessageBox(SPELLCHECK_SETUP_MSG, MB_OKCANCEL));

		while (!bCancel && !IsSpellCheckDll(m_sEnginePath))
		{
			// notify user and browse for dll
			CEnFileDialog dialog(TRUE, "dll", m_sEnginePath, OFN_PATHMUSTEXIST, SPELLCHECK_ENGINE_FILTER);

			dialog.m_ofn.lpstrTitle = SPELLCHECK_ENGINE_TITLE;

			if (dialog.DoModal() == IDOK)
				m_sEnginePath = dialog.GetPathName();
			else
				bCancel = TRUE;
		}
	}

	if (bCancel)
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// make the rich edit appear disabled if not using text
	if (m_pSpellCheck != &m_reSpellCheck)
		m_reText.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));

	if (IsInitialized() || InitDictionary(m_sSelDictionary))
	{
		if (!StartChecking() && m_bEndOnNoErrors)
		{
			EndDialog(IDNOERRORS);
			return FALSE;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSpellCheckDlg::StartChecking(CHECKFROM nFrom)
{
	CString sWord; 

	BOOL bMisspelt = FindNextMisspeltWord(sWord, nFrom);

	// if starting from the beginning and no misspelt words are found
	// then e return FALSE to indicate no further checking required
	if (nFrom == CH_START && !bMisspelt)
		return FALSE;

	else if (bMisspelt)
	{
		ProcessMisspeltWord(sWord);
		return TRUE;
	}

	// else reached the end so start again
	ProcessMisspeltWord("");

	return StartChecking(CH_START);
}

void CSpellCheckDlg::ProcessMisspeltWord(LPCTSTR szWord)
{
	if (!szWord || !*szWord) // no misspelling
	{
		m_lbSuggestions.ResetContent();
		GetDlgItem(IDC_REPLACE)->EnableWindow(FALSE);

		m_sMisspeltWord.Empty();
		UpdateData(FALSE);

		return;
	}

	// else
	m_sMisspeltWord.Format("'%s'", szWord);
	UpdateData(FALSE);

	// set new selection
	HighlightWord(TRUE);

	// add suggestions
	m_lbSuggestions.ResetContent();

	if (m_pSpellChecker)
	{
		char** pSuggestions = NULL;
		int nNumSuggestions = 0;

		m_pSpellChecker->CheckSpelling(szWord, pSuggestions, nNumSuggestions);

		for (int nSugg = 0; nSugg < nNumSuggestions; nSugg++)
			m_lbSuggestions.AddString(pSuggestions[nSugg]);

		m_pSpellChecker->FreeSuggestions(pSuggestions);
	}

	if (m_lbSuggestions.GetCount())
		m_lbSuggestions.GetText(0, m_sSuggestion);
	else
		m_sSuggestion.Empty();

	m_lbSuggestions.SelectString(-1, m_sSuggestion);
	GetDlgItem(IDC_REPLACE)->EnableWindow(m_lbSuggestions.GetCount());
}

void CSpellCheckDlg::HighlightWord(BOOL bHighlight)
{
	if (bHighlight)
		m_pSpellCheck->SelectCurrentWord();
	else
		m_pSpellCheck->ClearSelection();
}

BOOL CSpellCheckDlg::IsWordMisspelt(LPCTSTR szWord)
{
	if (m_pSpellChecker)
		return (m_pSpellChecker->CheckSpelling(szWord) == 0);

	return FALSE;
}

void CSpellCheckDlg::OnSelchangeSuggestions() 
{
	UpdateData();
	
	GetDlgItem(IDC_REPLACE)->EnableWindow(!m_sSuggestion.IsEmpty());
}

void CSpellCheckDlg::OnRestart() 
{
	if (m_pSpellChecker)
	{
		if (m_pSpellCheck == &m_reSpellCheck)
			m_bMadeChanges = FALSE;

		StartChecking();	
	}
}

void CSpellCheckDlg::OnDestroy() 
{
	m_pSpellCheck->ClearSelection();

	// save position
	if (m_ptTopLeft.x == -1 && m_ptTopLeft.y == -1)
	{
		CRect rWindow;
		GetWindowRect(rWindow);

		m_ptTopLeft = rWindow.TopLeft();
	}
	
	CRuntimeDlg::OnDestroy();
	
	AfxGetApp()->WriteProfileString("SpellChecker", "EnginePath", m_sEnginePath);
	AfxGetApp()->WriteProfileString("SpellChecker", "ActiveDictionary", m_sSelDictionary);

	// save dictionary list
	int nDicCount = m_cbDictionaries.GetCount();
	AfxGetApp()->WriteProfileInt("SpellChecker", "DictionaryCount", nDicCount);

	for (int nDic = 0; nDic < nDicCount; nDic++)
	{
		CString sKey;
		sKey.Format("Dictionary%d", nDic);

		CString sPath;
		m_cbDictionaries.GetLBText(nDic, sPath);

		AfxGetApp()->WriteProfileString("SpellChecker", sKey, sPath);
	}
}

void CSpellCheckDlg::OnDblClkSuggestions()
{
	UpdateData();

	if (m_lbSuggestions.GetCount() && !m_sSuggestion.IsEmpty())
		OnReplace();
}

CPoint CSpellCheckDlg::GetInitialPos() const 
{ 
	CPoint ptTemp(m_ptTopLeft);

	m_ptTopLeft.x = m_ptTopLeft.y = -1;  // reset

	return ptTemp;
}
