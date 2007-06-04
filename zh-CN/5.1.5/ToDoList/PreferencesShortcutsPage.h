#if !defined(AFX_PREFERENCESSHORTCUTSPAGE_H__DA5D005D_C6CC_453A_A431_A2B85A920CE5__INCLUDED_)
#define AFX_PREFERENCESSHORTCUTSPAGE_H__DA5D005D_C6CC_453A_A431_A2B85A920CE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefererencesShortcutsPage.h : header file
//

#include "..\shared\shortcutManager.h"
#include "..\shared\orderedtreectrl.h"
#include "..\shared\hotkeyctrlex.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesShortcutsPage dialog

#define PSP_SHORTCUTCOLUMNID (OTC_POSCOLUMNID - 1)

class CPreferencesShortcutsPage : public CPropertyPage
{
// Construction
public:
	CPreferencesShortcutsPage(CShortcutManager* pMgr, UINT nMenuID, BOOL bIgnoreGrayedItems = TRUE);
	~CPreferencesShortcutsPage();

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesShortcutsPage)
	enum { IDD = IDD_PREFSHORTCUTS_PAGE };
	CHotKeyCtrlEx	m_hkCur;
	COrderedTreeCtrl	m_tcCommands;
	CHotKeyCtrlEx	m_hkNew;
	CString	m_sOtherCmdID;
	//}}AFX_DATA

	CShortcutManager* m_pShortcutMgr;
	UINT m_nMenuID;
	CMap<UINT, UINT, DWORD, DWORD&> m_mapID2Shortcut;
	CMap<DWORD, DWORD, HTREEITEM, HTREEITEM&> m_mapShortcut2HTI;
	BOOL m_bIgnoreGrayedItems;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesShortcutsPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesShortcutsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedShortcuts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAssignshortcut();
	//}}AFX_MSG
	afx_msg void OnChangeShortcut();
	afx_msg LRESULT OnGutterDrawItem(WPARAM wParam, LPARAM lParam); // for drawing priority
	afx_msg LRESULT OnGutterRecalcColWidth(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	HTREEITEM AddMenuItem(HTREEITEM htiParent, const CMenu* pMenu, int nPos);
	int GetLongestShortcutText(HTREEITEM hti, CDC* pDC);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERERENCESSHORTCUTSPAGE_H__DA5D005D_C6CC_453A_A431_A2B85A920CE5__INCLUDED_)
