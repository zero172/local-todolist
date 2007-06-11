// TaskListDropTarget.h: interface for the CTaskListDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTDROPTARGET_H__56519FB1_2923_45BB_97A2_08B8B1DC7C97__INCLUDED_)
#define AFX_TASKLISTDROPTARGET_H__56519FB1_2923_45BB_97A2_08B8B1DC7C97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxole.h>

const UINT WM_TLDT_DROPFILE = ::RegisterWindowMessage("WM_TLDT_DROPFILE"); // wparam = HTREEITEM or 0, lParam == (LPCTSTR)filepath

class CTaskListDropTarget : public COleDropTarget  
{
public:
	CTaskListDropTarget();
	virtual ~CTaskListDropTarget();

	BOOL Register(CWnd* pTarget, CWnd* pParent);

protected:
	CWnd* m_pParent;

protected:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pObject, DROPEFFECT dropEffect, CPoint point);

protected:
	CString GetLongPathName(LPCTSTR szShortPath);
};

#endif // !defined(AFX_TASKLISTDROPTARGET_H__56519FB1_2923_45BB_97A2_08B8B1DC7C97__INCLUDED_)
