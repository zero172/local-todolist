// EnRecentFileList.cpp: implementation of the CEnRecentFileList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnRecentFileList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnRecentFileList::CEnRecentFileList(UINT nStart, LPCTSTR lpszSection,
	    LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen, LPCTSTR szOriginal) 
       : CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen) 
{
   m_strOriginal = szOriginal;
}

CEnRecentFileList::~CEnRecentFileList()
{

}

void CEnRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
    if (m_arrNames[0].IsEmpty())
	{
	   if (pCmdUI->m_pMenu == NULL)
		   return;

      // delete all but the first item
	   for (int iMRU = 1; iMRU < m_nSize; iMRU++)
		   pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
	}

   CRecentFileList::UpdateMenu(pCmdUI);
}

void CEnRecentFileList::RemoveAll(BOOL bClearProfile)
{
	int nItem = GetSize();

	while (nItem--)
		Remove(nItem);

	if (bClearProfile)
		WriteList();
}
