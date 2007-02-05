// FindTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindTaskDlg.h"

#include "..\shared\deferwndmove.h"
#include "..\shared\dlgunits.h"
#include "..\shared\enstring.h"
#include "..\shared\dialoghelper.h"

#include <multimon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

enum { COL_TASK, COL_MATCH, COL_PATH };
const int MATCH_COLWIDTH = 50;

struct SEARCHFIELD
{
	UINT nIDWhat;
	FIND_WHAT nWhat;
	UINT nIDColumn;
	int nAlign;
};

static SEARCHFIELD SEARCHFIELDS[] = 
{
	{ IDS_FTD_TITLECOMMENTS,FW_TITLECOMMENTS,	0,						LVCFMT_LEFT },
	{ IDS_FTD_PRIORITY,		FW_PRIORITY,		IDS_TDLBC_PRIORITY,		LVCFMT_RIGHT },
	{ IDS_FTD_PERCENT,		FW_PERCENTDONE,		IDS_TDLBC_PERCENT,		LVCFMT_RIGHT },
	{ IDS_FTD_TIMEEST,		FW_TIMEEST,			IDS_TDLBC_TIMEEST,		LVCFMT_RIGHT },
	{ IDS_FTD_TIMESPENT,	FW_TIMESPENT,		IDS_TDLBC_TIMESPENT,	LVCFMT_RIGHT },
	{ IDS_FTD_STARTDATE,	FW_STARTDATE,		IDS_TDLBC_STARTDATE,	LVCFMT_RIGHT },
	{ IDS_FTD_DUEDATE,		FW_DUEDATE,			IDS_TDLBC_DUEDATE,		LVCFMT_RIGHT },
	{ IDS_FTD_DONEDATE,		FW_DONEDATE,		IDS_TDLBC_DONEDATE,		LVCFMT_RIGHT },
	{ IDS_FTD_ALLOCTO,		FW_ALLOCTO,			IDS_TDLBC_ALLOCTO,		LVCFMT_LEFT },
	{ IDS_FTD_ALLOCBY,		FW_ALLOCBY,			IDS_TDLBC_ALLOCBY,		LVCFMT_LEFT },
	{ IDS_FTD_STATUS,		FW_STATUS,			IDS_TDLBC_STATUS,		LVCFMT_LEFT },
	{ IDS_FTD_CATEGORY,		FW_CATEGORY,		IDS_TDLBC_CATEGORY,		LVCFMT_LEFT },
	{ IDS_FTD_ID,			FW_TASKID,			IDS_TDLBC_ID,			LVCFMT_RIGHT },
	{ IDS_FTD_FLAG,			FW_FLAG,			0,						LVCFMT_LEFT },
	{ IDS_FTD_CREATEDATE,	FW_CREATIONDATE,	IDS_TDLBC_CREATEDATE,	LVCFMT_LEFT },
	{ IDS_FTD_CREATEDBY,	FW_CREATEDBY,		IDS_TDLBC_CREATEDBY,	LVCFMT_LEFT },
	{ IDS_FTD_RISK,			FW_RISK,			IDS_TDLBC_RISK,			LVCFMT_RIGHT },
	{ IDS_FTD_LASTMOD,		FW_LASTMOD,			IDS_TDLBC_MODIFYDATE,	LVCFMT_RIGHT },
	{ IDS_FTD_EXTID,		FW_EXTERNALID,		IDS_TDLBC_EXTERNALID,	LVCFMT_RIGHT },
	{ IDS_FTD_COST,			FW_COST,			IDS_TDLBC_COST,			LVCFMT_RIGHT },
	{ IDS_FTD_VERSION,		FW_VERSION,			IDS_TDLBC_VERSION,		LVCFMT_LEFT },
	// add new fields on the end
};

const UINT FIELDCOUNT = sizeof(SEARCHFIELDS) / sizeof(SEARCHFIELD);

const SEARCHFIELD* GetSearchField(FIND_WHAT nWhat)
{
	int nField = FIELDCOUNT;
	
	while (nField--)
	{
		if (SEARCHFIELDS[nField].nWhat == nWhat)
			return &SEARCHFIELDS[nField];
	}
	
	// else
	return NULL;
}

static int TABSTOPS = 20;

/////////////////////////////////////////////////////////////////////////////
// CFindTaskDlg dialog

