#if !defined(AFX_WELCOMEDIALOG_H__1F96AB75_AFD0_458D_99DF_06A021BA5527__INCLUDED_)
#define AFX_WELCOMEDIALOG_H__1F96AB75_AFD0_458D_99DF_06A021BA5527__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WelcomeDialog.h : header file
//

#include "..\3rdparty\xhtmlstatic.h"

/////////////////////////////////////////////////////////////////////////////
// CWelcomeDialog dialog

class CWelcomeDialog : public CDialog
{
// Construction
public:
	CWelcomeDialog(CWnd* pParent = NULL);   // standard constructor

	BOOL GetUseIniFile() { return m_bUseIniFile; }

protected:
// Dialog Data
	//{{AFX_DATA(CWelcomeDialog)
	enum { IDD = IDD_WELCOME_DIALOG };
	CXHTMLStatic	m_stText;
	CString	m_sText;
	int		m_bUseIniFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWelcomeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWelcomeDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELCOMEDIALOG_H__1F96AB75_AFD0_458D_99DF_06A021BA5527__INCLUDED_)
