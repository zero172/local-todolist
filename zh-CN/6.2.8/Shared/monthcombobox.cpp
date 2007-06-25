// monthcombobox.cpp : implementation file
//

#include "stdafx.h"
#include "monthcombobox.h"
#include "datehelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonthComboBox

CMonthComboBox::CMonthComboBox()
{
}

CMonthComboBox::~CMonthComboBox()
{
}


BEGIN_MESSAGE_MAP(CMonthComboBox, CComboBox)
	//{{AFX_MSG_MAP(CMonthComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonthComboBox message handlers

void CMonthComboBox::PreSubclassWindow() 
{
	InitCombo();
	
	CComboBox::PreSubclassWindow();
}

int CMonthComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitCombo();
	
	return 0;
}

void CMonthComboBox::InitCombo()
{
	ASSERT(GetSafeHwnd());

	ResetContent();

	for (int nMonth = 1; nMonth <= 12; nMonth++)
		AddString(CDateHelper::GetMonth(nMonth, FALSE));
}
