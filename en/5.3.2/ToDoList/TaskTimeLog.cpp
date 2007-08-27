// TaskTimeLog.cpp: implementation of the CTaskTimeLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskTimeLog.h"

#include "..\shared\Filemisc.h"
#include "..\shared\misc.h"
#include "..\shared\datehelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPCTSTR COLUMNHEADINGS = "Task ID, Time Spent (Hrs), User ID, End Date/Time, Start Date/Time";
LPCTSTR LOGFORMAT = "%ld, %.3f, %s, %s %s, %s %s";

CTaskTimeLog::CTaskTimeLog(LPCTSTR szRefPath) : m_sRefPath(szRefPath)
{
}

CTaskTimeLog::~CTaskTimeLog()
{

}

BOOL CTaskTimeLog::LogTime(DWORD dwTaskID, double dTime, BOOL bLogSeparately)
{
	CString sLogPath = GetLogPath(dwTaskID, bLogSeparately);

	// if the file doesn't exist then we insert the column headings as the first line
	if (!FileMisc::FileExists(sLogPath))
		FileMisc::AppendLineToFile(sLogPath, COLUMNHEADINGS);

	// then log the time spent
	CString sLog;

	COleDateTime dtEnd = COleDateTime::GetCurrentTime();
	COleDateTime dtStart = dtEnd - COleDateTime(dTime / 24); // dTime is in hours

	sLog.Format(LOGFORMAT, 
				dwTaskID, 
				dTime, 
				Misc::GetUserName(),
				CDateHelper::FormatDate(dtEnd, TRUE), 
				dtEnd.Format(VAR_TIMEVALUEONLY),
				CDateHelper::FormatDate(dtStart, TRUE), 
				dtStart.Format(VAR_TIMEVALUEONLY));

	return FileMisc::AppendLineToFile(sLogPath, sLog);
}

CString CTaskTimeLog::GetLogPath(DWORD dwTaskID, BOOL bLogSeparately)
{
	CString sLogPath, sDrive, sFolder, sFileName;

	// use ref filename as the basis for the log filename
	FileMisc::SplitPath(m_sRefPath, &sDrive, &sFolder, &sFileName);
	
	if (bLogSeparately)
		sLogPath.Format("%s%s%s\\%ld_Log.csv", sDrive, sFolder, sFileName, dwTaskID);
	else
		sLogPath.Format("%s%s%s_Log.csv", sDrive, sFolder, sFileName);

	return sLogPath;
}

double CTaskTimeLog::CalcAccumulatedTime(DWORD dwTaskID, BOOL bLogSeparately)
{
	double dTotalTime = 0;
	CStdioFile file;
	CString sLogPath = GetLogPath(dwTaskID, bLogSeparately);

	if (file.Open(sLogPath, CFile::modeRead | CFile::typeText))
	{
		CString sLine;
		double dLogTime;
		DWORD dwLogID;

		while (file.ReadString(sLine))
		{
			// decode it
			//fabio_2005
#if _MSC_VER >= 1400
			if (sscanf_s("%ld,%.3f", sLine, &dwLogID, &dLogTime) == 2)
#else
			if (sscanf("%ld,%.3f", sLine, &dwLogID, &dLogTime) == 2)
#endif

			{
				if (dwLogID == dwTaskID)
					dTotalTime += dLogTime;
			}
		}
	}

	return dTotalTime;
}
