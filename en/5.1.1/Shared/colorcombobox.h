#if !defined(AFX_COLORCOMBOBOX_H__47DE226A_7C73_48BA_AE5B_E43B90D752A9__INCLUDED_)
#define AFX_COLORCOMBOBOX_H__47DE226A_7C73_48BA_AE5B_E43B90D752A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// colorcombobox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox window

class CColorComboBox : public CComboBox
{
// Construction
public:
	CColorComboBox();

// Operations
public:
	int AddColor(COLORREF color, LPCTSTR szDescription = NULL);
	int InsertColor(int nIndex, COLORREF color, LPCTSTR szDescription = NULL);

// Attributes
protected:
	BOOL m_bResized;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCOMBOBOX_H__47DE226A_7C73_48BA_AE5B_E43B90D752A9__INCLUDED_)
