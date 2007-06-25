// TaskFileHtmlExporter.h: interface for the CTaskListHtmlExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKFILEHTMLEXPORTER_H__E4FD92AB_2BF2_40E3_9C8E_5018A72AEA89__INCLUDED_)
#define AFX_TASKFILEHTMLEXPORTER_H__E4FD92AB_2BF2_40E3_9C8E_5018A72AEA89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SHARED\Itasklist.h"
#include "..\SHARED\IImportExport.h"

class CTaskListHtmlExporter : public IExportTasklist  
{
public:
	CTaskListHtmlExporter();
	virtual ~CTaskListHtmlExporter();

	const char* GetMenuText() { return "Web Page"; }
	const char* GetFileFilter() { return "Web Pages (*.html)|*.html||"; }
	const char* GetFileExtension() { return "html"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath);
    void Release() { delete this; }

protected:
	CString DEFAULTFONT, HTMLNOTES;
	BOOL STRIKETHRUDONE, ROUNDTIMEFRACTIONS;

protected:
	CString& ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, CString& sOutput) const;
	CString FormatCharSet(const ITaskList6* pTasks) const;
	
	static BOOL FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName, 
								LPCTSTR szFormat, CString& sAttribText);
	static BOOL FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szNumAttribName, LPCTSTR szAttribName, 
                                          LPCTSTR szFormat, CString& sAttribText);
};

#endif // !defined(AFX_TASKFILEHTMLEXPORTER_H__E4FD92AB_2BF2_40E3_9C8E_5018A72AEA89__INCLUDED_)
