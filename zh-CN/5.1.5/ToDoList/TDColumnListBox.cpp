// TDColumnListBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TDColumnListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITLISTBOX (WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// CTDColumnListBox

CTDColumnListBox::CTDColumnListBox()
{
	// same order as enum
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_POS,		TDLBC_POSITION, TRUE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_ID,		TDLBC_ID, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_PRIORITY,	TDLBC_PRIORITY, TRUE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_PERCENT,	TDLBC_PERCENT, TRUE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_TIMEEST,	TDLBC_TIMEEST, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_TIMESPENT, TDLBC_TIMESPENT, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_TRACKTIME, TDLBC_TRACKTIME, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_CREATEDBY, TDLBC_CREATEDBY, FALSE));
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_CREATEDATE, TDLBC_CREATIONDATE, FALSE));
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_STARTDATE, TDLBC_STARTDATE, FALSE));
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_DUEDATE,	TDLBC_DUEDATE, TRUE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_DONEDATE,	TDLBC_DONEDATE, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_ALLOCTO,	TDLBC_ALLOCTO, TRUE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_ALLOCBY,	TDLBC_ALLOCBY, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_STATUS,	TDLBC_STATUS, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_CATEGORY,	TDLBC_CATEGORY, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_FILEREF,	TDLBC_FILEREF, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_FLAG,		TDLBC_FLAG, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_DONE,		TDLBC_DONE, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_MODIFYDATE, TDLBC_MODIFYDATE, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_RISK,		TDLBC_RISK, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_EXTERNALID, TDLBC_EXTERNALID, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_COST,		TDLBC_COST, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_DEPENDS,	TDLBC_DEPENDS, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_RECURRENCE, TDLBC_RECURRENCE, FALSE)); 
	m_aColumns.Add(COLUMNSTATE(IDS_TDLBC_VERSION,	TDLBC_VERSION, FALSE)); 
}

CTDColumnListBox::~CTDColumnListBox()
{
}


BEGIN_MESSAGE_MAP(CTDColumnListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CTDColumnListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CLBN_CHKCHANGE, OnReflectCheckChange)
	ON_MESSAGE(WM_INITLISTBOX, OnInitListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDColumnListBox message handlers

void CTDColumnListBox::PreSubclassWindow() 
{
	CCheckListBox::PreSubclassWindow();

	// too early to initialize the list box so we post a message
	PostMessage(WM_INITLISTBOX);
}

void CTDColumnListBox::SetColumnState(TDLB_COLUMN nCol, BOOL bOn)
{
	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
	{
		COLUMNSTATE& cs = m_aColumns[nIndex];

		if (cs.nCol == nCol)
		{
			cs.bOn = bOn;

			if (GetSafeHwnd())
				SetCheck(nIndex, bOn ? 1 : 0);

			break;
		}
	}
}

BOOL CTDColumnListBox::GetColumnState(TDLB_COLUMN nCol) const
{
	int nIndex = m_aColumns.GetSize();
	
	while (nIndex--)
	{
		const COLUMNSTATE& cs = m_aColumns[nIndex];

		if (cs.nCol == nCol)
			return cs.bOn;
	}

	return FALSE; // not found
}

BOOL CTDColumnListBox::OnReflectCheckChange()
{
	// update all check states because we don't know which one changed
	int nIndex = (int)m_aColumns.GetSize();
	
	while (nIndex--)
		m_aColumns[nIndex].bOn = GetCheck(nIndex);

	// continue routing
	return FALSE;
}

LRESULT CTDColumnListBox::OnInitListBox(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT (GetCount() == 0);

	for (int nIndex = 0; nIndex < (int)m_aColumns.GetSize(); nIndex++)
	{
		COLUMNSTATE& cs = m_aColumns[nIndex];

		int nPos = AddString(cs.sName); // same order as enum
		SetCheck(nPos, cs.bOn ? 1 : 0);

		// note: we can't use SetItemData because CCheckListBox uses it
	}

	return 0L;
}
