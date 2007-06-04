// ImportExportMgr.cpp: implementation of the CImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImportExportMgr.h"
#include "ITaskList.h"
#include "IImportExport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImportExportMgr::CImportExportMgr() : m_bInitialized(FALSE)
{

}

CImportExportMgr::~CImportExportMgr()
{
	// cleanup
	int nInterface = m_aImporters.GetSize();

	while (nInterface--)
		m_aImporters[nInterface]->Release();

	m_aImporters.RemoveAll();

	nInterface = m_aExporters.GetSize();

	while (nInterface--)
		m_aExporters[nInterface]->Release();

	m_aExporters.RemoveAll();
}

void CImportExportMgr::Initialize()
{
	if (m_bInitialized)
		return;

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

			if (IsImportExportDll(sDllPath))
			{
				IImportTasklist* pImporter = CreateImportInterface(sDllPath);

				if (pImporter)
					m_aImporters.Add(pImporter);

				IExportTasklist* pExporter = CreateExportInterface(sDllPath);

				if (pExporter)
					m_aExporters.Add(pExporter);
			}
		}
	}

	m_bInitialized = TRUE;
}

int CImportExportMgr::GetNumImporters() const
{
	if (!m_bInitialized) 
		return 0;

	return m_aImporters.GetSize();
}

int CImportExportMgr::GetNumExporters() const
{
	if (!m_bInitialized) 
		return 0;

	return m_aExporters.GetSize();
}

CString CImportExportMgr::GetImporterMenuText(int nImporter) const
{
	CString sText;

	if (m_bInitialized) 
	{
		if (nImporter >= 0 && nImporter < m_aImporters.GetSize())
		{
			ASSERT (m_aImporters[nImporter] != NULL);
			sText = m_aImporters[nImporter]->GetMenuText();
		}
	}

	return Clean(sText);
}

CString CImportExportMgr::GetImporterFileExtension(int nImporter) const
{
	CString sExt;

	if (m_bInitialized) 
	{
		if (nImporter >= 0 && nImporter < m_aImporters.GetSize())
		{
			ASSERT (m_aImporters[nImporter] != NULL);
			sExt = m_aImporters[nImporter]->GetFileExtension();
		}
	}
	
	return Clean(sExt);
}

CString CImportExportMgr::GetImporterFileFilter(int nImporter) const
{
	CString sFilter;

	if (m_bInitialized) 
	{
		if (nImporter >= 0 && nImporter < m_aImporters.GetSize())
		{
			ASSERT (m_aImporters[nImporter] != NULL);
			sFilter = m_aImporters[nImporter]->GetFileFilter();
		}
	}

	return Clean(sFilter);
}

CString CImportExportMgr::GetExporterMenuText(int nExporter) const
{
	CString sText;

	if (m_bInitialized) 
	{
		if (nExporter >= 0 && nExporter < m_aExporters.GetSize())
		{
			ASSERT (m_aExporters[nExporter] != NULL);
			sText = m_aExporters[nExporter]->GetMenuText();
		}
	}

	return Clean(sText);
}

CString CImportExportMgr::GetExporterFileExtension(int nExporter) const
{
	CString sExt;

	if (m_bInitialized) 
	{
		if (nExporter >= 0 && nExporter < m_aExporters.GetSize())
		{
			ASSERT (m_aExporters[nExporter] != NULL);
			sExt = m_aExporters[nExporter]->GetFileExtension();
		}
	}

	return Clean(sExt);
}

BOOL CImportExportMgr::ExporterHasFileExtension(int nExporter) const
{
	return !GetExporterFileExtension(nExporter).IsEmpty();
}

CString CImportExportMgr::GetExporterFileFilter(int nExporter) const
{
	CString sFilter;

	if (m_bInitialized) 
	{
		if (nExporter >= 0 && nExporter < m_aExporters.GetSize())
		{
			ASSERT (m_aExporters[nExporter] != NULL);
			sFilter = m_aExporters[nExporter]->GetFileFilter();
		}
	}

	return Clean(sFilter);
}