CFindTaskDlg::CFindTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindTaskDlg::IDD, pParent), 
	m_pageTimeEst(FALSE, FALSE), 
	m_pageTimeSpent(FALSE, FALSE), 
	m_pageCost(FALSE, FALSE), 
	m_pageTaskID(TRUE, FALSE),
	m_pageVersion(FALSE, FALSE),
	m_bDockable(FALSE),
	m_bSearchResults(FALSE)
{
	m_sResultsLabel.LoadString(IDS_FTD_RESULTS);
	
	//{{AFX_DATA_INIT(CFindTaskDlg)
	m_bAutoSelectSingles = FALSE;
	//}}AFX_DATA_INIT
	
	m_bAllTasklists = AfxGetApp()->GetProfileInt("FindTasks", "SearchAllTaskLists", FALSE);
	m_bIncludeDone = AfxGetApp()->GetProfileInt("FindTasks", "IncludeDoneTasks", FALSE);
	m_nFindOption = (FIND_WHAT)max(FW_TITLECOMMENTS, (int)AfxGetApp()->GetProfileInt("FindTasks", "LastFindOption", FW_TITLECOMMENTS));
	m_bAutoSelectSingles = AfxGetApp()->GetProfileInt("FindTasks", "AutoSelectSingles", FALSE);
	
	CPropertyPage* pPages[] = 
	{
		// same order as SEARCHFIELDS
		&m_pageTitleComments, &m_pagePriority, &m_pagePercent, &m_pageTimeEst, 
		&m_pageTimeSpent, &m_pageStartDate, &m_pageDueDate, &m_pageDoneDate, 
		&m_pageAllocTo, &m_pageAllocBy, &m_pageStatus, &m_pageCategory, 
		&m_pageTaskID, &m_pageFlag, &m_pageCreationDate, &m_pageCreatedBy,
		&m_pageRisk, &m_pageLastMod, &m_pageExtID, &m_pageCost, &m_pageVersion 
	};
	
	for (int nField = 0; nField < FIELDCOUNT; nField++)
	{
		const SEARCHFIELD& field = SEARCHFIELDS[nField];
		m_host.AddPage(pPages[nField], CEnString(field.nIDWhat), field.nWhat);
	}
}

CFindTaskDlg::~CFindTaskDlg()
{
}

void CFindTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTaskDlg)
	DDX_Control(pDX, IDC_FINDOPTION, m_cbFindOptions);
	DDX_Control(pDX, IDC_RESULTS, m_lcResults);
	DDX_CBIndex(pDX, IDC_TASKLISTOPTIONS, m_bAllTasklists);
	DDX_Text(pDX, IDC_RESULTSLABEL, m_sResultsLabel);
	DDX_Check(pDX, IDC_INCLUDEDONE, m_bIncludeDone);
	DDX_CBIndex(pDX, IDC_FINDOPTION, m_nSelFindIndex);
	DDX_Check(pDX, IDC_AUTOSELECTSINGLES, m_bAutoSelectSingles);
	DDX_Check(pDX, IDC_SEARCHRESULTS, m_bSearchResults);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindTaskDlg, CDialog)
//{{AFX_MSG_MAP(CFindTaskDlg)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_RESULTS, OnItemActivated)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_FINDOPTION, OnSelchangeFindoption)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_COMMAND(ID_DOCKRIGHT, OnDockright)
	ON_UPDATE_COMMAND_UI(ID_DOCKRIGHT, OnUpdateDockright)
	ON_COMMAND(ID_DOCKLEFT, OnDockleft)
	ON_UPDATE_COMMAND_UI(ID_DOCKLEFT, OnUpdateDockleft)
	ON_COMMAND(ID_UNDOCK, OnUndock)
	ON_UPDATE_COMMAND_UI(ID_UNDOCK, OnUpdateUndock)
	ON_COMMAND(ID_DOCKBELOW, OnDockbelow)
	ON_UPDATE_COMMAND_UI(ID_DOCKBELOW, OnUpdateDockbelow)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SEARCHRESULTS, OnSearchresults)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_RESULTS, OnCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////

BOOL CFindTaskDlg::Initialize(CWnd* pParent, BOOL bDockable)
{
	ASSERT (pParent || !bDockable);
	
	if (!pParent && bDockable)
		return FALSE;
	
	if (GetSafeHwnd())
		return TRUE;
	
	m_bDockable = bDockable;
	
	if (!Create(IDD_FIND_DIALOG, pParent))
		return FALSE;
	
	return TRUE;
}

void CFindTaskDlg::SetPriorityColors(const CDWordArray& aColors)
{
	m_pagePriority.SetPriorityColors(aColors);
}

