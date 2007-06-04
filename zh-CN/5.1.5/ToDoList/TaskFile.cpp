// TaskFile.cpp: implementation of the CTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskFile.h"
#include "tdlschemadef.h"
#include "mergetodolist.h"
#include "tdcstruct.h"

#include "..\shared\timeedit.h"
#include "..\shared\datehelper.h"
#include "..\3rdparty\Base64Coder.h"
#include "..\shared\misc.h"
#include "..\shared\filemisc.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define GET_TASK(t, h, r) { t = TaskFromHandle(h); if (!t) return r; }

CTaskFile::CTaskFile(LPCTSTR szPassword) :
	CXmlFileEx(TDL_ROOT, szPassword), m_dwNextUniqueID(0)
{
}

CTaskFile::~CTaskFile()
{
}

HRESULT CTaskFile::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;

	// compare riid against our supported version numbers
	if (IsEqualIID(riid, IID_TASKLIST2))
	{
		*ppvObject = reinterpret_cast<ITaskList2*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST3))
	{
		*ppvObject = reinterpret_cast<ITaskList3*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST4))
	{
		*ppvObject = reinterpret_cast<ITaskList4*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST5))
	{
		*ppvObject = reinterpret_cast<ITaskList5*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST6))
	{
		*ppvObject = reinterpret_cast<ITaskList6*>(this);
		AddRef();
	}
	
	return (*ppvObject ? S_OK : E_NOTIMPL);
}

BOOL CTaskFile::Decrypt(LPCTSTR szPassword)
{
	BOOL bWasEncrypted = IsEncrypted();
	BOOL bResult = CXmlFileEx::Decrypt(szPassword);

	if (bResult && bWasEncrypted)
	{
		// fix corrupted tasklist where the root item has an ID
		CXmlItem* pXI = GetItem(TDL_TASKID);

		while (pXI)
		{
			DeleteItem(pXI);
			pXI = GetItem(TDL_TASKID);
		}

		m_dwNextUniqueID = (DWORD)GetItemValueI(TDL_NEXTUNIQUEID);

		if (m_dwNextUniqueID <= 0)
			m_dwNextUniqueID = 1; // always > 0

		BuildHandleMap();
	}

	return bResult;
}

BOOL CTaskFile::Load(LPCTSTR szFilePath, LPCTSTR szRootItemName, IXmlParse* pCallback, BOOL bDecrypt)
{
  BOOL bRes = CXmlFileEx::Load(szFilePath, szRootItemName, pCallback, bDecrypt);

  if (bRes)
  {
		m_dwNextUniqueID = (DWORD)GetItemValueI(TDL_NEXTUNIQUEID);

		if (m_dwNextUniqueID <= 0)
			m_dwNextUniqueID = 1; // always > 0

		BuildHandleMap();
  }

  return bRes;
}

BOOL CTaskFile::LoadEx(LPCTSTR szRootItemName, IXmlParse* pCallback)
{
	BOOL bResult = CXmlFileEx::LoadEx(szRootItemName, pCallback);

	if (bResult && !IsEncrypted())
	{
		// fix corrupted tasklist where the root item has an ID
		CXmlItem* pXI = GetItem(TDL_TASKID);

		while (pXI)
		{
			DeleteItem(pXI);
			pXI = GetItem(TDL_TASKID);
		}

		m_dwNextUniqueID = (DWORD)GetItemValueI(TDL_NEXTUNIQUEID);

		if (m_dwNextUniqueID <= 0)
			m_dwNextUniqueID = 1; // always > 0

		BuildHandleMap();
	}

	return bResult;
}

BOOL CTaskFile::SaveEx()
{
	return CXmlFileEx::SaveEx();
}

void CTaskFile::Copy(const CTaskFile& tasks)
{
	CXmlFileEx::Copy(tasks);
	BuildHandleMap();
}

int CTaskFile::Merge(const CTaskFile& tasks, BOOL bByID, BOOL bMoveExist)
{
	CMergeToDoList mtdl(bByID ? TDLM_BYID : TDLM_BYTITLE, 
						bMoveExist ? TDLM_MOVE : TDLM_LEAVE);

	int nMerge = mtdl.Merge(tasks.Root(), Root());
	BuildHandleMap();

	return nMerge;
}

int CTaskFile::Merge(LPCTSTR szTaskFilePath, BOOL bByID, BOOL bMoveExist)
{
	CMergeToDoList mtdl(bByID ? TDLM_BYID : TDLM_BYTITLE, 
						bMoveExist ? TDLM_MOVE : TDLM_LEAVE);
	
	int nMerge = mtdl.Merge(szTaskFilePath, Root());
	BuildHandleMap();

	return nMerge;
}

void CTaskFile::SortTasksByID()
{
	SortItemsI(TDL_TASK, TDL_TASKID);
}

void CTaskFile::SortTasksByPos()
{
	SortItemsI(TDL_TASK, TDL_TASKPOS);
}

DWORD CTaskFile::GetNextUniqueID() const
{
	return m_dwNextUniqueID;
}

void CTaskFile::BuildHandleMap()
{
	m_mapHandles.RemoveAll();

	AddTaskToMap(GetItem(TDL_TASK), TRUE); // first top level item
}

void CTaskFile::AddTaskToMap(CXmlItem* pXITask, BOOL bRecurse)
{
	if (pXITask)
	{
		// sanity check
		ASSERT (pXITask->NameIs(TDL_TASK));
		
		m_mapHandles[(HTASKITEM)pXITask] = pXITask;

		if (bRecurse)
		{
			// next item
			AddTaskToMap(pXITask->GetSibling(), TRUE);

			// children
			// note: we only need do the first child
			AddTaskToMap(pXITask->GetItem(TDL_TASK), TRUE);
		}
	}
}

void CTaskFile::RemoveTaskFromMap(CXmlItem* pXITask)
{
	if (pXITask)
	{
		// sanity check
		ASSERT (pXITask->NameIs(TDL_TASK));
		
		m_mapHandles.RemoveKey((HTASKITEM)pXITask);

		// children
		CXmlItem* pXIChild = pXITask->GetItem(TDL_TASK);

		while (pXIChild)
		{
			RemoveTaskFromMap(pXIChild);
			pXIChild = pXIChild->GetSibling();
		}
	}
}

