// iCalExporter.cpp: implementation of the CiCalExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iCalImportExport.h"
#include "iCalExporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT ONEDAY = 24 * 60 * 60;

CiCalExporter::CiCalExporter()
{

}

CiCalExporter::~CiCalExporter()
{

}

bool CiCalExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath)
{
	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		// header
		WriteString(fileOut, "BEGIN:VCALENDAR");
		WriteString(fileOut, "PRODID:iCalExporter (c) AbstractSpoon 2005");
		WriteString(fileOut, "VERSION:1.0.0");

		// export first task
		ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), fileOut);

		// footer
		WriteString(fileOut, "END:VCALENDAR");

		return true;
	}

	return false;
}

void CiCalExporter::ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, CStdioFile& fileOut)
{
	if (!hTask)
		return;

	// header
	//	WriteString(fileOut, "BEGIN:VTODO");
	WriteString(fileOut, "BEGIN:VEVENT");
	
	// attributes
	time_t tDate = pSrcTaskFile->GetTaskStartDate(hTask);

	if (tDate)
	{
		COleDateTime date(tDate);
		WriteString(fileOut, "DTSTART;VALUE=DATE:%04d%02d%02d", date.GetYear(), date.GetMonth(), date.GetDay());
	}
	
	tDate = pSrcTaskFile->GetTaskLastModified(hTask);
	
	if (tDate)
	{
		COleDateTime date(tDate);
		WriteString(fileOut, "DTSTAMP;VALUE=DATE:%04d%02d%02d", date.GetYear(), date.GetMonth(), date.GetDay());
	}
	
	tDate = pSrcTaskFile->GetTaskDueDate(hTask);
	
	if (tDate)
	{
		COleDateTime date(tDate);
		WriteString(fileOut, "DUE;VALUE=DATE:%04d%02d%02d", date.GetYear(), date.GetMonth(), date.GetDay());
	}

	tDate = pSrcTaskFile->GetTaskDoneDate(hTask);
	
	if (tDate)
	{
		COleDateTime date(tDate);
		WriteString(fileOut, "DTEND;VALUE=DATE:%04d%02d%02d", date.GetYear(), date.GetMonth(), date.GetDay());
	}
	
	WriteString(fileOut, "SUMMARY:%s", pSrcTaskFile->GetTaskTitle(hTask));
	WriteString(fileOut, "DESCRIPTION:%s", pSrcTaskFile->GetTaskComments(hTask));
	WriteString(fileOut, "STATUS:%s", pSrcTaskFile->GetTaskStatus(hTask));
	WriteString(fileOut, "CATEGORIES:%s", pSrcTaskFile->GetTaskCategory(hTask));
	WriteString(fileOut, "URL:%s", pSrcTaskFile->GetTaskFileReferencePath(hTask));
	WriteString(fileOut, "ORGANIZER:%s", pSrcTaskFile->GetTaskAllocatedBy(hTask));
	WriteString(fileOut, "ATTENDEE:%s", pSrcTaskFile->GetTaskAllocatedTo(hTask));
	
	// use the filepath to assist in the uniqueness
	CString sFile = fileOut.GetFilePath();
	sFile.Replace("\\", "");
	sFile.Replace(":", "");
	
	WriteString(fileOut, "UID:%ld@%s.com", pSrcTaskFile->GetTaskID(hTask), sFile);
	
	// footer
	//		WriteString(fileOut, "END:VTODO");
	WriteString(fileOut, "END:VEVENT");

	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), fileOut);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), fileOut);
}

void __cdecl CiCalExporter::WriteString(CStdioFile& fileOut, LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));
	CString sLine;

	va_list argList;
	va_start(argList, lpszFormat);
	sLine.FormatV(lpszFormat, argList);
	va_end(argList);

	sLine.TrimRight();

	// write line out in pieces no longer than 75 bytes
	while (sLine.GetLength() > 75)
	{
		CString sTemp = sLine.Left(75);
		sLine = sLine.Mid(75);

		fileOut.WriteString(sTemp);
		fileOut.WriteString("\\n\n");
	}

	// write out whatever's left
	fileOut.WriteString(sLine);
	fileOut.WriteString("\n");
}
