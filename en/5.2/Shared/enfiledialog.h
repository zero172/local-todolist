#if !defined(AFX_ENFILEDIALOG_H__79203A03_61CE_4234_AC53_0A23B1FD2204__INCLUDED_)
#define AFX_ENFILEDIALOG_H__79203A03_61CE_4234_AC53_0A23B1FD2204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// filedialogex.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnFileDialog dialog

class CEnFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CEnFileDialog)

public:
	CEnFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CEnFileDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENFILEDIALOG_H__79203A03_61CE_4234_AC53_0A23B1FD2204__INCLUDED_)