CXmlItem* CTaskFile::TaskFromHandle(HTASKITEM hTask) const
{
#ifdef _DEBUG
	CXmlItem* pXITask = NULL;

	if (hTask)
		m_mapHandles.Lookup(hTask, pXITask);

	return pXITask;
#else
	return static_cast<CXmlItem*>(hTask);
#endif
}

bool CTaskFile::IsArchive() const
{
	return (NULL != GetItem(TDL_ARCHIVE));
}

bool CTaskFile::IsCheckedOut() const
{
	return (Misc::GetComputerName() == GetCheckOutTo());
}	

bool CTaskFile::IsSourceControlled() const
{
	return (NULL != GetItem(TDL_CHECKEDOUTTO));
}

const char* CTaskFile::GetProjectName() const
{
	return GetItemValue(TDL_PROJECTNAME);
}

const char* CTaskFile::GetReportTitle() const
{
	return GetItemValue(TDL_REPORTTITLE);
}

const char* CTaskFile::GetHtmlCharSet() const
{
	return GetItemValue(TDL_CHARSET);
}

const char* CTaskFile::GetAttribute(const char* szAttrib) const
{
	return GetItemValue(szAttrib);
}

const char* CTaskFile::GetReportDate() const
{
	return GetItemValue(TDL_REPORTDATE);
}

const char* CTaskFile::GetCheckOutTo() const
{
	return GetItemValue(TDL_CHECKEDOUTTO);
}	

unsigned long CTaskFile::GetFileFormat() const
{
	return GetItemValueI(TDL_FILEFORMAT);
}

unsigned long CTaskFile::GetFileVersion() const
{
	return GetItemValueI(TDL_FILEVERSION);
}
	
time_t CTaskFile::GetLastModified() const
{
	COleDateTime date(GetItemValueF(TDL_LASTMODIFIED));

	tm time = { date.GetHour(), 
		        0, 
				date.GetDay(), 
				date.GetMinute(), 
				date.GetMonth(), 
				date.GetSecond(), 
				0, 
				0, 
				date.GetYear() - 1900 };

	return mktime(&time);
}

BOOL CTaskFile::SetCustomCommentsType(const GUID& guid)
{
	CString sGuid;

	if (!Misc::GuidIsNull(guid) && Misc::GuidToString(guid, sGuid))
	{
		CXmlItem* pXItem = GetItem(TDL_CUSTOMCOMMENTSTYPE);

		if (pXItem)
		{
			pXItem->SetValue(sGuid);
			return TRUE;
		}

		// else
		return (NULL != AddItem(TDL_CUSTOMCOMMENTSTYPE, sGuid));
	}

	return FALSE;
}

BOOL CTaskFile::GetCustomCommentsType(GUID& guid) const
{
	CString sGuid = GetItemValue(TDL_CUSTOMCOMMENTSTYPE);

	return (Misc::GuidFromString(sGuid, guid) && !Misc::GuidIsNull(guid));
}

bool CTaskFile::SetProjectName(const char* szName)
{
	CXmlItem* pXItem = GetItem(TDL_PROJECTNAME);

	if (pXItem)
	{
		pXItem->SetValue(szName);
		return true;
	}

	// else
	return (NULL != AddItem(TDL_PROJECTNAME, szName));
}

BOOL CTaskFile::SetArchive(BOOL bArchive)
{
	CXmlItem* pXItem = GetItem(TDL_ARCHIVE);

	if (pXItem)
	{
		if (bArchive)
			pXItem->SetValue((int)1);
		else
			DeleteItem(pXItem);

		return true;
	}
	else if (bArchive)
		return (NULL != AddItem(TDL_ARCHIVE, (int)1));

	// else
	return TRUE;
}

bool CTaskFile::SetFileVersion(unsigned long nVersion)
{
	CXmlItem* pXItem = GetItem(TDL_FILEVERSION);

	if (pXItem)
	{
		pXItem->SetValue((int)nVersion);
		return true;
	}

	// else
	return (NULL != AddItem(TDL_FILEVERSION, (int)nVersion));
}

BOOL CTaskFile::SetCheckedOutTo(const CString& sCheckedOutTo)
{
	CXmlItem* pXItem = GetItem(TDL_CHECKEDOUTTO);

	if (pXItem)
	{
		pXItem->SetValue(sCheckedOutTo);
		return true;
	}

	// else
	return (NULL != AddItem(TDL_CHECKEDOUTTO, sCheckedOutTo));
}

BOOL CTaskFile::SetCharSet(LPCTSTR szCharSet)
{
	CXmlItem* pXItem = GetItem(TDL_CHARSET);

	if (pXItem)
	{
		if (szCharSet && *szCharSet)
			pXItem->SetValue(szCharSet);
		else
			DeleteItem(pXItem);
	
		return TRUE;
	}
	else if (szCharSet && *szCharSet)
		return (NULL != AddItem(TDL_CHARSET, szCharSet));

	// else
	return TRUE;
}

BOOL CTaskFile::SetFileName(LPCTSTR szFilename)
{
	CXmlItem* pXItem = GetItem(TDL_FILENAME);
	szFilename = FileMisc::FileNameFromPath(szFilename);

	if (pXItem)
	{
		if (szFilename && *szFilename)
			pXItem->SetValue(szFilename);
		else
			DeleteItem(pXItem);
	
		return TRUE;
	}
	else if (szFilename && *szFilename)
		return (NULL != AddItem(TDL_FILENAME, szFilename));

	// else
	return TRUE;
}

BOOL CTaskFile::SetFileFormat(unsigned long lFormat)
{
	CXmlItem* pXItem = GetItem(TDL_FILEFORMAT);

	if (pXItem)
	{
		pXItem->SetValue((int)lFormat);
		return true;
	}

	// else
	return (NULL != AddItem(TDL_FILEFORMAT, (int)lFormat));
}

BOOL CTaskFile::SetNextUniqueID(DWORD dwNextID)
{
	ASSERT (dwNextID >= m_dwNextUniqueID);

	BOOL bResult = FALSE;
	CXmlItem* pXItem = GetItem(TDL_NEXTUNIQUEID);

	if (pXItem)
	{
		pXItem->SetValue((int)dwNextID);
		bResult = TRUE;
	}
	else
		bResult = (NULL != AddItem(TDL_NEXTUNIQUEID, (int)dwNextID));

	if (bResult)
		m_dwNextUniqueID = dwNextID;

	return bResult;
}

