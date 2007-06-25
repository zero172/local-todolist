// ContentMgr.h: interface for the CContentMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
#define AFX_CONTENTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "contentctrl.h"
#include <afxtempl.h>

class IContent;

class CContentMgr  
{
public:
	CContentMgr();
	virtual ~CContentMgr();
	
	virtual BOOL Initialize();
	BOOL SomePluginsHaveBadversions() const { return m_bSomeBadVersions; }
	
	int GetNumContent() const;
	int FindContent(const GUID& type) const;
	
	BOOL GetContentTypeID(int nContent, GUID& type) const;
	CString GetContentTypeDescription(int nContent) const;

	BOOL CreateContentControl(int nContent, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle, 
							 DWORD dwExStyle, const CRect& rect, HWND hwndParent);
	BOOL CreateContentControl(const GUID& type, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle, 
							 DWORD dwExStyle, const CRect& rect, HWND hwndParent);

	BOOL ConvertContentToHtml(const CString& sContent, CString& sHtml, const GUID& type);
	
protected:
	BOOL m_bInitialized;
	BOOL m_bSomeBadVersions;
	CArray<IContent*, IContent*> m_aContent;
	
protected:
};

#endif // !defined(AFX_CONTENTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
