// TimeEdit.cpp: implementation of the CTimeEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimeEdit.h"
#include "stringres.h"
#include "misc.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int BTNWIDTHDLU = 11;
const int ID_BTN = 1;
const int LABELLEN = 10;

struct TIMEUNIT
{
	int nUnits;
	TCHAR szLabel[LABELLEN];
	TCHAR cAbbrLabel;
	UINT nMenuID;
};

enum
{
	ID_MINS = 0x8000,
	ID_HOURS,
	ID_DAYS,
	ID_WEEKS,
	ID_MONTHS,
	ID_YEARS,
};

static TIMEUNIT TIMEUNITS[] = 
{
	{ THU_MINS,		"",	TIME_MIN_ABBREV,	ID_MINS },
	{ THU_HOURS,	"",	TIME_HOUR_ABBREV,	ID_HOURS },
	{ THU_DAYS,		"",	TIME_DAY_ABBREV,	ID_DAYS },
	{ THU_WEEKS,	"",	TIME_WEEK_ABBREV,	ID_WEEKS },
	{ THU_MONTHS,	"", TIME_MONTH_ABBREV,	ID_MONTHS },
	{ THU_YEARS,	"",	TIME_YEAR_ABBREV,	ID_YEARS },
};

static LPCTSTR UNITLABELS[] = 
{
	TIME_MINS,	
	TIME_HOURS,	
	TIME_DAYS,	
	TIME_WEEKS,	
	TIME_MONTHS,
	TIME_YEARS	
};

const int NUM_UNITS = sizeof(TIMEUNITS) / sizeof (TIMEUNIT);

const TIMEUNIT& GetTimeUnit(int nUnits)
{
	int nItem = NUM_UNITS;

	while (nItem--)
	{
		if (TIMEUNITS[nItem].nUnits == nUnits)
			return TIMEUNITS[nItem];
	}

	return TIMEUNITS[0]; // hours
}

/////////////////////////////////////////////////////////////////////////////////////

CTimeEdit::CTimeEdit(int nUnits, int nMaxDecPlaces) : m_nUnits(nUnits), m_nMaxDecPlaces(nMaxDecPlaces)
{
	// init static units
	for (int nUnit = 0; nUnit < NUM_UNITS; nUnit++)
	{
		TIMEUNIT& tu = TIMEUNITS[nUnit];

		if (tu.szLabel[0] == 0) // empty string
		{
			//fabio_2005
#if _MSC_VER >= 1400
			strncpy_s(tu.szLabel, UNITLABELS[nUnit], LABELLEN - 1);
#else
			strncpy(tu.szLabel, UNITLABELS[nUnit], LABELLEN - 1);
#endif

			tu.szLabel[LABELLEN - 1] = 0;
		}
	}

	SetMask(".0123456789", ME_LOCALIZEDECIMAL);

	CString sLabel;
	sLabel.Format("%c", GetTimeUnit(nUnits).cAbbrLabel);

	AddButton(ID_BTN, sLabel, TIME_UNITS, CALC_BTNWIDTH);
	SetDropMenuButton(ID_BTN);
}

CTimeEdit::~CTimeEdit()
{

}

BEGIN_MESSAGE_MAP(CTimeEdit, CEnEdit)
	//{{AFX_MSG_MAP(CTimeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTimeEdit::PreSubclassWindow() 
{
	CEnEdit::PreSubclassWindow();

//	SetButtonWidthDLU(1, BTNWIDTHDLU);
}

double CTimeEdit::GetTime() const
{
	CString sTime;
	GetWindowText(sTime);
	return Misc::Atof(sTime);
}

void CTimeEdit::SetTime(double dTime)
{
	CString sTime = CTimeHelper::FormatTime(dTime, m_nMaxDecPlaces);
	RemoveTrailingZeros(sTime);

	SetWindowText(sTime);
}

void CTimeEdit::SetTime(double dTime, int nUnits)
{
	if (dTime != GetTime())
	{
		SetTime(dTime);
		SetUnits(nUnits);
	}
}

