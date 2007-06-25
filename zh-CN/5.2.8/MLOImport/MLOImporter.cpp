// MLOImporter.cpp: implementation of the CMLOImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MLOImporter.h"
#include <time.h>
#include <unknwn.h>

#include "..\SHARED\iTasklist.h"
#include "..\SHARED\xmlfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMLOImporter::CMLOImporter()
{

}

CMLOImporter::~CMLOImporter()
{

}

bool CMLOImporter::Import(const char* szSrcFilePath, ITaskList* pDestTaskFile)
{
	ITaskList5* pTL5 = 0;
	pDestTaskFile->QueryInterface(IID_TASKLIST5, reinterpret_cast<void**>(&pTL5));

	CXmlFile file;

	if (!file.Load(szSrcFilePath, "MyLifeOrganized-xml"))
		return false;

	const CXmlItem* pXIMLOTree = file.GetItem("TaskTree");

	if (!pXIMLOTree)
		return false;

	const CXmlItem* pXIMLOTask = pXIMLOTree->GetItem("TaskNode");

	if (!pXIMLOTask)
		return false;

	// this first node always seems to be untitled 
	// so we get the first subnode
	pXIMLOTask = pXIMLOTask->GetItem("TaskNode");

	if (!pXIMLOTask)
		return true; // just means it was an empty tasklist
	
	return ImportTask(pXIMLOTask, pTL5, NULL); // NULL ==  root
}

bool CMLOImporter::ImportTask(const CXmlItem* pXIMLOTask, ITaskList5* pDestTaskFile, HTASKITEM hParent) const
{
	ASSERT (pXIMLOTask);

	HTASKITEM hTask = pDestTaskFile->NewTask(pXIMLOTask->GetItemValue("Caption"), hParent);

	if (!hTask)
		return false;

	// priority (== Importance)
	int nPriority = (pXIMLOTask->GetItemValueI("Importance") * 10) / 100;

	pDestTaskFile->SetTaskPriority(hTask, (unsigned char)nPriority);

	// categories (== places)
	const CXmlItem* pXIPlace = pXIMLOTask->GetItem("Places", "Place");

	while (pXIPlace)
	{
		pDestTaskFile->AddTaskCategory(hTask, pXIPlace->GetValue());
		pXIPlace = pXIPlace->GetSibling();
	}

	// estimated time (== EstimateMax)
	double dTimeEst = pXIMLOTask->GetItemValueF("EstimateMax"); // in days

	pDestTaskFile->SetTaskTimeEstimate(hTask, dTimeEst, 'D');

	// due date (== DueDateTime)
	pDestTaskFile->SetTaskDueDate(hTask, GetDate(pXIMLOTask, "DueDateTime"));

	// completion (== CompletionDateTime)
	pDestTaskFile->SetTaskDoneDate(hTask, GetDate(pXIMLOTask, "CompletionDateTime"));

	// comments (== Note)
	CString sComments = pXIMLOTask->GetItemValue("Note");

	pDestTaskFile->SetTaskComments(hTask, sComments);

	// children
	const CXmlItem* pXIMLOSubTask = pXIMLOTask->GetItem("TaskNode");

	if (pXIMLOSubTask)
	{
		if (!ImportTask(pXIMLOSubTask, pDestTaskFile, hTask))
			return false;
	}

	// siblings
	pXIMLOTask = pXIMLOTask->GetSibling();

	if (pXIMLOTask)
		return ImportTask(pXIMLOTask, pDestTaskFile, hParent);

	// else
	return true;
}

time_t CMLOImporter::GetDate(const CXmlItem* pXIMLOTask, LPCTSTR szDateField) const
{
	CString sDate = pXIMLOTask->GetItemValue(szDateField);

	if (!sDate.IsEmpty())
	{
		// parse date string
		int nYear = atoi(sDate);
		sDate = sDate.Mid(sDate.Find('-') + 1);

		int nMonth = atoi(sDate);
		sDate = sDate.Mid(sDate.Find('-') + 1);

		int nDay = atoi(sDate);

		// set date
		tm tDate = { 0,  0,  0, nDay, nMonth - 1, nYear - 1900, 0, 0, 0 };
		return mktime(&tDate);
	}

	return 0;
}
