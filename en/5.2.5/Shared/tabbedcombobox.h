#if !defined(AFX_TABBEDCOMBOBOX_H__17C6DAEB_521B_4C43_BC64_C01F0AD655C3__INCLUDED_)
#define AFX_TABBEDCOMBOBOX_H__17C6DAEB_521B_4C43_BC64_C01F0AD655C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tabbedcombobox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabbedComboBox window

class CTabbedComboBox : public CComboBox
{
// Construction
public:
	CTabbedComboBox(int nTabStops = 20);

// Attributes
protected:
	const int TABSTOPS;
	BOOL m_bResized;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabbedComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabbedComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabbedComboBox)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBEDCOMBOBOX_H__17C6DAEB_521B_4C43_BC64_C01F0AD655C3__INCLUDED_)
