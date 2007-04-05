// ContentMgr.cpp: implementation of the CContentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ContentMgr.h"
#include "IContentControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentMgr::CContentMgr() : m_bInitialized(FALSE), m_bSomeBadVersions(FALSE)
{

}

CContentMgr::~CContentMgr()
{
	// cleanup
	int nInterface = m_aContent.GetSize();

	while (nInterface--)
		m_aContent[nInterface]->Release();

	m_aContent.RemoveAll();
}

BOOL CContentMgr::Initialize()
{
	if (m_bInitialized)
		return TRUE;

	// look at every dll from whereever we are now
	CFileFind ff;
    char szFolder[MAX_PATH], szDrive[_MAX_DRIVE], szSearchPath[MAX_PATH];

    GetModuleFileName(NULL, szSearchPath, MAX_PATH);
    _splitpath(szSearchPath, szDrive, szFolder, NULL, NULL);
    _makepath(szSearchPath, szDrive, szFolder, "*", ".dll");

	BOOL bContinue = ff.FindFile(szSearchPath);
	
	while (bContinue)
	{
		bContinue = ff.FindNextFile();
		
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			CString sDllPath = ff.GetFilePath();

			if (IsContentDll(sDllPath))
			{
				int nDllVer = 0;
				IContent* pContent = CreateContentInterface(sDllPath, &nDllVer);

				if (pContent)
				{
					// do settings initialization
/*					BOOL bReg = (AfxGetApp()->m_pszRegistryKey != NULL);
					LPCTSTR szIni = bReg ? NULL : AfxGetApp()->m_pszProfileName;

					pContent->SetIniLocation((bReg != FALSE), szIni);
*/
					// save
					m_aContent.Add(pContent);
				}
				else if (nDllVer < ICONTENTCTRL_VERSION)
				{
					m_bSomeBadVersions = TRUE;
				}
			}
		}
	}

	m_bInitialized = TRUE;
	return TRUE;
}

int CContentMgr::GetNumContent() const
{
	if (!m_bInitialized) 
		return 0;

	return m_aContent.GetSize();
}

BOOL CContentMgr::GetContentTypeID(int nContent, GUID& type) const
{
	if (!m_bInitialized) 
		return FALSE;

	if (nContent >= 0 && nContent < m_aContent.GetSize())
	{
		ASSERT (m_aContent[nContent] != NULL);
		return m_aContent[nContent]->GetTypeID(type);
	}

	// else
	return FALSE;
}

CString CContentMgr::GetContentTypeDescription(int nContent) const
{
	if (!m_bInitialized) 
		return "";

	if (nContent >= 0 && nContent < m_aContent.GetSize())
	{
		ASSERT (m_aContent[nContent] != NULL);
		return m_aContent[nContent]->GetTypeDescription();
	}
	
	// else
	return "";
}

BOOL CContentMgr::CreateContentControl(int nContent, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle, 
							 DWORD dwExStyle, const CRect& rect, HWND hwndParent)
{
	if (!m_bInitialized) 
		return FALSE;

	if (nContent >= 0 && nContent < m_aContent.GetSize())
	{
		ASSERT (m_aContent[nContent] != NULL);

		IContentControl* pControl = m_aContent[nContent]->CreateCtrl((WORD)nCtrlID, nStyle, rect.left, 
															rect.top, rect.Width(), rect.Height(), hwndParent);

		if (pControl && pControl->GetHwnd())
		{
			if (dwExStyle)
				CWnd::ModifyStyleEx(pControl->GetHwnd(), 0, dwExStyle, 0);

			return ctrl.Attach(pControl);
		}

		if (pControl)
			pControl->Release();
	}

	// else
	return FALSE;
}

BOOL CContentMgr::CreateContentControl(const GUID& type, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle, 
							 DWORD dwExStyle, const CRect& rect, HWND hwndParent)
{
	// check if the CContentCtrl already has a valid control
	if (ctrl.GetSafeHwnd() && ctrl.IsType(type))
		return TRUE;

	return CreateContentControl(FindContent(type), ctrl, nCtrlID, nStyle, dwExStyle, rect, hwndParent);
}

int CContentMgr::FindContent(const GUID& type) const
{
	int nContent = m_aContent.GetSize();
	GUID typeContent;

	while (nContent--)
	{
		ASSERT (m_aContent[nContent] != NULL);

		if (m_aContent[nContent]->GetTypeID(typeContent))
		{
			if (memcmp(&type, &typeContent, sizeof(GUID)) == 0)
				return nContent;
		}
	}

	// else not found
	return -1;
}

BOOL CContentMgr::ConvertContentToHtml(const CString& sContent, CString& sHtml, const GUID& type)
{
	int nContent = FindContent(type);

	if (nContent == -1)
		return FALSE;

	char* szHtml = NULL;

	ASSERT (m_aContent[nContent] != NULL);
	
	int nHtmlLen = m_aContent[nContent]->ConvertToHtml((const unsigned char*)(LPCTSTR)sContent, sContent.GetLength(), szHtml);

	if (nHtmlLen)
		sHtml = szHtml;

	delete [] szHtml; // cleanup
	
	return (nHtmlLen > 0);
}
