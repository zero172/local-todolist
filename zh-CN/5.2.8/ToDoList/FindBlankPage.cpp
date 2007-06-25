// FindBlankPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindBlankPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindBlankPage property page

IMPLEMENT_DYNCREATE(CFindBlankPage, CPropertyPage)

CFindBlankPage::CFindBlankPage() : CPropertyPage(CFindBlankPage::IDD)
{
	//{{AFX_DATA_INIT(CFindBlankPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFindBlankPage::~CFindBlankPage()
{
}

BEGIN_MESSAGE_MAP(CFindBlankPage, CPropertyPage)
END_MESSAGE_MAP()