BOOL CTaskFile::SetLastModified(const CString& sLastMod)
{
	CXmlItem* pXItem = GetItem(TDL_LASTMODIFIED);

	if (pXItem)
	{
		pXItem->SetValue(sLastMod);
		return true;
	}

	// else
	return (NULL != AddItem(TDL_LASTMODIFIED, sLastMod));
}

BOOL CTaskFile::CheckOut()
{
	CString sTemp;
	return CheckOut(sTemp);
}

BOOL CTaskFile::CheckOut(CString& sCheckedOutTo)
{
	// make sure its got a filepath attached
	if (GetFilePath().IsEmpty())
		return FALSE;
	
	sCheckedOutTo = GetCheckOutTo();
	
	if (sCheckedOutTo.GetLength())
	{
		if (sCheckedOutTo == Misc::GetComputerName()) // its us
			return TRUE;
		
		else if (!sCheckedOutTo.IsEmpty()) // someone else
			return FALSE;
		
		// else check it out
		SetCheckedOutTo(Misc::GetComputerName());
	}
	else // check it out
		SetCheckedOutTo(Misc::GetComputerName());
	
	// and rewrite the file but keeping the same timestamp
	FILETIME ftMod;
	::GetFileTime((HANDLE)GetFileHandle(), NULL, NULL, &ftMod);
	
	SortTasksByID();
	BOOL bCheckedOut = SaveEx();
	
	::SetFileTime((HANDLE)GetFileHandle(), NULL, NULL, &ftMod);
	
	if (bCheckedOut)
		sCheckedOutTo = Misc::GetComputerName();
	
	return bCheckedOut;
}

BOOL CTaskFile::SetReportAttributes(LPCTSTR szTitle, LPCTSTR szDate)
{
	BOOL bRes = TRUE;

	CXmlItem* pXItem = GetItem(TDL_REPORTTITLE);

	if (pXItem)
		pXItem->SetValue(szTitle);
	else
		bRes &= (NULL != AddItem(TDL_REPORTTITLE, szTitle));

	if (szDate && *szDate)
	{
		pXItem = GetItem(TDL_REPORTDATE);

		if (pXItem)
			pXItem->SetValue(szDate);
		else
			bRes &= (NULL != AddItem(TDL_REPORTDATE, szDate));
	}

	return bRes;
}

BOOL CTaskFile::DeleteTaskAttributes(HTASKITEM hTask)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	POSITION pos = pXITask->GetFirstItemPos();

	while (pos)
	{
		CXmlItem* pXIChild = pXITask->GetNextItem(pos);

		if (!pXIChild->NameMatches(TDL_TASK))
			pXITask->DeleteItem(pXIChild);
	}

	return TRUE;
}

HTASKITEM CTaskFile::FindTask(DWORD dwTaskID) const
{
	const CXmlItem* pXI = FindItem(TDL_TASKID, (int)dwTaskID);

	return pXI ? (HTASKITEM)(pXI->GetParent()) : NULL;
}

BOOL CTaskFile::DeleteTask(HTASKITEM hTask)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	CXmlItem* pXIParent = pXITask->GetParent();
	ASSERT (pXIParent);

	// remove task from map first
	RemoveTaskFromMap(pXITask);

	return pXIParent->DeleteItem(pXITask);
}

BOOL CTaskFile::SetTaskCustomComments(HTASKITEM hTask, const CString& sContent)
{
	if (!TaskFromHandle(hTask))
		return FALSE;

	Base64Coder b64;
	b64.Encode((const PBYTE)(LPCTSTR)sContent, sContent.GetLength());

	return SetTaskCChar(hTask, TDL_TASKCUSTOMCOMMENTS, b64.EncodedMessage());
}

BOOL CTaskFile::SetTaskHtmlComments(HTASKITEM hTask, const CString& sContent, BOOL bForTransform)
{
	if (!TaskFromHandle(hTask))
		return FALSE;

	return SetTaskCChar(hTask, TDL_TASKHTMLCOMMENTS, sContent, !bForTransform);
}

BOOL CTaskFile::GetTaskCustomComments(HTASKITEM hTask, CString& sContent) const
{
	CString sTemp = GetTaskCChar(hTask, TDL_TASKCUSTOMCOMMENTS);

	if (sTemp.IsEmpty())
		return FALSE;

	Base64Coder b64;
	b64.Decode(sTemp);

	unsigned long nLenContent;
	PBYTE pContent = b64.DecodedMessage(nLenContent);

	PBYTE szContent = (PBYTE)sContent.GetBuffer(nLenContent);
	CopyMemory(szContent, pContent, nLenContent);
	sContent.ReleaseBuffer(nLenContent);
	
	return nLenContent;
}

BOOL CTaskFile::SetTaskCategories(HTASKITEM hTask, const CStringArray& aCategories)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	// delete any existing categories
	pXITask->DeleteItem(TDL_TASKCATEGORY);

	// then add these
	Misc::Trace(aCategories);
	int nCatCount = aCategories.GetSize();
	
	for (int nCat = 0; nCat < nCatCount; nCat++)
		AddTaskCategory(hTask, aCategories[nCat]);
	
/*
	if (nCatCount)
	{
		pXITask->AddItem(TDL_TASKNUMCATEGORY, nCatCount); // num
		pXITask->AddItem(TDL_TASKCATEGORY, aCategories[0]); // first category

		// the rest have numbers after their names
		for (int nCat = 1; nCat < nCatCount; nCat++)
		{
			CString sItem;
			sItem.Format("%s%d", TDL_TASKCATEGORY, nCat);

			pXITask->AddItem(sItem, aCategories[nCat]);
		}
	}
*/

	return TRUE;
}

bool CTaskFile::AddTaskCategory(HTASKITEM hTask, const char* szCategory)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   int nCatCount = pXITask->GetItemValueI(TDL_TASKNUMCATEGORY);

   // see if it already exists
   if (nCatCount || pXITask->GetItem(TDL_TASKCATEGORY))
   {
	   CString sValue = pXITask->GetItemValue(TDL_TASKCATEGORY);

	   if (sValue.CompareNoCase(szCategory) == 0)
		  return false; // already exists

	   // the rest have numbers after their names
	   for (int nCat = 1; nCat < nCatCount; nCat++)
	   {
		  CString sItem;
		  sItem.Format("%s%d", TDL_TASKCATEGORY, nCat);
      
		  CString sValue = pXITask->GetItemValue(sItem);

		  if (sValue.CompareNoCase(szCategory) == 0)
			 return false; // already exists
	   }
   }

   // does the task have an existing category?
   if (!pXITask->GetItem(TDL_TASKCATEGORY))
   {
	   pXITask->AddItem(TDL_TASKNUMCATEGORY, 1); // num
	   pXITask->AddItem(TDL_TASKCATEGORY, szCategory); // first category
   }
   else // append
   {
	   // increment cat count
	   pXITask->SetItemValue(TDL_TASKNUMCATEGORY, nCatCount + 1);
	   
	   CString sItem;
	   sItem.Format("%s%d", TDL_TASKCATEGORY, nCatCount);
	   
	   pXITask->AddItem(sItem, szCategory);
   }
   
   return true;
}