BOOL CFindTaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_host.Create(IDC_PAGEHOST, this);
		
	// build combobox of pages
	int nPages = m_host.GetPageCount();
	
	for (int nPage = 0; nPage < nPages; nPage++)
	{
		int nIndex = m_cbFindOptions.InsertString(nPage, m_host.GetPageTitle(nPage));
		m_cbFindOptions.SetItemDataPtr(nIndex, m_host.GetPage(nPage));
		
		// init active page
		if (m_nFindOption == (int)m_host.GetPageItemData(nPage))
		{
			m_nSelFindIndex = nIndex;
			m_cbFindOptions.SetCurSel(nIndex);
			m_host.SetActivePage(nPage);
		}
	}
	
	// toolbar
	VERIFY (m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP)
		&& m_toolbar.LoadToolBar(IDR_FINDDLG, IDB_FIND16));
	
	m_tbHelper.Initialize(&m_toolbar, this);
	
	// very important - turn OFF all the auto positioning and sizing
	// by default have no borders
	UINT nStyle = m_toolbar.GetBarStyle();
	nStyle &= ~(CCS_NORESIZE | CCS_NOPARENTALIGN | CBRS_BORDER_ANY);
	nStyle |= (CBRS_SIZE_FIXED | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_toolbar.SetBarStyle(nStyle);
	
	CRect rToolbar;
	GetDlgItem(IDC_TOOLBAR)->GetWindowRect(rToolbar);
	ScreenToClient(rToolbar);
	m_toolbar.MoveWindow(rToolbar);

	// subclass the results header ctrl to allow locking
	CWnd* pHeader = m_lcResults.GetHeaderCtrl();

	if (pHeader)
		m_hdrResults.SubclassWindow(pHeader->GetSafeHwnd());
	
	// gripper
	// position resize icon as close to bottom left as pos
	GetDlgItem(IDC_GRIPPER)->ModifyStyle(0, SBS_SIZEGRIP | SBS_SIZEBOXTOPLEFTALIGN | WS_CLIPSIBLINGS);
	CDeferWndMove(1).OffsetCtrl(this, IDC_GRIPPER, 1, 2);
	
	// setup up result list
	m_lcResults.InsertColumn(COL_TASK, "Task", LVCFMT_LEFT, 200);
	m_lcResults.InsertColumn(COL_MATCH, "", LVCFMT_LEFT, MATCH_COLWIDTH);
	m_lcResults.InsertColumn(COL_PATH, "Path", LVCFMT_LEFT, 100);
	
	DWORD dwExStyle = m_lcResults.GetExtendedStyle() | 
		LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINEHOT;
	m_lcResults.SetExtendedStyle(dwExStyle);
	
	GetDlgItem(IDC_SELECTALL)->EnableWindow(FALSE);
	CenterWindow();
	
	LoadSettings();
	
    m_toolbar.RefreshButtonStates();
	
	ResizeDlg();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFindTaskDlg::LoadSettings()
{
	// restore column widths
	m_lcResults.SetColumnWidth(COL_MATCH, AfxGetApp()->GetProfileInt("FindTasks", "MatchColWidth", MATCH_COLWIDTH));
	m_lcResults.SetColumnWidth(COL_PATH, AfxGetApp()->GetProfileInt("FindTasks", "PathColWidth", 100));
	InitFieldColumn();
	
	if (m_bDockable)
	{
		DM_POS nPos = DMP_UNDOCKED, nLastPos = DMP_RIGHT;
		DWORD dwTopLeft = (DWORD)AfxGetApp()->GetProfileInt("FindTasks", "TopLeft", -1);
		DWORD dwBottomRight = (DWORD)AfxGetApp()->GetProfileInt("FindTasks", "BottomRight", -1);
		
		if (dwBottomRight != -1 && dwTopLeft != -1)
		{
			nPos = (DM_POS)AfxGetApp()->GetProfileInt("FindTasks", "DockPos", DMP_UNDOCKED);
			nLastPos = (DM_POS)AfxGetApp()->GetProfileInt("FindTasks", "LastDockPos", DMP_RIGHT);

			CRect rect(GET_X_LPARAM(dwTopLeft), GET_Y_LPARAM(dwTopLeft), 
				GET_X_LPARAM(dwBottomRight), GET_Y_LPARAM(dwBottomRight));
			
			// ensure this intersects with the desktop
			if (NULL != MonitorFromRect(rect, MONITOR_DEFAULTTONULL))
				MoveWindow(rect);
		}
		
		// use the default size of the page host to initialize the 
		// docked width/height
		CSize rDef = GetMinDockedSize(nPos);
		
		int nWidthDocked = AfxGetApp()->GetProfileInt("FindTasks", "DockedWidth", rDef.cx);
		int nWidthDockedMax = AfxGetApp()->GetProfileInt("FindTasks", "DockedWidthMax", -1);
		
		int nHeightDocked = AfxGetApp()->GetProfileInt("FindTasks", "DockedHeight", rDef.cy);
		int nHeightDockedMax = AfxGetApp()->GetProfileInt("FindTasks", "DockedHeightMax", -1);
		
		m_dockMgr.Initialize(GetParent(), this, nPos, nLastPos,
							nWidthDocked, nWidthDockedMax, nHeightDocked, nHeightDockedMax);
	}
}

CSize CFindTaskDlg::GetMinDockedSize(DM_POS nPos)
{
	// start with Host size
	CRect rHost;
	m_host.GetWindowRect(rHost);
	ScreenToClient(rHost);

	// and RHS of find button
	CRect rFind;
	GetDlgItem(IDC_FIND)->GetWindowRect(rFind);
	ScreenToClient(rFind);
	rHost.right = rFind.right;

	if (nPos == DMP_BELOW)
		rHost.right += CDlgUnits(*this).ToPixelsX(40); // so the results list is visible
	else	
		rHost.bottom += CDlgUnits(*this).ToPixelsY(40); // so the results list is visible
	
	// add border
	int nBorder = CDlgUnits(*this).ToPixelsX(7);
	rHost.InflateRect(nBorder, nBorder);
	
	// allow for non-client border
	rHost.left = rHost.top = 0;
	CalcWindowRect(rHost);
	
	return rHost.Size();
}

BOOL CFindTaskDlg::Show(BOOL bShow)
{
	if (!GetSafeHwnd())
		return FALSE;
	
	if (bShow)
	{
		// reset m_bSearchResults if dialog was previously hidden
		if (!IsWindowVisible())
		{
			m_bSearchResults = FALSE;
			UpdateData(FALSE);
		}
		
	    m_toolbar.RefreshButtonStates();

		ShowWindow(SW_SHOW);
		SetForegroundWindow(); // give it the focus

		// activate the current page
		m_host.GetActivePage()->OnSetActive();
	}
	else
		ShowWindow(SW_HIDE);
	
	return TRUE;
}

void CFindTaskDlg::AddResult(LPCTSTR szTask, LPCTSTR szMatch, LPCTSTR szPath, BOOL bDone, DWORD dwItemData, int nTaskList)
{
	if (GetSafeHwnd())
	{
		int nPos = m_lcResults.GetItemCount();
		
		// add result
		int nIndex = m_lcResults.InsertItem(nPos, szTask);
		
		m_lcResults.SetItemText(nIndex, COL_PATH, szPath);

		if (szMatch && lstrlen(szMatch))
			m_lcResults.SetItemText(nIndex, COL_MATCH, szMatch);
		
		m_lcResults.UpdateWindow();
		
		// map identifying data
		FTDRESULT res;
		res.dwItemData = dwItemData;
		res.nTaskList = nTaskList;
		res.bDone = bDone;
		
		DWORD dwKey = m_lcResults.GetItemCount();
		m_lcResults.SetItemData(nIndex, dwKey);
		m_mapResults[dwKey] = res;
		
		// update 'found' count
		m_sResultsLabel.Format(IDS_FTD_SOMERESULTS, nPos + 1);
		UpdateData(FALSE);
		
		// focus first item added
        if (m_lcResults.GetItemCount() == 1)
		{
            m_lcResults.SetItemState(nIndex, LVIS_FOCUSED | LVIS_SELECTED, 
												LVIS_FOCUSED | LVIS_SELECTED);

			// update 'search results' button' state
			m_toolbar.RefreshButtonStates();
			
			// enable 'select all' button
			GetDlgItem(IDC_SELECTALL)->EnableWindow(TRUE);
		}
	}
}

BOOL CFindTaskDlg::GetSearchAllTasklists()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_bAllTasklists;
}

