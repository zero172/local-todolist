#if !defined(AFX_SPELLCHECKDLG_H__796FCD48_463A_4291_9261_69EA5122ABA0__INCLUDED_)
#define AFX_SPELLCHECKDLG_H__796FCD48_463A_4291_9261_69EA5122ABA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpellCheckDlg.h : header file
//

#include "runtimedlg.h"
#include "richeditncborder.h"
#include "richeditspellcheck.h"

#include "..\3rdparty\statlink.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellCheckDlg dialog

const int IDNOERRORS = 20; // can be returned from DoModal

class ISpellChecker;
class ISpellCheck;

class CSpellCheckDlg : public CRuntimeDlg
{
// Construction
public:
	CSpellCheckDlg(LPCTSTR szDictionaryPath = NULL, ISpellCheck* pSpellCheck = NULL, LPCTSTR szText = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpellCheckDlg();

	BOOL IsInitialized() { return ((m_pSpellCheck || !m_sText.IsEmpty()) && m_pSpellChecker); }
	int DoModal(BOOL bEndOnNoErrors = FALSE);

	void SetSpellCheck(ISpellCheck* pSpellCheck);

	void SetText(LPCTSTR szText);
	CString GetCorrectedText() { return m_sText; }

	BOOL MadeChanges() const { return m_bMadeChanges; }

protected:
// Dialog Data
	//{{AFX_DATA(CSpellCheckDlg)
	CRichEditCtrl	m_reText;
	CListBox	m_lbSuggestions;
	CString	m_sText;
	CString	m_sMisspeltWord;
	CString	m_sSuggestion;
	//}}AFX_DATA
//	CHARRANGE m_crMisspeltWord;
	ISpellChecker* m_pSpellChecker;
	ISpellCheck* m_pSpellCheck;
	CString m_sEnginePath;
	CComboBox m_cbDictionaries;
	CString m_sSelDictionary;
	BOOL m_bEndOnNoErrors;
    CStaticLink m_stURL;
	CRichEditNcBorder m_ncBorder;
	CRichEditSpellCheck m_reSpellCheck;
	BOOL m_bMadeChanges;
	mutable CPoint m_ptTopLeft;

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

	enum CHECKFROM { CH_START, CH_CURRENT, CH_NEXT };

	BOOL StartChecking(CHECKFROM nFrom = CH_START);
	BOOL FindNextWord(CString& sWord, CHECKFROM nFrom = CH_NEXT);
	BOOL FindNextMisspeltWord(CString& sWord, CHECKFROM nFrom = CH_NEXT);
	void ProcessMisspeltWord(LPCTSTR szWord);
	BOOL IsWordMisspelt(LPCTSTR szWord);
	void HighlightWord(BOOL bHighlight = TRUE);
	BOOL InitDictionary(LPCTSTR szDicPath);

	void InitDialog(ISpellCheck* pSpellCheck, LPCTSTR szText);

	virtual CPoint GetInitialPos() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLCHECKDLG_H__796FCD48_463A_4291_9261_69EA5122ABA0__INCLUDED_)