int CTaskFile::GetTaskCategories(HTASKITEM hTask, CStringArray& aCategories) const
{
	aCategories.RemoveAll();

	// first category
	CString sCat = GetTaskCategory(hTask);

	if (!sCat.IsEmpty())
	{
		aCategories.Add(sCat);

		// rest
		int nCatCount = GetTaskCategoryCount(hTask);

		for (int nCat = 1; nCat < nCatCount; nCat++)
			aCategories.Add(GetTaskCategory(hTask, nCat));
	}

	return aCategories.GetSize();
}

CXmlItem* CTaskFile::NewItem(LPCTSTR szName)
{
	return new CXmlItem(NULL, szName);
}

///////////////////////////////////////////////////////////////////////

HTASKITEM CTaskFile::NewTask(const char* szTitle, HTASKITEM hParent, DWORD dwID)
{
	CXmlItem* pXIParent = hParent ? TaskFromHandle(hParent) : Root();

	if (!pXIParent)
	{
		// handle the fact that hParent might already point to Root()
		if (hParent == (HTASKITEM)Root())
			pXIParent = Root();
		else
			return NULL;
	}

	CXmlItem* pXINew = NewItem(TDL_TASK);

	if (pXINew)
	{
		pXIParent->AddItem(pXINew);
		AddTaskToMap(pXINew, FALSE);

		// set ID and name
		SetTaskTitle((HTASKITEM)pXINew, szTitle);

		if (dwID <= 0)
			dwID = m_dwNextUniqueID++;
		else
			m_dwNextUniqueID = max(m_dwNextUniqueID, dwID + 1);

		SetTaskID((HTASKITEM)pXINew, dwID);
	}

	return (HTASKITEM)pXINew;
}

HTASKITEM CTaskFile::NewTask(const char* szTitle, HTASKITEM hParent)
{
	return NewTask(szTitle, hParent, 0); // 0 means 'allocate new ID'
}

HTASKITEM CTaskFile::GetFirstTask(HTASKITEM hParent) const
{
	const CXmlItem* pXIParent = hParent ? TaskFromHandle(hParent) : Root();

	if (!pXIParent)
	{
		// handle the fact that hParent might already point to Root()
		if (hParent == (HTASKITEM)Root())
			pXIParent = Root();
		else
			return NULL;
	}

	return (HTASKITEM)pXIParent->GetItem(TDL_TASK);
}

HTASKITEM CTaskFile::GetNextTask(HTASKITEM hTask) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return (HTASKITEM)pXITask->GetSibling();
}

const char* CTaskFile::GetTaskTitle(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKTITLE);
}

const char* CTaskFile::GetTaskComments(HTASKITEM hTask) const
{
/*
	CString sComments = GetTaskCChar(hTask, TDL_TASKCOMMENTS);
	sComments.Replace("&#13;&#10;", "\r\n");

	return sComments;
*/
	return GetTaskCChar(hTask, TDL_TASKCOMMENTS);
}

const char* CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKALLOCTO);
}

const char* CTaskFile::GetTaskAllocatedBy(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKALLOCBY);
}

const char* CTaskFile::GetTaskCategory(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCATEGORY);
}

unsigned char CTaskFile::GetTaskCategoryCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMCATEGORY);
}

const char* CTaskFile::GetTaskCategory(HTASKITEM hTask, int nIndex) const
{
	if (nIndex == 0)
		return GetTaskCategory(hTask); // first category

	// else
	int nCatCount = GetTaskCategoryCount(hTask);

	if (!nCatCount || nIndex < 1 || nIndex > nCatCount - 1)
		return NULL;

	CString sCatItem;
	sCatItem.Format("%s%d", TDL_TASKCATEGORY, nIndex);
	
	return GetTaskCChar(hTask, sCatItem);
}

const char* CTaskFile::GetTaskDependency(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKDEPENDENCY);
}

const char* CTaskFile::GetTaskStatus(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKSTATUS);
}

const char* CTaskFile::GetTaskFileReferencePath(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKFILEREFPATH);
}

const char* CTaskFile::GetTaskCreatedBy(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCREATEDBY);
}

unsigned long CTaskFile::GetTaskID(HTASKITEM hTask) const
{
	return GetTaskULong(hTask, TDL_TASKID);
}

bool CTaskFile::IsTaskFlagged(HTASKITEM hTask) const
{
	return (GetTaskUChar(hTask, TDL_TASKFLAG) > 0);
}

unsigned long CTaskFile::GetTaskColor(HTASKITEM hTask) const
{
	return GetTaskULong(hTask, TDL_TASKCOLOR);
}

const char* CTaskFile::GetTaskWebColor(HTASKITEM hTask) const
{
	DWORD color = 0;

	if (TaskHasAttribute(hTask, TDL_TASKTEXTCOLOR))
		color = GetTaskULong(hTask, TDL_TASKTEXTCOLOR);
	else
		color = GetTaskULong(hTask, TDL_TASKCOLOR);
	
	return GetWebColor((COLORREF)color);
}

const char* CTaskFile::GetTaskPriorityWebColor(HTASKITEM hTask) const
{
	return GetWebColor(GetTaskPriorityColor(hTask));
}

unsigned long CTaskFile::GetTaskPriorityColor(HTASKITEM hTask) const
{
	DWORD color = RGB(255, 255, 255);

	if (TaskHasAttribute(hTask, TDL_TASKPRIORITYCOLOR))
		color = GetTaskULong(hTask, TDL_TASKPRIORITYCOLOR);

	return color;
}

unsigned char CTaskFile::GetTaskPriority(HTASKITEM hTask, BOOL bHighest) const
{
	if (bHighest && TaskHasAttribute(hTask, TDL_TASKHIGHESTPRIORITY))
		return GetTaskUChar(hTask, TDL_TASKHIGHESTPRIORITY);

	return GetTaskUChar(hTask, TDL_TASKPRIORITY);
}

