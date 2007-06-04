// ToDoCtrlData.cpp: implementation of the CToDoCtrlData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToDoCtrlData.h"

#include "..\shared\xmlfile.h"
#include "..\shared\timehelper.h"
#include "..\shared\datehelper.h"
#include "..\shared\misc.h"
#include "..\shared\enstring.h"

#include <float.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

IMPLEMENT_FIXED_ALLOC(TODOITEM, 1024);

TODOITEM::TODOITEM(LPCTSTR szTitle, LPCTSTR szComments) :
	sTitle(szTitle), 
	sComments(szComments),
	color(0), 
	nPriority(5),
	nRisk(0),
	nPercentDone(0),
	dTimeEstimate(0),
	dTimeSpent(0),
	nTimeEstUnits(TDITU_HOURS),
	nTimeSpentUnits(TDITU_HOURS),
	dCost(0),
	bFlagged(FALSE),
	dateCreated(COleDateTime::GetCurrentTime())
{ 
	SetModified();
	ResetCalcs();
}

TODOITEM::TODOITEM() :
	color(0), 
	nPriority(5),
	nRisk(0),
	nPercentDone(0),
	dTimeEstimate(0),
	dTimeSpent(0),
	nTimeEstUnits(TDITU_HOURS),
	nTimeSpentUnits(TDITU_HOURS),
	dCost(0),
	bFlagged(FALSE),
	dateCreated(COleDateTime::GetCurrentTime())
{ 
	SetModified();
	ResetCalcs();
}

TODOITEM::TODOITEM(const TODOITEM& tdi) :
	sTitle(tdi.sTitle),
	sComments(tdi.sComments),
	color(tdi.color), 
	sFileRefPath(tdi.sFileRefPath),
	sAllocTo(tdi.sAllocTo),
	sAllocBy(tdi.sAllocBy),
	sStatus(tdi.sStatus),
	nPriority(tdi.nPriority),
	nPercentDone(tdi.nPercentDone),
	dTimeEstimate(tdi.dTimeEstimate),
	dTimeSpent(tdi.dTimeSpent),
	nTimeEstUnits(tdi.nTimeEstUnits),
	nTimeSpentUnits(tdi.nTimeSpentUnits),
	dCost(tdi.dCost),
	dateStart(tdi.dateStart),
	dateDue(tdi.dateDue),
	dateDone(tdi.dateDone),
	dateCreated(tdi.dateCreated),
	bFlagged(tdi.bFlagged),
	sCreatedBy(tdi.sCreatedBy),
	nRisk(tdi.nRisk),
	sExternalID(tdi.sExternalID),
	trRecurrence(tdi.trRecurrence)
{ 
    if (dateCreated.m_dt == 0.0)
		dateCreated = COleDateTime::GetCurrentTime();

	aCategories.Copy(tdi.aCategories);

	SetModified();
	ResetCalcs();
}

BOOL TODOITEM::HasCreation() const 
{ 
	return (dateCreated.m_dt > 0) ? TRUE : FALSE; 
}

BOOL TODOITEM::HasLastMod() const 
{ 
	return (tLastMod.m_dt > 0) ? TRUE : FALSE; 
}

BOOL TODOITEM::HasStart() const 
{ 
	return (dateStart.m_dt > 0) ? TRUE : FALSE; 
}

BOOL TODOITEM::HasDue() const 
{ 
	return (dateDue.m_dt > 0) ? TRUE : FALSE; 
}

BOOL TODOITEM::IsDone() const 
{ 
	return (dateDone.m_dt > 0) ? TRUE : FALSE; 
}

void TODOITEM::ClearStart() 
{ 
	dateStart.m_dt = 0; 
}

void TODOITEM::ClearDue() 
{ 
	dateDue.m_dt = 0; 
}

void TODOITEM::ClearDone() 
{ 
	dateDone.m_dt = 0; 
}

BOOL TODOITEM::IsDue() const
{ 
	return IsDue(COleDateTime::GetCurrentTime());
}

BOOL TODOITEM::IsDue(const COleDateTime& dateDueBy) const
{ 
	if (IsDone() || !HasDue())
		return FALSE;
	
	return (floor(dateDue.m_dt) <= floor(dateDueBy.m_dt)); 
}

void TODOITEM::SetModified() 
{ 
	tLastMod = COleDateTime::GetCurrentTime(); 
}

void TODOITEM::ResetCalcs() 
{
	nCalcPriority = nCalcPercent = nCalcRisk = -1;
	dCalcTimeEstimate = dCalcTimeSpent = dCalcCost = -1;
	dateEarliestDue.m_dt = -1;
	bGoodAsDone = bDue = -1;
    nSubtasksCount = nSubtasksDone = -1;
}

CString TODOITEM::GetFirstCategory() const
{
	return aCategories.GetSize() ? aCategories[0] : "";
}

