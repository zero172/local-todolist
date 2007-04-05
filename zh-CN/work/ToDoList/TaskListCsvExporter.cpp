// TaskListCsvExporter.cpp: implementation of the CTaskListCsvExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskListCsvExporter.h"
#include "tdlschemadef.h"
#include "resource.h"
#include "recurringtaskedit.h"

#include <locale.h>

#include "..\shared\timehelper.h"
#include "..\shared\enstring.h"
#include "..\shared\misc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR SPACE = " ";
const LPCTSTR ENDL = "\n";
const LPCTSTR ONEDBLQUOTE = "\"";
const LPCTSTR TWODBLQUOTE = "\"\"";

CTaskListCsvExporter::CTaskListCsvExporter()
{

}

CTaskListCsvExporter::~CTaskListCsvExporter()
{

}

bool CTaskListCsvExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath)
{
	const ITaskList6* pTasks6 = NULL;
	
	if ((const_cast<ITaskList*>(pSrcTaskFile))->QueryInterface(IID_TASKLIST6, (void**)&pTasks6))
		return false;

	DELIM = Misc::GetListSeparator();
	
	// what follows is definitely a hack!
	// but that's the joy of programming :)
	ROUNDTIMEFRACTIONS = AfxGetApp()->GetProfileInt("Preferences", "RoundTimeFractions", FALSE);

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		BuildAttribList(pTasks6, NULL);
		
		CString sOutput;

		if (!ExportTask(pTasks6, NULL, 0, 0, "", sOutput).IsEmpty())
		{
			fileOut.WriteString(ColumnHeadings());
			fileOut.WriteString(sOutput);
			return true;
		}
	}

	return false;
}

CString& CTaskListCsvExporter::ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, 
										  int nPos, const CString& sParentPos, CString& sOutput) const
{
	CString sPos; // becomes parent pos for sub-tasks

	// handle locale specific decimal separator
	setlocale(LC_NUMERIC, "");

	if (hTask)
	{
		// same order as columns

		if (WantAttribute(TDL_TASKPOS))
		{
			// if there is a POS child item then this replaces nPos
			if (pTasks->TaskHasAttribute(hTask, TDL_TASKPOS))
			{
				nPos = pTasks->GetTaskPosition(hTask);

				if (nDepth == 1) // top level tasks
					sPos.Format("%d", nPos);
				else
					sPos.Format("%s.%d", sParentPos, nPos);
			}

			AppendAttribute(sPos, sOutput);
		}

		AppendAttribute(pTasks, hTask, TDL_TASKID, NULL, sOutput);

        // title is slightly different because we indent it for subtasks
        CString sPrefix(' ', (nDepth > 1) ? (nDepth - 1) * 4 : 0);
		AppendAttribute(pTasks, hTask, TDL_TASKTITLE, NULL, sOutput, sPrefix);
      
		AppendAttribute(pTasks, hTask, TDL_TASKCALCCOMPLETION, TDL_TASKPERCENTDONE, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKHIGHESTPRIORITY, TDL_TASKPRIORITY, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCALCTIMEESTIMATE, TDL_TASKTIMEESTIMATE, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCALCTIMESPENT, TDL_TASKTIMESPENT, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCREATIONDATESTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCREATEDBY, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKSTARTDATESTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKDUEDATESTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKEARLIESTDUEDATESTRING, TDL_TASKDUEDATESTRING, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKDONEDATESTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKALLOCTO, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKALLOCBY, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKVERSION, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKRECURRENCE, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCATEGORY, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKSTATUS, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKHIGHESTRISK, TDL_TASKRISK, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKEXTERNALID, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKLASTMODSTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCALCCOST, TDL_TASKCOST, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKFILEREFPATH, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKDEPENDENCY, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCOMMENTS, NULL, sOutput);
	}

	// add sub-tasks
	hTask = pTasks->GetFirstTask(hTask);

	if (hTask) // at least one sub-task
	{
		int nChildPos = 1;

		while (hTask)
		{
			CString sTask;
			sOutput += ENDL;
			sOutput += ExportTask(pTasks, hTask, nDepth + 1, nChildPos++, sPos, sTask);

			hTask = pTasks->GetNextTask(hTask);
		}
	}

	// extra line between top level items
	if (nDepth == 1)
		sOutput += ENDL;

	// restore decimal separator to '.'
	setlocale(LC_NUMERIC, "English");

	return sOutput;
}

