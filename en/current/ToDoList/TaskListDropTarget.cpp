// TaskListDropTarget.cpp: implementation of the CTaskListDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskListDropTarget.h"

#include "..\shared\wclassdefines.h"
#include "..\shared\winclasses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTaskListDropTarget::CTaskListDropTarget() : m_pParent(NULL)
{
}

CTaskListDropTarget::~CTaskListDropTarget()
{

}

BOOL CTaskListDropTarget::Register(CWnd* pTarget, CWnd* pParent)
{
	m_pParent = pParent;

	return COleDropTarget::Register(pTarget);
}

DROPEFFECT CTaskListDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* /*pObject*/, DWORD /*dwKeyState*/, CPoint /*point*/)
{
	if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) || CWinClasses::IsClass(*pWnd, WC_TREEVIEW))
		TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);

	return DROPEFFECT_NONE;
}

void CTaskListDropTarget::OnDragLeave(CWnd* pWnd)
{
	if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) || CWinClasses::IsClass(*pWnd, WC_TREEVIEW) == 0)
		TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
}

DROPEFFECT CTaskListDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pObject, DWORD /*dwKeyState*/, CPoint point)
{
	if (!pWnd->IsWindowEnabled())
		return DROPEFFECT_NONE;

	if (pObject->IsDataAvailable((CLIPFORMAT)::RegisterClipboardFormat("FileName")))
	{
		CString sClass = CWinClasses::GetClass(*pWnd);

		if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) || sClass.CompareNoCase(WC_TREEVIEW) == 0)
		{
			TVHITTESTINFO tvhti = { { point.x, point.y }, 0, 0 };

			TreeView_HitTest(pWnd->GetSafeHwnd(), &tvhti);
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), tvhti.hItem);

			if (tvhti.hItem)
				return DROPEFFECT_LINK;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CEdit)) || sClass.CompareNoCase(WC_EDIT) == 0)
		{
			if (!(pWnd->GetStyle() & ES_READONLY))
				return DROPEFFECT_LINK;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)) ||
				 pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			// allow dropping only on titlebar
			if ((pWnd->GetStyle() & WS_CAPTION) && point.y < 0)
				return DROPEFFECT_COPY;
		}
	}
	else
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) || CWinClasses::IsClass(*pWnd, WC_TREEVIEW) == 0)
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
	}

	// else
	return DROPEFFECT_NONE;
}

BOOL CTaskListDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pObject, DROPEFFECT /*dropEffect*/, CPoint point)
{
	CLIPFORMAT cf = (CLIPFORMAT)::RegisterClipboardFormat("FileName");

	if (pObject->IsDataAvailable(cf))
	{
		HGLOBAL hGlobal = pObject->GetGlobalData(cf);
    
		if (!hGlobal)
			return FALSE;
		
		LPCTSTR szFilePath = (LPCTSTR)GlobalLock(hGlobal);
		
		if (szFilePath)
		{
			CString sLongPath = GetLongPathName(szFilePath);
			CString sClass = CWinClasses::GetClass(*pWnd);

			m_pParent->SetForegroundWindow();

			if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) || sClass.CompareNoCase(WC_TREEVIEW) == 0)
			{
				TVHITTESTINFO tvhti = { { point.x, point.y }, 0, 0 };

				TreeView_HitTest(pWnd->GetSafeHwnd(), &tvhti);
				TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);

				if (tvhti.hItem)
					m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)tvhti.hItem, (LPARAM)(LPCTSTR)sLongPath);
			}
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CEdit)) || sClass.CompareNoCase(WC_EDIT) == 0)
			{
				if (!(pWnd->GetStyle() & ES_READONLY))
					m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)0, (LPARAM)(LPCTSTR)sLongPath);
			}
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)) ||
					 pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
			{
				// allow dropping only on titlebar
				if ((pWnd->GetStyle() & WS_CAPTION) && point.y < 0)
					m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)0, (LPARAM)(LPCTSTR)sLongPath);
			}
		}

		// cleanup
		GlobalUnlock(hGlobal);
	}
	else // cleanup
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) || CWinClasses::IsClass(*pWnd, WC_TREEVIEW) == 0)
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
	}
	
	return FALSE; // because we handle it
}

typedef DWORD (WINAPI *FNGETLONGPATHNAME)(LPCTSTR, LPTSTR, DWORD);

CString CTaskListDropTarget::GetLongPathName(LPCTSTR szShortPath)
{
	CString sLongPath(szShortPath);

	// must link dynamically to kernel32 else problem with win95/NT4
	HMODULE hLib = LoadLibrary("kernel32.dll");

	if (hLib)
	{
		FNGETLONGPATHNAME pFN = (FNGETLONGPATHNAME)GetProcAddress(hLib, "GetLongPathNameA");

		if (pFN)
		{
			char szLongPath[MAX_PATH + 1];
			pFN(szShortPath, szLongPath, MAX_PATH);

			sLongPath = szLongPath;
		}
	}

	return sLongPath;
}

