// ImportExportMgr.cpp: implementation of the CUIExtensionMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIExtensionMgr.h"
#include "ITaskList.h"
#include "IUIExtension.h"
#include "filemisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIExtension::CUIExtension(IUIExtension* pExt) : m_pExtension(pExt), m_pMapWindows(NULL) 
{

}

CUIExtension::~CUIExtension()
{
	// cleanup
	Release();
}

CString CUIExtension::GetMenuText()
{
	if (m_pExtension)
		return m_pExtension->GetMenuText();
	
	return "";
}

HICON CUIExtension::GetIcon()
{
	if (m_pExtension)
		return m_pExtension->GetIcon();
	
	return NULL;
}

void CUIExtension::Release() 
{ 
	if (m_pExtension) 
	{
		if (m_pMapWindows)
		{
			POSITION pos = m_pMapWindows->GetStartPosition();
			
			while (pos)
			{
				IUIExtensionWindow* pWindow = NULL;
				DWORD dwDummy;
				
				m_pMapWindows->GetNextAssoc(pos, dwDummy, pWindow);
				
				if (pWindow)
					pWindow->Release();
			}
			
			m_pMapWindows->RemoveAll();
			delete m_pMapWindows;
			m_pMapWindows = NULL;
		}
		m_pExtension->Release(); 
	}
	
	m_pExtension = NULL;
}

IUIExtensionWindow* CUIExtension::GetWindow(DWORD dwItemData, HWND hParent, BOOL bAutoCreate)
{
	IUIExtensionWindow* pWindow = FindWindow(dwItemData);
	
	if (!pWindow && bAutoCreate)
	{
		SIZE size = { 300, 200 };
		pWindow = m_pExtension->CreateExtensionWindow(hParent, FALSE, &size);
		WindowMap()->SetAt(dwItemData, pWindow);
	}
	
	return pWindow;
}

BOOL CUIExtension::HasWindow(DWORD dwItemData, BOOL bVisibleOnly) const
{
	IUIExtensionWindow* pWindow = FindWindow(dwItemData);

	if (bVisibleOnly)
		return (pWindow && pWindow->IsShowing());

	// else
	return (pWindow != NULL);
}

void CUIExtension::RemoveWindow(DWORD dwItemData)
{
	IUIExtensionWindow* pWindow = FindWindow(dwItemData);
	
	if (pWindow)
	{
		pWindow->Release();
		WindowMap()->RemoveKey(dwItemData);
	}
}

BOOL CUIExtension::ShowWindow(DWORD dwItemData, BOOL bShow)
{
	IUIExtensionWindow* pWindow = FindWindow(dwItemData);
	
	if (pWindow)
		return pWindow->Show(bShow);
	
	// else
	return FALSE;
}

void CUIExtension::UpdateWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags)
{
	IUIExtensionWindow* pWindow = FindWindow(dwItemData);
	
	if (pWindow)
		pWindow->Update(pTasks, dwFlags);
}

CUIExtensionWndMap* CUIExtension::WindowMap()
{
	if (!m_pMapWindows)
		m_pMapWindows = new CUIExtensionWndMap;
	
	return m_pMapWindows;
};

IUIExtensionWindow* CUIExtension::FindWindow(DWORD dwItemData) const
{
	IUIExtensionWindow* pWindow = NULL;

	if (m_pMapWindows)
		m_pMapWindows->Lookup(dwItemData, pWindow);
	
	return pWindow;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIExtensionMgr::CUIExtensionMgr() : m_bInitialized(FALSE)
{

}

CUIExtensionMgr::~CUIExtensionMgr()
{
	// cleanup
	int nInterface = m_aUIExtensions.GetSize();

	while (nInterface--)
		delete m_aUIExtensions[nInterface];

	m_aUIExtensions.RemoveAll();
}

void CUIExtensionMgr::Initialize()
{
	if (m_bInitialized)
		return;

	// look at every dll from wherever we are now
	CFileFind ff;
    CString sSearchPath = FileMisc::GetModuleFileName(), sFolder, sDrive;

	FileMisc::SplitPath(sSearchPath, &sDrive, &sFolder);
	FileMisc::MakePath(sSearchPath, sDrive, sFolder, "*", ".dll");

	BOOL bContinue = ff.FindFile(sSearchPath);
	
	while (bContinue)
	{
		bContinue = ff.FindNextFile();
		
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			CString sDllPath = ff.GetFilePath();

			if (IsUIExtemsionDll(sDllPath))
			{
				IUIExtension* m_pExtension = CreateUIExtensionInterface(sDllPath);

				if (m_pExtension)
					m_aUIExtensions.Add(new CUIExtension(m_pExtension));

			}
		}
	}

	m_bInitialized = TRUE;
}

int CUIExtensionMgr::GetNumUIExtensions() const
{
	if (!m_bInitialized) 
		return 0;

	return m_aUIExtensions.GetSize();
}

CString CUIExtensionMgr::GetUIExtensionMenuText(int nExtension) const
{
	CString sText;

	if (m_bInitialized) 
	{
		if (nExtension >= 0 && nExtension < m_aUIExtensions.GetSize())
			sText = m_aUIExtensions[nExtension]->GetMenuText();
	}

	return Clean(sText);
}

HICON CUIExtensionMgr::GetUIExtensionIcon(int nExtension) const
{
	if (m_bInitialized) 
	{
		if (nExtension >= 0 && nExtension < m_aUIExtensions.GetSize())
			return m_aUIExtensions[nExtension]->GetIcon();
	}

	return NULL;
}

CString& CUIExtensionMgr::Clean(CString& sText)
{
	sText.TrimLeft();
	sText.TrimRight();

	return sText;
}

IUIExtensionWindow* CUIExtensionMgr::GetExtensionWindow(int nExtension, DWORD dwItemData, 
														HWND hParent, BOOL bAutoCreate)
{
	IUIExtensionWindow* pWindow = NULL;

	if (m_bInitialized) 
	{
		if (nExtension >= 0 && nExtension < m_aUIExtensions.GetSize())
			pWindow = m_aUIExtensions[nExtension]->GetWindow(dwItemData, hParent, bAutoCreate);
	}

	return pWindow;
}

BOOL CUIExtensionMgr::HasAnyExtensionWindows(DWORD dwItemData, BOOL bVisibleOnly) const
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
	{
		if (m_aUIExtensions[nExtension]->HasWindow(dwItemData, bVisibleOnly))
			return TRUE;
	}

	return FALSE;
}

void CUIExtensionMgr::ShowAllExtensionsWindows(DWORD dwItemData, BOOL bShow)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
		m_aUIExtensions[nExtension]->ShowWindow(dwItemData, bShow);
}

void CUIExtensionMgr::RemoveAllExtensionsWindows(DWORD dwItemData)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
		m_aUIExtensions[nExtension]->RemoveWindow(dwItemData);
}

void CUIExtensionMgr::UpdateAllExtensionsWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
		m_aUIExtensions[nExtension]->UpdateWindow(dwItemData, pTasks, dwFlags);
}
