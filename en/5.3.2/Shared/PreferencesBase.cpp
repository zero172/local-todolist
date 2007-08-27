// PreferencesPageBase.cpp : implementation file
//

#include "stdafx.h"
#include "PreferencesBase.h"
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesStorage


CPreferencesStorage::CPreferencesStorage() : m_bDirty(FALSE), m_bIni(FALSE)
{
   // figure out whether we're using an ini file or the registry
   m_bIni = (AfxGetApp()->m_pszRegistryKey == NULL);

   if (m_bIni)
   {
      // read the ini file
      CStdioFile file;
	  DWORD dwTick = GetTickCount();

      if (file.Open(AfxGetApp()->m_pszProfileName, CFile::modeRead))
      {
         CString sLine;
         INISECTION* pCurSection = NULL;

         while (file.ReadString(sLine))
         {
            sLine.TrimLeft();
            sLine.TrimRight();

            if (sLine.IsEmpty())
               continue;

            // is it a section ?
            else if (sLine[0] == '[')
            {
               CString sSection = sLine.Mid(1, sLine.GetLength() - 2);

               pCurSection = GetSection(m_aIni, sSection, TRUE);
               ASSERT (pCurSection != NULL);
            }
            // else an entry
            else if (pCurSection)
            {
               int nEquals = sLine.Find('=');

               if (nEquals != -1)
               {
                  CString sEntry = sLine.Left(nEquals);
                  sEntry.TrimRight();

                  CString sValue = sLine.Mid(nEquals + 1);
                  sValue.TrimLeft();

                  // remove quotes
                  sValue.Replace("\"", "");

                  if (!sEntry.IsEmpty())
                     SetEntryValue(*pCurSection, sEntry, sValue);
               }
            }
         }
      }

	  TRACE ("CPreferencesStorage(Load Ini = %dms)\n", GetTickCount() - dwTick);
   }
}

CPreferencesStorage::~CPreferencesStorage()
{
   // save ini?
   if (m_bIni)
   {
	   if (m_bDirty)
	   {
		  CStdioFile file;
		  DWORD dwTick = GetTickCount();

		  if (file.Open(AfxGetApp()->m_pszProfileName, CFile::modeWrite | CFile::modeCreate))
		  {
			 for (int nSection = 0; nSection < m_aIni.GetSize(); nSection++)
			 {
				// write section line
				INISECTION* pSection = m_aIni[nSection];

				CString sLine;
				sLine.Format("[%s]\n", pSection->sSection);

				file.WriteString(sLine);

				// write entries
				POSITION pos = pSection->aEntries.GetStartPosition();

				while (pos)
				{
				   CString sDummy;
				   INIENTRY ie;

				   pSection->aEntries.GetNextAssoc(pos, sDummy, ie);

				   sLine.Format("%s=%s\n", ie.sName, ie.sValue);
				   file.WriteString(sLine);
				}
			 }
		  }

		  TRACE ("CPreferencesStorage(Save Ini = %dms)\n", GetTickCount() - dwTick);
	   }

	   // cleanup
	   int nSection = m_aIni.GetSize();

	   while (nSection--)
		  delete m_aIni[nSection];
   }
}

UINT CPreferencesStorage::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) const
{
   if (m_bIni)
   {
      CString sValue = GetProfileString(lpszSection, lpszEntry);

      if (sValue.IsEmpty())
         return nDefault;
      else
         return atol(sValue);
   }
   else
      return AfxGetApp()->GetProfileInt(lpszSection, lpszEntry, nDefault);
}

BOOL CPreferencesStorage::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
   if (m_bIni)
   {
      CString sValue;
      sValue.Format("%ld", nValue);

      return WriteProfileString(lpszSection, lpszEntry, sValue);
   }
   else
      return AfxGetApp()->WriteProfileInt(lpszSection, lpszEntry, nValue);
}

CString CPreferencesStorage::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) const
{
   if (m_bIni)
   {
      INISECTION* pSection = GetSection(m_aIni, lpszSection, FALSE);

      if (pSection)
         return GetEntryValue(*pSection, lpszEntry, lpszDefault);
      else
         return lpszDefault;
   }
   else
      return AfxGetApp()->GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

BOOL CPreferencesStorage::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
   if (m_bIni)
   {
      INISECTION* pSection = GetSection(m_aIni, lpszSection, TRUE);
      ASSERT(pSection);

      if (pSection)
      {
         m_bDirty = TRUE;

         SetEntryValue(*pSection, lpszEntry, lpszValue);
         return TRUE;
      }

      // else
      return FALSE;
   }
   else
      return AfxGetApp()->WriteProfileString(lpszSection, lpszEntry, lpszValue);
}

double CPreferencesStorage::GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault) const
{
	CString sValue = GetProfileString(lpszSection, lpszEntry);

	if (sValue.IsEmpty())
		return dDefault;
	else
		return Misc::Atof(sValue);
}

BOOL CPreferencesStorage::WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue)
{
	CString sValue;
	sValue.Format("%lf", dValue);
	
	return WriteProfileString(lpszSection, lpszEntry, sValue);
}

