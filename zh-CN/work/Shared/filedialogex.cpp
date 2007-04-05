// filedialogex.cpp : implementation file
//

#include "stdafx.h"
#include "filedialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)

// Windows 2000 version of OPENFILENAME
struct OPENFILENAMEEX : public OPENFILENAME 
{ 
  void *        pvReserved;
  DWORD         dwReserved;
  DWORD         FlagsEx;
};

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, 
							 LPCTSTR lpszFileName, DWORD dwFlags, 
							 LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, 
				dwFlags, lpszFilter, pParentWnd)
{
	DWORD dwVersion = ::GetVersion();
	DWORD dwWinMajor = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	if ((BYTE)dwWinMajor >= 5)
		m_ofn.lStructSize = sizeof(OPENFILENAMEEX);
		
}


BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
	//{{AFX_MSG_MAP(CFileDialogEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

