// CToDoCtrlMgr.cpp : implementation file
//

#include "stdafx.h"
#include "ToDolist.h"
#include "ToDoCtrlMgr.h"
#include "tdstringres.h"

#include "..\shared\holdredraw.h"
#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrlMgr dialog

enum { IM_NONE = -1, IM_READONLY, IM_CHECKEDIN, IM_CHECKEDOUT };

#define ASSERTVALIDINDEX(i) { ASSERT (i >= 0 && i < GetCount()); }

CToDoCtrlMgr::CToDoCtrlMgr(CTabCtrl& tabCtrl) : m_tabCtrl(tabCtrl)
{
}

CToDoCtrlMgr::~CToDoCtrlMgr()
{
}

void CToDoCtrlMgr::SetPrefs(const CPreferencesDlg* pPrefs)
{
	m_pPrefs = pPrefs;
}

const CPreferencesDlg& CToDoCtrlMgr::Prefs() const
{
	ASSERT (m_pPrefs);
	return *m_pPrefs;
}

CFilteredToDoCtrl& CToDoCtrlMgr::GetToDoCtrl(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	return *(m_aToDoCtrls[nIndex].pTDC);
}

const CFilteredToDoCtrl& CToDoCtrlMgr::GetToDoCtrl(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return *(m_aToDoCtrls[nIndex].pTDC);
}

CToDoCtrlMgr::TDCITEM& CToDoCtrlMgr::GetTDCItem(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	return m_aToDoCtrls[nIndex];
}

const CToDoCtrlMgr::TDCITEM& CToDoCtrlMgr::GetTDCItem(int nIndex) const
{
	// hack to get around const problem
	CToDoCtrlMgr* pThis = const_cast<CToDoCtrlMgr*>(this);
	
	return pThis->m_aToDoCtrls[nIndex];
}

CString CToDoCtrlMgr::GetFilePath(int nIndex, BOOL bStrict) const
{
	ASSERTVALIDINDEX(nIndex);

	CString sPath = GetToDoCtrl(nIndex).GetFilePath();

	if (sPath.IsEmpty() && !bStrict)
	{
		TCITEM tci;
		char szText[51];

		tci.mask = TCIF_TEXT;
		tci.cchTextMax = 50;
		tci.pszText = szText;

		m_tabCtrl.GetItem(nIndex, &tci);

		sPath = tci.pszText;
		FileMisc::ValidateFilename(sPath);
	}

	return sPath;
}

CString CToDoCtrlMgr::GetFriendlyProjectName(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return m_aToDoCtrls[nIndex].GetFriendlyProjectName();
}

void CToDoCtrlMgr::ClearFilePath(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	GetToDoCtrl(nIndex).ClearFilePath();
}

BOOL CToDoCtrlMgr::IsFilePathEmpty(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetFilePath(nIndex).IsEmpty();
}

TDCM_PATHTYPE CToDoCtrlMgr::GetFilePathType(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	// we do it this way because of const
	return m_aToDoCtrls[nIndex].GetPathType();
}

TDCM_PATHTYPE CToDoCtrlMgr::RefreshPathType(int nIndex) 
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);

	tdci.RefreshPathType();
	return tdci.nPathType;
}

BOOL CToDoCtrlMgr::IsPristine(int nIndex) const
{
	return IsFilePathEmpty(nIndex) && !GetModifiedStatus(nIndex);
}

BOOL CToDoCtrlMgr::RefreshLastModified(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);
	TDCITEM& tdci = GetTDCItem(nIndex);

	time_t timeNow = FileMisc::GetLastModified(tdci.pTDC->GetFilePath());
	time_t timePrev = tdci.tLastMod;

	tdci.tLastMod = timeNow;

	// if the tasklist is checked out then always return FALSE because
	// noone else should be able to modify it (by any means).
	if (tdci.pTDC->IsCheckedOut())
		return FALSE;

	// else
	return (timeNow > 0 && timePrev > 0 && timeNow != timePrev);
}

