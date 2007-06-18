// filedialogex.cpp : implementation file
//

#include "stdafx.h"
#include "enfiledialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnFileDialog

IMPLEMENT_DYNAMIC(CEnFileDialog, CFileDialog)

// Windows 2000 version of OPENFILENAME
#if _MSC_VER < 1300
struct OPENFILENAMEEX : public OPENFILENAME 
{ 
  void *        pvReserved;
  DWORD         dwReserved;
  DWORD         FlagsEx;
};
#endif		

CEnFileDialog::CEnFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, 
							 LPCTSTR lpszFileName, DWORD dwFlags, 
							 LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, 
				dwFlags, lpszFilter, pParentWnd)
{
#if _MSC_VER < 1300
	DWORD dwVersion = ::GetVersion();
	DWORD dwWinMajor = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	if ((BYTE)dwWinMajor >= 5)
		m_ofn.lStructSize = sizeof(OPENFILENAMEEX);
#endif		
}


BEGIN_MESSAGE_MAP(CEnFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CEnFileDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

