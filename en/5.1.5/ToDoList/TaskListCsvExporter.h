// TaskListCsvExporter.h: interface for the CTaskListCsvExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
#define AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SHARED\Itasklist.h"
#include "..\SHARED\IImportExport.h"

class CTaskListCsvExporter : public IExportTasklist  
{
public:
	CTaskListCsvExporter();
	virtual ~CTaskListCsvExporter();

	const char* GetMenuText() { return "Spreadsheet"; }
	const char* GetFileFilter() { return "Spreadsheet Files (*.csv)|*.csv||"; }
	const char* GetFileExtension() { return "csv"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath);
    void Release() { delete this; }

protected:
	BOOL ROUNDTIMEFRACTIONS;
	LPCTSTR DELIM;
	CStringArray m_aAttribs;

protected:
	CString& ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, 
						int nPos, const CString& sParentPos, CString& sOutput) const;
	CString ColumnHeadings() const;

	void AppendAttribute(const ITaskList6* pTasks, HTASKITEM hTask, 
						LPCTSTR szAttribName, LPCTSTR szAltAttribName, 
                        CString& sOutput, LPCTSTR szPrefix = NULL) const;
	void AppendAttribute(LPCTSTR szAttrib, CString& sOutput) const;
	void AppendAttribute(double dAttrib, LPCTSTR szFormat, 
								CString& sOutput) const;
	void AppendAttribute(int nAttrib, LPCTSTR szFormat, CString& sOutput) const;

	int BuildAttribList(const ITaskList6* pTasks, HTASKITEM hTask);
	BOOL WantAttribute(LPCTSTR szAttribName) const;
	void CheckAddAttribtoList(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName);

	CString CheckGetColumnHeading(LPCTSTR szAttribName, UINT nIDHeading) const;

};

#endif // !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
