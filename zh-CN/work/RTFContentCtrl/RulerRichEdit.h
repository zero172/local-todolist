#if !defined(AFX_RULERRICHEDIT_H__E10A8ED3_2E1D_402E_A599_003214085F1A__INCLUDED_)
#define AFX_RULERRICHEDIT_H__E10A8ED3_2E1D_402E_A599_003214085F1A__INCLUDED_

// RulerRichEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRulerRichEdit window

#include "..\shared\urlricheditctrl.h"

class CRulerRichEdit : public CUrlRichEditCtrl
{
public:
// Construction/destruction
	CRulerRichEdit();
	virtual ~CRulerRichEdit();

	void Paste(BOOL bSimple = FALSE);

protected:
	BOOL m_bPasteSimple;

protected:
// Message handlers
	//{{AFX_MSG(CRulerRichEdit)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnDropFiles(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	virtual CLIPFORMAT GetAcceptableClipFormat(LPDATAOBJECT lpDataOb, CLIPFORMAT format);

};

#endif // !defined(AFX_RULERRICHEDIT_H__E10A8ED3_2E1D_402E_A599_003214085F1A__INCLUDED_)