BOOL CToDoCtrlMgr::RefreshReadOnlyStatus(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);

	BOOL bReadOnlyNow = CDriveInfo::IsReadonlyPath(tdci.pTDC->GetFilePath());
	BOOL bReadOnlyPrev = tdci.bLastStatusReadOnly;

	GetTDCItem(nIndex).bLastStatusReadOnly = bReadOnlyNow;

	return (bReadOnlyNow != -1 && bReadOnlyPrev != -1 && bReadOnlyNow != bReadOnlyPrev);
}

int CToDoCtrlMgr::GetReadOnlyStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bLastStatusReadOnly;
}

void CToDoCtrlMgr::SetDueItemStatus(int nIndex, BOOL bDueItems)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bDueItems = bDueItems;
}

BOOL CToDoCtrlMgr::GetDueItemStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bDueItems;
}

int CToDoCtrlMgr::GetLastCheckoutStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bLastCheckoutSuccess;
}

void CToDoCtrlMgr::SetLastCheckoutStatus(int nIndex, BOOL bStatus)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bLastCheckoutSuccess = bStatus;
}

int CToDoCtrlMgr::CanCheckOut(int nIndex) const
{
	return (CanCheckInOut(nIndex) && !GetToDoCtrl(nIndex).IsCheckedOut());
}

int CToDoCtrlMgr::CanCheckIn(int nIndex) const
{
	return (CanCheckInOut(nIndex) && GetToDoCtrl(nIndex).IsCheckedOut());
}

int CToDoCtrlMgr::CanCheckInOut(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	if (!Prefs().GetEnableSourceControl())
		return FALSE;

	const CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	
	BOOL bCanCheckInOut = (tdc.CompareFileFormat() != TDCFF_NEWER);
	
	if (bCanCheckInOut)
	{
		bCanCheckInOut &= PathSupportsSourceControl(nIndex);
		
		if (bCanCheckInOut)
			bCanCheckInOut &= !GetReadOnlyStatus(nIndex);
	}

	return bCanCheckInOut;
}

void CToDoCtrlMgr::SetModifiedStatus(int nIndex, BOOL bMod)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bModified = bMod;
}

BOOL CToDoCtrlMgr::GetModifiedStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bModified;
}

void CToDoCtrlMgr::RefreshModifiedStatus(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);
	tdci.bModified = tdci.pTDC->IsModified();
}

void CToDoCtrlMgr::UpdateToDoCtrlReadOnlyUIState(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	UpdateToDoCtrlReadOnlyUIState(tdc);
}

void CToDoCtrlMgr::UpdateToDoCtrlReadOnlyUIState(CFilteredToDoCtrl& tdc)
{
    LPCTSTR szPath = tdc.GetFilePath();
	BOOL bReadOnly = CDriveInfo::IsReadonlyPath(szPath) >  0;
	
    if (!bReadOnly)
        bReadOnly = (tdc.CompareFileFormat() == TDCFF_NEWER);
	
	if (!bReadOnly)
		bReadOnly = (PathSupportsSourceControl(szPath) && !tdc.IsCheckedOut());
	
	if (!bReadOnly && !Prefs().GetEnableSourceControl())
		bReadOnly = tdc.IsSourceControlled();

	tdc.SetReadonly(bReadOnly);
}

int CToDoCtrlMgr::RemoveToDoCtrl(int nIndex, BOOL bDelete)
{
	ASSERTVALIDINDEX(nIndex);

	int nSel = GetSelToDoCtrl(), nNewSel = -1;
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	// save filter state first
	CString sFilePath(tdc.GetFilePath());

	if (!sFilePath.IsEmpty())
	{
		CString sKey;

		sFilePath.Replace('\\', '_');
		sKey.Format("Filter\\%s", sFilePath);
			
		FTDCFILTER filter;
		tdc.GetFilter(filter);

		AfxGetApp()->WriteProfileInt(sKey, "Filter", filter.nFilter);
		AfxGetApp()->WriteProfileInt(sKey, "Priority", filter.nPriority);
		AfxGetApp()->WriteProfileInt(sKey, "Risk", filter.nRisk);
		AfxGetApp()->WriteProfileString(sKey, "AllocTo", filter.sAllocTo);
		AfxGetApp()->WriteProfileString(sKey, "AllocBy", filter.sAllocBy);
		AfxGetApp()->WriteProfileString(sKey, "Category", Misc::FormatArray(filter.aCategories));
		AfxGetApp()->WriteProfileInt(sKey, "AnyCategory", filter.HasFlag(FT_ANYCATEGORY));
	}

	if (bDelete)
	{
		// checkin as our final task
		if (Prefs().GetCheckinOnClose() && PathSupportsSourceControl(nIndex))
			tdc.CheckIn();
	}
	
	m_aToDoCtrls.RemoveAt(nIndex);
	m_tabCtrl.DeleteItem(nIndex);

	if (bDelete)
	{
		tdc.DestroyWindow();
		delete &tdc;
	}

	// set new selection as close to previous as poss
	if (GetCount())
	{
		if (nIndex <= nSel)
			nNewSel = max(0, nSel - 1);
		else
			nNewSel = nSel;

		m_tabCtrl.SetCurSel(nNewSel);
	}

   return nNewSel;
}

