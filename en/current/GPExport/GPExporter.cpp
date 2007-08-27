// GPExporter.cpp: implementation of the CGPExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GPExport.h"
#include "GPExporter.h"

#include "..\shared\xmlfileex.h"
#include "..\todolist\tdlschemadef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPExporter::CGPExporter()
{

}

CGPExporter::~CGPExporter() 
{

}

bool CGPExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath)
{
	CXmlFile fileDest("Tasks");

	// export first task
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), fileDest.Root());

	// save result
	VERIFY (fileDest.Save(szDestFilePath));

	return true;
}

void CGPExporter::ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestParent)
{
	if (!hTask)
		return;

	// create a new item corresponding to pXITask at the dest
	CXmlItem* pXIDestItem = pXIDestParent->AddItem("task");

	// copy across the appropriate attributes
	pXIDestItem->AddItem("id", (int)pSrcTaskFile->GetTaskID(hTask));
	pXIDestItem->AddItem("name", pSrcTaskFile->GetTaskTitle(hTask));

	time_t tStart = pSrcTaskFile->GetTaskStartDate(hTask);

	if (tStart)
	{
		COleDateTime date(tStart);
		pXIDestItem->AddItem("start", date.Format("%d/%m/%Y"));

		time_t tDue = pSrcTaskFile->GetTaskDueDate(hTask);

		if (tDue > tStart)
			pXIDestItem->AddItem("duration", (int)((tDue - tStart) / (24 * 60 * 60)));
	}

	time_t tDone = pSrcTaskFile->GetTaskDoneDate(hTask);

	if (tDone)
	{
		pXIDestItem->AddItem("complete", "100");
		pXIDestItem->AddItem("priority", "-1");
	}
	else
	{
		pXIDestItem->AddItem("complete", pSrcTaskFile->GetTaskPercentDone(hTask, TRUE));

		int nPriority = pSrcTaskFile->GetTaskPriority(hTask, TRUE);
		
		if (nPriority <= 3)
			pXIDestItem->AddItem("priority", 0); // low
		else if (nPriority <= 6)
			pXIDestItem->AddItem("priority", 1); // medium
		else
			pXIDestItem->AddItem("priority", 2); // high
	}
	
	// dependency
	CString sDepends = pSrcTaskFile->GetTaskAttribute(hTask, TDL_TASKDEPENDENCY);

	if (!sDepends.IsEmpty())
	{
		int nDelim = sDepends.Find('?');

		if (nDelim == -1) // must be a dependency in the same tasklist
		{
			CXmlItem* pXIDepends = pXIDestItem->AddItem("depend");

			pXIDepends->AddItem("id", sDepends);
			pXIDepends->AddItem("type", "3");
		}
	}

	// file/weblink
	CString sFileRef = pSrcTaskFile->GetTaskFileReferencePath(hTask);
	sFileRef.TrimLeft();

	if (!sFileRef.IsEmpty())
	{
		// web or file link?
		if (sFileRef.Find(":\\") == 1 || sFileRef.Find("\\\\") == 0)
			sFileRef = "file://" + sFileRef;

		sFileRef.Replace(" ", "%20");

		pXIDestItem->AddItem("webLink", sFileRef);
	}

	// comments
	const char* szComments = pSrcTaskFile->GetTaskComments(hTask);

	if (szComments && *szComments)
		pXIDestItem->AddItem("notes", szComments, TRUE);
	
	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), pXIDestItem);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), pXIDestParent);
}