CString& CImportExportMgr::Clean(CString& sText)
{
	sText.TrimLeft();
	sText.TrimRight();

	return sText;
}

BOOL CImportExportMgr::ImportTaskList(LPCTSTR szSrcFile, ITaskList* pDestTasks, int nByImporter) const
{
	if (!m_bInitialized) 
		return FALSE;

	CWaitCursor cursor;

	if (nByImporter >= 0 && nByImporter < m_aImporters.GetSize())
	{
		ASSERT (m_aImporters[nByImporter] != NULL);
		return m_aImporters[nByImporter]->Import(szSrcFile, pDestTasks);
	}

	// else
	return FALSE;
}

BOOL CImportExportMgr::ExportTaskList(const ITaskList* pSrcTasks, LPCTSTR szDestFile, int nByExporter) const
{
	if (!m_bInitialized) 
		return FALSE;

	if (nByExporter >= 0 && nByExporter < m_aExporters.GetSize())
	{
		ASSERT (m_aExporters[nByExporter] != NULL);
		return m_aExporters[nByExporter]->Export(pSrcTasks, szDestFile);
	}

	// else
	return FALSE;
}

void CImportExportMgr::UpdateExportMenu(CCmdUI* pCmdUI, int nMaxCount, BOOL bEnabled) const
{
	if (pCmdUI->m_pMenu)
	{
		UINT nStartID = pCmdUI->m_nID;

		// delete existing tool entries first
		for (int nTool = 0; nTool < nMaxCount; nTool++)
			pCmdUI->m_pMenu->DeleteMenu(nStartID + nTool, MF_BYCOMMAND);
		
		// if we have any tools to add we do it here
		int nNumExporters = GetNumExporters();

		if (nNumExporters)
		{
			int nPos = 0;
			UINT nFlags = MF_BYPOSITION | MF_STRING | (bEnabled ? 0 : MF_GRAYED);
			
			for (nTool = 0; nTool < nMaxCount && nTool < nNumExporters; nTool++)
			{
				CString sMenuItem;
				
				if (nPos < 9)
					sMenuItem.Format("&%d %s", nPos + 1, GetExporterMenuText(nTool));
				else
					sMenuItem = GetExporterMenuText(nTool);

				pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++, nFlags, 
											nStartID + nTool, sMenuItem);
				
				nPos++;
			}
			
			
			// update end menu count
			pCmdUI->m_nIndex--; // point to last menu added
			pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
			
			pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
		}
		else // if nothing to add just re-add placeholder
		{
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex, MF_BYPOSITION | MF_STRING | MF_GRAYED, 
				nStartID, "3rd Party Exporters");
		}
	}
}

void CImportExportMgr::UpdateImportMenu(CCmdUI* pCmdUI, int nMaxCount, BOOL bEnabled) const
{
	if (pCmdUI->m_pMenu)
	{
		UINT nStartID = pCmdUI->m_nID;

		// delete existing tool entries first
		for (int nTool = 0; nTool < nMaxCount; nTool++)
			pCmdUI->m_pMenu->DeleteMenu(nStartID + nTool, MF_BYCOMMAND);
		
		// if we have any tools to add we do it here
		int nNumImporters = GetNumImporters();

		if (nNumImporters)
		{
			int nPos = 0;
			UINT nFlags = MF_BYPOSITION | MF_STRING | (bEnabled ? 0 : MF_GRAYED);
			
			for (nTool = 0; nTool < nMaxCount && nTool < nNumImporters; nTool++)
			{
				CString sMenuItem;
				
				if (nPos < 9)
					sMenuItem.Format("&%d %s", nPos + 1, GetImporterMenuText(nTool));
				else
					sMenuItem = GetImporterMenuText(nTool);
				
				pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++, nFlags, 
											nStartID + nTool, sMenuItem);
				
				nPos++;
			}
			
			// update end menu count
			pCmdUI->m_nIndex--; // point to last menu added
			pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
			
			pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
		}
		else // if nothing to add just re-add placeholder
		{
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex, MF_BYPOSITION | MF_STRING | MF_GRAYED, 
				nStartID, "3rd Party Importers");
		}
	}
}
