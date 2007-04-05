// TDLRiskComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "TDLRiskComboBox.h"
#include "resource.h"

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
// CTDLRiskComboBox

CTDLRiskComboBox::CTDLRiskComboBox()
{
}

CTDLRiskComboBox::~CTDLRiskComboBox()
{
}


BEGIN_MESSAGE_MAP(CTDLRiskComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTDLRiskComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLRiskComboBox message handlers

int CTDLRiskComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	BuildCombo();
	
	return 0;
}

void CTDLRiskComboBox::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

 	BuildCombo();
}

void CTDLRiskComboBox::BuildCombo()
{
	ASSERT(GetSafeHwnd());
	
	int nSel = GetCurSel(); // so we can restore it
	
	ResetContent();
	
	for (int nLevel = 0; nLevel <= 10; nLevel++)
	{
		CString sRisk;
		sRisk.Format("%d (%s)", nLevel, CEnString(IDS_TDC_SCALE[nLevel]));
		
		AddString(sRisk);
	}
	
	SetCurSel(nSel);
}

