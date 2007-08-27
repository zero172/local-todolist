#if !defined(AFX_CHECKCOMBOBOX_H__4DDD3C3D_BDB7_4F07_9C50_75495F6E7F66__INCLUDED_)
#define AFX_CHECKCOMBOBOX_H__4DDD3C3D_BDB7_4F07_9C50_75495F6E7F66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// checkcombobox.h : header file
//

#include "autocombobox.h"
#include "subclass.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckComboBox window

class CCheckComboBox : public CAutoComboBox, public CSubclasser
{
// Construction
public:
	CCheckComboBox(BOOL bAllowDelete = FALSE, BOOL bCaseSensitive = FALSE);

//	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL GetCheck(int nIndex) const;
	int GetChecked(CStringArray& aItems) const;
	void SetChecked(const CStringArray& aItems);
	int SetCheck(int nIndex, BOOL bCheck = TRUE);
	void CheckAll(BOOL bCheck = TRUE);

	virtual int AddUniqueItem(const CString& sItem); // returns index or CB_ERR
	int GetCurSel() const;

protected:
	CString m_sText;
	BOOL m_bItemHeightSet;
	mutable BOOL m_bDrawing;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// for listbox
	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

	// for editbox
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

// Implementation
public:
	virtual ~CCheckComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckComboBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg BOOL OnEditchange();
	afx_msg BOOL OnDropdown();
//	afx_msg BOOL OnCloseUp();
	afx_msg void OnLBSelChange();

	DECLARE_MESSAGE_MAP()

protected:
	void RecalcText(BOOL bUpdate = TRUE, BOOL bNotify = TRUE);
	void ParseText();
	BOOL IsType(UINT nComboType);
	virtual BOOL DeleteSelectedLBItem();
	virtual void DrawItemText(HDC hdc, int nItem, CRect rText, const CString& sText, UINT nState);
	virtual void RefreshMaxDropWidth();
	virtual void HandleReturnKey();
	virtual CString GetSelectedItem() const;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKCOMBOBOX_H__4DDD3C3D_BDB7_4F07_9C50_75495F6E7F66__INCLUDED_)
