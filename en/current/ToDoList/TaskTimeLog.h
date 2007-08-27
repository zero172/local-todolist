// TaskTimeLog.h: interface for the CTaskTimeLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKTIMELOG_H__6C9F21CD_509E_4890_9B28_F8C6E52FF54B__INCLUDED_)
#define AFX_TASKTIMELOG_H__6C9F21CD_509E_4890_9B28_F8C6E52FF54B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTaskTimeLog  
{
public:
	CTaskTimeLog(LPCTSTR szRefPath);
	virtual ~CTaskTimeLog();

	BOOL LogTime(DWORD dwTaskID, double dTime, BOOL bLogSeparately); // time must be in hours
	double CalcAccumulatedTime(DWORD dwTaskID, BOOL bLogSeparately); // returns time in hours
	CString GetLogPath(DWORD dwTaskID, BOOL bLogSeparately);

protected:
	CString m_sRefPath;

protected: 
};

#endif // !defined(AFX_TASKTIMELOG_H__6C9F21CD_509E_4890_9B28_F8C6E52FF54B__INCLUDED_)
