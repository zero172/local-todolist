#if !defined(AFX_RTFCONTENTCONTROL_H__4F1A93A0_7829_4DBB_AA0B_A2F62E4E7F50__INCLUDED_)
#define AFX_RTFCONTENTCONTROL_H__4F1A93A0_7829_4DBB_AA0B_A2F62E4E7F50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RTFContentControl.h : header file
//

#include "rulerricheditctrl.h"

#include "..\shared\IContentControl.h"
#include "..\shared\toolbarhelper.h"
#include "..\shared\richeditspellcheck.h"

/////////////////////////////////////////////////////////////////////////////
// CRTFContentControl window

class CRTFContentControl : public CRulerRichEditCtrl, public IContentControl
{
// Construction
public:
	CRTFContentControl();

	// ICustomControl implementation
	int GetContent(unsigned char* pContent) const;
	bool SetContent(unsigned char* pContent, int nLength);
	int GetTextContent(char* szContent, int nLength = -1) const;
	bool SetTextContent(const char* szContent);
	void SetReadOnly(bool bReadOnly);
	HWND GetHwnd() const;
	const char* GetTypeID() const;
	void Release();
	bool ProcessMessage(MSG* pMsg);
	ISpellCheck* GetSpellCheckInterface() { return &m_reSpellCheck; }

// Attributes
protected:
	BOOL m_bAllowNotify;
	CToolbarHelper m_tbHelper;
	CRichEditSpellCheck m_reSpellCheck;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTFContentControl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRTFContentControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRTFContentControl)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnChangeText();
	afx_msg LRESULT OnSetFont(WPARAM wp, LPARAM lp);
	afx_msg void OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg LRESULT OnCustomUrl(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	BOOL CanPaste();
	BOOL IsClipboardEmpty() const;
	static int GetContent(const CRTFContentControl* pCtrl, unsigned char* pContent);
	static void EnableMenuItem(CMenu* pMenu, UINT nCmdID, BOOL bEnable);
	static void CheckMenuItem(CMenu* pMenu, UINT nCmdID, BOOL bCheck);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTFCONTENTCONTROL_H__4F1A93A0_7829_4DBB_AA0B_A2F62E4E7F50__INCLUDED_)