unsigned char CTaskFile::GetTaskPercentDone(HTASKITEM hTask, BOOL bCalc) const
{
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCCOMPLETION))
		return GetTaskUChar(hTask, TDL_TASKCALCCOMPLETION);

	return GetTaskUChar(hTask, TDL_TASKPERCENTDONE);
}

double CTaskFile::GetTaskCost(HTASKITEM hTask, BOOL bCalc) const
{
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCCOST))
		return GetTaskDouble(hTask, TDL_TASKCALCCOST);

	// else
	return GetTaskDouble(hTask, TDL_TASKCOST);
}

BOOL CTaskFile::GetTaskRecurrence(HTASKITEM hTask, TDIRECURRENCE& tr) const
{
	return GetTaskRecurrence(hTask, (int&)tr.nRegularity, tr.dwSpecific1, 
							tr.dwSpecific2, tr.bRecalcFromDue);
}

bool CTaskFile::GetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1, 
									DWORD& dwSpecific2, BOOL& bRecalcFromDue) const
{
	CXmlItem* pXI = TaskFromHandle(hTask);

	if (pXI)
	{
		CXmlItem* pXIRecur = pXI->GetItem(TDL_TASKRECURRENCE);

		if (pXIRecur)
		{
			nRegularity = pXIRecur->GetItemValueI(TDL_TASKRECURRENCEREG);
			dwSpecific1 = pXIRecur->GetItemValueI(TDL_TASKRECURRENCESPEC1);
			dwSpecific2 = pXIRecur->GetItemValueI(TDL_TASKRECURRENCESPEC2);
			bRecalcFromDue = pXIRecur->GetItemValueI(TDL_TASKRECURRENCEFROMDUE);

			return true;
		}
	}

	return false;
}

const char* CTaskFile::GetTaskVersion(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKVERSION);
}

double CTaskFile::GetTaskTimeEstimate(HTASKITEM hTask, char& cUnits, BOOL bCalc) const
{
	cUnits = 0;
	
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCTIMEESTIMATE))
	{
		cUnits = 'H';
		return GetTaskDouble(hTask, TDL_TASKCALCTIMEESTIMATE);
	}
	else if (TaskHasAttribute(hTask, TDL_TASKTIMEESTUNITS))
	{
		const char* szUnits = GetTaskCChar(hTask, TDL_TASKTIMEESTUNITS);
		
		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (char)atoi(szUnits);
			
			if (!cUnits)
				cUnits = szUnits[0];
		}
	}
	
	if (!cUnits)
		cUnits = 'H';
	
	return GetTaskDouble(hTask, TDL_TASKTIMEESTIMATE);
}

double CTaskFile::GetTaskTimeSpent(HTASKITEM hTask, char& cUnits, BOOL bCalc) const
{
	cUnits = 0;
	
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCTIMESPENT))
	{
		cUnits = 'H';
		return GetTaskDouble(hTask, TDL_TASKCALCTIMESPENT);
	}
	else if (TaskHasAttribute(hTask, TDL_TASKTIMESPENTUNITS))
	{
		const char* szUnits = GetTaskCChar(hTask, TDL_TASKTIMESPENTUNITS);
		
		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (char)atoi(szUnits);
			
			if (!cUnits)
				cUnits = szUnits[0];
		}
	}
	// backwards compatibility
	else if (TaskHasAttribute(hTask, TDL_TASKTIMESPENTUNITS_OLD))
	{
		const char* szUnits = GetTaskCChar(hTask, TDL_TASKTIMESPENTUNITS_OLD);
		
		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (char)atoi(szUnits);
			
			if (!cUnits)
				cUnits = szUnits[0];
		}
	}
	
	if (!cUnits)
		cUnits = 'H';

	if (TaskHasAttribute(hTask, TDL_TASKTIMESPENT))
		return GetTaskDouble(hTask, TDL_TASKTIMESPENT);

	// else for backwards compatibility
		return GetTaskDouble(hTask, TDL_TASKTIMESPENT_OLD);
}

time_t CTaskFile::GetTaskLastModified(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKLASTMOD, TRUE);
}

time_t CTaskFile::GetTaskCreationDate(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKCREATIONDATE, FALSE);
}

time_t CTaskFile::GetTaskDoneDate(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKDONEDATE, FALSE);
}

time_t CTaskFile::GetTaskDueDate(HTASKITEM hTask) const
{
	return GetTaskDueDate(hTask, FALSE);
}

time_t CTaskFile::GetTaskDueDate(HTASKITEM hTask, BOOL bEarliest) const
{
	if (bEarliest && TaskHasAttribute(hTask, TDL_TASKEARLIESTDUEDATE))
		return GetTaskDate(hTask, TDL_TASKEARLIESTDUEDATE, FALSE);

	return GetTaskDate(hTask, TDL_TASKDUEDATE, FALSE);
}

const char* CTaskFile::GetTaskDueDateString(HTASKITEM hTask, BOOL bEarliest) const
{
	if (bEarliest && TaskHasAttribute(hTask, TDL_TASKEARLIESTDUEDATESTRING))
		return GetTaskCChar(hTask, TDL_TASKEARLIESTDUEDATESTRING);

	return GetTaskCChar(hTask, TDL_TASKDUEDATESTRING);
}

time_t CTaskFile::GetTaskStartDate(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKSTARTDATE, FALSE);
}

COleDateTime CTaskFile::GetTaskLastModifiedOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKLASTMOD, TRUE);
}

COleDateTime CTaskFile::GetTaskDoneDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKDONEDATE, FALSE);
}

COleDateTime CTaskFile::GetTaskDueDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKDUEDATE, FALSE);
}

COleDateTime CTaskFile::GetTaskStartDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKSTARTDATE, FALSE);
}

COleDateTime CTaskFile::GetTaskCreationDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKCREATIONDATE, FALSE);
}

const char* CTaskFile::GetTaskDoneDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKDONEDATESTRING);
}

const char* CTaskFile::GetTaskCreationDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCREATIONDATESTRING);
}

const char* CTaskFile::GetTaskDueDateString(HTASKITEM hTask) const
{
	return GetTaskDueDateString(hTask, FALSE);
}

const char* CTaskFile::GetTaskStartDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKSTARTDATESTRING);
}

