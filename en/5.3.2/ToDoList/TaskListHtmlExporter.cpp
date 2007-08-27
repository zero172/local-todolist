// TaskFileHtmlExporter.cpp: implementation of the CTaskListHtmlExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "todolist.h"
#include "TasklistHtmlExporter.h"
#include "tdlschemadef.h"
#include "recurringtaskedit.h"

#include "..\shared\xmlfile.h"

#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = "\n";

CTaskListHtmlExporter::CTaskListHtmlExporter()
{
}

CTaskListHtmlExporter::~CTaskListHtmlExporter()
{

}

bool CTaskListHtmlExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath)
{
	// what follows is definitely a hack!
	// but that's the joy of programming :)
	CString sFont = AfxGetApp()->GetProfileString("Preferences", "HTMLFont", "Verdana");
	int nFontSize = AfxGetApp()->GetProfileInt("Preferences", "HtmlFontSize", 3);

	DEFAULTFONT.Format("<font face='%s' size='%d'>", sFont, nFontSize);

	HTMLNOTES = AfxGetApp()->GetProfileInt("Preferences", "ExportSpaceForNotes", FALSE) ? 
										"<pre>\n\n\n\n\n\n\n\n</pre>" : "";
	STRIKETHRUDONE = AfxGetApp()->GetProfileInt("Preferences", "StrikethroughDone", TRUE);
	ROUNDTIMEFRACTIONS = AfxGetApp()->GetProfileInt("Preferences", "RoundTimeFractions", FALSE);

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		CString sOutput;
		const ITaskList6* pTasks6 = static_cast<const ITaskList6*>(pSrcTaskFile);

		if (!ExportTask(pTasks6, NULL, 0, 0, sOutput).IsEmpty())
		{
			fileOut.WriteString("<html>\n<head>\n");
			fileOut.WriteString(FormatCharSet(pTasks6));
			fileOut.WriteString("</head>\n<body>\n");
			fileOut.WriteString(sOutput);
			fileOut.WriteString("</body>\n</html>\n");
			return true;
		}
	}

	return false;
}

CString CTaskListHtmlExporter::FormatCharSet(const ITaskList6* pTasks) const
{
	// charset
	CString sCharSet, sCS = pTasks->GetHtmlCharSet();

	if (!sCS.IsEmpty())
		sCharSet.Format("<meta http-equiv=\"content-type\" content=\"text/html; charset=%s\">\n", sCS);

	return sCharSet;
}

