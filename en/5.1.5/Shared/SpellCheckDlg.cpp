// SpellCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpellCheckDlg.h"
#include "ispellcheck.h"
#include "filedialogex.h"
#include "stringres.h"

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


CSpellCheckDlg::CSpellCheckDlg(LPCTSTR szDictionaryPath, LPCTSTR szText, CWnd* /*pParent*/)
	: m_pSpellChecker(NULL), 
     m_sSelDictionary(szDictionaryPath), 
     m_stURL("http://wiki.services.openoffice.org/wiki/Dictionaries") 
{
	AfxEnableControlContainer();
	AfxInitRichEdit();

	AddRCControl("LTEXT", "", SPELLCHECK_ACTIVEDICT, 0,0, 7,9,65,8, (UINT)IDC_STATIC);
	AddRCControl("COMBOBOX", "", "", CBS_DROPDOWNLIST | WS_TABSTOP, 0, 66, 8, 182,100, IDC_DICTIONARIES);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_BROWSE,WS_TABSTOP, 0,256,7,50,14,IDC_BROWSE);
	AddRCControl("LTEXT", "", SPELLCHECK_DOWNLOADMORE, WS_TABSTOP,0, 66,21,90,8,IDC_URL);
	AddRCControl("LTEXT", "", SPELLCHECK_CHECKING, 0,0, 7,30,49,8, (UINT)IDC_STATIC);
	AddRCControl("CONTROL", "RICHEDIT", "",ES_MULTILINE | ES_AUTOVSCROLL | /*ES_NOHIDESEL |WS_BORDER | */ ES_READONLY | WS_VSCROLL | WS_TABSTOP,0, 7,40,242,68,IDC_TEXT);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_RESTART,WS_TABSTOP, 0,256,40,50,14,IDC_RESTART);
	AddRCControl("LTEXT", "", SPELLCHECK_REPLACE,0, 0,7,112,30,8, (UINT)IDC_STATIC);
	AddRCControl("LTEXT", "", "Static",0, 0,44,112,205,8,IDC_MISSPELTWORD);
	AddRCControl("LTEXT", "", SPELLCHECK_WITH,0, 0,7,124,18,8, (UINT)IDC_STATIC);
	AddRCControl("LISTBOX", "", "", LBS_SORT | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP | LBS_NOTIFY, 0,41,124,208,51, IDC_SUGGESTIONS);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_BTN_REPLACE,WS_TABSTOP, 0,256,124,50,14,IDC_REPLACE);
	AddRCControl("PUSHBUTTON", "", SPELLCHECK_NEXTWORD,WS_TABSTOP, 0,256,144,50,14,IDC_CONTINUE);
	AddRCControl("CONTROL", "static", "",SS_ETCHEDHORZ,0,7,182,299,1, (UINT)IDC_STATIC);
	AddRCControl("DEFPUSHBUTTON", "", BTN_OK, WS_TABSTOP, 0, 199,190,50,14,IDOK);
	AddRCControl("PUSHBUTTON", "", BTN_CANCEL, WS_TABSTOP, 0,256,190,50,14,IDCANCEL);

	//{{AFX_DATA_INIT(CSpellCheckDlg)
	m_sText = szText;
	m_sMisspeltWord = _T("");
	m_sSuggestion = _T("");
	//}}AFX_DATA_INIT
	m_crMisspeltWord.cpMin = m_crMisspeltWord.cpMax = 0;

	// init spell checker dll path
#ifndef _DEBUG
	m_sEnginePath = AfxGetApp()->GetProfileString("SpellChecker", "EnginePath", "MySpellCheck.dll");
#else
	m_sEnginePath = "MySpellCheck.dll";
#endif

	if (!IsSpellCheckDll(m_sEnginePath))
	{
		char szDllPath[MAX_PATH], szDrive[_MAX_DRIVE], szFolder[MAX_PATH];
		GetModuleFileName(NULL, szDllPath, MAX_PATH);
		_splitpath(szDllPath, szDrive, szFolder, NULL, NULL);
		_makepath(szDllPath, szDrive, szFolder, "MySpellCheck", ".dll");

		m_sEnginePath = szDllPath;
	}
}

