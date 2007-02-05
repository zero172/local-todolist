// DateHelper.cpp: implementation of the CDateHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DateHelper.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CDateHelper::CalcDaysFromTo(const COleDateTime& dateFrom, const COleDateTime& dateTo, BOOL bInclusive)
{
	int nDiff = (int)dateTo.m_dt - (int)dateFrom.m_dt;

	return nDiff + (bInclusive ? 1 : 0);
}

int CDateHelper::CalcDaysFromTo(const COleDateTime& dateFrom, DH_DATE nTo, BOOL bInclusive)
{
	return CalcDaysFromTo(dateFrom, GetDate(nTo), bInclusive);
}

int CDateHelper::CalcDaysFromTo(DH_DATE nFrom, DH_DATE nTo, BOOL bInclusive)
{
	ASSERT (nFrom <= nTo);

	if (nFrom > nTo)
		return 0;
	
	else if (nFrom == nTo)
		return bInclusive ? 1 : 0;

	// else
	return CalcDaysFromTo(GetDate(nFrom), GetDate(nTo), bInclusive);
}

double CDateHelper::GetDate(DH_DATE nDate)
{
	COleDateTime date;

	switch (nDate)
	{
	case DHD_TODAY:
		date = COleDateTime::GetCurrentTime();
		break;

	case DHD_TOMORROW:
		date = GetDate(DHD_TODAY) + 1;
		break;

	case DHD_ENDTHISWEEK:
		{
			// we must get the locale info to find out when this 
			// user's week starts
			date = COleDateTime::GetCurrentTime();
			date -= date.GetDayOfWeek() - FirstDayOfWeek(); // start of week
			date += 6;
		}
		break;

	case DHD_ENDNEXTWEEK:
		return GetDate(DHD_ENDTHISWEEK) + 7;

	case DHD_ENDTHISMONTH:
		{
			date = COleDateTime::GetCurrentTime();
			int nThisMonth = date.GetMonth();

			while (date.GetMonth() == nThisMonth)
				date += 20; // much quicker than doing it one day at a time

			date -= date.GetDay(); // because we went into next month
		}
		break;

	case DHD_ENDNEXTMONTH:
		{
			date = GetDate(DHD_ENDTHISMONTH) + 1; // first day of next month
			int nNextMonth = date.GetMonth();

			while (date.GetMonth() == nNextMonth)
				date += 20; // much quicker than doing it one day at a time

			date -= date.GetDay(); // because we went into next month + 1
		}
		break;

	case DHD_ENDTHISYEAR:
		date = COleDateTime::GetCurrentTime(); // for current year
		date = COleDateTime(date.GetYear(), 12, 31, 0, 0, 0);
		break;

	case DHD_ENDNEXTYEAR:
		date = COleDateTime::GetCurrentTime(); // for current year
		date = COleDateTime(date.GetYear() + 1, 12, 31, 0, 0, 0);
		break;

	default:
		ASSERT (0);
		date.m_dt = 0;
		break;
	}

	return (double)(int)date;
}

int CDateHelper::CalcWeekdaysFromTo(const COleDateTime& dateFrom, const COleDateTime& dateTo, BOOL bInclusive)
{
	int nWeekdays = 0;
	
	COleDateTime dFrom(floor(dateFrom.m_dt));
	COleDateTime dTo(floor(dateTo.m_dt));

	if (bInclusive)
		dTo += 1;

	int nDiff = (int)(double)(dTo - dFrom);

	if (nDiff > 0)
	{
		while (dFrom < dTo)
		{
			int nDOW = dFrom.GetDayOfWeek();

			if (nDOW != 1 && nDOW != 7)
				nWeekdays++;

			dFrom += 1;
		}
	}

	return nWeekdays;
}

int CDateHelper::FirstDayOfWeek()
{
	char szFDW[3]; // 2 + NULL

	::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szFDW, 2);

	int nFirstDOW = atoi(szFDW);

	// note: some cultures treat sunday as the first day of the week
	// which is reported as day '6'. however because sunday is 
	// conceptually day '-1' this makes everything a week out.
	// so we hack it thus.
	if (nFirstDOW == 6)
		nFirstDOW = -1;

	return nFirstDOW + 1;
}

CString CDateHelper::FormatDate(const COleDateTime& date, BOOL bISOFormat, BOOL bWantDOW)
{
	CString sDate;

	if (bISOFormat)
	{
		if (bWantDOW)
			sDate.Format("%s %s", GetWeekday(date.GetDayOfWeek(), TRUE), date.Format("%Y-%m-%d"));
		else
			sDate = date.Format("%Y-%m-%d");
	}
	else
	{
		if (bWantDOW)
			sDate.Format("%s %s", GetWeekday(date.GetDayOfWeek(), TRUE), date.Format(VAR_DATEVALUEONLY));
		else
			sDate = date.Format(VAR_DATEVALUEONLY);
	}

	return sDate;
}

CString CDateHelper::FormatCurrentDate(BOOL bISOFormat, BOOL bWantDOW)
{
	return FormatDate(COleDateTime::GetCurrentTime(), bISOFormat, bWantDOW);
}