BOOL CFindTaskDlg::GetAutoSelectSingles()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_bAutoSelectSingles;
}

POSITION CFindTaskDlg::GetFirstResultPos() const
{
	return m_mapResults.GetStartPosition();
}

BOOL CFindTaskDlg::GetNextResult(POSITION& pos, FTDRESULT& res) const
{
	if (!pos)
		return FALSE;
	
	DWORD dwKey = 0;
	m_mapResults.GetNextAssoc(pos, dwKey, res);
	
	return TRUE;
}

int CFindTaskDlg::GetResultCount() const
{
	return m_mapResults.GetCount();
}

int CFindTaskDlg::GetResultCount(int nTaskList) const
{
	int nCount = 0;
	
	POSITION pos = GetFirstResultPos();
	
	while (pos)
	{
		FTDRESULT res;
		GetNextResult(pos, res);
		
		if (res.nTaskList == nTaskList)
			nCount++;
	}
	
	return nCount;
}

int CFindTaskDlg::GetAllResults(CFTDResultsArray& results) const
{
	int nRes = GetResultCount();
	results.RemoveAll();
	results.SetSize(nRes);

	while (nRes--)
		m_mapResults.Lookup((DWORD)(nRes + 1), results[nRes]);

	return results.GetSize();
}

BOOL CFindTaskDlg::GetIncludeDone()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_bIncludeDone;
}

BOOL CFindTaskDlg::GetSearchResults()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_bSearchResults;
}