int CToDoCtrlMgr::AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bReOrder)
{
	static int nNextIndex = 0;

	TDCITEM tdci(pCtrl, nNextIndex++);

	int nSel = m_aToDoCtrls.Add(tdci);
	m_tabCtrl.InsertItem(nSel, "");

	UpdateTabItemText(nSel);
	RefreshPathType(nSel);

	if (bReOrder)
		SortToDoCtrlsByName();

	// restore previous filter state
	if (Prefs().GetRestoreTasklistFilters())
	{
		CString sFilePath(pCtrl->GetFilePath());

		if (!sFilePath.IsEmpty())
		{
			CString sKey;

			sFilePath.Replace('\\', '_');
			sKey.Format("Filter\\%s", sFilePath);
				
			FTDCFILTER filter;

			filter.nFilter = (FILTER_TYPE)AfxGetApp()->GetProfileInt(sKey, "Filter", FT_ALL);
			filter.nPriority = AfxGetApp()->GetProfileInt(sKey, "Priority", -1);
			filter.nRisk = AfxGetApp()->GetProfileInt(sKey, "Risk", -1);
			filter.sAllocTo = AfxGetApp()->GetProfileString(sKey, "AllocTo");
			filter.sAllocBy = AfxGetApp()->GetProfileString(sKey, "AllocBy");

			filter.SetFlag(FT_ANYCATEGORY, AfxGetApp()->GetProfileInt(sKey, "AnyCategory", FALSE));

			CString sCategory = AfxGetApp()->GetProfileString(sKey, "Category");
			Misc::ParseIntoArray(sCategory, filter.aCategories, TRUE);

			pCtrl->SetFilter(filter);
		}
	}

	return nSel;
}

time_t CToDoCtrlMgr::GetLastModified(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).tLastMod;
}

void CToDoCtrlMgr::MoveToDoCtrl(int nIndex, int nNumPlaces)
{
	ASSERTVALIDINDEX(nIndex);

	int nOrgCount = m_tabCtrl.GetItemCount();

	if (!CanMoveToDoCtrl(nIndex, nNumPlaces))
		return;

	// cache selection so we can restore it afterwards
	int nSel = GetSelToDoCtrl(), nNewSel = nSel;

	// work out what the new selection should be
	if (nIndex == nSel)
		nNewSel = (nIndex + nNumPlaces);

	else if (nIndex > nSel && (nIndex + nNumPlaces) <= nSel)
		nNewSel++;

	else if (nIndex < nSel && (nIndex + nNumPlaces) >= nSel)
		nNewSel--;

	// make copies of existing
	TCITEM tci;
	char szText[128];
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = szText;
	tci.cchTextMax = 127;
		
	m_tabCtrl.GetItem(nIndex, &tci);
	TDCITEM tdci = GetTDCItem(nIndex);

	// remove and re-add
	RemoveToDoCtrl(nIndex);

	nIndex += nNumPlaces;

	m_aToDoCtrls.InsertAt(nIndex, tdci);
	m_tabCtrl.InsertItem(nIndex, &tci);

	// set selection
	m_tabCtrl.SetCurSel(nNewSel);

	ASSERT (nOrgCount == m_tabCtrl.GetItemCount());
}

BOOL CToDoCtrlMgr::CanMoveToDoCtrl(int nIndex, int nNumPlaces) const
{
	ASSERTVALIDINDEX(nIndex);

	nIndex += nNumPlaces;

	return (nIndex >= 0 && nIndex < GetCount());
}

