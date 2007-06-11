// ImportExportMgr.cpp: implementation of the CImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImportExportMgr.h"
#include "ITaskList.h"
#include "IImportExport.h"
#include "filemisc.h"

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

int CImportExportMgr::FindImporter(LPCTSTR szFilePath)
{
	CString sExt;
	FileMisc::SplitPath(szFilePath, NULL, NULL, NULL, &sExt);

	if (sExt.IsEmpty()) // no extension
		return -1;

	int nImporter = m_aImporters.GetSize();

	while (nImporter--)
	{
		if (GetImporterFileExtension(nImporter).CompareNoCase(sExt) == 0) // match
			break;
	}
	
	return nImporter; // match or -1
}