unsigned long CTaskFile::GetTaskPosition(HTASKITEM hTask) const
{
	return GetTaskULong(hTask, TDL_TASKPOS);
}

bool CTaskFile::IsTaskDone(HTASKITEM hTask) const
{
	return (GetTaskDoneDateOle(hTask).m_dt > 0.0 ||
			GetTaskPercentDone(hTask, FALSE) == 100);
}

bool CTaskFile::IsTaskDue(HTASKITEM hTask) const
{
	double dDue = GetTaskDueDateOle(hTask);

	if (dDue > 0.0)
	{
		double dToday = floor(COleDateTime::GetCurrentTime());
		return dDue <= dToday;
	}

	return false;
}

bool CTaskFile::TaskHasAttribute(HTASKITEM hTask, const char* szAttrib) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);
	
	CXmlItem* pXIAttrib = pXITask->GetItem(szAttrib);
	
	if (!pXIAttrib || !pXIAttrib->GetValueLen()) // not found
	{
		// some fallbacks
		if (lstrcmp(szAttrib, TDL_TASKCOLOR) == 0)
			return TaskHasAttribute(hTask, TDL_TASKTEXTCOLOR);
		
		else if (lstrcmp(szAttrib, TDL_TASKWEBCOLOR) == 0)
			return TaskHasAttribute(hTask, TDL_TASKTEXTWEBCOLOR);

		// else
		return FALSE;
	}
	
	// finally check for hidden attribute
	return (pXIAttrib->GetItemValueI("HIDE") == 0);
}

const char* CTaskFile::GetTaskAttribute(HTASKITEM hTask, const char* szAttrib) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, "");

	return (pXITask->GetItemValue(szAttrib));
}

HTASKITEM CTaskFile::GetTaskParent(HTASKITEM hTask) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return (HTASKITEM)pXITask->GetParent();
}

unsigned long CTaskFile::GetTaskTextColor(HTASKITEM hTask) const
{
	if (TaskHasAttribute(hTask, TDL_TASKTEXTCOLOR))
		return GetTaskULong(hTask, TDL_TASKTEXTCOLOR);

	else if (TaskHasAttribute(hTask, TDL_TASKCOLOR))
		return GetTaskULong(hTask, TDL_TASKCOLOR);

	// else
	return 0;
}

unsigned char CTaskFile::GetTaskRisk(HTASKITEM hTask, BOOL bHighest) const
{
	if (bHighest && TaskHasAttribute(hTask, TDL_TASKHIGHESTRISK))
		return GetTaskUChar(hTask, TDL_TASKHIGHESTRISK);

	return GetTaskUChar(hTask, TDL_TASKRISK);
}

const char* CTaskFile::GetTaskExternalID(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKEXTERNALID);
}

///////////////////////////////////////////////

bool CTaskFile::SetTaskTitle(HTASKITEM hTask, const char* szTitle)
{
	return SetTaskCChar(hTask, TDL_TASKTITLE, szTitle);
}

bool CTaskFile::SetTaskComments(HTASKITEM hTask, const char* szComments)
{
	// if the first two chars are '\r\n' then we need to translate them 
	// because otherwise MSXML will remove them
	


	CString sComments(szComments);
//	sComments.Replace("\r\n", "&#13;&#10;");

	return SetTaskCChar(hTask, TDL_TASKCOMMENTS, sComments);
}

bool CTaskFile::SetTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo)
{
	return SetTaskCChar(hTask, TDL_TASKALLOCTO, szAllocTo);
}

bool CTaskFile::SetTaskDependency(HTASKITEM hTask, const char* szDepends)
{
	return SetTaskCChar(hTask, TDL_TASKDEPENDENCY, szDepends);
}

bool CTaskFile::SetTaskAllocatedBy(HTASKITEM hTask, const char* szAllocBy)
{
	return SetTaskCChar(hTask, TDL_TASKALLOCBY, szAllocBy);
}

bool CTaskFile::SetTaskCategory(HTASKITEM hTask, const char* szCategory)
{
	return SetTaskCChar(hTask, TDL_TASKCATEGORY, szCategory);
}

bool CTaskFile::SetTaskStatus(HTASKITEM hTask, const char* szStatus)
{
	return SetTaskCChar(hTask, TDL_TASKSTATUS, szStatus);
}

bool CTaskFile::SetTaskFlag(HTASKITEM hTask, bool bFlag)
{
	return SetTaskUChar(hTask, TDL_TASKFLAG, (unsigned char)(bFlag ? 1 : 0));
}

bool CTaskFile::SetTaskFileReferencePath(HTASKITEM hTask, const char* szFileRefpath)
{
	return SetTaskCChar(hTask, TDL_TASKFILEREFPATH, szFileRefpath);
}

bool CTaskFile::SetTaskCreatedBy(HTASKITEM hTask, const char* szCreatedBy)
{
	return SetTaskCChar(hTask, TDL_TASKCREATEDBY, szCreatedBy);
}

BOOL CTaskFile::SetTaskID(HTASKITEM hTask, unsigned long nID, BOOL bVisible)
{
	if (SetTaskULong(hTask, TDL_TASKID, nID))
	{
		// update m_dwNextUniqueID
		m_dwNextUniqueID = max(m_dwNextUniqueID, nID + 1);

		HideAttribute(hTask, TDL_TASKID, !bVisible);

		return true;
	}
	
	return false;
}

BOOL CTaskFile::HideAttribute(HTASKITEM hTask, const char* szAttrib, BOOL bHide)
      {
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	CXmlItem* pXIAttrib = pXITask->GetItem(szAttrib);

	if (pXIAttrib)
	{
		CXmlItem* pXIHide = pXIAttrib->GetItem("HIDE");

		// if not visible add 'HIDE' attribute
		if (bHide)
		{
			if (pXIHide)
				pXIHide->SetValue(1);
			else
				pXIAttrib->AddItem("HIDE", 1);
		}
		else if (pXIHide)
			pXIAttrib->DeleteItem(pXIHide);
		
		return TRUE;
	}
	
	return FALSE;
}

bool CTaskFile::SetTaskColor(HTASKITEM hTask, unsigned long nColor)
{
	return SetTaskULong(hTask, TDL_TASKCOLOR, nColor) &&
			SetTaskCChar(hTask, TDL_TASKWEBCOLOR, GetWebColor((COLORREF)nColor));
}

