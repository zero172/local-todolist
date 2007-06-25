#if !defined(AFX_FINDBLANKPAGE_H__BAAFF03D_18CB_4D1F_8507_9B931065CB5A__INCLUDED_)
#define AFX_FINDBLANKPAGE_H__BAAFF03D_18CB_4D1F_8507_9B931065CB5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindBlankPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindBlankPage dialog

class CFindBlankPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFindBlankPage)

// Construction
public:
	CFindBlankPage();
	~CFindBlankPage();

protected:
	enum { IDD = IDD_FINDBLANK_PAGE };

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDBLANKPAGE_H__BAAFF03D_18CB_4D1F_8507_9B931065CB5A__INCLUDED_)