void CTaskListCsvExporter::CheckAddAttribtoList(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName)
{
	if (pTasks->TaskHasAttribute(hTask, szAttribName))
	{
		// have we already got it ?
		if (!WantAttribute(szAttribName))
			m_aAttribs.Add(szAttribName); // add it
	}
}

void CTaskListCsvExporter::AppendAttribute(const ITaskList6* pTasks, HTASKITEM hTask, 
										            LPCTSTR szAttribName, LPCTSTR szAltAttribName, 
                                          CString& sOutput, LPCTSTR szPrefix) const
{
	if (WantAttribute(szAttribName) || (szAltAttribName && WantAttribute(szAltAttribName)))
	{
		if (pTasks->TaskHasAttribute(hTask, szAttribName))
      {
         CString sAttrib(szPrefix);
         sAttrib += pTasks->GetTaskAttribute(hTask, szAttribName);

			AppendAttribute(sAttrib, sOutput);
      }
		else if (szAltAttribName && pTasks->TaskHasAttribute(hTask, szAltAttribName))
      {
         CString sAttrib(szPrefix);
         sAttrib += pTasks->GetTaskAttribute(hTask, szAltAttribName);

			AppendAttribute(sAttrib, sOutput);
      }
		else
			sOutput += DELIM;
	}
}

void CTaskListCsvExporter::AppendAttribute(LPCTSTR szAttrib, CString& sOutput) const
	{
	BOOL bNeedQuoting = FALSE;
	CString sAttrib(szAttrib);
	
	// double up quotes
	if (sAttrib.Find(ONEDBLQUOTE) != -1)
	{
		sAttrib.Replace(ONEDBLQUOTE, TWODBLQUOTE);
		bNeedQuoting = TRUE;
	}

	if (sAttrib.Find(DELIM) != -1)
		bNeedQuoting = TRUE;
	
	if (bNeedQuoting)
		sAttrib = ONEDBLQUOTE + sAttrib + ONEDBLQUOTE;
	
	// replace carriage returns
	sAttrib.Replace(ENDL, SPACE);
	
	sAttrib += DELIM;
	sOutput += sAttrib;
}

void CTaskListCsvExporter::AppendAttribute(double dAttrib, LPCTSTR szFormat, CString& sOutput) const
{
	CString sAttrib;

	sAttrib.Format(szFormat, dAttrib);
	AppendAttribute(sAttrib, sOutput);
}

void CTaskListCsvExporter::AppendAttribute(int nAttrib, LPCTSTR szFormat, CString& sOutput) const
{
	CString sAttrib;

	sAttrib.Format(szFormat, nAttrib);
	AppendAttribute(sAttrib, sOutput);
}

CString CTaskListCsvExporter::CheckGetColumnHeading(LPCTSTR szAttribName, UINT nIDHeading) const
{
	if (WantAttribute(szAttribName))
		return CEnString(nIDHeading) + DELIM;

	// else
	return "";
}

