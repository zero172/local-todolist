// ImportExportMgr.h: interface for the CImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPORTEXPORTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
#define AFX_IMPORTEXPORTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "itasklist.h"
#include <afxtempl.h>

class IImportTasklist;
class IExportTasklist;

class CImportExportMgr  
{
public:
	CImportExportMgr();
	virtual ~CImportExportMgr();
	
	virtual void Initialize();
	
	int GetNumImporters() const;
	int GetNumExporters() const;
	
	CString GetImporterMenuText(int nImporter) const;
	CString GetImporterFileExtension(int nImporter) const;
	CString GetImporterFileFilter(int nImporter) const;
	
	CString GetExporterMenuText(int nExporter) const;
	CString GetExporterFileExtension(int nExporter) const;
	BOOL ExporterHasFileExtension(int nExporter) const;
	CString GetExporterFileFilter(int nExporter) const;
	
	BOOL ImportTaskList(LPCTSTR szSrcFile, ITaskList* pDestTasks, int nByImporter) const;
	BOOL ExportTaskList(const ITaskList* pSrcTasks, LPCTSTR szDestFile, int nByExporter) const;
	
	void UpdateExportMenu(CCmdUI* pCmdUI, int nMaxCount, BOOL bEnabled = TRUE) const;
	void UpdateImportMenu(CCmdUI* pCmdUI, int nMaxCount, BOOL bEnabled = TRUE) const;
	
protected:
	BOOL m_bInitialized;
	CArray<IImportTasklist*, IImportTasklist*> m_aImporters;
	CArray<IExportTasklist*, IExportTasklist*> m_aExporters;
	
protected:
	static CString& Clean(CString& sText);

};

#endif // !defined(AFX_IMPORTEXPORTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
