// TDLPriorityComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "TDLPriorityComboBox.h"

#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT IDS_TDC_SCALE[] = { IDS_TDC_SCALE0,
								IDS_TDC_SCALE1,
								IDS_TDC_SCALE2,
								IDS_TDC_SCALE3,
								IDS_TDC_SCALE4,
								IDS_TDC_SCALE5,
								IDS_TDC_SCALE6,
								IDS_TDC_SCALE7,
								IDS_TDC_SCALE8,
								IDS_TDC_SCALE9,
								IDS_TDC_SCALE10 };


const int TDC_NUMSCALES = sizeof(IDS_TDC_SCALE) / sizeof(UINT);

/////////////////////////////////////////////////////////////////////////////
// CTDLPriorityComboBox

CTDLPriorityComboBox::CTDLPriorityComboBox() : m_bReverse(FALSE)
{
}

CTDLPriorityComboBox::~CTDLPriorityComboBox()
{
}


BEGIN_MESSAGE_MAP(CTDLPriorityComboBox, CColorComboBox)
	//{{AFX_MSG_MAP(CTDLPriorityComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLPriorityComboBox message handlers

int CTDLPriorityComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CColorComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	BuildCombo();
	
	return 0;
}

void CTDLPriorityComboBox::PreSubclassWindow() 
{
	CColorComboBox::PreSubclassWindow();
	
	BuildCombo();
}

BOOL CTDLPriorityComboBox::SetColors(const CDWordArray& aColors)
{
	if (aColors.GetSize() < 11)
		return FALSE;
	
	m_aColors.Copy(aColors);
	
	if (GetSafeHwnd())
		BuildCombo();
	
	return TRUE;
}

void CTDLPriorityComboBox::SetReverseOrder(BOOL bReverse)
{
	if (m_bReverse != bReverse)
	{
		m_bReverse = bReverse;
		
		if (GetSafeHwnd())
			BuildCombo();
	}
}

void CTDLPriorityComboBox::BuildCombo()
{
	ASSERT(GetSafeHwnd());
	
	int nSel = GetCurSel(); // so we can restore it
	
	ResetContent();
	BOOL bHasColors = m_aColors.GetSize();
	
	for (int nLevel = 0; nLevel <= 10; nLevel++)
	{
		COLORREF color = bHasColors ? m_aColors[nLevel] : -1;
		int nPriority = m_bReverse ? 11 - nLevel : nLevel;
		
		CString sPriority;
		sPriority.Format("%d (%s)", nPriority, CEnString(IDS_TDC_SCALE[nLevel]));
		
		AddColor(color, sPriority);
	}
	
	SetCurSel(nSel);
}