CSpellCheckDlg::~CSpellCheckDlg()
{
	ReleaseSpellCheckInterface(m_pSpellChecker);
}

void CSpellCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellCheckDlg)
	DDX_Control(pDX, IDC_TEXT, m_reText);
	DDX_Control(pDX, IDC_SUGGESTIONS, m_lbSuggestions);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_Text(pDX, IDC_MISSPELTWORD, m_sMisspeltWord);
	DDX_LBString(pDX, IDC_SUGGESTIONS, m_sSuggestion);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_URL, m_stURL);
	DDX_Control(pDX, IDC_DICTIONARIES, m_cbDictionaries);

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

	return CRuntimeDlg::DoModal(SPELLCHECK_TITLE); 
}

BOOL CSpellCheckDlg::InitDictionary(LPCTSTR szDicPath)
{
	CString sAffixPath(szDicPath);
	sAffixPath.MakeLower();
	sAffixPath.Replace(".dic", ".aff");

	ISpellCheck* pTemp = CreateSpellCheckInterface(m_sEnginePath, sAffixPath, szDicPath);

	if (pTemp) // alls well
	{
		ReleaseSpellCheckInterface(m_pSpellChecker);
		m_pSpellChecker = pTemp;
		pTemp = NULL;

		m_sSelDictionary = szDicPath;
		UpdateData(FALSE);

		// clear the current word and reset 
		HighlightWord(m_crMisspeltWord, FALSE);
		m_crMisspeltWord.cpMax = m_crMisspeltWord.cpMin;
	}

	return (NULL != m_pSpellChecker);
}

void CSpellCheckDlg::SetText(LPCTSTR szText) 
{ 
	m_sText = szText; 

	m_sMisspeltWord.Empty();
	m_sSuggestion.Empty();
	m_crMisspeltWord.cpMin = m_crMisspeltWord.cpMax = 0;

	if (GetSafeHwnd())
	{
		m_lbSuggestions.ResetContent();
		UpdateData(FALSE);
		StartChecking();
	}
}

void CSpellCheckDlg::OnChangeDictionary()
{
	UpdateData();

	if (InitDictionary(m_sSelDictionary))
		StartChecking(m_crMisspeltWord.cpMin);
}

void CSpellCheckDlg::OnBrowse()
{
	UpdateData();
	CFileDialogEx dialog(TRUE, "dic", m_sSelDictionary, OFN_PATHMUSTEXIST, SPELLCHECK_DICT_FILTER);
	
	dialog.m_ofn.lpstrTitle = SPELLCHECK_BROWSE_TITLE;
	
	if (dialog.DoModal() == IDOK)
	{
		if (InitDictionary(dialog.GetPathName()))
			StartChecking(m_crMisspeltWord.cpMin);
	}
}

void CSpellCheckDlg::OnReplace() 
{
	if (!m_pSpellChecker)
		return;

	UpdateData();
	ASSERT (!m_sSuggestion.IsEmpty());
	
	m_sText = m_sText.Left(m_crMisspeltWord.cpMin) + m_sSuggestion + m_sText.Mid(m_crMisspeltWord.cpMax);
	m_crMisspeltWord.cpMax = m_crMisspeltWord.cpMin + m_sSuggestion.GetLength();
	UpdateData(FALSE);

	OnContinue();
}

void CSpellCheckDlg::OnContinue() 
{
	if (!m_pSpellChecker)
		return;

	StartChecking(m_crMisspeltWord.cpMax + 1);
}

BOOL CSpellCheckDlg::FindNextWord(int nFrom, CHARRANGE& range, CString& sWord)
{
	const CString DELIMS = " .,<>;:-'\"!?()\t\r\n~@#$%^&*+=|\\{}[]/1234567890";
	int nTextLen = m_sText.GetLength();

	// walk along m_sText until we find the start of the word (next non-delimeter)
	while (nFrom < nTextLen - 1 && DELIMS.Find(m_sText[nFrom]) != -1)
		nFrom++;

	// now look ahead for end of the word
	int nTo = nFrom;

	while (nTo < nTextLen && DELIMS.Find(m_sText[nTo]) == -1)
		nTo++;

	if (nFrom == nTo)
		return FALSE;

	range.cpMin = nFrom;
	range.cpMax = nTo;
	sWord = m_sText.Mid(nFrom, nTo - nFrom);

	return TRUE;
}

