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
	CString sOutputPath(szDestFilePath);
	sOutputPath.MakeLower();

	// inset .gp before the xml extension unless
	// we are replacing an existing file
	if (sOutputPath.Find(".gp.xml") == -1)
	{
		if (!sOutputPath.Replace(".xml", ".gp.xml"))
			sOutputPath += ".gp.xml";
	}
	
	VERIFY (fileDest.Save(sOutputPath));

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
		pXIDestItem->AddItem("priority", pSrcTaskFile->GetTaskPriority(hTask, TRUE));
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

	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), pXIDestItem);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), pXIDestParent);
}
