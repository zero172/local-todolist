// maskedit.cpp : implementation file
//

#include "stdafx.h"
#include "maskedit.h"
#include "winclasses.h"
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit

CMaskEdit::CMaskEdit(LPCTSTR szMask, DWORD dwFlags) : m_sMask(szMask), m_dwFlags(dwFlags)
{
	if (dwFlags & ME_LOCALIZEDECIMAL)
		m_sMask.Replace('.', Misc::GetDecimalSeparator()[0]);
}

CMaskEdit::~CMaskEdit()
{
}

BEGIN_MESSAGE_MAP(CMaskEdit, CEdit)
	//{{AFX_MSG_MAP(CMaskEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit message handlers

void CMaskEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_BACK:
		break;

	// something wierd going on with edit control accelerators
	case 3:   // c
	case 22:  // v
	case 24:  // x
		if (CWinClasses::IsEditControl(*this))
		{
			ASSERT (GetKeyState(VK_CONTROL) & 0x8000);
			break;
		}
		// else drop thru
		
	case 'c':
	case 'C':
	case 'v':
	case 'V':
	case 'x':
	case 'X':
		if (GetKeyState(VK_CONTROL) & 0x8000)
			break;
		// else drop thru
		
	default:
		if (!m_sMask.IsEmpty())
		{
		if ((m_dwFlags & ME_EXCLUDE) && m_sMask.Find((char)nChar) != -1)
			return;

		else if (!(m_dwFlags & ME_EXCLUDE) && m_sMask.Find((char)nChar) == -1)
			return;
		}
		break;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CMaskEdit::SetMask(LPCTSTR szMask, DWORD dwFlags) 
{ 
	m_sMask = szMask; 
	m_dwFlags = dwFlags; 

	if (dwFlags & ME_LOCALIZEDECIMAL)
		m_sMask.Replace('.', Misc::GetDecimalSeparator()[0]);
}