CString CFindTaskDlg::GetText()
{
	switch (m_nFindOption)
	{
	case FW_TITLECOMMENTS:
		return m_pageTitleComments.GetText();
		
	case FW_ALLOCTO:
		return m_pageAllocTo.GetText();
		
	case FW_ALLOCBY:
		return m_pageAllocBy.GetText();
		
	case FW_STATUS:
		return m_pageStatus.GetText();
		
	case FW_CREATEDBY:
		return m_pageCreatedBy.GetText();
		
	case FW_EXTERNALID:
		return m_pageExtID.GetText();
		
	case FW_VERSION:
		return m_pageVersion.GetText();
	}
	
	// all else
	ASSERT(0);
	return "";
}

int CFindTaskDlg::GetCategories(CStringArray& aCats)
{
	ASSERT(m_nFindOption == FW_CATEGORY);

	return m_pageCategory.GetCategories(aCats);
}

BOOL CFindTaskDlg::GetMatchAllCategories()
{
	ASSERT(m_nFindOption == FW_CATEGORY);

	return m_pageCategory.GetMatchAllCategories();
}

BOOL CFindTaskDlg::GetMatchCase()
{
	switch (m_nFindOption)
	{
	case FW_TITLECOMMENTS:
		return m_pageTitleComments.GetMatchCase();
		
	case FW_ALLOCTO:
		return m_pageAllocTo.GetMatchCase();
		
	case FW_ALLOCBY:
		return m_pageAllocBy.GetMatchCase();
		
	case FW_STATUS:
		return m_pageStatus.GetMatchCase();
		
	case FW_CREATEDBY:
		return m_pageCreatedBy.GetMatchCase();
		
	case FW_EXTERNALID:
		return m_pageExtID.GetMatchCase();
		
	case FW_VERSION:
		return FALSE;
	}
	
	// all else
	ASSERT(0);
	return FALSE;
}

BOOL CFindTaskDlg::GetMatchWholeWord()
{
	switch (m_nFindOption)
	{
	case FW_TITLECOMMENTS:
		return m_pageTitleComments.GetMatchWholeWord();
		
	case FW_ALLOCTO:
		return m_pageAllocTo.GetMatchWholeWord();
		
	case FW_ALLOCBY:
		return m_pageAllocBy.GetMatchWholeWord();
		
	case FW_STATUS:
		return m_pageStatus.GetMatchWholeWord();
		
	case FW_CREATEDBY:
		return m_pageCreatedBy.GetMatchWholeWord();
		
	case FW_EXTERNALID:
		return m_pageExtID.GetMatchWholeWord();
		
	case FW_VERSION:
		return m_pageVersion.GetMatchWholeWord();
	}
	
	// all else
	ASSERT(0);
	return FALSE;
}

FIND_WHAT CFindTaskDlg::GetFindWhat()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_nFindOption;
}

BOOL CFindTaskDlg::GetRange(COleDateTime& dateFrom, COleDateTime& dateTo)
{
	switch (m_nFindOption)
	{
	case FW_CREATIONDATE:
		m_pageCreationDate.GetDateRange(dateFrom, dateTo);
		return TRUE;
		
	case FW_STARTDATE:
		m_pageStartDate.GetDateRange(dateFrom, dateTo);
		return TRUE;
		
	case FW_DUEDATE:
		m_pageDueDate.GetDateRange(dateFrom, dateTo);
		return TRUE;
		
	case FW_DONEDATE:
		m_pageDoneDate.GetDateRange(dateFrom, dateTo);
		return TRUE;
		
	case FW_LASTMOD:
		m_pageLastMod.GetDateRange(dateFrom, dateTo);
		return TRUE;
	}
	
	// all else
	ASSERT(0);
	return FALSE;
}

BOOL CFindTaskDlg::GetRange(int& nFrom, int& nTo)
{
	switch (m_nFindOption)
	{
	case FW_PRIORITY:
		m_pagePriority.GetRange(nFrom, nTo);
		return TRUE;
		
	case FW_PERCENTDONE:
		m_pagePercent.GetNumberRange(nFrom, nTo);
		return TRUE;
		
	case FW_TASKID:
		m_pageTaskID.GetNumberRange(nFrom, nTo);
		return TRUE;
		
	case FW_RISK:
		m_pageRisk.GetRange(nFrom, nTo);
		return TRUE;
	}
	
	// all else
	ASSERT(0);
	return FALSE;
}

BOOL CFindTaskDlg::GetRange(double& dFrom, double& dTo)
{	
	switch (m_nFindOption)
	{
	case FW_TIMEEST:
		m_pageTimeEst.GetNumberRange(dFrom, dTo);
		return TRUE;
		
	case FW_TIMESPENT:
		m_pageTimeSpent.GetNumberRange(dFrom, dTo);
		return TRUE;
		
	case FW_COST:
		m_pageCost.GetNumberRange(dFrom, dTo);
		return TRUE;
	}
	
	// all else
	ASSERT(0);
	return FALSE;
}