BOOL CSpellCheckDlg::FindNextMisspeltWord(int nFrom, CHARRANGE& range, CString& sWord)
{
	while (TRUE)
	{
		CHARRANGE crWord;
		
		if (!FindNextWord(nFrom, crWord, sWord))
		{
			range.cpMin = range.cpMax = nFrom;
			return FALSE;
		}
		else if (IsWordMisspelt(sWord))
		{
			range = crWord;
			return TRUE;
		}

		// else
		nFrom = crWord.cpMax + 1;
	}

	// cant get here
	ASSERT (0);
	return FALSE;
}

BOOL CSpellCheckDlg::OnInitDialog() 
{
	CRuntimeDlg::OnInitDialog();

	m_ncBorder.Initialize(m_reText);

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
			CFileDialogEx dialog(TRUE, "dll", m_sEnginePath, OFN_PATHMUSTEXIST, SPELLCHECK_ENGINE_FILTER);

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

	if (IsInitialized() || InitDictionary(m_sSelDictionary))
	{
		StartChecking();

		if (m_bEndOnNoErrors && m_crMisspeltWord.cpMin == m_crMisspeltWord.cpMax)
		{
			EndDialog(IDNOERRORS);
			return FALSE;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpellCheckDlg::StartChecking(int nFrom)
{
	CString sWord; 
	CHARRANGE crPrev = m_crMisspeltWord, crTemp;

	if (FindNextMisspeltWord(nFrom, m_crMisspeltWord, sWord))
		ProcessMisspeltWord(sWord, m_crMisspeltWord, crPrev);

	// reached the end so start again
	else
	{
		m_crMisspeltWord.cpMin = m_crMisspeltWord.cpMax = 0;
		ProcessMisspeltWord("", m_crMisspeltWord, crPrev);

		if (FindNextMisspeltWord(0, crTemp, sWord))
			StartChecking();
	}
}

void CSpellCheckDlg::ProcessMisspeltWord(LPCTSTR szWord, CHARRANGE crWord, CHARRANGE crPrev)
{
	if (crPrev.cpMin == crWord.cpMin && crPrev.cpMax == crWord.cpMax)
		return;

	// always remove previous selection
	HighlightWord(crPrev, FALSE);
	
	if (crWord.cpMin == crWord.cpMax) // no misspelling
	{
		m_lbSuggestions.ResetContent();
		GetDlgItem(IDC_REPLACE)->EnableWindow(FALSE);
		return;
	}

	// else
	m_sMisspeltWord.Format("'%s'", szWord);
	UpdateData(FALSE);

	// set new selection
	HighlightWord(crWord);

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

void CSpellCheckDlg::HighlightWord(CHARRANGE& crWord, BOOL bHighlight)
{
	if (crWord.cpMin == crWord.cpMax)
		return;

	CHARFORMAT cf = 
	{ 
		sizeof(cf), 
		CFM_COLOR | CFM_BOLD, 
		bHighlight ? CFE_BOLD : 0, 
		0, 
		0, 
		bHighlight ? 255 : 0, 
		0, 
		0, 
		0 
	};

	m_reText.SetSel(crWord);
	m_reText.SetWordCharFormat(cf); 
	m_reText.SetSel(crWord.cpMax, crWord.cpMax);

	// need to make sure line is visible
	CPoint ptSel = m_reText.GetCharPos(crWord.cpMax);

	CRect rClient;
	m_reText.GetClientRect(rClient);

	if (ptSel.y >= rClient.bottom)
	{
		while (ptSel.y >= rClient.bottom)
		{
			m_reText.LineScroll(1);
			ptSel = m_reText.GetCharPos(crWord.cpMax);
		}

		// one more for good measure
		m_reText.LineScroll(1);
	}
	else if (ptSel.y <= rClient.top)
	{
		while (ptSel.y <= rClient.top)
		{
			m_reText.LineScroll(-1);
			ptSel = m_reText.GetCharPos(crWord.cpMax);
		}

		// one more for good measure
		m_reText.LineScroll(-1);
	}
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
		StartChecking();	
}

void CSpellCheckDlg::OnDestroy() 
{
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

