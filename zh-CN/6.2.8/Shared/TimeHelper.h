// TimeHelper.h: interface for the TimeHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEHELPER_H__BA0C1E67_FAAA_4E65_8EF3_01B011ACFBBC__INCLUDED_)
#define AFX_TIMEHELPER_H__BA0C1E67_FAAA_4E65_8EF3_01B011ACFBBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

enum 
{
	THU_MINS	= 'I',
	THU_HOURS	= 'H',
	THU_DAYS	= 'D',
	THU_WEEKS	= 'W',
	THU_MONTHS	= 'M',
	THU_YEARS	= 'Y',
};

class CTimeHelper  
{
public:
	static double GetTime(double dTime, int nFromUnits, int nToUnits);
	static CString FormatTimeHMS(double dTime, int nUnitsFrom, BOOL bDecPlaces = TRUE);
	static CString FormatTime(double dTime, int nUnits, int nDecPlaces);
	static CString FormatTime(double dTime, int nDecPlaces);
	static BOOL SetHoursInOneDay(double dHours);
	static BOOL SetDaysInOneWeek(double dDays);
	static void SetUnits(int nUnits, LPCTSTR szUnits);
	static void SetUnits(int nUnits, char cUnits);
	static char GetUnits(int nUnits);

protected:
	static double HOURS2DAYS, DAYS2WEEKS; // user definable
	static CMap<int, int, char, char&> MAPUNIT2CH; // user definable

protected:
	static BOOL Compare(int nFromUnits, int nToUnits); // 0=same, -1=nFrom < nTo else 1
	static CString FormatTimeHMS(double dTime, int nUnits, int nLeftOverUnits, 
								   double dLeftOverMultiplier, BOOL bDecPlaces);
};

#endif // !defined(AFX_TIMEHELPER_H__BA0C1E67_FAAA_4E65_8EF3_01B011ACFBBC__INCLUDED_)