CString CDateHelper::GetWeekday(int nWeekday, BOOL bShort)
{
	LCTYPE lct = bShort ? LOCALE_SABBREVDAYNAME1 : LOCALE_SDAYNAME1;
	CString sWeekday;

	// data check
	if (nWeekday < 1 || nWeekday> 7)
		return "";

	switch (nWeekday)
	{
	case 1: lct = bShort ? LOCALE_SABBREVDAYNAME7 : LOCALE_SDAYNAME7; break; // sun
	case 2:	lct = bShort ? LOCALE_SABBREVDAYNAME1 : LOCALE_SDAYNAME1; break; // mon
	case 3:	lct = bShort ? LOCALE_SABBREVDAYNAME2 : LOCALE_SDAYNAME2; break; // tue
	case 4:	lct = bShort ? LOCALE_SABBREVDAYNAME3 : LOCALE_SDAYNAME3; break; // wed
	case 5:	lct = bShort ? LOCALE_SABBREVDAYNAME4 : LOCALE_SDAYNAME4; break; // thu
	case 6:	lct = bShort ? LOCALE_SABBREVDAYNAME5 : LOCALE_SDAYNAME5; break; // fri
	case 7:	lct = bShort ? LOCALE_SABBREVDAYNAME6 : LOCALE_SDAYNAME6; break; // sat
	}
	
	GetLocaleInfo(LOCALE_USER_DEFAULT, lct, sWeekday.GetBuffer(30),	29);
	sWeekday.ReleaseBuffer();

	return sWeekday;
}

void CDateHelper::OffsetDate(COleDateTime& date, int nAmount, DH_UNITS nUnits)
{
	if (date.m_dt > 0)
	{
		switch (nUnits)
		{
		case DHU_DAYS:
			date += (double)nAmount;
			break;

		case DHU_WEEKS:
			date += nAmount * 7.0;
			break;

		case DHU_MONTHS:
			{
				SYSTEMTIME st;
				date.GetAsSystemTime(st);

				// convert amount to years and months
				st.wYear += (nAmount / 12);
				st.wMonth += (nAmount % 12);

				// handle overflow
				if (st.wMonth > 12)
				{
					st.wYear++;
					st.wMonth -= 12;
				}
				else if (st.wMonth < 1)
				{
					st.wYear--;
					st.wMonth += 12;
				}

				// update time
				date = COleDateTime(st);
			}
			break;

		case DHU_YEARS:
			{
				SYSTEMTIME st;
				date.GetAsSystemTime(st);

				// update year
				st.wYear += nAmount;

				// update time
				date = COleDateTime(st);
			}
			break;
		}
	}
}

void CDateHelper::GetWeekdays(BOOL bShort, CStringArray& aWeekDays)
{
	aWeekDays.RemoveAll();

	for (int nDay = 1; nDay <= 7; nDay++)
		aWeekDays.Add(GetWeekday(nDay, bShort));
}

CString CDateHelper::GetMonth(int nMonth, BOOL bShort)
{
	LCTYPE lct = LOCALE_SABBREVMONTHNAME1;
	CString sMonth;

	// data check
	if (nMonth < 1 || nMonth> 12)
		return "";

	switch (nMonth)
	{
	case 1:  lct = bShort ? LOCALE_SABBREVMONTHNAME1  : LOCALE_SMONTHNAME1;  break; // jan
	case 2:  lct = bShort ? LOCALE_SABBREVMONTHNAME2  : LOCALE_SMONTHNAME2;  break; // feb
	case 3:  lct = bShort ? LOCALE_SABBREVMONTHNAME3  : LOCALE_SMONTHNAME3;  break; // mar
	case 4:  lct = bShort ? LOCALE_SABBREVMONTHNAME4  : LOCALE_SMONTHNAME4;  break; // apr
	case 5:  lct = bShort ? LOCALE_SABBREVMONTHNAME5  : LOCALE_SMONTHNAME5;  break; // may
	case 6:  lct = bShort ? LOCALE_SABBREVMONTHNAME6  : LOCALE_SMONTHNAME6;  break; // jun
	case 7:  lct = bShort ? LOCALE_SABBREVMONTHNAME7  : LOCALE_SMONTHNAME7;  break; // jul
	case 8:  lct = bShort ? LOCALE_SABBREVMONTHNAME8  : LOCALE_SMONTHNAME8;  break; // aug
	case 9:  lct = bShort ? LOCALE_SABBREVMONTHNAME9  : LOCALE_SMONTHNAME9;  break; // sep
	case 10: lct = bShort ? LOCALE_SABBREVMONTHNAME10 : LOCALE_SMONTHNAME10; break; // oct
	case 11: lct = bShort ? LOCALE_SABBREVMONTHNAME11 : LOCALE_SMONTHNAME11; break; // nov
	case 12: lct = bShort ? LOCALE_SABBREVMONTHNAME12 : LOCALE_SMONTHNAME12; break; // dec
	}

	GetLocaleInfo(LOCALE_USER_DEFAULT, lct, sMonth.GetBuffer(30),	29);
	sMonth.ReleaseBuffer();

	return sMonth;
}

void CDateHelper::GetMonths(BOOL bShort, CStringArray& aMonths)
{
	aMonths.RemoveAll();

	for (int nMonth = 1; nMonth <= 12; nMonth++)
		aMonths.Add(GetMonth(nMonth, bShort));
}

