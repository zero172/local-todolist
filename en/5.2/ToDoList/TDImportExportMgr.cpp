// TDImportExportMgr.cpp: implementation of the CTDImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "todolist.h"
#include "TDImportExportMgr.h"
#include "tasklisthtmlexporter.h"
#include "tasklisttxtexporter.h"
#include "tasklistcsvexporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDImportExportMgr::CTDImportExportMgr()
{

}

CTDImportExportMgr::~CTDImportExportMgr()
{

}

void CTDImportExportMgr::Initialize()
{
	// add html and text exporters first
	if (!m_bInitialized)
	{
		m_aExporters.InsertAt(EXP2HTML, new CTaskListHtmlExporter);
		m_aExporters.InsertAt(EXP2TXT, new CTaskListTxtExporter);
		m_aExporters.InsertAt(EXP2CSV, new CTaskListCsvExporter);
	}

	CImportExportMgr::Initialize();
}

BOOL CTDImportExportMgr::ExportTaskListToHtml(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXP2HTML);
}

CString CTDImportExportMgr::ExportTaskListToHtml(const ITaskList* pSrcTasks) const
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
		
	if (!::GetTempPath(MAX_PATH, szTempPath) || 
		!::GetTempFileName(szTempPath, "tdl", 0, szTempFile))
		return "";
		
	CString sFile;

	if (ExportTaskListToHtml(pSrcTasks, szTempFile))
	{
		CStdioFile file;

		if (file.Open(szTempFile, CFile::modeRead))
		{
			CString sLine;

			while (file.ReadString(sLine))
				sFile += sLine;
		}
	}

	::DeleteFile(szTempFile);
	return sFile;
}

BOOL CTDImportExportMgr::ExportTaskListToText(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXP2TXT);
}

CString CTDImportExportMgr::ExportTaskListToText(const ITaskList* pSrcTasks) const
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
		
	if (!::GetTempPath(MAX_PATH, szTempPath) || 
		!::GetTempFileName(szTempPath, "tdl", 0, szTempFile))
		return "";
		
	CString sFile;

	if (ExportTaskListToText(pSrcTasks, szTempFile))
	{
		CStdioFile file;

		if (file.Open(szTempFile, CFile::modeRead))
		{
			CString sLine;

			while (file.ReadString(sLine))
				sFile += sLine + "\n";
		}
	}

	::DeleteFile(szTempFile);
	return sFile;
}

BOOL CTDImportExportMgr::ExportTaskListToCsv(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXP2CSV);
}

CString CTDImportExportMgr::ExportTaskListToCsv(const ITaskList* pSrcTasks) const
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
		
	if (!::GetTempPath(MAX_PATH, szTempPath) || 
		!::GetTempFileName(szTempPath, "tdl", 0, szTempFile))
		return "";
		
	CString sFile;

	if (ExportTaskListToCsv(pSrcTasks, szTempFile))
	{
		CStdioFile file;

		if (file.Open(szTempFile, CFile::modeRead))
		{
			CString sLine;

			while (file.ReadString(sLine))
				sFile += sLine + "\n";
		}
	}

	::DeleteFile(szTempFile);
	return sFile;
}