CString& CTaskListHtmlExporter::ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, CString& sOutput) const
{
	// handle locale specific decimal separator
	setlocale(LC_NUMERIC, "");

	// if depth == 0 then we're at the root so just add sub-tasks
	// else insert pItem first
	if (nDepth > 0)
	{
		// if there is a POS child item then this replaces nPos
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKPOS))
			nPos = pTasks->GetTaskPosition(hTask);

		CString sTitle, sID, sItem, sPriority, sStartDate, sDueDate, sDoneDate;
		CString sAllocTo, sAllocBy, sCategory, sStatus, sColor("#000000");
		CString sComments, sPercent, sTimeEst, sTimeSpent, sFileRef;
		CString sLastMod, sRisk, sExtID, sCreateDate, sCreateBy, sCost;
		CString sVersion, sRecurrence;
		char cTemp;

		// ID
		FormatAttribute(pTasks, hTask, TDL_TASKID, "(ID: %s) ", sID);

		// title
		if (nDepth == 1) // toplevel == bold
			sTitle.Format("<b>%s</b>", pTasks->GetTaskTitle(hTask));
		else
			sTitle = pTasks->GetTaskTitle(hTask);

		// priority, start/due/done dates
		int nPercent = pTasks->GetTaskPercentDone(hTask, TRUE);
		BOOL bDone = pTasks->IsTaskDone(hTask) || (nPercent == 100);

		// format color string
		if (bDone)
			sColor = "#c4c4c4";
		else
			sColor = pTasks->GetTaskWebColor(hTask);

		if (bDone && pTasks->TaskHasAttribute(hTask, TDL_TASKDONEDATESTRING))
			sDoneDate.Format(" (completed: %s)", pTasks->GetTaskDoneDateString(hTask));
		else
		{
			if (pTasks->TaskHasAttribute(hTask, TDL_TASKHIGHESTPRIORITY) ||
				pTasks->TaskHasAttribute(hTask, TDL_TASKPRIORITY))
			{
				CString sPriorityCol = pTasks->GetTaskPriorityWebColor(hTask);
				sPriority.Format("[<font color='%s'>%d</font>] ", sPriorityCol, pTasks->GetTaskPriority(hTask, TRUE));
			}

			FormatAttribute(pTasks, hTask, TDL_TASKSTARTDATESTRING, " (start: %s)", sStartDate);
			FormatAttribute(pTasks, hTask, TDL_TASKDUEDATESTRING, " (due: %s)", sDueDate);

			int nTimePlaces = ROUNDTIMEFRACTIONS ? 0 : 2;

			if (pTasks->TaskHasAttribute(hTask, TDL_TASKTIMEESTIMATE) ||
				pTasks->TaskHasAttribute(hTask, TDL_TASKCALCTIMEESTIMATE))
				sTimeEst.Format(" (time est: %.*f hrs)", nTimePlaces, pTasks->GetTaskTimeEstimate(hTask, cTemp, TRUE));

			if (pTasks->TaskHasAttribute(hTask, TDL_TASKTIMESPENT) ||
				pTasks->TaskHasAttribute(hTask, TDL_TASKCALCTIMESPENT))
				sTimeEst.Format(" (time spent: %.*f hrs)", nTimePlaces, pTasks->GetTaskTimeSpent(hTask, cTemp, TRUE));

			if (pTasks->TaskHasAttribute(hTask, TDL_TASKPERCENTDONE))
				sPercent.Format(" (%d%%) ", nPercent);
		}

		FormatAttribute(pTasks, hTask, TDL_TASKCREATIONDATESTRING, " (created: %s)", sCreateDate);
		FormatAttribute(pTasks, hTask, TDL_TASKCREATEDBY, " (created by: %s)", sCreateBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, " (allocated to: %s)", sAllocTo);
		FormatAttribute(pTasks, hTask, TDL_TASKALLOCBY, " (allocated by: %s)", sAllocBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, " (category: %s)", sCategory);
		FormatAttribute(pTasks, hTask, TDL_TASKSTATUS, " (status: %s)", sStatus);
		FormatAttribute(pTasks, hTask, TDL_TASKRISK, " (risk: %s)", sRisk);
		FormatAttribute(pTasks, hTask, TDL_TASKEXTERNALID, " (ext.ID: %s)", sExtID);
		FormatAttribute(pTasks, hTask, TDL_TASKLASTMODSTRING, " (last mod: %s)", sLastMod);
		FormatAttribute(pTasks, hTask, TDL_TASKCOST, " (cost: %.2f)", sCost);
		FormatAttribute(pTasks, hTask, TDL_TASKVERSION, " (version: %s)", sVersion);
		FormatAttribute(pTasks, hTask, TDL_TASKRECURRENCE, " (occurs: %s)", sRecurrence);

		// fileref
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKFILEREFPATH))
		{
			CString sFilePath = pTasks->GetTaskFileReferencePath(hTask);
			sFileRef.Format("<br>(link: <a href=\"%s\">%s</a>)", sFilePath, sFilePath);
		}
		
		// comments
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKHTMLCOMMENTS))
		{
			CString sItemComments = pTasks->GetTaskAttribute(hTask, TDL_TASKHTMLCOMMENTS);
			sItemComments.TrimRight();

			if (!sItemComments.IsEmpty())
			{
				// note: we reset the font after the comments because the font
				// face and size may well have been changed
				sComments.Format(bDone ? 
							"<br><font color='#c4c4c4'>[</font>%s%s<font color='#c4c4c4'>]</font>" : "<br>[%s%s]", 
							sItemComments, DEFAULTFONT);
			}
		}
		else if (pTasks->TaskHasAttribute(hTask, TDL_TASKCOMMENTS))
		{
			CString sItemComments = pTasks->GetTaskComments(hTask);
			sItemComments.TrimRight();

			if (!sItemComments.IsEmpty())
			{
				TXT2XML(sItemComments); // TODO

				sComments.Format(bDone ? "<br><font color='#c4c4c4'>[%s] </font>" : "<br><font color='#606060'>[%s] </font>", 
								sItemComments);

				// replace carriage returns with <br>
				sComments.Replace(ENDL, "<br>");
				sComments.Replace("\n", "<br>");

				// replace tab characters with multiple &nbsp;
				sComments.Replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
			}
		}

		CString sFormat;
		
		if (bDone)
		{
			if (STRIKETHRUDONE)
				sFormat = "%s%s%s<font color='%s'><s>%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s</s></font>%s<s>%s</s>%s";
			else
				sFormat = "%s%s%s<font color='%s'>%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s</font>%s%s%s";
		}
		else
			sFormat = "%s%s%s<font color='%s'>%s</font>%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s";

		sItem.Format(sFormat, sID, sPriority, sPercent, sColor, sTitle, sRisk, 
					sAllocTo, sAllocBy, sVersion, sRecurrence, sCategory, sStatus, 
					sCreateDate, sCreateBy, sStartDate, sDueDate, sTimeEst, 
					sCost, sDoneDate, sExtID, sLastMod, sComments, sFileRef);

		// notes section
		if (!bDone)
			sItem += HTMLNOTES;

		if (nDepth == 1) // toplevel
			sOutput += "<br>";

		sOutput += "<li>";
		sOutput += sItem;
	}
	else
	{
		sOutput += DEFAULTFONT;

		// title and date
		CString sTitle = pTasks->GetReportTitle();
		CString sDate = pTasks->GetReportDate();

		if (!sTitle.IsEmpty())
		{
			CString sProjTitle;
			sProjTitle.Format("<h2>%s</h2>%s<br>", sTitle, sDate);
			sOutput += sProjTitle;
		}
		else if (!sDate.IsEmpty())
		{
			sOutput += sDate;
			sOutput += "<br>";
		}

	}

	// begin new ordered list for sub-tasks
	hTask = pTasks->GetFirstTask(hTask);

	if (hTask) // at least one sub-task
	{
		sOutput += ENDL;
		sOutput += "<ol>";
		sOutput += ENDL;

		int nChildPos = 1;

		while (hTask)
		{
			CString sTask;
			sOutput += ENDL;
			sOutput += ExportTask(pTasks, hTask, nDepth + 1, nChildPos++, sTask);

			hTask = pTasks->GetNextTask(hTask);
		}

		// end ordered list
		sOutput += "</ol>";
		sOutput += ENDL;
	}

	// end of item
	if (nDepth > 0)
	{
		sOutput += "</li>";
		sOutput += ENDL;
	}
	else
		sOutput += "</font>"; // we're done

	// restore decimal separator to '.'
	setlocale(LC_NUMERIC, "English");

	return sOutput;
}

BOOL CTaskListHtmlExporter::FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szAttribName, LPCTSTR szFormat, CString& sAttribText)
{
	if (pTasks->TaskHasAttribute(hTask, szAttribName))
	{
		sAttribText.Format(szFormat, pTasks->GetTaskAttribute(hTask, szAttribName));
		return TRUE;
	}

	return FALSE;
}

BOOL CTaskListHtmlExporter::FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szNumAttribName, LPCTSTR szAttribName, 
                                           LPCTSTR szFormat, CString& sAttribText)
{
	int nItemCount = atoi(pTasks->GetTaskAttribute(hTask, szNumAttribName));

	if (nItemCount <= 1)
		return FormatAttribute(pTasks, hTask, szAttribName, szFormat, sAttribText);

	// else more than one (use plus sign as delimiter)
	CString sAttribs = pTasks->GetTaskAttribute(hTask, szAttribName);
	
	for (int nItem = 1; nItem < nItemCount; nItem++)
	{
		CString sAttribName;
		sAttribName.Format("%s%d", szAttribName, nItem);
		
		sAttribs += '+';
		sAttribs += pTasks->GetTaskAttribute(hTask, sAttribName);
	}
	
	sAttribText.Format(szFormat, sAttribs);
	return TRUE;
}

