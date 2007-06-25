#if !defined(AFX_SPELLCHECKDLG_H__796FCD48_463A_4291_9261_69EA5122ABA0__INCLUDED_)
#define AFX_SPELLCHECKDLG_H__796FCD48_463A_4291_9261_69EA5122ABA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpellCheckDlg.h : header file
//

#include "runtimedlg.h"
#include "richeditncborder.h"

#include "..\3rdparty\statlink.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellCheckDlg dialog

const int IDNOERRORS = 20; // can be returned from DoModal

class ISpellCheck;

class CSpellCheckDlg : public CRuntimeDlg
{
// Construction
public:
	CSpellCheckDlg(LPCTSTR szDictionaryPath, LPCTSTR szText, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpellCheckDlg();

	BOOL IsInitialized() { return (NULL != m_pSpellChecker); }
	int DoModal(BOOL bEndOnNoErrors = FALSE);

	void SetText(LPCTSTR szText);
	CString GetCorrectedText() { return m_sText; }

protected:
// Dialog Data
	//{{AFX_DATA(CSpellCheckDlg)
	CRichEditCtrl	m_reText;
	CListBox	m_lbSuggestions;
	CString	m_sText;
	CString	m_sMisspeltWord;
	CString	m_sSuggestion;
	//}}AFX_DATA
	CHARRANGE m_crMisspeltWord;
	ISpellCheck* m_pSpellChecker;
	CString m_sEnginePath;
	CComboBox m_cbDictionaries;
	CString m_sSelDictionary;
	BOOL m_bEndOnNoErrors;
    CStaticLink m_stURL;
	CRichEditNcBorder m_ncBorder;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpellCheckDlg)
	afx_msg void OnReplace();
	afx_msg void OnContinue();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSuggestions();
	afx_msg void OnRestart();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnBrowse();
	afx_msg void OnChangeDictionary();
	afx_msg void OnDblClkSuggestions();
	DECLARE_MESSAGE_MAP()

	void StartChecking(int nFrom = 0);
	BOOL FindNextWord(int nFrom, CHARRANGE& range, CString& sWord);
	BOOL FindNextMisspeltWord(int nFrom, CHARRANGE& range, CString& sWord);
	void ProcessMisspeltWord(LPCTSTR szWord, CHARRANGE crWord, CHARRANGE crPrev);
	BOOL IsWordMisspelt(LPCTSTR szWord);
	void HighlightWord(CHARRANGE& crWord, BOOL bHighlight = TRUE);
	BOOL InitDictionary(LPCTSTR szDicPath);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLCHECKDLG_H__796FCD48_463A_4291_9261_69EA5122ABA0__INCLUDED_)