bool CTaskFile::SetTaskPriority(HTASKITEM hTask, unsigned char nPriority)
{
	return SetTaskUChar(hTask, TDL_TASKPRIORITY, (unsigned char)max(0, min(10, nPriority)));
}

bool CTaskFile::SetTaskPercentDone(HTASKITEM hTask, unsigned char nPercent)
{
	return SetTaskUChar(hTask, TDL_TASKPERCENTDONE, nPercent);
}

bool CTaskFile::SetTaskCost(HTASKITEM hTask, double dCost)
{
	return SetTaskDouble(hTask, TDL_TASKCOST, dCost);
}

bool CTaskFile::SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, char cUnits)
{
	return SetTaskTime(hTask, TDL_TASKTIMEESTIMATE, dTimeEst,
						TDL_TASKTIMEESTUNITS, cUnits);
}

bool CTaskFile::SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, char cUnits)
{
	return SetTaskTime(hTask, TDL_TASKTIMESPENT, dTimeSpent,
						TDL_TASKTIMESPENTUNITS, cUnits);
}

bool CTaskFile::SetTaskLastModified(HTASKITEM hTask, time_t tLastMod)
{
	return SetTaskDate(hTask, TDL_TASKLASTMOD, tLastMod, TRUE);
}

bool CTaskFile::SetTaskDoneDate(HTASKITEM hTask, time_t tDoneDate)
{
	return SetTaskDate(hTask, TDL_TASKDONEDATE, tDoneDate, FALSE);
}

bool CTaskFile::SetTaskDueDate(HTASKITEM hTask, time_t tDueDate)
{
	return SetTaskDate(hTask, TDL_TASKDUEDATE, tDueDate, FALSE);
}

bool CTaskFile::SetTaskStartDate(HTASKITEM hTask, time_t tStartDate)
{
	return SetTaskDate(hTask, TDL_TASKSTARTDATE, tStartDate, FALSE);
}

bool CTaskFile::SetTaskCreationDate(HTASKITEM hTask, time_t tCreationDate)
{
	return SetTaskDate(hTask, TDL_TASKCREATIONDATE, tCreationDate, FALSE);
}

BOOL CTaskFile::SetTaskLastModified(HTASKITEM hTask, const COleDateTime& tLastMod)
{
	return SetTaskDate(hTask, TDL_TASKLASTMOD, tLastMod, TRUE);
}