void CFindTaskDlg::RefreshSearch()
{
	ASSERT (GetSafeHwnd());
	
	OnFind();
}

void CFindTaskDlg::OnFind() 
{
	// add typed text to combobox
	UpdateData();
	
	// notify parent
	GetParent()->PostMessage(WM_FTD_FIND);
	
	// clear results
	m_mapResults.RemoveAll();
	m_lcResults.DeleteAllItems();
	m_lcResults.SetFocus();
	m_sResultsLabel.Format(IDS_FTD_SOMERESULTS, 0);
	UpdateData(FALSE);
	
	// update field column
	InitFieldColumn();
	
	// update 'search results' button' state
	m_toolbar.RefreshButtonStates();
			
	// disable 'select all' button
	GetDlgItem(IDC_SELECTALL)->EnableWindow(FALSE);
}

void CFindTaskDlg::OnClose() 
{
	CDialog::OnClose();
	
	// hide
	ShowWindow(SW_HIDE);
	
	// notify parent
	GetParent()->SendMessage(WM_FTD_CLOSE);
}

void CFindTaskDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	ResizeDlg(cx, cy);

	if (m_toolbar.GetSafeHwnd())
		m_toolbar.RefreshButtonStates();
}

void CFindTaskDlg::ResizeDlg(int cx, int cy)
{
	if (m_lcResults.GetSafeHwnd())
	{
		if (!cx && !cy)
		{
			CRect rClient;
			GetClientRect(rClient);
			
			cx = rClient.right;
			cy = rClient.bottom;
			
			// check again 
			if (!cx && !cy)
				return;
		}
		
		// we compare the new size with the lower right hand corner
		// of the gripper list
		CRect rect;
		GetDlgItem(IDC_GRIPPER)->GetWindowRect(rect);
		ScreenToClient(rect);
		
		int nXOffset = cx - rect.right;
		int nYOffset = cy - rect.bottom;
		
		BOOL bDockedBelow = (m_dockMgr.GetDockPos() == DMP_BELOW);
		
		{
			CDeferWndMove dwm(10);
			
			CRect rHDiv = dwm.ResizeCtrl(this, IDC_HDIVIDER, nXOffset);
			CRect rVDiv = dwm.ResizeCtrl(this, IDC_VDIVIDER);
			rVDiv = dwm.ResizeCtrl(this, IDC_VDIVIDER, 0, cy - rVDiv.bottom - 10);
			
			dwm.OffsetCtrl(this, IDC_GRIPPER, nXOffset, nYOffset);
			
			// move 'select all', results and other assorted items
			// depending on where we are docked
			CRect rSelAll = dwm.ResizeCtrl(this, IDC_SELECTALL);
			
			if (bDockedBelow)
			{
				nXOffset = rVDiv.right - rSelAll.left + 10;
				nYOffset = rVDiv.top - rSelAll.top;
			}
			else
			{
				nXOffset = rHDiv.left - rSelAll.left;
				nYOffset = rHDiv.top - rSelAll.top + 10;
			}
			
			dwm.OffsetCtrl(this, IDC_SELECTALL, nXOffset, nYOffset);
			dwm.OffsetCtrl(this, IDC_RESULTSLABEL, nXOffset, nYOffset);
			
			CRect rAuto = dwm.OffsetCtrl(this, IDC_AUTOSELECTSINGLES);
			rAuto = dwm.OffsetCtrl(this, IDC_AUTOSELECTSINGLES, nXOffset, cy - rAuto.bottom - 10);
			
			CRect rResults = dwm.OffsetCtrl(this, IDC_RESULTS, nXOffset, nYOffset);
			
			rResults.right = cx - 10;
			rResults.bottom = rAuto.top - 10;
			
			dwm.MoveWindow(&m_lcResults, rResults);
		}
		
		// hide the gripper if we are maximized
		GetDlgItem(IDC_GRIPPER)->ShowWindow(m_dockMgr.Maximized() ? SW_HIDE : SW_SHOW);
		
		// hide appropriate divider
		GetDlgItem(IDC_HDIVIDER)->ShowWindow(bDockedBelow ? SW_HIDE : SW_SHOW);
		GetDlgItem(IDC_VDIVIDER)->ShowWindow(bDockedBelow ? SW_SHOW : SW_HIDE);
		
		// resize the column headers to fit the available space
		// provided it does not get less than 100
		int nFixed = m_lcResults.GetColumnWidth(COL_PATH) + m_lcResults.GetColumnWidth(COL_MATCH);
		
		if (!(m_lcResults.GetStyle() & WS_VSCROLL))
			nFixed += GetSystemMetrics(SM_CXVSCROLL);
		
		m_lcResults.GetClientRect(rect);
		m_lcResults.SetColumnWidth(COL_TASK, max(100, rect.Width() - nFixed));
	}	
}