CString CTaskListCsvExporter::ColumnHeadings() const
{
	ASSERT (m_aAttribs.GetSize());

	CString sHeadings;

	sHeadings += CheckGetColumnHeading(TDL_TASKPOS, IDS_TDLBC_POS);
	sHeadings += CheckGetColumnHeading(TDL_TASKID, IDS_TDLBC_ID);
	sHeadings += CheckGetColumnHeading(TDL_TASKTITLE, IDS_TDLBC_TITLE);
	sHeadings += CheckGetColumnHeading(TDL_TASKPERCENTDONE, IDS_TDLBC_PERCENT);
	sHeadings += CheckGetColumnHeading(TDL_TASKPRIORITY, IDS_TDLBC_PRIORITY);
	sHeadings += CheckGetColumnHeading(TDL_TASKTIMEESTIMATE, IDS_TDLBC_TIMEEST);
	sHeadings += CheckGetColumnHeading(TDL_TASKTIMESPENT, IDS_TDLBC_TIMESPENT);
	sHeadings += CheckGetColumnHeading(TDL_TASKCREATIONDATESTRING, IDS_TDLBC_CREATEDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKCREATEDBY, IDS_TDLBC_CREATEDBY);
	sHeadings += CheckGetColumnHeading(TDL_TASKSTARTDATESTRING, IDS_TDLBC_STARTDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKDUEDATESTRING, IDS_TDLBC_DUEDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKDONEDATESTRING, IDS_TDLBC_DONEDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKALLOCTO, IDS_TDLBC_ALLOCTO);
	sHeadings += CheckGetColumnHeading(TDL_TASKALLOCBY, IDS_TDLBC_ALLOCBY);
	sHeadings += CheckGetColumnHeading(TDL_TASKVERSION, IDS_TDLBC_VERSION);
	sHeadings += CheckGetColumnHeading(TDL_TASKRECURRENCE, IDS_TDLBC_RECURRENCE);
	sHeadings += CheckGetColumnHeading(TDL_TASKCATEGORY, IDS_TDLBC_CATEGORY);
	sHeadings += CheckGetColumnHeading(TDL_TASKSTATUS, IDS_TDLBC_STATUS);
	sHeadings += CheckGetColumnHeading(TDL_TASKRISK, IDS_TDLBC_RISK);
	sHeadings += CheckGetColumnHeading(TDL_TASKEXTERNALID, IDS_TDLBC_EXTERNALID);
	sHeadings += CheckGetColumnHeading(TDL_TASKLASTMODSTRING, IDS_TDLBC_MODIFYDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKCOST, IDS_TDLBC_COST);
	sHeadings += CheckGetColumnHeading(TDL_TASKFILEREFPATH, IDS_TDLBC_FILEREF);
	sHeadings += CheckGetColumnHeading(TDL_TASKDEPENDENCY, IDS_TDLBC_DEPENDS);
	sHeadings += CheckGetColumnHeading(TDL_TASKCOMMENTS, IDS_TDLBC_COMMENTS);

	return sHeadings;
}

int CTaskListCsvExporter::BuildAttribList(const ITaskList6* pTasks, HTASKITEM hTask)
{
	if (hTask)
	{
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKPOS);					
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKID);					
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKTITLE);				
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCOMMENTS); 			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKALLOCTO);				
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKALLOCBY);				
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCATEGORY); 			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKSTATUS);				
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKFILEREFPATH);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCREATEDBY);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKFLAG);				
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKDONEDATESTRING);		
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKDUEDATESTRING);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKSTARTDATESTRING);		
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCREATIONDATESTRING);	
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKTIMEESTIMATE);	
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKTIMESPENT);		
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKPERCENTDONE);		
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKPRIORITY);		
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKRISK);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKLASTMODSTRING);		
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKEXTERNALID);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCOST);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKDEPENDENCY);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKVERSION);			
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKRECURRENCE);			
	}

	// subtasks
	hTask = pTasks->GetFirstTask(hTask);

	while (hTask) // at least one sub-task
	{
		BuildAttribList(pTasks, hTask);

		// next subtask
		hTask = pTasks->GetNextTask(hTask);
	}

	return m_aAttribs.GetSize();
}

BOOL CTaskListCsvExporter::WantAttribute(LPCTSTR szAttribName) const
{
	int nAttrib = m_aAttribs.GetSize();

	while (nAttrib--)
	{
		if (m_aAttribs[nAttrib] == szAttribName)
			return TRUE;
	}

	// nope
	return FALSE;
}
