// TDImportExportMgr.h: interface for the CTDLImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDIMPORTEXPORTMGR_H__0810C7FB_A895_4035_BEFA_B7097ABC2A7B__INCLUDED_)
#define AFX_TDIMPORTEXPORTMGR_H__0810C7FB_A895_4035_BEFA_B7097ABC2A7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SHARED\ImportExportMgr.h"

enum { EXP2HTML, EXP2TXT, EXP2CSV };

class CTDLImportExportMgr : public CImportExportMgr  
{
public:
	CTDLImportExportMgr();
	virtual ~CTDLImportExportMgr();

	BOOL ExportTaskListToHtml(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const;
	CString ExportTaskListToHtml(const ITaskList* pSrcTasks) const;

	BOOL ExportTaskListToText(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const;
	CString ExportTaskListToText(const ITaskList* pSrcTasks) const;

	BOOL ExportTaskListToCsv(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const;
	CString ExportTaskListToCsv(const ITaskList* pSrcTasks) const;

	void Initialize();
};

#endif // !defined(AFX_TDIMPORTEXPORTMGR_H__0810C7FB_A895_4035_BEFA_B7097ABC2A7B__INCLUDED_)