void CFindTaskDlg::OnItemActivated(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	ASSERT (pNMListView->iItem >= 0);
	
	FTDRESULT res;
	DWORD dwKey = m_lcResults.GetItemData(pNMListView->iItem);
	
	if (m_mapResults.Lookup(dwKey, res))
		GetParent()->SendMessage(WM_FTD_SELECTRESULT, dwKey, (LPARAM)&res);
	
	*pResult = 0;
}

void CFindTaskDlg::OnDestroy() 
{
	SaveSettings();
	
	CDialog::OnDestroy();
}

void CFindTaskDlg::SaveSettings()
{
	UpdateData();
	
	AfxGetApp()->WriteProfileInt("FindTasks", "SearchAllTaskLists", m_bAllTasklists);
	AfxGetApp()->WriteProfileInt("FindTasks", "IncludeDoneTasks", m_bIncludeDone);
	AfxGetApp()->WriteProfileInt("FindTasks", "LastFindOption", m_nFindOption);
	AfxGetApp()->WriteProfileInt("FindTasks", "AutoSelectSingles", m_bAutoSelectSingles);
	
	// save column widths
	AfxGetApp()->WriteProfileInt("FindTasks", "PathColWidth", m_lcResults.GetColumnWidth(COL_PATH));
	AfxGetApp()->WriteProfileInt("FindTasks", "MatchColWidth", m_lcResults.GetColumnWidth(COL_MATCH));
	
	// pos
	CRect rDialog = m_dockMgr.GetUnDockedRect();
	
	AfxGetApp()->WriteProfileInt("FindTasks", "TopLeft", MAKELPARAM(rDialog.left, rDialog.top));
	AfxGetApp()->WriteProfileInt("FindTasks", "BottomRight", MAKELPARAM(rDialog.right, rDialog.bottom));
	AfxGetApp()->WriteProfileInt("FindTasks", "DockPos", m_dockMgr.GetDockPos());
	AfxGetApp()->WriteProfileInt("FindTasks", "LastDockPos", m_dockMgr.GetLastDockPos());
	AfxGetApp()->WriteProfileInt("FindTasks", "DockedWidth", m_dockMgr.GetDockedWidth(FALSE));
	AfxGetApp()->WriteProfileInt("FindTasks", "DockedWidthMax", m_dockMgr.GetDockedWidth(TRUE));
	AfxGetApp()->WriteProfileInt("FindTasks", "DockedHeight", m_dockMgr.GetDockedHeight(FALSE));
	AfxGetApp()->WriteProfileInt("FindTasks", "DockedHeightMax", m_dockMgr.GetDockedHeight(TRUE));
	
}

void CFindTaskDlg::OnSelchangeFindoption() 
{
	UpdateData();
	
	m_nFindOption = (FIND_WHAT)m_host.GetPageItemData(m_nSelFindIndex);
	
	CPropertyPage* pPage = (CPropertyPage*)m_cbFindOptions.GetItemDataPtr(m_nSelFindIndex);
	ASSERT (pPage);
	
	m_host.SetActivePage(pPage, FALSE);	

	// set focus back to combo to allow user to continue navigating
	m_cbFindOptions.SetFocus();
}

void CFindTaskDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	
	if (m_dockMgr.Initialized())
	{
		if (!m_dockMgr.IsDocked())
		{
			CDlgUnits dlu(*this);
			
			lpMMI->ptMinTrackSize.x = dlu.ToPixelsX(290);
			lpMMI->ptMinTrackSize.y = dlu.ToPixelsY(230);
		}
		else
		{
			CSize rDef = GetMinDockedSize(m_dockMgr.GetDockPos());

			lpMMI->ptMinTrackSize.y = rDef.cy;
			lpMMI->ptMinTrackSize.x = rDef.cx;
		}
	}
}

