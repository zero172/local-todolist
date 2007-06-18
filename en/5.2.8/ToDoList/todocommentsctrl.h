#if !defined(AFX_TODOCOMMENTSCTRL_H__E9A5D982_4D40_43B2_A071_E3BE70D122B7__INCLUDED_)
#define AFX_TODOCOMMENTSCTRL_H__E9A5D982_4D40_43B2_A071_E3BE70D122B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tdlcommentsctrl.h : header file
//

#include "..\shared\urlricheditctrl.h"
#include "..\shared\IContentControl.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoCommentsCtrl window

class CToDoCommentsCtrl : public CUrlRichEditCtrl, public IContentControl
{
// Construction
public:
	CToDoCommentsCtrl();
	
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// IContentControl implementation
	// supports plain text content only
	int GetTextContent(char* szContent, int nLength) const;
	bool SetTextContent(const char* szContent);
	int GetContent(unsigned char* /*pContent*/) const { return 0; }
	bool SetContent(unsigned char* /*pContent*/, int /*nLength*/) { return false; }
	void SetReadOnly(bool bReadOnly);
	HWND GetHwnd() const { return GetSafeHwnd(); }
	bool HasTypeID() const { return false; }
	bool GetTypeID(GUID& /*id*/) const { return false; }
	void Release() { delete this; }
	bool ProcessMessage(MSG* pMsg);

// Attributes
protected:
	BOOL m_bAllowNotify;
	BOOL m_bWordWrap;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDoCommentsCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToDoCommentsCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToDoCommentsCtrl)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg void OnCommentsMenuCmd(UINT nCmdID);
	afx_msg void OnUpdateCommentsMenuCmd(CCmdUI* pCmdUI);
	afx_msg void OnNeedTooltip(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnChangeText();
	afx_msg LRESULT OnSetFont(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	virtual LRESULT SendNotifyCustomUrl(LPCTSTR szUrl) const;
	virtual int OnToolHitTest(CPoint pt, TOOLINFO* pTI) const;

protected:
	BOOL IsClipboardEmpty() const;
	void SetWordWrap(BOOL bWrap);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOCOMMENTSCTRL_H__E9A5D982_4D40_43B2_A071_E3BE70D122B7__INCLUDED_)
