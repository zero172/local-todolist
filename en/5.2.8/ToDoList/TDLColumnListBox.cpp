// TDColumnListBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TDLColumnListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITLISTBOX (WM_APP+1)

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnListBox

CTDLColumnListBox::CTDLColumnListBox()
{
	// same order as enum
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_POS,		TDLBC_POSITION,		TDCC_POSITION,	TRUE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ID,		TDLBC_ID,			TDCC_ID,		FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_PRIORITY,TDLBC_PRIORITY,		TDCC_PRIORITY,	TRUE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_PERCENT,	TDLBC_PERCENT,		TDCC_PERCENT,	TRUE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_TIMEEST,	TDLBC_TIMEEST,		TDCC_TIMEEST,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_TIMESPENT, TDLBC_TIMESPENT,	TDCC_TIMESPENT, FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_TRACKTIME, TDLBC_TRACKTIME,	TDCC_TRACKTIME, FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_CREATEDBY, TDLBC_CREATEDBY,	TDCC_CREATEDBY, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_CREATEDATE, TDLBC_CREATIONDATE,TDCC_CREATIONDATE, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_STARTDATE, TDLBC_STARTDATE,	TDCC_STARTDATE, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DUEDATE,	TDLBC_DUEDATE,		TDCC_DUEDATE,	TRUE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DONEDATE,TDLBC_DONEDATE,		TDCC_DONEDATE,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ALLOCTO,	TDLBC_ALLOCTO,		TDCC_ALLOCTO,	TRUE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ALLOCBY,	TDLBC_ALLOCBY,		TDCC_ALLOCBY,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_STATUS,	TDLBC_STATUS,		TDCC_STATUS,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_CATEGORY,TDLBC_CATEGORY,		TDCC_CATEGORY,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_FILEREF,	TDLBC_FILEREF,		TDCC_FILEREF,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_FLAG,	TDLBC_FLAG,			TDCC_FLAG,		FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DONE,	TDLBC_DONE,			TDCC_DONE,		FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_MODIFYDATE, TDLBC_MODIFYDATE,TDCC_LASTMOD,	FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_RISK,	TDLBC_RISK,			TDCC_RISK,		FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_EXTERNALID, TDLBC_EXTERNALID,TDCC_EXTERNALID, FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_COST,	TDLBC_COST,			TDCC_COST,		FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DEPENDS,	TDLBC_DEPENDS,		TDCC_DEPENDENCY, FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_RECURRENCE, TDLBC_RECURRENCE,TDCC_RECURRENCE, FALSE)); 
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_VERSION,	TDLBC_VERSION,		TDCC_VERSION,	FALSE)); 
}

CTDLColumnListBox::~CTDLColumnListBox()
{
}


BEGIN_MESSAGE_MAP(CTDLColumnListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CTDLColumnListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CLBN_CHKCHANGE, OnReflectCheckChange)
	ON_MESSAGE(WM_INITLISTBOX, OnInitListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnListBox message handlers

void CTDLColumnListBox::PreSubclassWindow() 
{
	CCheckListBox::PreSubclassWindow();

	// too early to initialize the list box so we post a message
	PostMessage(WM_INITLISTBOX);
}

int CTDLColumnListBox::FindColumn(TDLB_COLUMN nCol) const
{
	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
	{
		if (m_aColumns[nIndex].nCol == nCol)
			return nIndex;
	}

	ASSERT(0);
	return -1;
}

int CTDLColumnListBox::FindColumn(TDC_COLUMN nCol) const
{
	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
	{
		if (m_aColumns[nIndex].nTDCCol == nCol)
			return nIndex;
	}

	ASSERT(0);
	return -1;
}

void CTDLColumnListBox::SetAllColumnsVisible(BOOL bVisible)
{
	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
	{
		m_aColumns[nIndex].bVisible = bVisible;

		if (GetSafeHwnd())
			SetCheck(nIndex, bVisible ? 1 : 0);
	}
}

void CTDLColumnListBox::SetColumnVisible(TDLB_COLUMN nCol, BOOL bVisible)
{
	int nIndex = FindColumn(nCol);
	
	if (nIndex != -1)
	{
		m_aColumns[nIndex].bVisible = bVisible;

		if (GetSafeHwnd())
			SetCheck(nIndex, bVisible ? 1 : 0);
	}
}

BOOL CTDLColumnListBox::IsColumnVisible(TDLB_COLUMN nCol) const
{
	int nIndex = FindColumn(nCol);
	
	if (nIndex != -1)
		return m_aColumns[nIndex].bVisible;

	return FALSE; // not found
}

BOOL CTDLColumnListBox::OnReflectCheckChange()
{
	// update all check states because we don't know which one changed
	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
		m_aColumns[nIndex].bVisible = GetCheck(nIndex);

	// continue routing
	return FALSE;
}

LRESULT CTDLColumnListBox::OnInitListBox(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT (GetCount() == 0);

	for (int nIndex = 0; nIndex < (int)m_aColumns.GetSize(); nIndex++)
	{
		COLUMNVIS& cs = m_aColumns[nIndex];

		int nPos = AddString(cs.sName); // same order as enum
		SetCheck(nPos, cs.bVisible ? 1 : 0);

		// note: we can't use SetItemData because CCheckListBox uses it
	}

	return 0L;
}

void CTDLColumnListBox::SetVisibleColumns(const CTDCColumnArray& aColumns)
{
	SetAllColumnsVisible(FALSE);

	int nCol = (int)aColumns.GetSize();
	
	while (nCol--)
	{
		int nIndex = FindColumn(aColumns[nCol]);

		if (nIndex != -1)
		{
			m_aColumns[nIndex].bVisible = TRUE;

			if (GetSafeHwnd())
				SetCheck(nIndex, 1);
		}
	}
}

int CTDLColumnListBox::GetVisibleColumns(CTDCColumnArray& aColumns) const
{
	aColumns.RemoveAll();

	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
	{
		COLUMNVIS cs = m_aColumns[nIndex];

		if (cs.bVisible)
			aColumns.Add(cs.nTDCCol);
	}

	return aColumns.GetSize();

}
