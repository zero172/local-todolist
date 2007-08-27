#if !defined(AFX_ORDEREDTREECTRL_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
#define AFX_ORDEREDTREECTRL_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToDoCtrl.h : header file
//

#include "..\shared\ncgutter.h"
#include "..\shared\treectrlhelper.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// COrderedTreeCtrl window

#define OTC_GRIDCOLOR ::GetSysColor(COLOR_3DFACE)
#define OTC_POSCOLUMNID (NCG_CLIENTCOLUMNID - 1)

class COrderedTreeCtrl : public CTreeCtrl//, public CTreeCtrlHelper
{
// Construction
public:
	COrderedTreeCtrl(DWORD dwGutterStyles = NCGS_SHOWHEADER);
	virtual ~COrderedTreeCtrl();

	int AddGutterColumn(UINT nColID, LPCTSTR szTitle = NULL, UINT nWidth = 0, UINT nTextAlign = DT_LEFT);
	void PressGutterColumnHeader(UINT nColID, BOOL bPress = TRUE);
	void SetGutterColumnHeaderTitle(UINT nColID, LPCTSTR szTitle, LPCTSTR szFont = NULL);
	void SetGutterColumnHeaderTitle(UINT nColID, UINT nSymbol, LPCTSTR szFont = NULL);
	void EnableGutterColumnHeaderClicking(UINT nColID, BOOL bEnable = TRUE);
	void SetGutterColumnSort(UINT nColID, NCGSORT nSortDir);

	BOOL ShowGutterPosColumn(BOOL bShow = TRUE); // returns TRUE if changed
	BOOL IsGutterPosColumnShowing() const { return m_bShowingPosColumn; }
	void SetGridlineColor(COLORREF color);
    COLORREF GetGridlineColor() const { return m_crGridlines; }

	void EnableGutterStyle(DWORD dwStyle, BOOL bEnable = TRUE) { m_gutter.EnableStyle(dwStyle, bEnable); }
	BOOL GutterHasStyle(DWORD dwStyle) const { return m_gutter.HasStyle(dwStyle); }

	inline void RedrawGutter() { m_gutter.Redraw(); }
	inline void RedrawGutterItem(DWORD dwItem) { m_gutter.RedrawItem(dwItem); }
	inline void RecalcGutter(BOOL bForceRedraw = TRUE) { m_gutter.RecalcGutter(bForceRedraw); }
	inline BOOL RecalcGutterColumn(UINT nColID) { return m_gutter.RecalcColumn(nColID); }
	inline int GetGutterWidth() const { return m_gutter.GetGutterWidth(); }

	inline COLORREF GetAlternateLineColor() const { return m_crAltLines; }
	void SetAlternateLineColor(COLORREF color);
	COLORREF GetItemLineColor(HTREEITEM hti);

	BOOL PtInHeader(CPoint ptScreen) const;

	CTreeCtrlHelper& TCH() { return m_ht; }
	const CTreeCtrlHelper& TCH() const { return m_ht; }

protected:
	CNcGutter m_gutter;
	BOOL m_bShowingPosColumn;
	COLORREF m_crGridlines, m_crAltLines;
	BOOL m_bWantInit;
	CTreeCtrlHelper m_ht;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrderedTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(COrderedTreeCtrl)
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
	afx_msg BOOL OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpSS);
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);

	// callbacks for gutter
	afx_msg LRESULT OnGutterGetFirstVisibleTopLevelItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetNextItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetFirstChildItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterDrawItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterPostDrawItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterPostNcDraw(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterRecalcColWidth(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetItemRect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterIsItemSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetSelectedCount(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterNotifyItemClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetParentID(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterWantRecalc(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterWantRedraw(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void NcDrawItem(CDC* pDC, DWORD dwItem, DWORD dwParentItem, UINT nColID, CRect& rItem, 
					int nLevel, int nPos, BOOL bSelected, const CRect& rWindow);
	void PostNcDrawItem(CDC* pDC, DWORD dwItem, const CRect& rItem, int nLevel);
	void PostNcDraw(CDC* pDC, const CRect& rWindow);

	BOOL RecalcColumnWidth(CDC* pDC, UINT nColID, UINT& nWidth);
	UINT GetGutterWidth(HTREEITEM hti, int nLevel, int nPos, CDC* pDC);
	static UINT GetWidth(int nNumber, CDC* pDC); // includes a trailing '.'

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORDEREDTREECTRL_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
