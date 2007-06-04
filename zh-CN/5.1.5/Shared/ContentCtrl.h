// ContentCtrl.h: interface for the CContentCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTCTRL_H__BE763781_9932_4582_B3D0_64DEE59B1A92__INCLUDED_)
#define AFX_CONTENTCTRL_H__BE763781_9932_4582_B3D0_64DEE59B1A92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IContentControl;

class CContentCtrl  
{
public:
	CContentCtrl(IContentControl* pContentCtrl = NULL);
	virtual ~CContentCtrl();

	BOOL Attach(IContentControl* pContentCtrl);

	int GetContent(unsigned char* pContent) const;
	int GetContent(CString& sContent) const;
	int GetTextContent(CString& sContent) const;

	BOOL SetContent(unsigned char* pContent, int nLength);
	BOOL SetContent(const CString& sContent);
	BOOL SetTextContent(const char* szContent);
	BOOL SettingContent() { return m_bSettingContent; }

	BOOL HasTypeID() const;
	BOOL GetTypeID(GUID& id) const;
	BOOL IsType(const GUID& id) const;

	BOOL SetReadOnly(BOOL bReadOnly);
	
	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
	BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

	operator HWND() const;
	HWND GetSafeHwnd() const { return *this; }

	UINT GetDlgCtrlID() { return ::GetDlgCtrlID(*this); }

	LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	BOOL PreTranslateMessage(MSG* pMsg);

protected:
	IContentControl* m_pContentCtrl;
	BOOL m_bSettingContent;
};

#endif // !defined(AFX_CONTENTCTRL_H__BE763781_9932_4582_B3D0_64DEE59B1A92__INCLUDED_)
