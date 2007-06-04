// TaskListTxtExporter.h: interface for the CTaskListTxtExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTTXTEXPORTER_H__CF68988D_FBBD_431D_BB56_464E8737D993__INCLUDED_)
#define AFX_TASKLISTTXTEXPORTER_H__CF68988D_FBBD_431D_BB56_464E8737D993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SHARED\Itasklist.h"
#include "..\SHARED\IImportExport.h"

class CTaskListTxtExporter : public IExportTasklist  
{
public:
	CTaskListTxtExporter();
	virtual ~CTaskListTxtExporter();

	const char* GetMenuText() { return "Plain Text"; }
	const char* GetFileFilter() { return "Text Files (*.txt)|*.txt||"; }
	const char* GetFileExtension() { return "txt"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath);
    void Release() { delete this; }

protected:
	CString TEXTNOTES, INDENT;
	BOOL ROUNDTIMEFRACTIONS;

protected:
	CString& ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, CString& sOutput) const;

	static BOOL FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szNumAttribName, LPCTSTR szAttribName, 
                                          LPCTSTR szFormat, CString& sAttribText);
	static BOOL FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName, 
								LPCTSTR szFormat, CString& sAttribText);
};

#endif // !defined(AFX_TASKLISTTXTEXPORTER_H__CF68988D_FBBD_431D_BB56_464E8737D993__INCLUDED_)