CString CToDoCtrlMgr::GetArchivePath(LPCTSTR szFilePath) const
{
	CString sArchivePath, sFilePath(szFilePath);
	sFilePath.MakeLower();

	CEnString sDone(".%s", CEnString(IDS_TDC_DONE));
	
	if (!sFilePath.IsEmpty() && sFilePath.Find(sDone) == -1) // don't archive archives!
	{
		char szDrive[_MAX_DRIVE], szPath[MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];
		_splitpath(szFilePath, szDrive, szPath, szFName, szExt);
		
		sArchivePath.Format("%s%s%s%s%s", szDrive, szPath, szFName, sDone, szExt);
	}
	
	return sArchivePath;
}

CString CToDoCtrlMgr::GetArchivePath(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	const CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	return GetArchivePath(tdc.GetFilePath());
}

int CToDoCtrlMgr::ArchiveDoneTasks(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	TDC_ARCHIVE nRemove = TDC_REMOVENONE;
	
	if (Prefs().GetRemoveArchivedTasks())
	{
		if (Prefs().GetRemoveOnlyOnAbsoluteCompletion())
			nRemove = TDC_REMOVEIFSIBLINGSANDSUBTASKSCOMPLETE;
		else
			nRemove = TDC_REMOVEALL;
	}
	
	return tdc.ArchiveDoneTasks(GetArchivePath(tdc.GetFilePath()), nRemove,
								!Prefs().GetDontRemoveFlagged());
}

int CToDoCtrlMgr::SortToDoCtrlsByName()
{
	int nSel = GetSelToDoCtrl();

	if (!AreToDoCtrlsSorted())
	{
		// save off current selection 
		CFilteredToDoCtrl* pTDC = &GetToDoCtrl(nSel);

		qsort(m_aToDoCtrls.GetData(), m_aToDoCtrls.GetSize(), sizeof(TDCITEM), SortProc);

		nSel = -1;

		// redo the tab text
		for (int nTDC = 0; nTDC < GetCount(); nTDC++)
		{
			UpdateTabItemText(nTDC);

			// check if this was the selected item
			if (pTDC == &GetToDoCtrl(nTDC))
				nSel = nTDC;
		}

		// set selection
		ASSERT (nSel != -1);
	}

	return nSel;
}

int CToDoCtrlMgr::SortProc(const void* v1, const void* v2)
{
	TDCITEM* pTDCI1 = (TDCITEM*)v1;
	TDCITEM* pTDCI2 = (TDCITEM*)v2;

	CString sName1 = pTDCI1->GetFriendlyProjectName();
	CString sName2 = pTDCI2->GetFriendlyProjectName();

	return sName1.CompareNoCase(sName2);
}

BOOL CToDoCtrlMgr::AreToDoCtrlsSorted() const
{
	if (GetCount() <= 1)
		return TRUE;

	// check that each item is 'less' than the next
	CString sPrev = GetTDCItem(0).GetFriendlyProjectName();

	for (int nTDC = 1; nTDC < GetCount(); nTDC++)
	{
		CString sTDC = GetTDCItem(nTDC).GetFriendlyProjectName();

		if (sPrev.CompareNoCase(sTDC) > 0)
			return FALSE;

		sPrev = sTDC;
	}

	return TRUE;
}

CString CToDoCtrlMgr::UpdateTabItemText(int nIndex)
{
	if (nIndex < 0)
	{
		nIndex = GetSelToDoCtrl();
		
		if (nIndex < 0)
			return "";
	}
	
	TDCITEM& tdci = GetTDCItem(nIndex);
	
	// project name
	CString sProjectName = tdci.GetFriendlyProjectName();

	// double up ampersands to avoid unexpected underlining
	sProjectName.Replace("&", "&&");

	if (tdci.pTDC->IsModified() && !tdci.pTDC->IsReadOnly())
		sProjectName += "*";
	
	// appropriate icon
	int nImage = IM_NONE;
	
	if (tdci.bLastStatusReadOnly > 0)
		nImage = IM_READONLY;
	
	else if (PathSupportsSourceControl(nIndex))
        nImage = tdci.pTDC->IsCheckedOut() ? IM_CHECKEDOUT : IM_CHECKEDIN;
	
	else if (tdci.pTDC->IsReadOnly())
		nImage = IM_READONLY;
	
    // update tab array
	TCITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)sProjectName;
	tci.iImage = nImage;
	
	m_tabCtrl.SetItem(nIndex, &tci);

	// remove doubled-up ampersands
	sProjectName.Replace("&&", "&");
	
	return sProjectName;
}