BOOL CTaskFile::SetTaskLastModified(HTASKITEM hTask, const COleDateTime& tLastMod, BOOL bISO)
{
	if (SetTaskLastModified(hTask, tLastMod))
	{
		CString sLastMod;
		sLastMod.Format("%s (%s)", CDateHelper::FormatDate(tLastMod, bISO), tLastMod.Format(VAR_TIMEVALUEONLY));
		
		SetTaskCChar(hTask, TDL_TASKLASTMODSTRING, sLastMod);
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskCreationDate(HTASKITEM hTask, const COleDateTime& date, BOOL bISO)
{
	if (SetTaskDate(hTask, TDL_TASKCREATIONDATE, date, FALSE))
	{
		SetTaskCChar(hTask, TDL_TASKCREATIONDATESTRING, CDateHelper::FormatDate(date, bISO));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskDoneDate(HTASKITEM hTask, const COleDateTime& date, BOOL bISO)
{
	if (SetTaskDate(hTask, TDL_TASKDONEDATE, date, FALSE))
	{
		SetTaskCChar(hTask, TDL_TASKDONEDATESTRING, CDateHelper::FormatDate(date, bISO));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskDueDate(HTASKITEM hTask, const COleDateTime& date, BOOL bISO)
{
	if (SetTaskDate(hTask, TDL_TASKDUEDATE, date, FALSE))
	{
		SetTaskCChar(hTask, TDL_TASKDUEDATESTRING, CDateHelper::FormatDate(date, bISO));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskStartDate(HTASKITEM hTask, const COleDateTime& date, BOOL bISO)
{
	if (SetTaskDate(hTask, TDL_TASKSTARTDATE, date, FALSE))
	{
		SetTaskCChar(hTask, TDL_TASKSTARTDATESTRING, CDateHelper::FormatDate(date, bISO));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskRecurrence(HTASKITEM hTask, const TDIRECURRENCE& tr, LPCTSTR szRegularity)
{
	if (SetTaskRecurrence(hTask, (int)tr.nRegularity, tr.dwSpecific1, tr.dwSpecific2, tr.bRecalcFromDue))
	{
		SetTaskCChar(hTask, TDL_TASKRECURRENCE, szRegularity); // set human readable
		return true;
	}

	return false;
}

bool CTaskFile::SetTaskRecurrence(HTASKITEM hTask, int nRegularity, DWORD dwSpecific1, 
									DWORD dwSpecific2, BOOL bRecalcFromDue)
{
	CXmlItem* pXI = TaskFromHandle(hTask);

	if (pXI)
	{
		CXmlItem* pXIRecur = pXI->GetItem(TDL_TASKRECURRENCE);

		if (!pXIRecur)
			pXIRecur = pXI->AddItem(TDL_TASKRECURRENCE);
		
		pXIRecur->SetItemValue(TDL_TASKRECURRENCEREG, nRegularity);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCESPEC1, (int)dwSpecific1);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCESPEC2, (int)dwSpecific2);

		if (bRecalcFromDue)
			pXIRecur->SetItemValue(TDL_TASKRECURRENCEFROMDUE, TRUE);
		else
			pXIRecur->DeleteItem(TDL_TASKRECURRENCEFROMDUE);

		return true;
	}

	return false;
}

bool CTaskFile::SetTaskVersion(HTASKITEM hTask, const char* szVersion)
{
	return SetTaskCChar(hTask, TDL_TASKVERSION, szVersion);
}

BOOL CTaskFile::SetTaskTextColor(HTASKITEM hTask, COLORREF color)
{
	return SetTaskULong(hTask, TDL_TASKTEXTCOLOR, color) &&
			SetTaskCChar(hTask, TDL_TASKTEXTWEBCOLOR, GetWebColor(color));
}

BOOL CTaskFile::SetTaskPriorityColor(HTASKITEM hTask, COLORREF color)
{
	return SetTaskULong(hTask, TDL_TASKPRIORITYCOLOR, color) &&
			SetTaskCChar(hTask, TDL_TASKPRIORITYWEBCOLOR, GetWebColor(color));
}

BOOL CTaskFile::SetTaskCalcTimeEstimate(HTASKITEM hTask, double dTime)
{
	return SetTaskDouble(hTask, TDL_TASKCALCTIMEESTIMATE, dTime);
}

BOOL CTaskFile::SetTaskCalcTimeSpent(HTASKITEM hTask, double dTime)
{
	return SetTaskDouble(hTask, TDL_TASKCALCTIMESPENT, dTime);
}

BOOL CTaskFile::SetTaskEarliestDueDate(HTASKITEM hTask, const COleDateTime& date)
{
	return SetTaskDate(hTask, TDL_TASKEARLIESTDUEDATE, date, FALSE);
}

BOOL CTaskFile::SetTaskCalcCompletion(HTASKITEM hTask, int nPercent)
{
	return SetTaskUChar(hTask, TDL_TASKCALCCOMPLETION, (unsigned char)nPercent);
}

BOOL CTaskFile::SetTaskHighestPriority(HTASKITEM hTask, int nPriority)
{
	return SetTaskUChar(hTask, TDL_TASKHIGHESTPRIORITY, (unsigned char)max(0, min(10, nPriority)));
}

BOOL CTaskFile::SetTaskHighestRisk(HTASKITEM hTask, int nRisk)
{
	return SetTaskUChar(hTask, TDL_TASKHIGHESTRISK, (unsigned char)max(0, min(10, nRisk)));
}

BOOL CTaskFile::SetTaskCalcCost(HTASKITEM hTask, double dCost)
{
	return SetTaskDouble(hTask, TDL_TASKCALCCOST, dCost);
}

bool CTaskFile::SetTaskPosition(HTASKITEM hTask, unsigned long nPos)
{
	return SetTaskULong(hTask, TDL_TASKPOS, nPos);
}

bool CTaskFile::SetTaskRisk(HTASKITEM hTask, unsigned char nRisk)
{
	return SetTaskUChar(hTask, TDL_TASKRISK, (unsigned char)max(0, min(10, nRisk)));
}

bool CTaskFile::SetTaskExternalID(HTASKITEM hTask, const char* szID)
{
	return SetTaskCChar(hTask, TDL_TASKEXTERNALID, szID);
}

////////////////////////////////////////////////
// utility functions

time_t CTaskFile::GetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const
{
	COleDateTime date = GetTaskDateOle(hTask, szDateItem, bIncTime);

	if (date.m_dt)
	{
		tm time = { date.GetSecond(), 
					date.GetMinute(),
					date.GetHour(), 
					date.GetDay(),
					date.GetMonth() - 1, 
					date.GetYear() - 1900, 
					date.GetDayOfWeek() - 1, 
					date.GetDayOfYear(), 
					-1};

		time_t timet = mktime(&time);

		return (timet == -1) ? 0 : timet;
	}

	return 0;
}

COleDateTime CTaskFile::GetTaskDateOle(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const
{
	double dDate = GetTaskDouble(hTask, szDateItem);

	if (!bIncTime)
		dDate = floor(dDate);

	return dDate;
}

unsigned char CTaskFile::GetTaskUChar(HTASKITEM hTask, LPCTSTR szUCharItem) const
{
	unsigned long lVal = GetTaskULong(hTask, szUCharItem);

	return (unsigned char)min(max(0, lVal), 255);
}

unsigned long CTaskFile::GetTaskULong(HTASKITEM hTask, LPCTSTR szULongItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, 0);

	return (unsigned long)pXITask->GetItemValueI(szULongItem);
}

const char* CTaskFile::GetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return pXITask->GetItemValue(szCCharItem);
}

double CTaskFile::GetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, 0.0);

	return pXITask->GetItemValueF(szDoubleItem);
}

double CTaskFile::GetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem) const
{
	return GetTaskDouble(hTask, szTimeItem);
}

char CTaskFile::GetTaskTimeUnits(HTASKITEM hTask, LPCTSTR szUnitsItem) const
{
	return (char)GetTaskUChar(hTask, szUnitsItem);
}

////////////////////////////////////////////////////////////////////

bool CTaskFile::SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, const COleDateTime& tVal, 
							BOOL bIncTime)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

	double dDate = bIncTime ? tVal.m_dt : floor(tVal.m_dt);

   return (pXITask->SetItemValue(szDateItem, dDate) != NULL);
}

bool CTaskFile::SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, time_t tVal, BOOL bIncTime)
{
	tm* pTime = localtime(&tVal);

	if (!pTime)
		return false;

	// else
	COleDateTime date(pTime->tm_year + 1900,
					  pTime->tm_mon,
					  pTime->tm_mday,
					  pTime->tm_hour,
					  pTime->tm_min,
					  pTime->tm_sec);
	
	return SetTaskDate(hTask, szDateItem, date, bIncTime);
}

bool CTaskFile::SetTaskUChar(HTASKITEM hTask, LPCTSTR szUCharItem, unsigned char cVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szUCharItem, (int)cVal) != NULL);
}

bool CTaskFile::SetTaskULong(HTASKITEM hTask, LPCTSTR szULongItem, unsigned long lVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szULongItem, (int)lVal) != NULL);
}

bool CTaskFile::SetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem, const char* szVal, BOOL bCData)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szCCharItem, szVal, bCData) != NULL);
}

bool CTaskFile::SetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem, double dVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szDoubleItem, dVal) != NULL);
}

bool CTaskFile::SetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem, double dTime,
							LPCTSTR szUnitsItem, char cUnits)
{
  char* szUnits;

	switch (cUnits)
	{
	case 'i': 
	case 'I': szUnits = "I"; // minutes
    break;

	case 'h': 
	case 'H': szUnits = "H";
    break;

	case 'd': 
	case 'D': szUnits = "D";
    break;

	case 'w':
	case 'W': szUnits = "W";
    break;

	case 'm': 
	case 'M': szUnits = "M";
    break;

	case 'y': 
	case 'Y': szUnits = "Y";
    break;

	default: // all else
		return false;
	}

	return SetTaskDouble(hTask, szTimeItem, dTime) &&
		  	SetTaskCChar(hTask, szUnitsItem, szUnits);
}

CString CTaskFile::GetWebColor(COLORREF color)
{
	unsigned char cRed = GetRValue(color);
	unsigned char cGreen = GetGValue(color);
	unsigned char cBlue = GetBValue(color);
	
	static CString sColor;
	sColor.Format("#%02X%02X%02X", cRed, cGreen, cBlue);
	
	return sColor;
}