BOOL CFindTaskDlg::PreTranslateMessage(MSG* pMsg) 
{
	// handle enter key if results list has the focus
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_lcResults)
	{
		int nSel = m_lcResults.GetNextItem(-1, LVIS_FOCUSED | LVIS_SELECTED);
		
		if (nSel != -1)
		{
			FTDRESULT res;
			DWORD dwKey = m_lcResults.GetItemData(nSel);
			
			if (m_mapResults.Lookup(dwKey, res))
				GetParent()->SendMessage(WM_FTD_SELECTRESULT, dwKey, (LPARAM)&res);
			
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFindTaskDlg::OnSelectall() 
{
	int nRes = GetResultCount();

	// if there is only one result then act as if the user 
	// clicked on it directly
	if (nRes == 1)
	{
		FTDRESULT res;
		DWORD dwKey = m_lcResults.GetItemData(0);
	
		VERIFY (m_mapResults.Lookup(dwKey, res));
		GetParent()->SendMessage(WM_FTD_SELECTRESULT, dwKey, (LPARAM)&res);
	}
	else if (nRes) // > 1
		GetParent()->SendMessage(WM_FTD_SELECTALL);
}

void CFindTaskDlg::OnDockright() 
{
	// toggle docked state
	if (m_dockMgr.GetDockPos() == DMP_RIGHT)
		m_dockMgr.UnDock();
	else
		m_dockMgr.Dock(DMP_RIGHT);

	ResizeDlg();
    m_toolbar.RefreshButtonStates();
}

void CFindTaskDlg::OnUpdateDockright(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_RIGHT);
}

void CFindTaskDlg::OnDockleft() 
{
	// toggle docked state
	if (m_dockMgr.GetDockPos() == DMP_LEFT)
		m_dockMgr.UnDock();
	else
		m_dockMgr.Dock(DMP_LEFT);

	ResizeDlg();
	
    m_toolbar.RefreshButtonStates();
}

void CFindTaskDlg::OnUpdateDockleft(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_LEFT);
}

void CFindTaskDlg::OnUndock() 
{
	m_dockMgr.Dock(DMP_UNDOCKED);
	ResizeDlg();
	
    m_toolbar.RefreshButtonStates();
}

void CFindTaskDlg::OnUpdateUndock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_UNDOCKED);
}

void CFindTaskDlg::InitFieldColumn()
{
	const SEARCHFIELD* pField = GetSearchField(m_nFindOption);
	static int nPrevFieldWidth = MATCH_COLWIDTH;
	int nColWidth = m_lcResults.GetColumnWidth(COL_MATCH);
	
	if (!(pField && pField->nIDColumn))
	{
		// hide the column
		LVCOLUMN lvc = { LVCF_WIDTH, 0, 0 };
		m_lcResults.SetColumn(COL_MATCH, &lvc);

		// and prevent resizing
		m_hdrResults.LockColumn(COL_MATCH);
	}
	else
	{
		// prepare the column
		CEnString sColumn(pField->nIDColumn);
		LVCOLUMN lvc = { LVCF_TEXT | LVCF_FMT, pField->nAlign, 0, (LPTSTR)(LPCTSTR)sColumn };
		
		if (!nColWidth && nPrevFieldWidth)
		{
			lvc.mask |= LVCF_WIDTH;
			lvc.cx = nPrevFieldWidth;
		}
		
		m_lcResults.SetColumn(COL_MATCH, &lvc);

		// and allow resizing
		m_hdrResults.LockColumn(COL_MATCH, FALSE);
	}
	
	if (nColWidth)
		nPrevFieldWidth = nColWidth;
}

void CFindTaskDlg::OnDockbelow() 
{
	// toggle docked state
	if (m_dockMgr.GetDockPos() == DMP_BELOW)
		m_dockMgr.UnDock();
	else
		m_dockMgr.Dock(DMP_BELOW);
	ResizeDlg();
	
    m_toolbar.RefreshButtonStates();
}

void CFindTaskDlg::OnUpdateDockbelow(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_BELOW);
}

void CFindTaskDlg::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCUSTOMDRAW* pNMCD = (NMCUSTOMDRAW*)pNMHDR;
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMCD;
	
	if (pNMCD->dwDrawStage == CDDS_PREPAINT)
	   *pResult = CDRF_NOTIFYITEMDRAW;          // ask for item notifications.

	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		FTDRESULT res;
		DWORD dwKey = m_lcResults.GetItemData((int)pNMCD->dwItemSpec);
		
		if (m_mapResults.Lookup(dwKey, res) && res.bDone)
		{
			// get the users 'done' colour
			if (AfxGetApp()->GetProfileInt("Preferences", "SpecifyDoneColor", FALSE))
			{
				COLORREF crText = (COLORREF)AfxGetApp()->GetProfileInt("Preferences\\Colors", "TaskDone", -1);

				if (crText != (COLORREF)-1)
				{
					pLVCD->clrText = crText;
					*pResult |= CDRF_NEWFONT;
				}
			}
		}
	}
}

void CFindTaskDlg::DeleteResults(int nTaskList)
{
	// work backwards from the last list res
	int nItem = m_lcResults.GetItemCount();

	while (nItem--)
	{
		FTDRESULT res;
		DWORD dwKey = m_lcResults.GetItemData(nItem);

		VERIFY (m_mapResults.Lookup(dwKey, res));

		if (res.nTaskList == nTaskList)
		{
			m_lcResults.DeleteItem(nItem);
			m_mapResults.RemoveKey(dwKey);
		}
	}

	// update 'found' count
	m_sResultsLabel.Format(IDS_FTD_SOMERESULTS, m_lcResults.GetItemCount());
	UpdateData(FALSE);
}


void CFindTaskDlg::OnSearchresults() 
{
	UpdateData();
}
