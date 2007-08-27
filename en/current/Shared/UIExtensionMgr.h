// ImportExportMgr.h: interface for the CUIExtensionMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIEXTENSIONMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
#define AFX_UIEXTENSIONMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iuiextension.h"
#include "itasklist.h"
#include <afxtempl.h>

class IUIExtension;
class IUIExtensionWindow;

typedef CMap<DWORD, DWORD, IUIExtensionWindow*, IUIExtensionWindow*&> CUIExtensionWndMap;

class CUIExtension
{
public:
	CUIExtension(IUIExtension* pExt = NULL);
	virtual ~CUIExtension();

	CString GetMenuText();
	HICON GetIcon();

	IUIExtensionWindow* GetWindow(DWORD dwItemData, HWND hParent, BOOL bAutoCreate = TRUE);

	BOOL HasWindow(DWORD dwItemData, BOOL bVisibleOnly = FALSE) const;
	void RemoveWindow(DWORD dwItemData);
	BOOL ShowWindow(DWORD dwItemData, BOOL bShow = TRUE);
	void UpdateWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags = UIU_ALL);
	
protected:
	IUIExtension* m_pExtension;
	CUIExtensionWndMap* m_pMapWindows;

protected:
	void Release();
	IUIExtensionWindow* FindWindow(DWORD dwItemData) const;
	CUIExtensionWndMap* WindowMap();
};

class CUIExtensionMgr  
{
public:
	CUIExtensionMgr();
	virtual ~CUIExtensionMgr();
	
	virtual void Initialize();
	
	int GetNumUIExtensions() const;
	CString GetUIExtensionMenuText(int nExtension) const;
	HICON GetUIExtensionIcon(int nExtension) const;

	IUIExtensionWindow* GetExtensionWindow(int nExtension, DWORD dwItemData, HWND hParent, BOOL bAutoCreate = TRUE);

	BOOL HasAnyExtensionWindows(DWORD dwItemData, BOOL bVisibleOnly = FALSE) const;
	void ShowAllExtensionsWindows(DWORD dwItemData, BOOL bShow = TRUE);
	void RemoveAllExtensionsWindows(DWORD dwItemData);
	void UpdateAllExtensionsWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags = UIU_ALL);
	
protected:
	BOOL m_bInitialized;
	CArray<CUIExtension*, CUIExtension*> m_aUIExtensions;
	
protected:
	static CString& Clean(CString& sText);

};

#endif // !defined(AFX_UIEXTENSIONMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
