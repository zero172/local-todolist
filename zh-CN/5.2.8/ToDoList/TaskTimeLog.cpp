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

CTaskTimeLog::CTaskTimeLog(LPCTSTR szRefPath) : m_sRefPath(szRefPath)
{
}

CTaskTimeLog::~CTaskTimeLog()
{

}

BOOL CTaskTimeLog::LogTime(DWORD dwTaskID, double dTime, BOOL bLogSeparately)
{
	CString sLog;

	sLog.Format("%ld,%.3f,%s,%s", dwTaskID, dTime, Misc::GetUserName(),
				CDateHelper::FormatCurrentDate(TRUE));
	
	return FileMisc::AppendLineToFile(GetLogPath(dwTaskID, bLogSeparately), sLog);
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
