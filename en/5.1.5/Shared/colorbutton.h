#if !defined(AFX_COLORBUTTON_H__4FA393FA_20E7_4C07_A682_5AA7A8306DF4__INCLUDED_)
#define AFX_COLORBUTTON_H__4FA393FA_20E7_4C07_A682_5AA7A8306DF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// colorbutton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
// Construction
public:
	CColorButton();

	COLORREF GetColor() { return m_color; }
	void SetColor(COLORREF color);

protected:
	COLORREF m_color;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnClicked();
	//}}AFX_MSG
	afx_msg LRESULT OnShowAccelerators(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__4FA393FA_20E7_4C07_A682_5AA7A8306DF4__INCLUDED_)