BOOL TODOITEM::GetNextOccurence(COleDateTime& dtNext) const
{
	if (trRecurrence.bRecalcFromDue)
		return trRecurrence.GetNextOccurence(dateDue, dtNext);

	// else
//	return trRecurrence.GetNextOccurence(dateDone, dtNext);
	return trRecurrence.GetNextOccurence(COleDateTime::GetCurrentTime(), dtNext);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CToDoCtrlData::CToDoCtrlData(CTreeCtrl& tree, const CWordArray& aStyles) : 
m_tree(tree), m_aStyles(aStyles)
{
	
}

CToDoCtrlData::~CToDoCtrlData()
{
	DeleteAllTasks();
}

TODOITEM* CToDoCtrlData::NewTask(const TODOITEM* pTDIRef)
{
	if (pTDIRef)
		return new TODOITEM(*pTDIRef);
	else
		return new TODOITEM;
}

TODOITEM* CToDoCtrlData::GetTask(DWORD dwID) const
{
	TODOITEM* pTDI;
	
	return (dwID && m_mapTDItems.Lookup(dwID, pTDI)) ? pTDI : NULL;
}

TODOITEM* CToDoCtrlData::GetTask(HTREEITEM hti) const
{
	return GetTask(GetTaskID(hti));
}

void CToDoCtrlData::AddTask(DWORD dwID, TODOITEM* pTDI) 
{ 
	if (dwID && pTDI)
	{
		// must delete duplicates else we'll get a memory leak
		TODOITEM* pExist = GetTask(dwID);
		
		if (pExist)
		{
			m_mapTDItems.RemoveKey(dwID);
			delete pExist;
		}
	}
	
	m_mapTDItems.SetAt(dwID, pTDI); 
}

void CToDoCtrlData::DeleteTask(DWORD dwID)
{
	ASSERT (dwID && GetTask(dwID));
	
	delete GetTask(dwID);
	m_mapTDItems.RemoveKey(dwID);
}

void CToDoCtrlData::DeleteAllTasks(BOOL bIncTree)
{
	POSITION pos = m_mapTDItems.GetStartPosition();
	TODOITEM* pTDI;
	DWORD dwID;
	
	while (pos)
	{
		m_mapTDItems.GetNextAssoc(pos, dwID, pTDI);
		delete pTDI;
	}
	
	m_mapTDItems.RemoveAll();
	
	if (bIncTree && m_tree.GetSafeHwnd()) 
	{
		m_tree.SelectItem(NULL);
		m_tree.DeleteAllItems(); 
	}
}

int CToDoCtrlData::FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const
{
	if (!GetTaskCount())
		return 0;
	
	HTREEITEM hti = m_tree.GetChildItem(NULL);
	
	while (hti)
	{
		FindTasks(hti, params, aResults);
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}
	
	// else
	return aResults.GetSize();
}

int CToDoCtrlData::FindTasks(HTREEITEM hti, const SEARCHPARAMS& params, CResultArray& aResults) const
{
	SEARCHRESULT result;
	
	if (TaskMatches(hti, params, result))
		aResults.Add(result);
	
	// also check children and their children recursively
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	
	while (htiChild)
	{
		FindTasks(htiChild, params, aResults);
		
		// next
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return aResults.GetSize();
}

DWORD CToDoCtrlData::FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	if (!GetTaskCount())
		return 0;
	
	HTREEITEM hti = m_tree.GetChildItem(NULL);
	DWORD dwTaskID = 0;
	
	while (hti && !dwTaskID)
	{
		dwTaskID = FindFirstTask(hti, params, result);
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}
	
	// else
	return dwTaskID;
}

DWORD CToDoCtrlData::FindFirstTask(HTREEITEM hti, const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	if (TaskMatches(hti, params, result))
		return result.dwID;
	
	// also check children and their children recursively
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	DWORD dwTaskID = 0;
	
	while (htiChild && !dwTaskID)
	{
		dwTaskID = FindFirstTask(htiChild, params, result);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return dwTaskID;
}

BOOL CToDoCtrlData::TaskMatches(DWORD dwID, const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	return TaskMatches(FindItem(dwID, NULL), params, result);
}

BOOL CToDoCtrlData::TaskMatches(HTREEITEM hti, const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	BOOL bMatch = FALSE;
	DWORD dwID = m_tree.GetItemData(hti);
	
	BOOL bIncDone = (params.dwFlags & FIND_INCLUDEDONE);
	BOOL bIsDone = IsTaskDone(hti, TDCCHECKALL);

	if (params.nFindWhat == FIND_DONEDATE)
	{
		if (pTDI->IsDone())
			bMatch = TaskMatches(pTDI->dateDone, params, result);
	}
	else if (bIncDone || !bIsDone)
	{
		switch (params.nFindWhat)
		{
		case FIND_TITLECOMMENTS:
			bMatch = (TaskMatches(pTDI->sTitle, params, result) || TaskMatches(pTDI->sComments, params, result));
			break;
			
		case FIND_ALLOCTO:
			bMatch = TaskMatches(pTDI->sAllocTo, params, result);
			break;
			
		case FIND_ALLOCBY:
			bMatch = TaskMatches(pTDI->sAllocBy, params, result);
			break;
			
		case FIND_CREATEDBY:
			bMatch = TaskMatches(pTDI->sCreatedBy, params, result);
			break;
			
		case FIND_STATUS:
			bMatch = TaskMatches(pTDI->sStatus, params, result);
			break;
			
		case FIND_CATEGORY:
			bMatch = TaskMatches(pTDI->aCategories, params, result);
			break;
			
		case FIND_EXTERNALID:
			bMatch = TaskMatches(pTDI->sExternalID, params, result);
			break;
			
		case FIND_CREATIONDATE:
			if (pTDI->HasCreation())
				bMatch = TaskMatches(pTDI->dateCreated, params, result);
			break;
			
		case FIND_STARTDATE:
			if (pTDI->HasStart())
				bMatch = TaskMatches(pTDI->dateStart, params, result);
			break;
			
		case FIND_DUEDATE:
			if (pTDI->HasDue())
				bMatch = TaskMatches(pTDI->dateDue, params, result);
			break;
			
		case FIND_LASTMOD:
			if (pTDI->HasLastMod())
				bMatch = TaskMatches(pTDI->tLastMod, params, result);
			break;
			
		case FIND_PRIORITY:
			bMatch = TaskMatches(pTDI->nPriority, params, result);
			break;
			
		case FIND_RISK:
			bMatch = TaskMatches(pTDI->nRisk, params, result);
			break;
			
		case FIND_TASKID:
			bMatch = TaskMatches((int)dwID, params, result);
			break;
			
		case FIND_PERCENTDONE:
			bMatch = TaskMatches(pTDI->nPercentDone, params, result);
			break;
			
		case FIND_TIMEEST:
			bMatch = TaskMatches(pTDI->dTimeEstimate, params, result);
			break;
			
		case FIND_TIMESPENT:
			bMatch = TaskMatches(pTDI->dTimeSpent, params, result);
			break;
			
		case FIND_COST:
			bMatch = TaskMatches(pTDI->dCost, params, result);
			break;
			
		case FIND_FLAG:
			bMatch = pTDI->bFlagged;
			break;
			
		case FIND_VERSION:
			bMatch = TaskMatches(pTDI->sVersion, params, result);
			break;
		}
	}
	
	if (bMatch)
	{
		result.bDone = bIsDone;
		result.dwID = dwID;
		result.hti = hti;
	}
	
	return bMatch;
}

BOOL CToDoCtrlData::TaskMatches(const COleDateTime& date, const SEARCHPARAMS& params, SEARCHRESULT& result)
{
	if (date >= params.dateFrom && date <= params.dateTo)
	{
		result.dateMatch = date;
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CToDoCtrlData::TaskMatches(const CString& sText, const SEARCHPARAMS& params, SEARCHRESULT& result)
{
	CStringArray aWords;
	
	if (!ParseSearchString(params.sText, aWords))
		return FALSE;
	
	BOOL bMatchCase = (params.dwFlags & FIND_MATCHCASE);
	BOOL bMatchWholeWord = (params.dwFlags & FIND_MATCHWHOLEWORD);
	
	// cycle all the words
	for (int nWord = 0; nWord < aWords.GetSize(); nWord++)
	{
		CString sWord = aWords.GetAt(nWord);
		
		if (FindWord(sWord, sText, bMatchCase, bMatchWholeWord))
		{
			result.sMatch = sText;
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CToDoCtrlData::TaskMatches(const CStringArray& aCats, const SEARCHPARAMS& params, SEARCHRESULT& result)
{
	BOOL bMatch = FALSE;

	if (params.dwFlags & FIND_MATCHALLCATS)
		bMatch = Misc::ArraysMatch(aCats, params.aCategories);
	else
	{
		if (aCats.GetSize())
			bMatch = Misc::MatchAny(aCats, params.aCategories);
		else
			// special case: task has no category and param.aCategories
			// contains an empty item
			bMatch = (Misc::Find(params.aCategories, "") != -1);
	}

	if (bMatch)
		result.sMatch = Misc::FormatArray(aCats);

	return bMatch;
}

BOOL CToDoCtrlData::TaskMatches(const double& dValue, const SEARCHPARAMS& params, SEARCHRESULT& result)
{
	if (dValue >= params.dFrom && dValue <= params.dTo)
	{
		result.dMatch = dValue;
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CToDoCtrlData::TaskMatches(int nValue, const SEARCHPARAMS& params, SEARCHRESULT& result)
{
	if (nValue >= params.nFrom && nValue <= params.nTo)
	{
		result.nMatch = nValue;
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CToDoCtrlData::FindWord(LPCTSTR szWord, LPCTSTR szText, BOOL bMatchCase, BOOL bMatchWholeWord)
{
	CString sWord(szWord), sText(szText);
	
	if (sWord.GetLength() > sText.GetLength())
		return FALSE;
	
	sWord.TrimLeft();
	sWord.TrimRight();
	
	if (!bMatchCase)
	{
		sWord.MakeUpper();
		sText.MakeUpper();
	}
	
	int nFind = sText.Find(sWord);
	
	if (nFind == -1)
		return FALSE;
	
	else if (bMatchWholeWord) // test whole word
	{
		const CString DELIMS("()-\\/{}[]:;,. ?\"'");
		
		// prior and next chars must be delimeters
		char cPrevChar = 0, cNextChar = 0;
		
		// prev
		if (nFind == 0) // word starts at start
			cPrevChar = ' '; // known delim
		else
			cPrevChar = sText[nFind - 1];
		
		// next
		if ((nFind + sWord.GetLength()) < sText.GetLength())
			cNextChar = sText[nFind + sWord.GetLength()];
		else
			cNextChar = ' '; // known delim
		
		if (DELIMS.Find(cPrevChar) == -1 || DELIMS.Find(cNextChar) == -1)
			return FALSE;
	}
	
	return TRUE;
}

int CToDoCtrlData::ParseSearchString(LPCTSTR szLookFor, CStringArray& aWords)
{
	aWords.RemoveAll();
	
	// parse on spaces unless enclosed in double-quotes
	int nLen = lstrlen(szLookFor);
	BOOL bInQuotes = FALSE, bAddWord = FALSE;
	CString sWord;
	
	for (int nPos = 0; nPos < nLen; nPos++)
	{
		switch (szLookFor[nPos])
		{
		case ' ': // word break
			if (bInQuotes)
				sWord += szLookFor[nPos];
			else
				bAddWord = TRUE;
			break;
			
		case '\"':
			// whether its the start or end we add the current word
			// and flip bInQuotes
			bInQuotes = !bInQuotes;
			bAddWord = TRUE;
			break;
			
		default: // everything else
			sWord += szLookFor[nPos];
			
			// also if its the last char then add it
			bAddWord = (nPos == nLen - 1);
			break;
		}
		
		if (bAddWord)
		{
			sWord.TrimLeft();
			sWord.TrimRight();
			
			if (!sWord.IsEmpty())
				aWords.Add(sWord);
			
			sWord.Empty(); // next word
		}
	}
	
	return aWords.GetSize();
}

CString CToDoCtrlData::GetTaskTitle(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sTitle;
	
	return "";
}

CString CToDoCtrlData::GetTaskComments(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sComments;
	
	return "";
}

CString CToDoCtrlData::GetTaskCustomComments(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sCustomComments;
	
	return "";
}

double CToDoCtrlData::GetTaskCost(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->dCost;
	
	return 0;
}

double CToDoCtrlData::GetTaskTimeEstimate(DWORD dwID, int& nUnits) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
	{
		nUnits = pTDI->nTimeEstUnits;
		return pTDI->dTimeEstimate;
	}
	
	return 0;
}

double CToDoCtrlData::GetTaskTimeSpent(DWORD dwID, int& nUnits) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
	{
		nUnits = pTDI->nTimeSpentUnits;
		return pTDI->dTimeSpent;
	}
	
	return 0;
}

CString CToDoCtrlData::GetTaskAllocTo(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sAllocTo;
	
	return "";
}

CString CToDoCtrlData::GetTaskAllocBy(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sAllocBy;
	
	return "";
}

CString CToDoCtrlData::GetTaskVersion(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sVersion;
	
	return "";
}

CString CToDoCtrlData::GetTaskStatus(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sStatus;
	
	return "";
}

int CToDoCtrlData::GetTaskCategories(DWORD dwID, CStringArray& aCategories) const
{
	TODOITEM* pTDI = GetTask(dwID);
	aCategories.RemoveAll();

	if (pTDI)
		aCategories.Copy(pTDI->aCategories);

	return aCategories.GetSize();
}

CString CToDoCtrlData::GetTaskDependency(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sDependency;
	
	return "";
}

int CToDoCtrlData::GetTaskDependents(DWORD dwTaskID, CDWordArray& aDependents) const
{
	if (!dwTaskID)
		return 0;

	POSITION pos = m_mapTDItems.GetStartPosition();
	CEnString sTaskID("%d", dwTaskID);
	
	CDWordArray aTemp;
	
	while (pos)
	{
		TODOITEM* pTDI;
		DWORD dwID;
		
		m_mapTDItems.GetNextAssoc(pos, dwID, pTDI);
		
		if (dwID != dwTaskID && pTDI->sDependency == sTaskID)
			aDependents.Add(dwID);
	}
	
	return aDependents.GetSize();
}

int CToDoCtrlData::GetTaskDependencies(DWORD dwTaskID, CDWordArray& aDepends) const
{
	static int nDepth = 0;

	// note: only clear aDepends if first call
	if (nDepth == 0)
		aDepends.RemoveAll();

	nDepth++;

	TODOITEM* pTDI = GetTask(dwTaskID);
	
	if (pTDI)
	{
		// we only handle 'same file' links
		DWORD dwIDLink = atoi(pTDI->sDependency);
		
		if (dwIDLink && dwIDLink != dwTaskID)
		{
			// stop if we already have this ID because it means there's a 
			// circular dependency 
			int nLink = aDepends.GetSize();
			
			while (nLink--)
			{
				if (aDepends[nLink] == dwIDLink)
					return aDepends.GetSize();
			}
			
			// insert dependency at head of list
			aDepends.InsertAt(0, dwIDLink);
			
			// add this links dependencies too
			GetTaskDependencies(dwIDLink, aDepends);
		}
	}

	nDepth--;
	
	return aDepends.GetSize();
}

BOOL CToDoCtrlData::HasCircularDependencies(DWORD dwTaskID) const
{
	// a task has circular dependencies if it's dependencies end up back at itself
	TODOITEM* pTDI = GetTask(dwTaskID);
	
	while (pTDI)
	{
		DWORD dwIDLink = atoi(pTDI->sDependency);

		if (dwIDLink == dwTaskID)
			return TRUE;

		else if (dwTaskID == 0) // no dependency or file dependency
			return FALSE;

		// keep going
		pTDI = GetTask(dwIDLink);
	}

	return FALSE;
}

CString CToDoCtrlData::GetTaskExtID(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sExternalID;
	
	return "";
}

CString CToDoCtrlData::GetTaskCreatedBy(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sCreatedBy;
	
	return "";
}

COLORREF CToDoCtrlData::GetTaskColor(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->color;
	
	return 0;
}

int CToDoCtrlData::GetTaskPriority(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->nPriority;
	
	return 0;
}

int CToDoCtrlData::GetTaskRisk(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->nRisk;
	
	return 0;
}

BOOL CToDoCtrlData::IsTaskFlagged(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->bFlagged;
	
	return FALSE;
}

BOOL CToDoCtrlData::GetTaskRecurrence(DWORD dwID, TDIRECURRENCE& tr) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
	{
		tr = pTDI->trRecurrence;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CToDoCtrlData::GetTaskNextOccurrence(DWORD dwID, COleDateTime& dtNext) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->GetNextOccurence(dtNext);

	// else
	return FALSE;
}

COleDateTime CToDoCtrlData::GetTaskDate(DWORD dwID, TDC_DATE nDate) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
	{
		switch (nDate)
		{
		case TDCD_CREATE:	return pTDI->dateCreated;
		case TDCD_START:	return pTDI->dateStart;
		case TDCD_DUE:		return pTDI->dateDue;
		case TDCD_DONE:		return pTDI->dateDone;
		}
	}
	
	ASSERT(0);
	return COleDateTime();
}

BOOL CToDoCtrlData::IsTaskDone(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->IsDone();
	
	return FALSE;
}

int CToDoCtrlData::GetTaskPercent(DWORD dwID, BOOL bCheckIfDone) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
	{
		if (bCheckIfDone)
			return pTDI->IsDone() ? 100 : pTDI->nPercentDone;
		else
			return pTDI->nPercentDone;
	}
	
	return 0;
}

int CToDoCtrlData::GetTaskPercent(HTREEITEM hti) const
{
   return GetTaskPercent(GetTaskID(hti), FALSE);
}

CString CToDoCtrlData::GetTaskFileRef(DWORD dwID) const
{
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
		return pTDI->sFileRefPath;
	
	return "";
}

double CToDoCtrlData::GetTaskTimeEstimate(HTREEITEM hti, int& nUnits) const
{
	return GetTaskTimeEstimate(GetTaskID(hti), nUnits);
}

double CToDoCtrlData::GetTaskTimeSpent(HTREEITEM hti, int& nUnits) const
{
	return GetTaskTimeSpent(GetTaskID(hti), nUnits);
}

CString CToDoCtrlData::GetTaskAllocTo(HTREEITEM hti) const
{
	return GetTaskAllocTo(GetTaskID(hti));
}

CString CToDoCtrlData::GetTaskAllocBy(HTREEITEM hti) const
{
	return GetTaskAllocBy(GetTaskID(hti));
}

CString CToDoCtrlData::GetTaskVersion(HTREEITEM hti) const
{
	return GetTaskVersion(GetTaskID(hti));
}

CString CToDoCtrlData::GetTaskStatus(HTREEITEM hti) const
{
	return GetTaskStatus(GetTaskID(hti));
}

int CToDoCtrlData::GetTaskCategories(HTREEITEM hti, CStringArray& aCategories) const
{
	return GetTaskCategories(GetTaskID(hti), aCategories);
}

CString CToDoCtrlData::GetTaskExtID(HTREEITEM hti) const
{
	return GetTaskExtID(GetTaskID(hti));
}

COLORREF CToDoCtrlData::GetTaskColor(HTREEITEM hti) const
{
	return GetTaskColor(GetTaskID(hti));
}

BOOL CToDoCtrlData::IsTaskFlagged(HTREEITEM hti) const
{
	return IsTaskFlagged(GetTaskID(hti));
}

BOOL CToDoCtrlData::GetTaskRecurrence(HTREEITEM hti, TDIRECURRENCE& tr) const
{
	return GetTaskRecurrence(GetTaskID(hti), tr);
}

int CToDoCtrlData::GetTaskPriority(HTREEITEM hti) const
{
	return GetTaskPriority(GetTaskID(hti));
}

double CToDoCtrlData::GetTaskCost(HTREEITEM hti) const
{
	return GetTaskCost(GetTaskID(hti));
}

COleDateTime CToDoCtrlData::GetTaskDate(HTREEITEM hti, TDC_DATE nDate) const
{
	return GetTaskDate(GetTaskID(hti), nDate);
}

int CToDoCtrlData::GetTaskRisk(HTREEITEM hti) const
{
	return GetTaskRisk(GetTaskID(hti));
}

BOOL CToDoCtrlData::IsParentTaskDone(HTREEITEM hti) const
{
	if (!hti)
		return FALSE;
	
	HTREEITEM htiParent = m_tree.GetParentItem(hti);
	
	if (!htiParent)
		return FALSE;
	
	TODOITEM* tdiParent = GetTask(GetTaskID(htiParent));
	
	if (!tdiParent)
		return FALSE;
	
	if (tdiParent->IsDone())
		return TRUE;
	
	// else check parent's parent
	return IsParentTaskDone(htiParent);
}


int CToDoCtrlData::AreChildTasksDone(HTREEITEM hti) const
{
	if (!m_tree.ItemHasChildren(hti))
		return -1;
	
	// else check children and their children recursively
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	
	while (htiChild)
	{
		int bDone = IsTaskDone(htiChild, TDCCHECKCHILDREN);
		
		if (!bDone)
			return FALSE;
		
		// next
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return TRUE;
}
/*
BOOL CToDoCtrlData::IsTaskDue(HTREEITEM hti) const
{
return IsTaskDue(GetTaskID(hti));
}
*/
BOOL CToDoCtrlData::DeleteTask(HTREEITEM hti)
{
	// do children first to ensure entire branch is deleted
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	
	while (htiChild)
	{
		DeleteTask(htiChild);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	// then this item
	DWORD dwID = GetTaskID(hti);
	
	if (m_tree.DeleteItem(hti))
	{
		// delete mapping
		DeleteTask(dwID);
		return TRUE;
	}
	
	return FALSE;
}

void CToDoCtrlData::Sort(TDC_SORTBY nBy, BOOL bAscending, HTREEITEM htiRoot)
{
	CHTIMap mapHTI;
	BuildHTIMap(mapHTI, htiRoot);
	
	TDSORTSTRUCT ss = { this, &mapHTI, nBy, bAscending };
	
	Sort(htiRoot, ss);
}

void CToDoCtrlData::Sort(HTREEITEM hti, const TDSORTSTRUCT& ss)
{
	// don't sort if not expanded and not the root item (hti == NULL)
	if (hti && hti != TVI_ROOT && ss.pData->HasStyle(TDCS_SORTVISIBLETASKSONLY))
	{
		if (!(m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED))
			return;
	}
	
	TVSORTCB tvs = { hti, CompareFunc, (LPARAM)&ss };
	
	// sort this items children first
	m_tree.SortChildrenCB(&tvs);
	
	// then its childrens children
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	
	while (htiChild)
	{
		Sort(htiChild, ss);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

int CALLBACK CToDoCtrlData::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nCompare = 0;
	
	TDSORTSTRUCT* pSS = (TDSORTSTRUCT*)lParamSort;
	
	// sort by id can be optimized since the IDs are the lParams
	if (pSS->nSortBy == TDC_SORTBYID)
		return (pSS->bAscending ? (lParam1 - lParam2) : (lParam2 - lParam1));
	
	// else all the rest require the task lookup
	TODOITEM* tdi1 = pSS->pData->GetTask(lParam1);
	TODOITEM* tdi2 = pSS->pData->GetTask(lParam2);
	
	HTREEITEM hti1 = NULL, hti2 = NULL;
	VERIFY(pSS->pMapHTItems->Lookup(lParam1, hti1) && hti1);
	VERIFY(pSS->pMapHTItems->Lookup(lParam2, hti2) && hti2);
	
	// figure out if either or both tasks are completed
	// but only if the user has specified to sort these differently
	BOOL bHideDone = pSS->pData->HasStyle(TDCS_HIDESTARTDUEFORDONETASKS);
	BOOL bSortDoneBelow = pSS->pData->HasStyle(TDCS_SORTDONETASKSATBOTTOM);
	BOOL bDone1 = FALSE, bDone2 = FALSE;
	
	if (bSortDoneBelow || 
		pSS->nSortBy == TDC_SORTBYDONE || pSS->nSortBy == TDC_SORTBYDONEDATE ||
		(bHideDone && (pSS->nSortBy == TDC_SORTBYSTARTDATE || pSS->nSortBy == TDC_SORTBYDUEDATE)))
	{
		bDone1 = pSS->pData->IsTaskDone(hti1, TDCCHECKALL);
		bDone2 = pSS->pData->IsTaskDone(hti2, TDCCHECKALL);
		
		// can also do a partial optimization
		if (bSortDoneBelow && (pSS->nSortBy != TDC_SORTBYDONE && pSS->nSortBy != TDC_SORTBYDONEDATE))
		{
			if (bDone1 != bDone2)
				return bDone1 ? 1 : -1;
		}
	}
	
	switch (pSS->nSortBy)
	{
	case TDC_SORTBYNAME:
		nCompare = Compare(tdi1->sTitle, tdi2->sTitle);
		break;
		
	case TDC_SORTBYDONE:
		nCompare = Compare(bDone1, bDone2);
		break;
		
	case TDC_SORTBYFLAG:
		nCompare = Compare(tdi1->bFlagged, tdi2->bFlagged);
		break;
		
	case TDC_SORTBYRECURRENCE:
		nCompare = Compare(tdi1->trRecurrence.nRegularity, tdi2->trRecurrence.nRegularity);
		break;
		
	case TDC_SORTBYVERSION:
		nCompare = Misc::CompareVersions(tdi1->sVersion, tdi2->sVersion);
		break;
		
	case TDC_SORTBYCREATIONDATE:
		nCompare = Compare(tdi1->dateCreated, tdi2->dateCreated);
		break;
		
	case TDC_SORTBYLASTMOD:
		{
			BOOL bHasModify1 = tdi1->HasLastMod() && !(bHideDone && bDone1);
			BOOL bHasModify2 = tdi2->HasLastMod() && !(bHideDone && bDone2);
			
			if (bHasModify1 != bHasModify2)
				return bHasModify1 ? -1 : 1;
			
			else if (!bHasModify1) //  and !bHasStart2
				return 0;
			
			nCompare = Compare(tdi1->tLastMod, tdi2->tLastMod);
		}
		break;
		
		
	case TDC_SORTBYDONEDATE:
		{
			COleDateTime date1 = tdi1->dateDone; // default
			COleDateTime date2 = tdi2->dateDone; // default
			
			// sort tasks 'good as done' between done and not-done
			if (date1 <= 0 && bDone1)
				date1 = 0.1;
			
			if (date2 <= 0 && bDone2)
				date2 = 0.1;
			
			nCompare = Compare(date1, date2);
		}
		break;
		
	case TDC_SORTBYDUEDATE:
		{
			COleDateTime date1, date2;
			
			BOOL bUseEarliestDueDate = pSS->pData->HasStyle(TDCS_USEEARLIESTDUEDATE);
			
			if (!bHideDone || !bDone1)
			{
				if (bUseEarliestDueDate)
					date1 = pSS->pData->GetEarliestDueDate(hti1, tdi1);
				else
					date1 = tdi1->dateDue;
			}
			
			if (!bHideDone || !bDone2)
			{
				if (bUseEarliestDueDate)
					date2 = pSS->pData->GetEarliestDueDate(hti2, tdi2);
				else
					date2 = tdi2->dateDue;
			}
			
			// Sort undated options below others
			BOOL bHasDue1 = (date1.m_dt > 0) ? 1 : 0;
			BOOL bHasDue2 = (date2.m_dt > 0) ? 1 : 0;
			
			if (bHasDue1 != bHasDue2)
				return bHasDue1 ? -1 : 1;
			
			else if (!bHasDue1) //  and !bHasStart2
				return 0;
			
			// compare
			nCompare = Compare(date1, date2);
		}
		break;
		
	case TDC_SORTBYSTARTDATE:
		{
			BOOL bHasStart1 = tdi1->HasStart() && !(bHideDone && bDone1);
			BOOL bHasStart2 = tdi2->HasStart() && !(bHideDone && bDone2);
			
			if (bHasStart1 != bHasStart2)
				return bHasStart1 ? -1 : 1;
			
			else if (!bHasStart1) //  and !bHasStart2
				return 0;
			
			nCompare = Compare(tdi1->dateStart, tdi2->dateStart);
		}
		break;
		
	case TDC_SORTBYPRIORITY:
		{
			// done items have even less than zero priority!
			// and due items have greater than the highest priority
			int nPriority1 = tdi1->nPriority; // default
			int nPriority2 = tdi2->nPriority; // default
			
			BOOL bUseHighestPriority = pSS->pData->HasStyle(TDCS_USEHIGHESTPRIORITY);
			
			// item1
			if (bDone1)
				nPriority1 = -1;
			
			else if (pSS->pData->IsTaskDue(hti1, tdi1))
				nPriority1 = tdi1->nPriority + 11;
			
			else if (bUseHighestPriority)
				nPriority1 = pSS->pData->GetHighestPriority(hti1, tdi1);
			
			// item2
			if (bDone2)
				nPriority2 = -1;
			
			else if (pSS->pData->IsTaskDue(hti2, tdi2))
				nPriority2 = tdi2->nPriority + 11;
			
			else if (bUseHighestPriority)
				nPriority2 = pSS->pData->GetHighestPriority(hti2, tdi2);
			
			nCompare = Compare(nPriority1, nPriority2);
		}
		break;
		
	case TDC_SORTBYRISK:
		{
			// done items have even less than zero priority!
			// and due items have greater than the highest priority
			int nRisk1 = tdi1->nRisk; // default
			int nRisk2 = tdi2->nRisk; // default
			
			BOOL bUseHighestRisk = pSS->pData->HasStyle(TDCS_USEHIGHESTRISK);
			
			// item1
			if (bDone1)
				nRisk1 = -1;
			
//			else if (pSS->pData->IsTaskDue(hti1, tdi1))
//				nRisk1 = tdi1->nRisk + 11;
			
			else if (bUseHighestRisk)
				nRisk1 = pSS->pData->GetHighestRisk(hti1, tdi1);
			
			// item2
			if (bDone2)
				nRisk2 = -1;
			
//			else if (pSS->pData->IsTaskDue(hti2, tdi2))
//				nRisk2 = tdi2->nRisk + 11;
			
			else if (bUseHighestRisk)
				nRisk2 = pSS->pData->GetHighestRisk(hti2, tdi2);
			
			nCompare = Compare(nRisk1, nRisk2);
		}
		break;
		
	case TDC_SORTBYCOLOR:
		nCompare = Compare((int)tdi1->color, (int)tdi2->color);
		break;
		
	case TDC_SORTBYALLOCTO:
		nCompare = Compare(tdi1->sAllocTo, tdi2->sAllocTo, TRUE);
		break;
		
	case TDC_SORTBYALLOCBY:
		nCompare = Compare(tdi1->sAllocBy, tdi2->sAllocBy, TRUE);
		break;
		
	case TDC_SORTBYCREATEDBY:
		nCompare = Compare(tdi1->sCreatedBy, tdi2->sCreatedBy, TRUE);
		break;
		
	case TDC_SORTBYSTATUS:
		nCompare = Compare(tdi1->sStatus, tdi2->sStatus, TRUE);
		break;
		
	case TDC_SORTBYEXTERNALID:
		nCompare = Compare(tdi1->sExternalID, tdi2->sExternalID, TRUE);
		break;
		
	case TDC_SORTBYCATEGORY:
		nCompare = Compare(tdi1->GetFirstCategory(), tdi2->GetFirstCategory(), TRUE);
		break;
		
	case TDC_SORTBYPERCENT:
		{
			int nPercent1 = pSS->pData->CalcPercentDone(hti1, tdi1);
			int nPercent2 = pSS->pData->CalcPercentDone(hti2, tdi2);
			
			nCompare = Compare(nPercent1, nPercent2);
		}
		break;
		
	case TDC_SORTBYCOST:
		{
			double dCost1 = pSS->pData->CalcCost(hti1, tdi1);
			double dCost2 = pSS->pData->CalcCost(hti2, tdi2);
			
			nCompare = Compare(dCost1, dCost2);
		}
		break;
		
	case TDC_SORTBYTIMEEST:
		{
			double dTime1 = pSS->pData->CalcTimeEstimate(hti1, tdi1, TDITU_HOURS);
			double dTime2 = pSS->pData->CalcTimeEstimate(hti2, tdi2, TDITU_HOURS);
			
			nCompare = Compare(dTime1, dTime2);
		}
		break;
		
	case TDC_SORTBYTIMESPENT:
		{
			double dTime1 = pSS->pData->CalcTimeSpent(hti1, tdi1, TDITU_HOURS);
			double dTime2 = pSS->pData->CalcTimeSpent(hti2, tdi2, TDITU_HOURS);
			
			nCompare = Compare(dTime1, dTime2);
		}
		break;
		
	default:
		ASSERT(0);
		break;
	}
	
	if (!pSS->bAscending)
		nCompare = -nCompare;
	
	return nCompare;
}

int CToDoCtrlData::Compare(const COleDateTime& date1, const COleDateTime& date2)
{
	return (date1 < date2) ? -1 : (date1 > date2) ? 1 : 0;
}

int CToDoCtrlData::Compare(const CString& sText1, const CString& sText2, BOOL bCheckEmpty)
{
	if (bCheckEmpty)
	{
		BOOL bEmpty1 = sText1.IsEmpty();
		BOOL bEmpty2 = sText2.IsEmpty();
		
		if (bEmpty1 != bEmpty2)
			return bEmpty1 ? 1 : -1;
	}
	
	return sText1.CompareNoCase(sText2);
}

int CToDoCtrlData::Compare(int nNum1, int nNum2)
{
    return (nNum1 - nNum2);
}

int CToDoCtrlData::Compare(const double& dNum1, const double& dNum2)
{
    return (dNum1 < dNum2) ? -1 : (dNum1 > dNum2) ? 1 : 0;
}

int CToDoCtrlData::GetItemPos(HTREEITEM hti, HTREEITEM htiSearch) const
{
	// traverse the entire tree util we find the item
	if (!htiSearch)
		htiSearch = TVI_ROOT;
	
	int nPos = 1; // always 1-based
	HTREEITEM htiChild = m_tree.GetChildItem(htiSearch);
	
	while (htiChild)
	{
		if (htiChild == hti)
			return nPos;
		
		// try items children
		int nChildPos = GetItemPos(hti, htiChild);
		
		if (nChildPos)
			return nChildPos;
		
		nPos++;
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return 0; // not found
}

void CToDoCtrlData::ResetCachedCalculations()
{
	// sets the bNeedRecalc flag on all items
	POSITION pos = m_mapTDItems.GetStartPosition();
	TODOITEM* pTDI;
	DWORD dwID;
	
	while (pos)
	{
		m_mapTDItems.GetNextAssoc(pos, dwID, pTDI);
		
		pTDI->ResetCalcs();
		m_mapTDItems[dwID] = pTDI;
	}
}

BOOL CToDoCtrlData::GetSubtaskTotals(HTREEITEM hti, const TODOITEM* pTDI, 
                              int& nSubtasksCount, int& nSubtasksDone) const
{
	if (!m_tree.ItemHasChildren(hti))
		return FALSE;
	
	// simple optimization
	// try pre-calculated value first
	if (pTDI->nSubtasksCount > 0)
	{
		nSubtasksCount = pTDI->nSubtasksCount;
		nSubtasksDone = pTDI->nSubtasksDone;
      return TRUE;
	}
	
	nSubtasksDone = nSubtasksCount = 0;
	
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	
	while (htiChild)
	{
		nSubtasksCount++;
		
		if (IsTaskDone(htiChild, TDCCHECKCHILDREN))
			nSubtasksDone++;
		
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	pTDI->nSubtasksDone = nSubtasksDone;
	pTDI->nSubtasksCount = nSubtasksCount;
	
	return TRUE;
}

int CToDoCtrlData::CalcPercentDone(HTREEITEM hti, const TODOITEM* pTDI) const
{
	// simple optimization
	// try pre-calculated value first
	if (pTDI->nCalcPercent >= 0)
		return pTDI->nCalcPercent;
	
	int nPercent = 0;
	
	if (!HasStyle(TDCS_AVERAGEPERCENTSUBCOMPLETION))
	{
		if (pTDI->IsDone())
			nPercent = 100;
		
		else if(HasStyle(TDCS_AUTOCALCPERCENTDONE))
			nPercent = CalcPercentFromTime(hti, pTDI);
		
		else
			nPercent = pTDI->nPercentDone;
	}
	else
	{
		// else
		double dTotalPercent = 0, dTotalWeighting = 0, dPercent = 0;
		
		SumPercentDone(hti, pTDI, dTotalPercent, dTotalWeighting);
		
		if (dTotalWeighting > 0)
			dPercent = dTotalPercent / dTotalWeighting;
		else
			ASSERT (dTotalPercent == 0); // sanity check
		
		nPercent = (int)dPercent;
	}
	
	// else update calc'ed value
	pTDI->nCalcPercent = nPercent;
	
	return pTDI->nCalcPercent;
}

int CToDoCtrlData::CalcPercentFromTime(HTREEITEM hti, const TODOITEM* pTDI) const
{
	ASSERT (HasStyle(TDCS_AUTOCALCPERCENTDONE)); // sanity check
	
	double dSpent = CalcTimeSpent(hti, pTDI, THU_HOURS);
	double dEstimate = CalcTimeEstimate(hti, pTDI, THU_HOURS);
	
	if (dSpent > 0 && dEstimate > 0)
		return min(100, (int)(100 * dSpent / dEstimate));
	else
		return 0;
}

void CToDoCtrlData::SumPercentDone(const HTREEITEM hti, const TODOITEM* pTDI,
								   double& dTotalPercent, double& dTotalWeighting) const
{
	ASSERT (HasStyle(TDCS_AVERAGEPERCENTSUBCOMPLETION)); // sanity check

	if (!m_tree.ItemHasChildren(hti))
	{
		double dWeighting = 1; // default
		double dPercent = 0;

		// base percent
		if (pTDI->IsDone())
			dPercent = 100;

		else if(HasStyle(TDCS_AUTOCALCPERCENTDONE))
			dPercent = CalcPercentFromTime(hti, pTDI);

		else
			dPercent = pTDI->nPercentDone;

		// weighting
		if (HasStyle(TDCS_WEIGHTPERCENTCALCBYTIMEEST))
			dWeighting *= CTimeHelper::GetTime(pTDI->dTimeEstimate, pTDI->nTimeEstUnits, THU_HOURS);

		if (HasStyle(TDCS_WEIGHTPERCENTCALCBYPRIORITY))
			dWeighting *= pTDI->nPriority;
		
		dPercent *= dWeighting;

		dTotalWeighting += dWeighting;
		dTotalPercent += dPercent;
	}
	else // aggregate child percentages
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		double dChildWeighting = 0;
		double dChildPercent = 0;

		while (htiChild)
		{
			TODOITEM* tdiChild = GetTask(htiChild);
			ASSERT(tdiChild);

			if (!tdiChild->IsDone() || HasStyle(TDCS_INCLUDEDONEINAVERAGECALC))
				SumPercentDone(htiChild, tdiChild, dChildPercent, dChildWeighting);

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}

		if (HasStyle(TDCS_WEIGHTPERCENTCALCBYNUMSUB))
		{
			dTotalPercent += dChildPercent;
			dTotalWeighting += dChildWeighting;
		}
		else
		{
			dTotalPercent += (dChildPercent / dChildWeighting);
			dTotalWeighting += 1;
		}
	}
}

double CToDoCtrlData::CalcCost(HTREEITEM hti, const TODOITEM* pTDI) const
{
	// update calc'ed value in hours
	if (pTDI->dCalcCost < 0)
	{	
		double dCost = 0;
		
		if (!m_tree.ItemHasChildren(hti))
			dCost = pTDI->dCost;
		else // children
		{
			HTREEITEM htiChild = m_tree.GetChildItem(hti);
			
			while (htiChild)
			{
				dCost += CalcCost(htiChild);
				
				htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
			}
		}
		
		// update calc'ed value
		pTDI->dCalcCost = dCost;
	}
	
	return pTDI->dCalcCost;
}

double CToDoCtrlData::CalcTimeEstimate(HTREEITEM hti, const TODOITEM* pTDI, int nUnits) const
{
	// update calc'ed value in hours
	if (pTDI->dCalcTimeEstimate < 0)
	{	
		double dTime = 0;
		BOOL bIsParent = m_tree.ItemHasChildren(hti);
		
		// task's own time
		if (!bIsParent || HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
		{
			dTime = CTimeHelper::GetTime(pTDI->dTimeEstimate, pTDI->nTimeEstUnits, THU_HOURS);
			
			if (HasStyle(TDCS_USEPERCENTDONEINTIMEEST))
			{
				if (pTDI->IsDone())
					dTime = 0;
				else
					dTime *= ((100 - pTDI->nPercentDone) / 100.0); // estimating time left
			}
		}

		if (bIsParent) // children's time
		{
			HTREEITEM htiChild = m_tree.GetChildItem(hti);
			
			while (htiChild)
			{
				dTime += CalcTimeEstimate(htiChild, THU_HOURS);
				
				htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
			}
		}
		
		// update calc'ed value
		pTDI->dCalcTimeEstimate = dTime;
	}
	
	// its in hours (always) so we need to convert it to nUnits
	return CTimeHelper::GetTime(pTDI->dCalcTimeEstimate, THU_HOURS, nUnits);
}

double CToDoCtrlData::CalcTimeSpent(HTREEITEM hti, const TODOITEM* pTDI, int nUnits) const
{
	// update calc'ed value in hours
	if (pTDI->dCalcTimeSpent < 0)
	{
		double dTime = 0;
		BOOL bIsParent = m_tree.ItemHasChildren(hti);
		
		// task's own time
		if (!bIsParent || HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
		{
			dTime = CTimeHelper::GetTime(pTDI->dTimeSpent, pTDI->nTimeSpentUnits, THU_HOURS);
		}

		if (bIsParent) // children's time
		{
			HTREEITEM htiChild = m_tree.GetChildItem(hti);
			
			while (htiChild)
			{
				dTime += CalcTimeSpent(htiChild, THU_HOURS);
				
				htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
			}
		}
		
		// update calc'ed value
		pTDI->dCalcTimeSpent = dTime;
	}
	
	// convert it back from hours to nUnits
	return CTimeHelper::GetTime(pTDI->dCalcTimeSpent, THU_HOURS, nUnits);
}

void CToDoCtrlData::BuildHTIMap(CHTIMap& mapHTI, HTREEITEM htiRoot) const
{
	mapHTI.RemoveAll();
	
	// traverse toplevel items
	HTREEITEM hti = m_tree.GetChildItem(htiRoot);
	
	while (hti)
	{
		UpdateHTIMapEntry(mapHTI, hti);
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}
}

void CToDoCtrlData::UpdateHTIMapEntry(CHTIMap& mapHTI, HTREEITEM hti) const
{
	// update our own mapping
	mapHTI[GetTaskID(hti)] = hti;
	
	// then our children
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	
	while (htiChild)
	{
		UpdateHTIMapEntry(mapHTI, htiChild);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

BOOL CToDoCtrlData::IsTaskDue(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return FALSE;
	
	return IsTaskDue(hti, pTDI);
}

double CToDoCtrlData::GetEarliestDueDate(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return GetEarliestDueDate(hti, pTDI);
}

int CToDoCtrlData::GetHighestPriority(HTREEITEM hti, BOOL bIncludeDue) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return MIN_TDPRIORITY;
	
	return GetHighestPriority(hti, pTDI, bIncludeDue);
}

int CToDoCtrlData::GetHighestRisk(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return GetHighestRisk(hti, pTDI);
}

double CToDoCtrlData::CalcCost(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return CalcCost(hti, pTDI);
}

int CToDoCtrlData::CalcPercentDone(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return CalcPercentDone(hti, pTDI);
}

double CToDoCtrlData::CalcTimeEstimate(HTREEITEM hti, int nUnits) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return CalcTimeEstimate(hti, pTDI, nUnits);
}

double CToDoCtrlData::CalcTimeSpent(HTREEITEM hti, int nUnits) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return CalcTimeSpent(hti, pTDI, nUnits);
}

BOOL CToDoCtrlData::GetSubtaskTotals(HTREEITEM hti, int& nSubtasksTotal, int& nSubtasksDone) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return 0;
	
	return GetSubtaskTotals(hti, pTDI, nSubtasksTotal, nSubtasksDone);
}

CString CToDoCtrlData::GetLongestExternalID(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return "";
	
	return GetLongestExternalID(hti, pTDI);
}

CString CToDoCtrlData::GetLongestCategory(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return "";
	
	return GetLongestCategory(hti, pTDI);
}

CString CToDoCtrlData::GetLongestExternalID(HTREEITEM hti, const TODOITEM* pTDI) const
{
	CString sLongest = pTDI->sExternalID;

	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestExternalID(htiChild);

			if (sChildLongest.GetLength() > sLongest.GetLength())
				sLongest = sChildLongest;

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

CString CToDoCtrlData::GetLongestCategory(HTREEITEM hti, const TODOITEM* pTDI) const
{
	CString sLongest = Misc::FormatArray(pTDI->aCategories);

	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestCategory(htiChild);

			if (sChildLongest.GetLength() > sLongest.GetLength())
				sLongest = sChildLongest;

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

BOOL CToDoCtrlData::IsTaskFullyDone(HTREEITEM hti, const TODOITEM* pTDI, BOOL bCheckSiblings) const
{
	if (bCheckSiblings)
	{
		// note: its not intuitive to apply the sibling rule to top level tasks
		// so we only apply it to tasks whose parent is not the root
		HTREEITEM htiParent = m_tree.GetParentItem(hti);
		
		if (htiParent && htiParent != TVI_ROOT)
		{
			HTREEITEM htiSibling = m_tree.GetChildItem(htiParent);
			
			while (htiSibling)
			{
				if (htiSibling != hti) // else we would recurse
				{
					TODOITEM* tdiSibling = GetTask(htiSibling);
					
					if (tdiSibling)
					{
						// exit on first failure
						if (!IsTaskFullyDone(htiSibling, tdiSibling, FALSE)) // FALSE: we're doing the siblings
							return FALSE;
					}
				}
				
				htiSibling = m_tree.GetNextItem(htiSibling, TVGN_NEXT);
			}
		}
	}
	
	// check children
	BOOL bTreatSubCompleteDone = HasStyle(TDCS_TREATSUBCOMPLETEDASDONE);
	
	if (bTreatSubCompleteDone && m_tree.ItemHasChildren(hti))
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);
		
		while (htiChild)
		{
			TODOITEM* tdiChild = GetTask(htiChild);
			
			if (tdiChild)
			{
				// exit on first failure
				if (!IsTaskFullyDone(htiChild, tdiChild, FALSE)) // FALSE: we're doing the siblings
					return FALSE;
			}
			
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
	else 
		return pTDI->IsDone(); // no children => relies on our state only
	
	// if we got here there were no sibling or child failures
	return TRUE;
}

BOOL CToDoCtrlData::IsTaskDue(HTREEITEM hti, const TODOITEM* pTDI) const
{
	// some optimizations
	// check chached value
	if (pTDI->bDue >= 0)
		return pTDI->bDue;

	double dDue = GetEarliestDueDate(hti,  pTDI);
	BOOL bDue = (dDue > 0 && dDue < COleDateTime::GetCurrentTime());

	// update cached value
	pTDI->bDue = bDue;
	
	return pTDI->bDue;
}

double CToDoCtrlData::GetEarliestDueDate(HTREEITEM hti, const TODOITEM* pTDI) const
{
	ASSERT (hti);
	ASSERT (pTDI);
	
	// some optimizations
	if (pTDI->dateEarliestDue.m_dt >= 0)
		return pTDI->dateEarliestDue;
	
	double dEarliest = pTDI->dateDue;
	
	if (IsTaskDone(hti, TDCCHECKCHILDREN))
		dEarliest = 0;
	
	else if (HasStyle(TDCS_USEEARLIESTDUEDATE) && m_tree.ItemHasChildren(hti))
	{
		// check children
		dEarliest = pTDI->HasDue() ? pTDI->dateDue : DBL_MAX;
		
		HTREEITEM htiChild = m_tree.GetChildItem(hti);
		
		while (htiChild)
		{
			double dChildDue = GetEarliestDueDate(htiChild);
			
			if (dChildDue > 0 && dChildDue < dEarliest)
				dEarliest = dChildDue;
			
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
	
	// update calc'ed value
	pTDI->dateEarliestDue = (dEarliest == DBL_MAX) ? 0 : dEarliest;
	
	return pTDI->dateEarliestDue;
}

int CToDoCtrlData::GetHighestPriority(HTREEITEM hti, const TODOITEM* pTDI, BOOL bIncludeDue) const
{
	ASSERT (hti);
	ASSERT(pTDI);
	
	// some optimizations
	// try pre-calculated value first
	if (!bIncludeDue && pTDI->nCalcPriority >= 0)
		return pTDI->nCalcPriority;
	
	int nHighest = pTDI->nPriority;
	
	if (nHighest < MAX_TDPRIORITY)
	{
		if (pTDI->IsDone())
			nHighest = MIN_TDPRIORITY;

		else if (bIncludeDue && IsTaskDue(hti, pTDI))
			nHighest = MAX_TDPRIORITY;
		
		else if (HasStyle(TDCS_USEHIGHESTPRIORITY) && m_tree.ItemHasChildren(hti))
		{
			// check children
			HTREEITEM htiChild = m_tree.GetChildItem(hti);
			
			while (htiChild)
			{
				if (HasStyle(TDCS_INCLUDEDONEINPRIORITYCALC) || !IsTaskDone(htiChild, TDCCHECKALL))
				{
					int nChildHighest = GetHighestPriority(htiChild, bIncludeDue);
					
					// optimization
					if (nChildHighest == MAX_TDPRIORITY)
						return MAX_TDPRIORITY;
					else 
						nHighest = max(nChildHighest, nHighest);
				}
				
				htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
			}
		}
	}
	
	// update calc'ed value
	if (!bIncludeDue)
		pTDI->nCalcPriority = nHighest;
	
	return nHighest;
}

int CToDoCtrlData::GetHighestRisk(HTREEITEM hti, const TODOITEM* pTDI) const
{
	ASSERT (hti);
	ASSERT(pTDI);
	
	// some optimizations
	// try pre-calculated value first
	if (pTDI->nCalcRisk >= 0)
		return pTDI->nCalcRisk;
	
	int nHighest = pTDI->nRisk;
	
	if (nHighest < MAX_TDRISK)
	{
		if (pTDI->IsDone())
			nHighest = MIN_TDRISK;
		
		else if (HasStyle(TDCS_USEHIGHESTRISK) && m_tree.ItemHasChildren(hti))
		{
			// check children
			nHighest = MIN_TDRISK;
			
			HTREEITEM htiChild = m_tree.GetChildItem(hti);
			
			while (htiChild)
			{
				if (HasStyle(TDCS_INCLUDEDONEINRISKCALC) || !IsTaskDone(htiChild, TDCCHECKALL))
				{
					int nChildHighest = GetHighestRisk(htiChild);
					
					// optimization
					if (nChildHighest == MAX_TDRISK)
						return MAX_TDRISK;
					else 
						nHighest = max(nChildHighest, nHighest);
				}
				
				htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
			}
		}
	}
	
	// update calc'ed value
	pTDI->nCalcRisk = nHighest;
	
	return pTDI->nCalcRisk;
}

BOOL CToDoCtrlData::IsTaskDone(HTREEITEM hti, DWORD dwExtraCheck) const
{
	if (!hti || hti == TVI_ROOT)
		return FALSE;
	
	TODOITEM* pTDI = GetTask(hti);
	
	if (!pTDI)
		return FALSE;
	
	// check cached value
	if (dwExtraCheck == TDCCHECKALL && pTDI->bGoodAsDone >= 0)
		return pTDI->bGoodAsDone;
	
	if (pTDI->IsDone())
		return TRUE;
	
	BOOL bDone = FALSE;
	
	if (dwExtraCheck & TDCCHECKPARENT)
	{
		HTREEITEM htiParent = m_tree.GetParentItem(hti);
		
		if (htiParent && IsTaskDone(htiParent, TDCCHECKPARENT))
			bDone = TRUE;
	}
	
	// else check children for 'good-as-done'
	if (!bDone)
	{
		BOOL bTreatSubCompleteDone = HasStyle(TDCS_TREATSUBCOMPLETEDASDONE);
		
		if ((dwExtraCheck & TDCCHECKCHILDREN) && bTreatSubCompleteDone)
		{
			if (AreChildTasksDone(hti) == 1)
				bDone = TRUE;
		}
	}
	
	// update cached value
	if (dwExtraCheck == TDCCHECKALL)
		pTDI->bGoodAsDone = bDone;
	   
	// else return as is
	return bDone;
}

BOOL CToDoCtrlData::IsTaskTimeTrackable(HTREEITEM hti) const
{
	if (!hti)
		return FALSE;
	
	// not trackable if a container or complete
	if ((!HasStyle(TDCS_ALLOWPARENTTIMETRACKING) && m_tree.ItemHasChildren(hti)) || IsTaskDone(hti, TDCCHECKNONE))
		return FALSE;
	
	return TRUE;
}

BOOL CToDoCtrlData::SetTaskAttributeAsParent(HTREEITEM hti, TDC_ATTRIBUTE nAttrib)
{
	HTREEITEM htiParent = m_tree.GetParentItem(hti);
	
	if (!htiParent)
		return FALSE;
	
	DWORD dwID = m_tree.GetItemData(hti);
	DWORD dwParentID = m_tree.GetItemData(htiParent);
	int nRes = SET_NOCHANGE;
		
	switch (nAttrib)
	{
	case TDCA_TASKNAME:
		nRes = SetTaskTitle(dwID, GetTaskTitle(dwParentID));
		break;
		
	case TDCA_DONEDATE:
		nRes = SetTaskDate(dwID, TDCD_DONE, GetTaskDate(dwParentID, TDCD_DONE));
		break;
		
	case TDCA_DUEDATE:
		nRes = SetTaskDate(dwID, TDCD_DUE, GetTaskDate(dwParentID, TDCD_DUE));
		break;
		
	case TDCA_STARTDATE:
		nRes = SetTaskDate(dwID, TDCD_START, GetTaskDate(dwParentID, TDCD_START));
		break;
		
	case TDCA_PRIORITY:
		nRes = SetTaskPriority(dwID, GetTaskPriority(dwParentID));
		break;
		
	case TDCA_COLOR:
		nRes = SetTaskColor(dwID, GetTaskColor(dwParentID));
		break;
		
	case TDCA_ALLOCTO:
		nRes = SetTaskAllocTo(dwID, GetTaskAllocTo(dwParentID));
		break;
		
	case TDCA_ALLOCBY:
		nRes = SetTaskAllocBy(dwID, GetTaskAllocBy(dwParentID));
		break;
		
	case TDCA_STATUS:
		nRes = SetTaskStatus(dwID, GetTaskStatus(dwParentID));
		break;
		
	case TDCA_CATEGORY:
		{
			CStringArray aCats;
			GetTaskCategories(dwParentID, aCats);
			nRes = SetTaskCategories(dwID, aCats);
		}
		break;
		
	case TDCA_PERCENT:
		nRes = SetTaskPercent(dwID, GetTaskPercent(dwParentID, FALSE));
		break;
		
	case TDCA_TIMEEST:
		{
			int nUnits;
			double dTime = GetTaskTimeEstimate(dwParentID, nUnits);
			nRes = SetTaskTimeEstimate(dwID, dTime, nUnits);
		}
		break;
		
	case TDCA_FILEREF:
		nRes = SetTaskFileRef(dwID, GetTaskFileRef(dwParentID));
		break;
		
	case TDCA_COMMENTS:
		nRes = SetTaskComments(dwID, GetTaskComments(dwParentID));
		break;
	}

	return nRes;
}

void CToDoCtrlData::ApplyLastChangeToSubtasks(HTREEITEM hti, const TODOITEM* pTDI, TDC_ATTRIBUTE nAttrib)
{
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	TODOITEM* tdiChild;
	
	while (htiChild)
	{
		DWORD dwIDChild = GetTaskID(htiChild);
		tdiChild = GetTask(dwIDChild);
		ASSERT (tdiChild);
		
		// apply the change based on nAttrib
		switch (nAttrib)
		{
		case TDCA_DONEDATE:
			tdiChild->dateDone = pTDI->dateDone;
			break;
			
		case TDCA_DUEDATE:
			tdiChild->dateDue = pTDI->dateDue;
			break;
			
		case TDCA_STARTDATE:
			tdiChild->dateStart = pTDI->dateStart;
			break;
			
		case TDCA_PRIORITY:
			tdiChild->nPriority = pTDI->nPriority;
			break;
			
		case TDCA_COLOR:
			tdiChild->color = pTDI->color;
			break;
			
		case TDCA_ALLOCTO:
			tdiChild->sAllocTo = pTDI->sAllocTo;
			break;
			
		case TDCA_ALLOCBY:
			tdiChild->sAllocBy = pTDI->sAllocBy;
			break;
			
		case TDCA_STATUS:
			tdiChild->sStatus = pTDI->sStatus;
			break;
			
		case TDCA_CATEGORY:
			tdiChild->aCategories.Copy(pTDI->aCategories);
			break;
			
		case TDCA_PERCENT:
			tdiChild->nPercentDone = pTDI->nPercentDone;
			break;
			
		case TDCA_TIMEEST:
			tdiChild->dTimeEstimate = pTDI->dTimeEstimate;
			break;
			
		case TDCA_FILEREF:
			tdiChild->sFileRefPath = pTDI->sFileRefPath;
			break;
			
		default:
			ASSERT (0);
			return;
		}
		
		// and its children too
		if (m_tree.ItemHasChildren(htiChild))
			ApplyLastChangeToSubtasks(htiChild, tdiChild, nAttrib);
		
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

int CToDoCtrlData::SetTaskColor(DWORD dwID, COLORREF color)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			// if the color is 0 then add 1 to discern from unset
			if (!color)
				color++;
			
			if (pTDI->color != color)
			{
				pTDI->color = color;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskComments(DWORD dwID, LPCTSTR szComments)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sComments != szComments)
			{
				pTDI->sComments = szComments;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCustomComments(DWORD dwID, const CString& sComments)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sCustomComments.GetLength() != sComments.GetLength() ||
				memcmp((LPCTSTR)pTDI->sCustomComments, (LPCTSTR)sComments, sComments.GetLength()))
			{
				pTDI->sCustomComments = sComments;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskTitle(DWORD dwID, LPCTSTR szTitle)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sTitle != szTitle)
			{
				pTDI->sTitle = szTitle;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskFlag(DWORD dwID, BOOL bFlagged)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->bFlagged != bFlagged)
			{
				pTDI->bFlagged = bFlagged;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskRecurrence(DWORD dwID, const TDIRECURRENCE& tr)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->trRecurrence != tr)
			{
				pTDI->trRecurrence = tr;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskPriority(DWORD dwID, int nPriority)
{
	if (dwID && nPriority >= 0 && nPriority <= 10)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->nPriority != nPriority)
			{
				pTDI->nPriority = nPriority;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskRisk(DWORD dwID, int nRisk)
{
	if (dwID && nRisk >= 0 && nRisk <= 10)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->nRisk != nRisk)
			{
				pTDI->nRisk = nRisk;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskDate(DWORD dwID, TDC_DATE nDate, const COleDateTime& date)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			// whole days only
			COleDateTime dateDay = floor(date.m_dt);
			COleDateTime dateCur = GetTaskDate(dwID, nDate);

			if (dateCur != dateDay)
			{
				switch (nDate)
				{
				case TDCD_CREATE:	
					pTDI->dateCreated = dateDay;	
					break;

				case TDCD_START:	
					pTDI->dateStart = dateDay;		
					break;

				case TDCD_DUE:		
					pTDI->dateDue = dateDay;		
					break;

				case TDCD_DONE:		
					pTDI->dateDone = dateDay;
					break;

				default:
					ASSERT(0);
					return SET_FAILED;
				}
			
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::OffsetTaskDate(DWORD dwID, TDC_DATE nDate, int nAmount, int nUnits)
{
	DH_UNITS nDHUnits = (DH_UNITS)nUnits;
	ASSERT (nDHUnits == DHU_DAYS || nDHUnits == DHU_WEEKS || nDHUnits == DHU_MONTHS);
	
	COleDateTime date = GetTaskDate(dwID, nDate);
	CDateHelper::OffsetDate(date, nAmount, nDHUnits);

	return SetTaskDate(dwID, nDate, date);
}

int CToDoCtrlData::OffsetTaskDate(HTREEITEM hti, TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks)
{
	DWORD dwID = GetTaskID(hti);

	int nRes = SET_NOCHANGE;
	int nItemRes = OffsetTaskDate(dwID, nDate, nAmount, nUnits);

	if (nItemRes == SET_CHANGE)
		nRes = SET_CHANGE;

	// children
	if (bAndSubtasks)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			nItemRes = OffsetTaskDate(htiChild, nDate, nAmount, nUnits, TRUE);

			if (nItemRes == SET_CHANGE)
				nRes = SET_CHANGE;

			// next child
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return nRes;
}


int CToDoCtrlData::SetTaskDone(DWORD dwID, BOOL bDone, int& nPrevPercent)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			BOOL bWasDone = pTDI->IsDone();
			
			if (bWasDone != bDone)
			{
				if (!bDone)
				{
					// restore last known percentage unless is 100%
					if (pTDI->nPercentDone == 100)
						pTDI->nPercentDone = 0;
					
					nPrevPercent = pTDI->nPercentDone;
				}
				
				pTDI->dateDone = bDone ? floor(COleDateTime::GetCurrentTime()) : COleDateTime();
				pTDI->SetModified();
				return SET_CHANGE;
			}
			
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskPercent(DWORD dwID, int nPercent)
{
	if (nPercent < 0 || nPercent > 100)
		return FALSE;
	
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->nPercentDone != nPercent)
			{
				pTDI->nPercentDone = nPercent;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCost(DWORD dwID, const double& dCost)
{
	if (dCost < 0)
		return FALSE;
	
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->dCost != dCost)
			{
				pTDI->dCost = dCost;
				pTDI->SetModified();
				return SET_CHANGE;
			}	
			
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskTimeEstimate(DWORD dwID, const double& dTime, int nUnits)
{
	if (dTime < 0)
		return FALSE;
	
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->dTimeEstimate != dTime || pTDI->nTimeEstUnits != nUnits)
			{
				pTDI->dTimeEstimate = dTime;
				pTDI->nTimeEstUnits = nUnits;
				pTDI->SetModified();
				return SET_CHANGE;
			}	
			
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskTimeSpent(DWORD dwID, const double& dTime, int nUnits)
{
	if (dTime < 0)
		return FALSE;
	
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->dTimeSpent != dTime || pTDI->nTimeSpentUnits != nUnits)
			{
				pTDI->dTimeSpent = dTime;
				pTDI->nTimeSpentUnits = nUnits;
				pTDI->SetModified();
				return SET_CHANGE;
			}	
			
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskAllocTo(DWORD dwID, LPCTSTR szAllocTo)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sAllocTo != szAllocTo)
			{
				pTDI->sAllocTo = szAllocTo;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskAllocBy(DWORD dwID, LPCTSTR szAllocBy)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sAllocBy != szAllocBy)
			{
				pTDI->sAllocBy = szAllocBy;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskVersion(DWORD dwID, LPCTSTR szVersion)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sVersion != szVersion)
			{
				pTDI->sVersion = szVersion;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskStatus(DWORD dwID, LPCTSTR szStatus)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sStatus != szStatus)
			{
				pTDI->sStatus = szStatus;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCategory(DWORD dwID, LPCTSTR szCategory)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->GetFirstCategory() != szCategory)
			{
				pTDI->aCategories.RemoveAll();
				pTDI->aCategories.Add(szCategory);

				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCategories(DWORD dwID, const CStringArray& aCategories)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (!Misc::ArraysMatch(pTDI->aCategories, aCategories))
			{
				pTDI->aCategories.Copy(aCategories);
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskDependency(DWORD dwID, LPCTSTR szDepends)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sDependency != szDepends)
			{
				pTDI->sDependency = szDepends;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskExtID(DWORD dwID, LPCTSTR szID)
{
	if (dwID)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sExternalID != szID)
			{
				pTDI->sExternalID = szID;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::SetTaskFileRef(DWORD dwID, LPCTSTR szFilePath)
{
	if (dwID && szFilePath)
	{
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sFileRefPath.CompareNoCase(szFilePath))
			{
				pTDI->sFileRefPath = szFilePath;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}
	
	return SET_FAILED;
}

int CToDoCtrlData::MapTimeUnits(const CString& sUnits)
{
	if (sUnits.IsEmpty())
		return TDITU_HOURS; // default
	
	switch (sUnits[0])
	{
	case 'I':
	case 'i': return TDITU_MINS;
		
	case 'D':
	case 'd': return TDITU_DAYS;
		
	case 'W':
	case 'w': return TDITU_WEEKS;
		
	case 'M':
	case 'm': return TDITU_MONTHS;
		
	case 'Y':
	case 'y': return TDITU_YEARS;
	}
	
	// all else
	return TDITU_HOURS;
}

CString CToDoCtrlData::MapTimeUnits(int nUnits)
{
	switch (nUnits)
	{
	case TDITU_MINS:	return "I";
	case TDITU_DAYS:	return "D";
	case TDITU_WEEKS:	return "W";
	case TDITU_MONTHS:	return "M";
	case TDITU_YEARS:	return "Y";
	}
	
	// all else
	return "H";
}

HTREEITEM CToDoCtrlData::FindItem(DWORD dwID, HTREEITEM htiStart) const
{
	// try htiStart first
	if (htiStart && m_tree.GetItemData(htiStart) == dwID)
		return htiStart;
	
	// else try htiStart's children
	HTREEITEM htiFound = NULL;
	HTREEITEM htiChild = m_tree.GetChildItem(htiStart);
	
	while (htiChild && !htiFound)
	{
		htiFound = FindItem(dwID, htiChild);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return htiFound;
}

void CToDoCtrlData::LoadTreeExpandedState(LPCTSTR szRegKey)
{
	int nCount = AfxGetApp()->GetProfileInt(szRegKey, "Count", 0);
	CString sItem;
	
	CHTIMap map;
	BuildHTIMap(map);
	
	while (nCount--)
	{
		sItem.Format("Item%d", nCount);
		
		DWORD dwID = (DWORD)AfxGetApp()->GetProfileInt(szRegKey, sItem, 0);
		HTREEITEM hti = NULL;
		
		if (dwID && map.Lookup(dwID, hti) && hti)
			m_tree.Expand(hti, TVE_EXPAND);
	}
}

int CToDoCtrlData::SaveTreeExpandedState(LPCTSTR szRegKey, HTREEITEM hti, int nStart)
{
	CTreeCtrlHelper tch(m_tree);
	
	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	int nCount = nStart;
	
	while (htiChild)
	{
		if (tch.IsItemExpanded(htiChild))
		{
			CString sItem;
			sItem.Format("Item%d", nCount);
			
			AfxGetApp()->WriteProfileInt(szRegKey, sItem, (int)GetTaskID(htiChild));
			nCount++;
			
			// now its children
			nCount += SaveTreeExpandedState(szRegKey, htiChild, nCount);
		}
		
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}	
	
	return (nCount - nStart);
}

/*
void CToDoCtrlData::RefreshItemCheckState(HTREEITEM hti, int nDone, BOOL bRefreshParents)
{
	if (!hti || hti == TVI_ROOT)
		return;
	
	BOOL bTreatSubCompleteAsDone = HasStyle(TDCS_TREATSUBCOMPLETEDASDONE);
	TCH_CHECK nCheck = TCHC_UNCHECKED;
	
	if (nDone == -1) // => get current state
		nDone = IsTaskDone(hti);
	
	if (nDone)
		nCheck = TCHC_CHECKED;
	
	else if (bTreatSubCompleteAsDone && m_tree.ItemHasChildren(hti))
	{
		if (IsTaskDone(hti, TDCCHECKCHILDREN))
			nCheck = TCHC_MIXED;
	}
	
	CTreeCtrlHelper(m_tree).SetItemChecked(hti, nCheck);
	
	if (bTreatSubCompleteAsDone && bRefreshParents)
	{
		hti = m_tree.GetParentItem(hti);
		RefreshItemCheckState(hti, IsTaskDone(hti), TRUE);
	}
}

void CToDoCtrlData::RefreshAllCheckStates(HTREEITEM htiRoot)
{
	// process this item first
	RefreshItemCheckState(htiRoot);

	// then children
	HTREEITEM htiChild = m_tree.GetChildItem(htiRoot);

	while (htiChild)
	{
		RefreshItemCheckState(htiChild);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}
*/

CString CToDoCtrlData::GetTaskPath(HTREEITEM hti, int nMaxElementLen) const
{ 
	CString sPath;
	
	HTREEITEM htiParent = m_tree.GetParentItem(hti);
	
	while (htiParent)
	{
		TODOITEM* pTDI = GetTask(htiParent);
		CString sParent = pTDI->sTitle;
		
		if (nMaxElementLen != -1 && sParent.GetLength() > nMaxElementLen)
			sParent = sParent.Left(nMaxElementLen) + "...";
		
		sParent.TrimLeft();
		sParent.TrimRight();
		
		sPath = sParent + " / "+ sPath;
		htiParent = m_tree.GetParentItem(htiParent);
	}
	
	return sPath;
}