CString CToDoCtrlMgr::GetTabItemText(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	TCITEM tci;
	CString sText;
	
	tci.mask = TCIF_TEXT;
	tci.pszText = sText.GetBuffer(128);
	tci.cchTextMax = 127;
		
	m_tabCtrl.GetItem(nIndex, &tci);

	sText.ReleaseBuffer();
	sText.Replace("&&", "&"); // remove doubled-up ampersands

	return sText;
}

BOOL CToDoCtrlMgr::PathTypeSupportsSourceControl(TDCM_PATHTYPE nType) const
{
	switch (nType)
    {
    case TDCM_FIXED:          
    case TDCM_REMOTE:         
        break;
		
    default: // everything else
        return FALSE;
    }
	
	BOOL bEnableSourceControl = Prefs().GetEnableSourceControl();
	BOOL bLanOnly = Prefs().GetSourceControlLanOnly();
	
    return (bEnableSourceControl && (!bLanOnly || nType == TDCM_REMOTE));
}

BOOL CToDoCtrlMgr::PathSupportsSourceControl(LPCTSTR szPath) const
{
    int nType = CDriveInfo::GetPathType(szPath);

	return PathTypeSupportsSourceControl(TDCITEM::TranslatePathType(nType));
}

BOOL CToDoCtrlMgr::PathSupportsSourceControl(int nIndex) const
{
	TDCM_PATHTYPE nType = GetFilePathType(nIndex);
	
	return PathTypeSupportsSourceControl(nType);
}

BOOL CToDoCtrlMgr::IsSourceControlled(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);
	
	return GetToDoCtrl(nIndex).IsSourceControlled();
}

void CToDoCtrlMgr::SetNeedsPreferenceUpdate(int nIndex, BOOL bNeed)
{
	ASSERTVALIDINDEX(nIndex);
	
	GetTDCItem(nIndex).bNeedPrefUpdate = bNeed;
}

BOOL CToDoCtrlMgr::GetNeedsPreferenceUpdate(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);
	
	return GetTDCItem(nIndex).bNeedPrefUpdate;
}

void CToDoCtrlMgr::SetAllNeedPreferenceUpdate(BOOL bNeed)
{
	for (int nIndex = 0; nIndex < m_aToDoCtrls.GetSize(); nIndex++)
		GetTDCItem(nIndex).bNeedPrefUpdate = bNeed;
}

int CToDoCtrlMgr::FindToDoCtrl(LPCTSTR szFilePath)
{
	if (!szFilePath || GetFileAttributes(szFilePath) != 0xffffffff)
	{
		int nCtrl = GetCount();
	
		while (nCtrl--)
		{
			if (GetFilePath(nCtrl).CompareNoCase(szFilePath) == 0)
				return nCtrl;
		}
	}

	return -1;
}

void CToDoCtrlMgr::PreparePopupMenu(CMenu& menu, UINT nID1, int nMax)
{
	for (int nTDC = 0; nTDC < GetCount() && nTDC < nMax; nTDC++)
	{
		CString sTaskList = GetFriendlyProjectName(nTDC);

		menu.ModifyMenu(nID1 + nTDC, MF_BYCOMMAND, nID1 + nTDC, sTaskList);
	}

	// delete any unused items
	for (; nTDC < nMax; nTDC++)
		menu.RemoveMenu(nID1 + nTDC, MF_BYCOMMAND);

}

BOOL CToDoCtrlMgr::HasTasks(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);
	
	return GetToDoCtrl(nIndex).GetTaskCount();
}

BOOL CToDoCtrlMgr::AnyHasTasks() const
{
	int nCtrl = GetCount();
	
	while (nCtrl--)
	{
		if (HasTasks(nCtrl))
			return TRUE;
	}
	
	return FALSE;
}