void CTimeEdit::SetUnits(int nUnits)
{
	if (nUnits != m_nUnits)
	{
		m_nUnits = nUnits;

		if (GetSafeHwnd())
		{
			CString sLabel;
			sLabel.Format("%c", GetTimeUnit(nUnits).cAbbrLabel);
			SetButtonCaption(1, sLabel);
		}
	}
}

void CTimeEdit::SetMaxDecimalPlaces(int nMaxDecPlaces)
{
	if (m_nMaxDecPlaces != nMaxDecPlaces)
	{
		m_nMaxDecPlaces = nMaxDecPlaces;

		SetTime(GetTime());
	}
}

void CTimeEdit::OnBtnClick(UINT nID)
{
	if (nID != ID_BTN)
		return;

	CMenu menu;
	
	if (menu.CreatePopupMenu())
	{			
		int nUnit = 0; 
		for (nUnit = 0; nUnit < NUM_UNITS; nUnit++)
		{
			const TIMEUNIT& tu = TIMEUNITS[nUnit];

			menu.AppendMenu(MF_STRING, tu.nMenuID, tu.szLabel);

			if (tu.nUnits == m_nUnits)
				menu.CheckMenuItem(nUnit, MF_CHECKED | MF_BYPOSITION);
		}
		CRect rButton = GetButtonRect(nID);
		
		TPMPARAMS tpmp;
		tpmp.cbSize = sizeof(TPMPARAMS);
		tpmp.rcExclude = rButton;
		
		UINT nID = ::TrackPopupMenuEx(menu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, 
									rButton.right, rButton.top, *this, &tpmp);

		// handle result
		for (nUnit = 0; nUnit < NUM_UNITS; nUnit++)
		{
			const TIMEUNIT& tu = TIMEUNITS[nUnit];

			if (tu.nMenuID == nID)
			{
				if (m_nUnits != tu.nUnits)
				{
					// update the units
					int nPrevUnits = m_nUnits;
					SetUnits(tu.nUnits);

					// inform parent and check for cancel
					if (GetParent()->SendMessage(WM_TEN_UNITSCHANGE, (WPARAM)GetDlgCtrlID(), nPrevUnits))
						SetUnits(nPrevUnits);
				}
				break;
			}
		}
	}
}

double CTimeEdit::GetTime(int nUnits) const
{
	return CTimeHelper::GetTime(GetTime(), m_nUnits, nUnits);
}

CString CTimeEdit::FormatTime(BOOL bUnits) const
{
	return CTimeHelper::FormatTime(GetTime(), bUnits ? m_nUnits : 0, m_nMaxDecPlaces);
}

CString CTimeEdit::FormatTimeHMS() const
{
	return CTimeHelper::FormatTimeHMS(GetTime(), GetUnits(), TRUE); 
}

void CTimeEdit::OnSetReadOnly(BOOL bReadOnly)
{
	EnableButton(1, !bReadOnly && IsWindowEnabled());
}

void CTimeEdit::SetUnits(int nUnits, LPCTSTR szLongUnits, LPCTSTR szAbbrevUnits)
{
	for (int nUnit = 0; nUnit < NUM_UNITS; nUnit++)
	{
		TIMEUNIT& tu = TIMEUNITS[nUnit];

		if (tu.nUnits == nUnits)
		{
			if (szLongUnits && *szLongUnits)
			{
				//fabio_2005
#if _MSC_VER >= 1300
				strncpy_s(tu.szLabel, szLongUnits, LABELLEN - 1);
#else
				strncpy(tu.szLabel, szLongUnits, LABELLEN - 1);
#endif

				tu.szLabel[LABELLEN - 1] = 0;
			}

			if (szAbbrevUnits && *szAbbrevUnits)
				tu.cAbbrLabel = szAbbrevUnits[0];
		}
	}

	// abbrev units
	CTimeHelper::SetUnits(nUnits, szAbbrevUnits);
}

void CTimeEdit::RemoveTrailingZeros(CString& sTime)
{
	sTime.TrimRight();

	while (!sTime.IsEmpty())
	{
		int nLen = sTime.GetLength();

		if (sTime[nLen - 1] == '0')
			sTime = sTime.Left(nLen - 1);

		else if (sTime[nLen - 1] == '.')
		{
			sTime = sTime.Left(nLen - 1);
			break;
		}
		else
			break;
	}
}
