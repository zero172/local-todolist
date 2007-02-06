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
	CString sLogPath;

	// use ref filename as the basis for the log filename
	char szDrive[_MAX_DRIVE], szFolder[_MAX_DIR], szFileName[_MAX_FNAME];
	_splitpath(m_sRefPath, szDrive, szFolder, szFileName, NULL);
	
	if (bLogSeparately)
		sLogPath.Format("%s%s%s\\%ld_Log.csv", szDrive, szFolder, szFileName, dwTaskID);
	else
		sLogPath.Format("%s%s%s_Log.csv", szDrive, szFolder, szFileName);

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
			if (sscanf("%ld,%.3f", sLine, &dwLogID, &dLogTime) == 2)
			{
				if (dwLogID == dwTaskID)
					dTotalTime += dLogTime;
			}
		}
	}

	return dTotalTime;
}