int CPreferencesStorage::GetArrayItems(LPCTSTR szRegKey, CStringArray& aItems) const
{
	aItems.RemoveAll();
	int nCount = GetProfileInt(szRegKey, "ItemCount", 0);

	// items
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey, sItem;
		sItemKey.Format("Item%d", nItem);
		sItem = GetProfileString(szRegKey, sItemKey);

		if (!sItem.IsEmpty())
			aItems.Add(sItem);
	}

	return aItems.GetSize();
}

void CPreferencesStorage::WriteArrayItems(const CStringArray& aItems, LPCTSTR szRegKey)
{
	int nCount = aItems.GetSize();

	// items
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey;
		sItemKey.Format("Item%d", nItem);
		WriteProfileString(szRegKey, sItemKey, aItems[nItem]);
	}

	// item count
	WriteProfileInt(szRegKey, "ItemCount", nCount);
}

CString CPreferencesStorage::GetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
   CString sKey(lpszEntry);
   sKey.MakeUpper();

   INIENTRY ie;
   
   if (section.aEntries.Lookup(sKey, ie) && !ie.sValue.IsEmpty())
      return ie.sValue;

   return lpszDefault;
}

void CPreferencesStorage::SetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR szValue)
{
   CString sKey(lpszEntry);
   sKey.MakeUpper();

   section.aEntries[sKey] = INIENTRY(lpszEntry, szValue);
}

INISECTION* CPreferencesStorage::GetSection(CIniSectionArray& ini, LPCTSTR lpszSection, BOOL bCreateNotExist)
{
   int nSection = ini.GetSize();

   while (nSection--)
   {
      if (ini[nSection]->sSection.CompareNoCase(lpszSection) == 0)
         return ini[nSection];
   }

   // add a new section
   if (bCreateNotExist)
   {
      INISECTION* pSection = new INISECTION(lpszSection);

      ini.Add(pSection);
      return pSection;
   }

   // else
   return NULL;
}
/////////////////////////////////////////////////////////////////////////////
// CPreferencesPageBase property page

IMPLEMENT_DYNCREATE(CPreferencesPageBase, CPropertyPage)

CPreferencesPageBase::CPreferencesPageBase(UINT nID) : CPropertyPage(nID)
{
}

CPreferencesPageBase::~CPreferencesPageBase()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlgBase dialog


CPreferencesDlgBase::CPreferencesDlgBase(UINT nID, CWnd* pParent) : CDialog(nID, pParent)
{
//   LoadPreferences();
}

BEGIN_MESSAGE_MAP(CPreferencesDlgBase, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPreferencesDlgBase::OnOK()
{
	CDialog::OnOK();

   m_pphost.OnOK();
   SavePreferences();
}

int CPreferencesDlgBase::DoModal(int nInitPage)
{
	m_nInitPage = nInitPage;

	return CDialog::DoModal();
}

BOOL CPreferencesDlgBase::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPreferencesDlgBase::CreatePPHost(UINT nHostID)
{
   if (GetDlgItem(nHostID))
   {
	   CRect rPPHost;
	   GetDlgItem(nHostID)->GetWindowRect(rPPHost);
	   ScreenToClient(rPPHost);

      return CreatePPHost(rPPHost);
   }

   // else
   return FALSE;
}

BOOL CPreferencesDlgBase::CreatePPHost(LPRECT pRect)
{
	if (m_pphost.Create(pRect, this))
	{
		if (m_nInitPage < 0 || m_nInitPage >= m_pphost.GetPageCount())
			m_nInitPage = AfxGetApp()->GetProfileInt("Preferences", "StartPage", 0);

		return SetActivePage(m_nInitPage);
	}

   // else
   return FALSE;
}

BOOL CPreferencesDlgBase::SetActivePage(int nPage)
{
	CPropertyPage* pPage = m_pphost.GetPage(nPage);
	ASSERT (pPage);

	return m_pphost.SetActivePage(pPage);
}

BOOL CPreferencesDlgBase::SetActivePage(CPreferencesPageBase* pPage)
{
   if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
      return m_pphost.SetActivePage(pPage);

   // else
   return FALSE;
}

BOOL CPreferencesDlgBase::AddPage(CPreferencesPageBase* pPage)
{
   if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
      return m_pphost.AddPage(pPage);

   // else
   return FALSE;
}

void CPreferencesDlgBase::OnDestroy() 
{
	CDialog::OnDestroy();
	
	AfxGetApp()->WriteProfileInt("Preferences", "StartPage", m_pphost.GetActiveIndex());
}

void CPreferencesDlgBase::LoadPreferences()
{
   // new storage object
   CPreferencesStorage ps;

   // cycle the page loading the preferences for each one
   int nPage = m_pphost.GetPageCount();

   while (nPage--)
   {
      CPreferencesPageBase* pPage = (CPreferencesPageBase*)m_pphost.GetPage(nPage);
 
      if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
         pPage->LoadPreferences(ps);
   }
}

void CPreferencesDlgBase::SavePreferences()
{
   // new storage object
   CPreferencesStorage ps;

   // cycle the page saving the preferences for each one
   int nPage = m_pphost.GetPageCount();

   while (nPage--)
   {
      CPreferencesPageBase* pPage = (CPreferencesPageBase*)m_pphost.GetPage(nPage);
 
      if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
         pPage->SavePreferences(ps);
   }
}


/////////////////////////////////////////////////////////////////////////////
