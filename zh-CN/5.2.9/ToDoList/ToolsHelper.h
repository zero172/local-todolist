// ToolsHelper.h: interface for the CToolsHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLSHELPER_H__6BAD432D_0189_46A9_95ED_EF869CFC6CE1__INCLUDED_)
#define AFX_TOOLSHELPER_H__6BAD432D_0189_46A9_95ED_EF869CFC6CE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolsCmdlineParser.h"
#include "..\shared\toolbarhelper.h"
#include "..\shared\menuiconmgr.h"

struct USERTOOL
{
	CString sToolName;
	CString sToolPath;
	CString sCmdline;
	BOOL bRunMinimized;
	CString sIconPath;
};

struct USERTOOLARGS
{
	CString sTasklist;
	DWORD dwTaskID;
	CString sTaskTitle;
	CString sTaskExtID;
	CString sTaskComments;
};

class CSysImageList;

typedef CArray<USERTOOL, USERTOOL&> CUserToolArray;

class CToolsHelper  
{
public:
	CToolsHelper(UINT nStart, int nSize = 16);
	virtual ~CToolsHelper();
	
	void UpdateMenu(CCmdUI* pCmdUI, const CUserToolArray& tools, CMenuIconMgr& iconMgr);
	BOOL RunTool(const USERTOOL& tool, const USERTOOLARGS& args, CWnd* pWnd = NULL);
	void TestTool(const USERTOOL& tool, const USERTOOLARGS& args, CWnd* pWnd = NULL);
	void AppendToolsToToolbar(const CUserToolArray& aTools, CToolBar& toolbar, UINT nCmdAfter, 
		CToolbarHelper* pTBHelper = NULL);
	void RemoveToolsFromToolbar(CToolBar& toolbar, UINT nCmdAfter);
	
protected:
	UINT m_nStartID;
	int m_nSize;
	
protected:
	BOOL PrepareCmdline(const USERTOOL& tool, CString& sCmdline, const USERTOOLARGS& args);
   	LPCTSTR GetFileFilter();
	LPCTSTR GetDefaultFileExt();

	static HICON GetToolIcon(CSysImageList& sil, const USERTOOL& ut);

};

#endif // !defined(AFX_TOOLSHELPER_H__6BAD432D_0189_46A9_95ED_EF869CFC6CE1__INCLUDED_)
