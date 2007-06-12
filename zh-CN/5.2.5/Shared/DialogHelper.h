// EDialog.h: interface for the EDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOGHELPER_H__8543A453_171B_11D4_AE08_0000E8425C3E__INCLUDED_)
#define AFX_DIALOGHELPER_H__8543A453_171B_11D4_AE08_0000E8425C3E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDialogHelper
{
public:
	// safe versions if the window text is empty
	static void DDX_Text(CDataExchange* pDX, int nIDC, BYTE& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, short& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, int& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, UINT& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, long& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, DWORD& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, CString& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, float& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, double& value);

	static UINT MessageBoxEx(CWnd* pWnd, UINT nIDText, UINT nIDCaption, UINT nType = MB_OK);
	static UINT MessageBoxEx(CWnd* pWnd, LPCTSTR szText, UINT nIDCaption, UINT nType = MB_OK);

	static BOOL IsChildOrSame(HWND hWnd, HWND hwndChild);

	// font helper
	static void SetFont(CWnd* pWnd, HFONT hFont, BOOL bRedraw = TRUE);
    static HFONT GetFont(CWnd* pWnd);
    static HFONT GetFont(HWND hWnd);
	
	// comboboxes
	static int SetComboBoxItems(CComboBox& combo, const CStringArray& aItems);
	static int RefreshMaxDropWidth(CComboBox& combo, CDC* pDCRef = NULL);
	static int CalcMaxTextWidth(CComboBox& combo, int nMinWidth = 0, BOOL bDropped = FALSE, CDC* pDCRef = NULL);
	
	// better dialog control shortcut handling
	static BOOL ProcessDialogControlShortcut(const MSG* pMsg);
	static UINT GetShortcut(const CString& sText);

	// child clipping
	void ClipChild(CDC* pDC, CWnd* pWnd, UINT nChildID);
	void ClipChild(CDC* pDC, CWnd* pChild);

protected:
	CDialogHelper() : m_bInUpdateEx(FALSE) {}

	// helpers for updating only a single item
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, BYTE& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, short& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, int& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, UINT& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, long& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, DWORD& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, float& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, double& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, CWnd& ctrl, BOOL bSaveAndValidate = TRUE);
	BOOL InUpdateEx() { return m_bInUpdateEx; }

	// for combo and listboxes only
	BOOL UpdateDataExact(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate = TRUE);

	static CWnd* FindNextMatch(CWnd* pCurrent, UINT nShortcut);

private:
	BOOL m_bInUpdateEx;

};

#endif // !defined(AFX_DIALOGHELPER_H__8543A453_171B_11D4_AE08_0000E8425C3E__INCLUDED_)
