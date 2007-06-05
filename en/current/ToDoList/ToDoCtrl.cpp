// ToDoCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ToDoCtrl.h"
#include "tdlschemadef.h"
#include "taskfile.h"
#include "tdcmsg.h"
#include "todocommentsctrl.h"
#include "tdstringres.h"
#include "resource.h"
#include "tasktimelog.h"

#include "..\shared\holdredraw.h"
#include "..\shared\deferWndMove.h"
#include "..\shared\dlgunits.h"
#include "..\shared\themed.h"
#include "..\shared\datehelper.h"
#include "..\shared\driveinfo.h"
#include "..\shared\toolbarhelper.h"
#include "..\shared\colordef.h"
#include "..\shared\spellcheckdlg.h"
#include "..\shared\passworddialog.h"
#include "..\shared\winclasses.h"
#include "..\shared\wclassdefines.h"
#include "..\shared\enfiledialog.h"
#include "..\shared\misc.h"
#include "..\shared\enstring.h"
#include "..\shared\stringres.h"
#include "..\shared\treectrlhelper.h"
#include "..\shared\filemisc.h"
#include "..\shared\osversion.h"

#include <Windowsx.h>
#include <float.h>
#include <Lmcons.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// note: do not use 'c' as a shortcut letter
static TDCCONTROL TDCCONTROLS[] = 
{
	{ WC_STATIC,	IDS_TDC_FIELD_PROJECT, 0, 0, 0,3,28,8, IDC_PROJECTLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | WS_TABSTOP, 0, 28,1,142,13, IDC_PROJECTNAME },
	{ WC_STATIC,	IDS_TDC_FIELD_TASKLIST, WS_NOTVISIBLE, 0, 0,0,0,0, (UINT)IDC_STATIC },
	{ WC_TREEVIEW,	0, TVS_EDITLABELS | WS_TABSTOP | TVS_SHOWSELALWAYS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0, 0,16,190,108, IDC_TASKLIST },
	{ WC_STATIC,	IDS_TDC_FIELD_PRIORITY, SS_CENTERIMAGE, 0, 119,131,22,8, IDC_PRIORITYLABEL },
	{ WC_COMBOBOX,	0, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, 0, 159,128,65,300, IDC_PRIORITY },
	{ WC_STATIC,	IDS_TDC_FIELD_RISK, SS_CENTERIMAGE, 0, 119,131,22,8, IDC_RISKLABEL },
	{ WC_COMBOBOX,	0, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 159,128,65,300, IDC_RISK },
	{ WC_STATIC,	IDS_TDC_FIELD_PERCENT, SS_CENTERIMAGE, 0, 119,148,37,8, IDC_PERCENTLABEL },
	{ WC_EDIT,		0, WS_TABSTOP, 0, 159,146,65,13, IDC_PERCENT },
	{ WC_SPIN,		0, UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 97,180,13,14, IDC_PERCENTSPIN },
	{ WC_STATIC,	IDS_TDC_FIELD_TIMEEST, SS_CENTERIMAGE, 0, 1,148,40,8, IDC_TIMEESTLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | WS_TABSTOP, 0, 45,146,65,13, IDC_TIMEEST },
	{ WC_STATIC,	IDS_TDC_FIELD_TIMESPENT, SS_CENTERIMAGE, 0, 1,148,40,8, IDC_TIMESPENTLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | WS_TABSTOP, 0, 45,146,65,13, IDC_TIMESPENT },
	{ WC_STATIC,	IDS_TDC_FIELD_STARTDATE, SS_CENTERIMAGE, 0, 1,166,33,8, IDC_STARTLABEL },
	{ WC_DATETIMEPICK, 0, DTS_RIGHTALIGN | DTS_SHOWNONE | WS_TABSTOP, 0, 45,164,65,13, IDC_STARTDATE },
	{ WC_STATIC,	IDS_TDC_FIELD_DUEDATE, SS_CENTERIMAGE, 0, 119,166,32,8, IDC_DUELABEL },
	{ WC_DATETIMEPICK, 0, DTS_RIGHTALIGN | DTS_SHOWNONE | WS_TABSTOP, 0, 159,164,65,13, IDC_DUEDATE },
	{ WC_STATIC,	IDS_TDC_FIELD_DONEDATE, SS_CENTERIMAGE, 0, 1,184,34,8, IDC_DONELABEL },
	{ WC_DATETIMEPICK, 0, DTS_RIGHTALIGN | DTS_SHOWNONE | WS_TABSTOP, 0, 45,182,65,13, IDC_DONEDATE },
	{ WC_STATIC,	IDS_TDC_FIELD_RECURRENCE, SS_CENTERIMAGE, 0, 119,184,37,8, IDC_RECURRENCELABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP, 0, 159,182,65,13, IDC_RECURRENCE },
	{ WC_STATIC,	IDS_TDC_FIELD_ALLOCTO, SS_CENTERIMAGE, 0, 1,131,38,8, IDC_ALLOCTOLABEL },
	{ WC_COMBOBOX,	0, CBS_SORT | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_AUTOHSCROLL | WS_TABSTOP | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, 0, 45,128,65,200, IDC_ALLOCTO },
	{ WC_STATIC,	IDS_TDC_FIELD_ALLOCBY, SS_CENTERIMAGE, 0, 1,131,38,8, IDC_ALLOCBYLABEL },
	{ WC_COMBOBOX,	0, CBS_SORT | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_AUTOHSCROLL | WS_TABSTOP, 0, 45,128,65,200, IDC_ALLOCBY },
	{ WC_STATIC,	IDS_TDC_FIELD_STATUS, SS_CENTERIMAGE, 0, 1,131,38,8, IDC_STATUSLABEL },
	{ WC_COMBOBOX,	0, CBS_SORT | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_AUTOHSCROLL | WS_TABSTOP, 0, 45,128,65,200, IDC_STATUS },
	{ WC_STATIC,	IDS_TDC_FIELD_CATEGORY, SS_CENTERIMAGE, 0, 1,131,38,8, IDC_CATEGORYLABEL },
	{ WC_COMBOBOX,	0, CBS_SORT | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_AUTOHSCROLL | WS_TABSTOP | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, 0, 45,128,65,200, IDC_CATEGORY },
	{ WC_STATIC,	IDS_TDC_FIELD_EXTID, SS_CENTERIMAGE, 0, 119,184,37,8, IDC_EXTERNALIDLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP, 0, 159,182,65,13, IDC_EXTERNALID },
	{ WC_STATIC,	IDS_TDC_FIELD_COST, SS_CENTERIMAGE, 0, 119,184,37,8, IDC_COSTLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP, 0, 159,182,65,13, IDC_COST },
	{ WC_STATIC,	IDS_TDC_FIELD_DEPENDENCY, SS_CENTERIMAGE, 0, 119,184,37,8, IDC_DEPENDSLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP, 0, 159,182,65,13, IDC_DEPENDS },
	{ WC_STATIC,	IDS_TDC_FIELD_VERSION, SS_CENTERIMAGE, 0, 119,184,37,8, IDC_VERSIONLABEL },
	{ WC_COMBOBOX,	0, CBS_SORT | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_AUTOHSCROLL | WS_TABSTOP, 0, 45,128,65,200, IDC_VERSION },
	{ WC_STATIC,	IDS_TDC_FIELD_FILEREF, SS_CENTERIMAGE, 0, 119,184,37,8, IDC_FILEPATHLABEL },
	{ WC_EDIT,		0, ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP, 0, 159,182,65,13, IDC_FILEPATH },
	{ WC_STATIC,	IDS_TDC_FIELD_COMMENTS, 0, 0, 1,201,34,8, IDC_COMMENTSLABEL },
	{ WC_STATIC,	0, SS_ETCHEDHORZ, 0, 35, 204, 187, 2, IDC_SPLITTER },
};

int NUM_CTRLS = sizeof(TDCCONTROLS) / sizeof(TDCCONTROL);

static TDCCOLUMN COLUMNS[] = 
{
	{ TDCC_ID, IDS_TDC_COLUMN_ID, TDC_SORTBYID, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_PRIORITY, IDS_TDC_COLUMN_PRIORITY, TDC_SORTBYPRIORITY, DT_CENTER, TRUE, NULL, FALSE, FALSE },
	{ TDCC_RISK, 0x4d, TDC_SORTBYRISK, DT_CENTER, TRUE, "Wingdings", TRUE, TRUE },
	{ TDCC_PERCENT, IDS_TDC_COLUMN_PERCENT, TDC_SORTBYPERCENT, DT_CENTER, TRUE, NULL, FALSE, FALSE },
	{ TDCC_TIMEEST, IDS_TDC_COLUMN_TIMEEST, TDC_SORTBYTIMEEST, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_TIMESPENT, IDS_TDC_COLUMN_TIMESPENT, TDC_SORTBYTIMESPENT, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_TRACKTIME, 0xb9, (TDC_SORTBY)-1, DT_CENTER, FALSE, "Wingdings", TRUE, TRUE },
	{ TDCC_CREATIONDATE, IDS_TDC_COLUMN_CREATEDATE, TDC_SORTBYCREATIONDATE, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_CREATEDBY, IDS_TDC_COLUMN_CREATEDBY, TDC_SORTBYCREATEDBY, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_LASTMOD, IDS_TDC_COLUMN_MODIFYDATE, TDC_SORTBYLASTMOD, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_STARTDATE, IDS_TDC_COLUMN_STARTDATE, TDC_SORTBYSTARTDATE, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_DUEDATE, IDS_TDC_COLUMN_DUEDATE, TDC_SORTBYDUEDATE, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_DONEDATE, IDS_TDC_COLUMN_DONEDATE, TDC_SORTBYDONEDATE, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_RECURRENCE, IDS_TDC_COLUMN_RECURRENCE, TDC_SORTBYRECURRENCE, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_ALLOCTO, IDS_TDC_COLUMN_ALLOCTO, TDC_SORTBYALLOCTO, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_ALLOCBY, IDS_TDC_COLUMN_ALLOCBY, TDC_SORTBYALLOCBY, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_STATUS, IDS_TDC_COLUMN_STATUS, TDC_SORTBYSTATUS, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_CATEGORY, IDS_TDC_COLUMN_CATEGORY, TDC_SORTBYCATEGORY, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_EXTERNALID, IDS_TDC_COLUMN_EXTID, TDC_SORTBYEXTERNALID, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_COST, IDS_TDC_COLUMN_COST, TDC_SORTBYCOST, DT_RIGHT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_DEPENDENCY, 0xc3, (TDC_SORTBY)-1, DT_LEFT, FALSE, "Wingdings", TRUE, TRUE },
	{ TDCC_VERSION, IDS_TDC_COLUMN_VERSION, TDC_SORTBYVERSION, DT_LEFT, TRUE, NULL, FALSE, TRUE },
	{ TDCC_FILEREF, 0x3c, (TDC_SORTBY)-1, DT_LEFT, FALSE, "Wingdings", TRUE, TRUE },
	{ TDCC_FLAG, 0x4f, TDC_SORTBYFLAG, DT_CENTER, TRUE, "Wingdings", TRUE, FALSE },
	{ TDCC_DONE, 0x61, TDC_SORTBYDONE, DT_CENTER, TRUE, "Marlett", TRUE, FALSE },
	
	// special client column
	{ TDCC_CLIENT, IDS_TDC_COLUMN_TASK, TDC_SORTBYNAME, DT_LEFT, TRUE, NULL, FALSE, TRUE },
};

int NUM_COLUMNS = sizeof(COLUMNS) / sizeof(TDCCOLUMN);

static CTRLITEM CTRLITEMS[] = 
{
	{ IDC_PRIORITY,		IDC_PRIORITYLABEL,	TDCC_PRIORITY },
	{ IDC_RISK,			IDC_RISKLABEL,		TDCC_RISK },
	{ IDC_PERCENT,		IDC_PERCENTLABEL,	TDCC_PERCENT },
	{ IDC_TIMEEST,		IDC_TIMEESTLABEL,	TDCC_TIMEEST },
	{ IDC_TIMESPENT,	IDC_TIMESPENTLABEL,	TDCC_TIMESPENT },
	{ IDC_STARTDATE,	IDC_STARTLABEL,		TDCC_STARTDATE },
	{ IDC_DUEDATE,		IDC_DUELABEL,		TDCC_DUEDATE },
	{ IDC_DONEDATE,		IDC_DONELABEL,		TDCC_DONEDATE },
	{ IDC_RECURRENCE,	IDC_RECURRENCELABEL,TDCC_RECURRENCE },
	{ IDC_ALLOCTO,		IDC_ALLOCTOLABEL,	TDCC_ALLOCTO },
	{ IDC_ALLOCBY,		IDC_ALLOCBYLABEL,	TDCC_ALLOCBY },
	{ IDC_STATUS,		IDC_STATUSLABEL,	TDCC_STATUS },
	{ IDC_CATEGORY,		IDC_CATEGORYLABEL,	TDCC_CATEGORY },
	{ IDC_EXTERNALID,	IDC_EXTERNALIDLABEL,TDCC_EXTERNALID },
	{ IDC_COST,			IDC_COSTLABEL,		TDCC_COST },
	{ IDC_DEPENDS,		IDC_DEPENDSLABEL,	TDCC_DEPENDENCY },
	{ IDC_VERSION,		IDC_VERSIONLABEL,	TDCC_VERSION },
	{ IDC_FILEPATH,		IDC_FILEPATHLABEL,	TDCC_FILEREF },
};

const int NUM_CTRLITEMS = sizeof(CTRLITEMS) / sizeof(CTRLITEM);

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrl dialog

enum 
{
	ID_TIMEEDITBTN = 0xfff0,
	ID_EXTERNALID_LINK,
	ID_DEPENDS_LINK,
};

const int CTRLHEIGHT = 14; // dlu
const int CTRLVSPACING = 5; // dlu
const int CTRLHSPACING = 4; // dlu
const int CTRLSTARTOFFSET = 45; // dlu
const int CTRLENDOFFSET = 110; // dlu
const unsigned short FILEFORMAT = 8; // increment this when format changes
const unsigned short SPLITSIZE = 6; 
const unsigned short MINCOMMENTSIZE = 0;
const unsigned short DEFCOMMENTSIZE = 260;
const unsigned short MINNONCOMMENTHEIGHT = 250; // what's above the comment section
const unsigned short MINNONCOMMENTWIDTH = 350; // what's to the left of the comment section
const COLORREF BLACK = RGB(0, 0, 0);
const COLORREF WHITE = RGB(240, 240, 240);
const COLORREF NOCOLOR = ((COLORREF)-1);
const UINT CLOCKBTN = 0xb9;
const UINT LINKBTN = 0x24;
const COLORREF COMMENTSCOLOR = RGB(98, 98, 98);
const UINT DAY_IN_SECS = 24 * 60 * 60;

enum 
{
	WM_TDC_RESTOREFOCUSEDITEM = (WM_APP + 1),
	WM_TDC_REFRESHPERCENTSPINVISIBILITY,
};

enum { TIMER_TRACK = 1, TIMER_MIDNIGHT };

const int TIMETRACKPERIOD = 10000; // 10 secs in millesecs
const int MIDNIGHTPERIOD = 60000; // 1 minute in millesecs

#ifdef _DEBUG
const double TICKS2HOURS = 10.0 / (1000 * 3600);
#else
const double TICKS2HOURS = 1.0 / (1000 * 3600);
#endif


// private class to help optimize xml parsing
class CXmlParseController : public IXmlParse
{
public:
	CXmlParseController(LPCTSTR szItem) : m_sItem(szItem) {}
	
	virtual BOOL Continue(LPCTSTR szItem, LPCTSTR /*szValue*/) const
	{
		return (m_sItem.CompareNoCase(szItem) != 0);
	}
	
protected:
	CString m_sItem;
};

//////////////////////////////////////////////////////////////////////////////

// static variables
TDLCLIPBOARD CToDoCtrl::s_clipboard;
int CToDoCtrl::s_nCommentsSize = 0;
TODOITEM CToDoCtrl::s_tdDefault;

CToDoCtrl::CToDoCtrl(CContentMgr& mgr, int nDefaultContent) : 
						m_bModified(FALSE), 
						m_dwNextUniqueID(1), 
						m_nPriority(-1), 
						m_nSortBy(TDC_SORTUNDEF),
						m_bSortAscending(-1),
						m_nFileVersion(0),
						m_bArchive(FALSE),
						m_dTimeEstimate(0),
						m_dTimeSpent(0),
						m_eFileRef(FES_COMBOSTYLEBTN | FES_GOBUTTON | FES_ALLOWURL),
						m_nFileFormat(FILEFORMAT),
						m_bModSinceLastSort(FALSE),
						m_dwVisibleColumns(0),
						m_crTaskDone(RGB(128, 128, 128)),
						m_bCheckedOut(FALSE),
						m_nCommentsSize(DEFCOMMENTSIZE),
						m_bSplitting(FALSE),
						m_data(m_tree, m_aStyles),
						m_selection(m_tree),
						m_wKeyPress(FALSE),
						m_dwTimeTrackTaskID(0),
						m_treeDragDrop(Selection(), m_tree, TRUE, TRUE),
						m_htiLastAdded(NULL),
						m_cbAllocBy(TRUE, FALSE),
						m_cbAllocTo(TRUE),
						m_cbCategory(TRUE),
						m_cbStatus(TRUE, FALSE),
						m_cbVersion(TRUE, FALSE),
						m_fontTree(NULL),
						m_fontDone(NULL),
						m_fontComments(NULL),
						m_nMaxInfotipCommentsLength(-1),
						m_ctrlComments(NULL),
						m_bSourceControlled(FALSE),
						m_mgrContent(mgr),
						m_nDefaultContent(nDefaultContent),
						m_hilDone(NULL),
						m_dLogTime(0),
						m_fontBold(NULL),
						m_htiEditing(NULL),
						m_crDue(NOCOLOR), 
						m_crDueToday(NOCOLOR)
{
	SetBordersDLU(0);
	
	// add controls
	for (int nCtrl = 0; nCtrl < NUM_CTRLS; nCtrl++)
	{
		const TDCCONTROL& ctrl = TDCCONTROLS[nCtrl];

		AddRCControl("CONTROL", ctrl.szClass, CEnString(ctrl.nIDCaption), 
					ctrl.dwStyle, ctrl.dwExStyle,
					ctrl.nX, ctrl.nY, ctrl.nCx, ctrl.nCy, ctrl.nID);
	}
	
	// machine name
	DWORD LEN = MAX_COMPUTERNAME_LENGTH + 1;
	
	::GetComputerName(m_sMachineName.GetBuffer(LEN), &LEN);
	m_sMachineName.ReleaseBuffer();
	
    // init styles array
    // MFC sets all elements to zero
    m_aStyles.SetSize(TDCS_LAST);
	m_aStyles.SetAt(TDCS_RIGHTALIGNLABELS, 1);
	
	// set up number masks
	m_ePercentDone.SetMask("0123456789");
	m_eCost.SetMask(".0123456789", ME_LOCALIZEDECIMAL);
	
	// add chess clock button to 'time spent'
	m_eTimeSpent.InsertButton(0, ID_TIMEEDITBTN, CLOCKBTN, 
								CEnString(IDS_TDC_STARTSTOPCLOCK), 
								DEF_BTNWIDTH + 4, "Wingdings");

	// add link button to externalID
/*	m_eExternalID.AddButton(ID_EXTERNALID_LINK, LINKBTN, 
							CEnString(IDS_TDC_EXTIDLINK_TIP),
							CALC_BTNWIDTH, "Wingdings");
*/
	// add link button to dependency
	m_eDependency.AddButton(ID_DEPENDS_LINK, LINKBTN, 
							CEnString(IDS_TDC_DEPENDSLINK_TIP),
							CALC_BTNWIDTH, "Wingdings");

	ZeroMemory(&m_idCommentsType, sizeof(GUID));

	// set default task creation date to zero so that new tasks
	// always get the current date
	s_tdDefault.dateCreated.m_dt = 0.0;
}

CToDoCtrl::~CToDoCtrl()
{
	EndTimeTracking();
	
	if (m_fontDone)
		::DeleteObject(m_fontDone);
	
	m_brDue.DeleteObject();
	m_brDueToday.DeleteObject();
}

void CToDoCtrl::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_TASKLIST, m_tree);
	DDX_Text(pDX, IDC_PROJECTNAME, m_sProjectName);
	DDX_Control(pDX, IDC_ALLOCTO, m_cbAllocTo);
	DDX_AutoCBString(pDX, IDC_ALLOCBY, m_sAllocBy);
	DDX_Control(pDX, IDC_ALLOCBY, m_cbAllocBy);
	DDX_AutoCBString(pDX, IDC_STATUS, m_sStatus);
	DDX_Control(pDX, IDC_STATUS, m_cbStatus);
	DDX_Control(pDX, IDC_CATEGORY, m_cbCategory);
	DDX_Text(pDX, IDC_TIMEEST, m_dTimeEstimate);
	DDX_Text(pDX, IDC_TIMESPENT, m_dTimeSpent);
	DDX_Text(pDX, IDC_EXTERNALID, m_sExternalID);
	DDX_Text(pDX, IDC_DEPENDS, m_sDepends);
	DDX_Control(pDX, IDC_TIMEEST, m_eTimeEstimate);
	DDX_Control(pDX, IDC_TIMESPENT, m_eTimeSpent);
	DDX_Control(pDX, IDC_DUEDATE, m_dateDue);
	DDX_Control(pDX, IDC_DONEDATE, m_dateDone);
	DDX_Control(pDX, IDC_STARTDATE, m_dateStart);
	DDX_Control(pDX, IDC_PRIORITY, m_cbPriority);
	DDX_CBPriority(pDX, IDC_PRIORITY, m_nPriority);
	DDX_Control(pDX, IDC_RISK, m_cbRisk);
	DDX_CBRisk(pDX, IDC_RISK, m_nRisk);
	DDX_Control(pDX, IDC_PERCENT, m_ePercentDone);
	DDX_Control(pDX, IDC_COST, m_eCost);
	DDX_Control(pDX, IDC_DEPENDS, m_eDependency);
	DDX_Control(pDX, IDC_PERCENTSPIN, m_spinPercent);
	DDX_Control(pDX, IDC_FILEPATH, m_eFileRef);
	DDX_Control(pDX, IDC_EXTERNALID, m_eExternalID);
	DDX_Text(pDX, IDC_FILEPATH, m_sFileRefPath);
	DDX_Text(pDX, IDC_COST, m_dCost);
	DDX_Control(pDX, IDC_RECURRENCE, m_eRecurrence);
	DDX_Control(pDX, IDC_VERSION, m_cbVersion);
	DDX_AutoCBString(pDX, IDC_VERSION, m_sVersion);
	
	// custom
	if (pDX->m_bSaveAndValidate)
	{
		CString sPercent;
		GetDlgItem(IDC_PERCENT)->GetWindowText(sPercent);
		
		m_nPercentDone = max(0, atoi(sPercent));
		m_nPercentDone = min(100, m_nPercentDone);

//		m_ctrlComments.GetTextContent(m_sTextComments);
//		m_ctrlComments.GetContent(m_CustomComments);
		
		m_nTimeEstUnits = m_eTimeEstimate.GetUnits();
		m_nTimeSpentUnits = m_eTimeSpent.GetUnits();

		m_cbCategory.GetChecked(m_aCategory);
		m_cbAllocTo.GetChecked(m_aAllocTo);
		m_eRecurrence.GetRecurrenceOptions(m_tRecurrence);
	}
	else
	{
		m_spinPercent.SetPos(m_nPercentDone);
		
		// if we can't set the custom comments or
		// there are no custom comments then try setting the text comments
//		if (!m_ctrlComments.SetContent(m_CustomComments) || m_CustomComments.IsEmpty())
//			m_ctrlComments.SetTextContent(m_sTextComments);
		
		m_eTimeEstimate.SetUnits(m_nTimeEstUnits);
		m_eTimeSpent.SetUnits(m_nTimeSpentUnits);

		m_cbCategory.SetChecked(m_aCategory);
		m_cbAllocTo.SetChecked(m_aAllocTo);

		m_eRecurrence.SetRecurrenceOptions(m_tRecurrence);
	}
}

void CToDoCtrl::UpdateComments(BOOL bSaveAndValidate)
{
	if (bSaveAndValidate)
	{
		m_ctrlComments.GetTextContent(m_sTextComments);
		m_ctrlComments.GetContent(m_CustomComments);
	}
	else
	{
		// if we can't set the custom comments or
		// there are no custom comments then try setting the text comments
		if (!m_ctrlComments.SetContent(m_CustomComments) || m_CustomComments.IsEmpty())
			m_ctrlComments.SetTextContent(m_sTextComments);
	}
}

BEGIN_MESSAGE_MAP(CToDoCtrl, CRuntimeDlg)
//{{AFX_MSG_MAP(CToDoCtrl)
//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CAPTURECHANGED()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_NOTIFY(NM_SETFOCUS, IDC_TASKLIST, OnTreeChangeFocus)
	ON_NOTIFY(NM_KILLFOCUS, IDC_TASKLIST, OnTreeChangeFocus)
	ON_NOTIFY(NM_CLICK, IDC_TASKLIST, OnTreeClick)
	ON_NOTIFY(NM_DBLCLK, IDC_TASKLIST, OnTreeDblClk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TASKLIST, OnTreeCustomDraw)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TASKLIST, OnTreeGetDispInfo)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TASKLIST, OnTreeKeyDown)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TASKLIST, OnTreeEndlabeledit)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TASKLIST, OnTreeBeginlabeledit)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TASKLIST, OnTreeSelChanged)
	ON_NOTIFY(TVN_GETINFOTIP, IDC_TASKLIST, OnTreeGetInfoTip)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DONEDATE, OnTaskDatechange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DUEDATE, OnTaskDatechange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTDATE, OnTaskDatechange)
	ON_CBN_SELCHANGE(IDC_PRIORITY, OnChangePriority)
	ON_CBN_SELCHANGE(IDC_RISK, OnChangeRisk)
	ON_EN_CHANGE(IDC_TIMEEST, OnChangeTimeEstimate)
	ON_EN_CHANGE(IDC_TIMESPENT, OnChangeTimeSpent)
	ON_EN_CHANGE(IDC_PROJECTNAME, OnChangeProjectName)
	ON_EN_CHANGE(IDC_EXTERNALID, OnChangeExternalID)
	ON_EN_CHANGE(IDC_COST, OnChangeCost)
	ON_EN_CHANGE(IDC_DEPENDS, OnChangeDependency)
	ON_EN_CHANGE(IDC_RECURRENCE, OnChangeRecurrence)
	ON_REGISTERED_MESSAGE(WM_TDCN_COMMENTSCHANGE, OnChangeComments)
	ON_REGISTERED_MESSAGE(WM_NCG_WANTREDRAW, OnGutterWantRedraw)
	ON_REGISTERED_MESSAGE(WM_NCG_DRAWITEM, OnGutterDrawItem)
	ON_REGISTERED_MESSAGE(WM_NCG_POSTDRAWITEM, OnGutterPostDrawItem)
	ON_REGISTERED_MESSAGE(WM_NCG_RECALCCOLWIDTH, OnGutterRecalcColWidth)
	ON_REGISTERED_MESSAGE(WM_NCG_NOTIFYHEADERCLICK, OnGutterNotifyHeaderClick)
	ON_REGISTERED_MESSAGE(WM_NCG_WIDTHCHANGE, OnGutterWidthChange)
	ON_REGISTERED_MESSAGE(WM_NCG_GETCURSOR, OnGutterGetCursor)
	ON_REGISTERED_MESSAGE(WM_NCG_NOTIFYITEMCLICK, OnGutterNotifyItemClick)
	ON_REGISTERED_MESSAGE(WM_NCG_ISITEMSELECTED, OnGutterIsItemSelected)
	ON_REGISTERED_MESSAGE(WM_NCG_GETSELECTEDCOUNT, OnGutterGetSelectedCount)
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnEEBtnClick)
	ON_CBN_EDITCHANGE(IDC_ALLOCTO, OnEditChangeAllocTo)
	ON_CBN_SELENDOK(IDC_ALLOCTO, OnSelChangeAllocTo)
	ON_CBN_EDITCHANGE(IDC_ALLOCBY, OnEditChangeAllocBy)
	ON_CBN_SELENDOK(IDC_ALLOCBY, OnSelChangeAllocBy)
	ON_CBN_EDITCHANGE(IDC_STATUS, OnEditChangeStatus)
	ON_CBN_SELENDOK(IDC_STATUS, OnSelChangeStatus)
	ON_CBN_EDITCHANGE(IDC_VERSION, OnEditChangeVersion)
	ON_CBN_SELENDOK(IDC_VERSION, OnSelChangeVersion)
	ON_CBN_EDITCHANGE(IDC_CATEGORY, OnEditChangeCategory)
	ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelChangeCategory)
	ON_CBN_SELENDOK(IDC_CATEGORY, OnSelChangeCategory)
	ON_EN_CHANGE(IDC_PERCENT, OnChangePercent)
	ON_EN_CHANGE(IDC_FILEPATH, OnChangeFileRefPath)
	ON_REGISTERED_MESSAGE(WM_FE_GETFILEICON, OnFileEditWantIcon)
	ON_REGISTERED_MESSAGE(WM_FE_DISPLAYFILE, OnFileEditDisplayFile)
	ON_REGISTERED_MESSAGE(WM_TLDT_DROPFILE, OnDropFileRef)
	ON_REGISTERED_MESSAGE(WM_TEN_UNITSCHANGE, OnTimeUnitsChange)
	ON_REGISTERED_MESSAGE(WM_DD_DRAGABORT, OnTreeDragDrop)
	ON_REGISTERED_MESSAGE(WM_DD_DRAGDROP, OnTreeDragDrop)
	ON_REGISTERED_MESSAGE(WM_DD_DRAGENTER, OnTreeDragDrop)
	ON_REGISTERED_MESSAGE(WM_DD_DRAGOVER, OnTreeDragDrop)
	ON_REGISTERED_MESSAGE(WM_DD_PREDRAGMOVE, OnTreeDragDrop)
	ON_MESSAGE(WM_TDC_RESTOREFOCUSEDITEM, OnTreeRestoreFocusedItem)
	ON_REGISTERED_MESSAGE(WM_TDCM_HASCLIPBOARD, OnTDCHasClipboard)
	ON_REGISTERED_MESSAGE(WM_TDCM_GETCLIPBOARD, OnTDCGetClipboard)
	ON_REGISTERED_MESSAGE(WM_TDCM_TASKLINK, OnTDCDoTaskLink)
	ON_REGISTERED_MESSAGE(WM_ACB_ITEMADDED, OnAutoComboChange)
	ON_REGISTERED_MESSAGE(WM_ACB_ITEMDELETED, OnAutoComboChange)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_TDC_REFRESHPERCENTSPINVISIBILITY, OnRefreshPercentSpinVisibility)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrl message handlers

TDC_FILEFMT CToDoCtrl::CompareFileFormat() const
{
    if (m_nFileFormat == FILEFORMAT)
        return TDCFF_SAME;
	
    else if (m_nFileFormat > FILEFORMAT)
        return TDCFF_NEWER;
	
    else
        return TDCFF_OLDER;
}

void CToDoCtrl::EnableEncryption(BOOL bEnable)
{
	if (!bEnable && !m_sPassword.IsEmpty())
	{
		m_sPassword.Empty();
		SetModified(TRUE, TDCA_NONE);
	}
	else if (bEnable && m_sPassword.IsEmpty())
	{
		CEnString sExplanation(IDS_ENABLEENCRYPTION);

		if (CPasswordDialog::RetrievePassword(TRUE, m_sPassword, sExplanation))
			SetModified(TRUE, TDCA_NONE);
	}
}

BOOL CToDoCtrl::VerifyPassword(LPCTSTR szExplanation) const
{
	if (!IsEncrypted())
		return TRUE; // no password to check

	return CPasswordDialog::VerifyPassword(m_sPassword, szExplanation);
}

BOOL CToDoCtrl::CanEncrypt()
{
	return CTaskFile::CanEncrypt();
}

BOOL CToDoCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bVisible)
{
	DWORD dwStyle = WS_CHILD | (bVisible ? WS_VISIBLE : 0);
	
	return CRuntimeDlg::Create(NULL, dwStyle, WS_EX_NOPARENTNOTIFY, rect, pParentWnd, nID);
}

BOOL CToDoCtrl::OnInitDialog() 
{
	CRuntimeDlg::OnInitDialog();

	VERIFY (CreateContentControl());

	// percent spin
	m_spinPercent.SetRange(0, 100);
	m_spinPercent.SetBuddy(GetDlgItem(IDC_PERCENT));
	
	UDACCEL uda = { 0, 5 };
	m_spinPercent.SetAccel(1, &uda);
	
	// extra gutter columns
	for (int nCol = 0; nCol < NUM_COLUMNS; nCol++)
	{
		TDCCOLUMN& tdcc = COLUMNS[nCol];
		
		if (tdcc.nColID != TDCC_CLIENT)
		{
			m_tree.AddGutterColumn(tdcc.nColID, NULL, 0, tdcc.nAlignment);
			
			if (tdcc.bSymbolFont)
				m_tree.SetGutterColumnHeaderTitle(tdcc.nColID, (UINT)tdcc.nIDName, tdcc.szFont);
			else
				m_tree.SetGutterColumnHeaderTitle(tdcc.nColID, CEnString(tdcc.nIDName), tdcc.szFont);
		}
		else
			m_tree.SetGutterColumnHeaderTitle(tdcc.nColID, CEnString(tdcc.nIDName));
	}
	
	// set name of pos column
	m_tree.SetGutterColumnHeaderTitle(OTC_POSCOLUMNID, CEnString(IDS_TDC_COLUMN_POS));
	
	UpdateColumnHeaderClicking();
	
	// init dates
	m_dateStart.SendMessage(DTM_SETSYSTEMTIME, GDT_NONE, 0);
	m_dateDue.SendMessage(DTM_SETSYSTEMTIME, GDT_NONE, 0);
	m_dateDone.SendMessage(DTM_SETSYSTEMTIME, GDT_NONE, 0);
	
	m_dtTree.Register(&m_tree, this);
	m_dtFileRef.Register(&m_eFileRef, this); 

	// custom font
	if (m_fontTree)
	{
		m_tree.SendMessage(WM_SETFONT, (WPARAM)m_fontTree, TRUE);
		
		if (HasStyle(TDCS_COMMENTSUSETREEFONT))
			m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)m_fontTree, TRUE);
		else if (m_fontComments)
			m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)m_fontComments, TRUE);
	}

	InitEditPrompts();
	
	// tree drag drop
	m_treeDragDrop.Initialize(this);
	
	// enabled states
	UpdateControls();
	SetLabelAlignment(HasStyle(TDCS_RIGHTALIGNLABELS) ? SS_RIGHT : SS_LEFT);
	m_tree.SetFocus();
	
	LoadSplitPos();

	// notify parent that we have been created
	CWnd* pParent = GetParent();

	if (pParent)
		pParent->SendMessage(WM_PARENTNOTIFY, 
						MAKEWPARAM(WM_CREATE, GetDlgCtrlID()), (LPARAM)GetSafeHwnd());

	// and start the track timer
	// which runs persistently
	SetTimer(TIMER_TRACK, TIMETRACKPERIOD, NULL);

	// and the time which checks for midnight (day changeover)
	SetTimer(TIMER_MIDNIGHT, MIDNIGHTPERIOD, NULL);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CToDoCtrl::GetCustomCommentsType(GUID& idType) const
{
   return m_ctrlComments.GetTypeID(idType);
}

BOOL CToDoCtrl::CreateContentControl()
{
	CRect rect(0, 211, 171, 238); // in DLU
	CDlgUnits(*this).ToPixels(rect);

	if (m_ctrlComments.GetSafeHwnd())
	{
		::GetWindowRect(m_ctrlComments, &rect);
		ScreenToClient(rect);
	}

	BOOL bSuccess = FALSE;

	if (!Misc::GuidIsNull(m_idCommentsType))
	{
		bSuccess = m_mgrContent.CreateContentControl(m_idCommentsType, m_ctrlComments, IDC_COMMENTS, 
													WS_TABSTOP | WS_CHILD, 
													WS_EX_CLIENTEDGE, rect, *this);
	}

	if (!bSuccess)
	{
		bSuccess = m_mgrContent.CreateContentControl(m_nDefaultContent, m_ctrlComments, 
													IDC_COMMENTS, WS_TABSTOP | WS_CHILD, 
													WS_EX_CLIENTEDGE, rect, *this);

		// update comments type so long as it was previously null
		if (Misc::GuidIsNull(m_idCommentsType))
			m_mgrContent.GetContentTypeID(m_nDefaultContent, m_idCommentsType);
	}

	// make sure its in the right pos in the tab order
	::SetWindowPos(m_ctrlComments, ::GetDlgItem(*this, IDC_COMMENTSLABEL), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return bSuccess; 
}

void CToDoCtrl::InitEditPrompts()
{
	m_mgrPrompts.SetEditPrompt(IDC_PROJECTNAME, *this, IDS_TDC_EDITPROMPT_PROJECT);
	m_mgrPrompts.SetEditPrompt(m_eFileRef, IDS_TDC_EDITPROMPT_FILEREF);
	m_mgrPrompts.SetEditPrompt(m_eExternalID, IDS_TDC_EDITPROMPT_EXTID);
	m_mgrPrompts.SetEditPrompt(m_eDependency, IDS_TDC_EDITPROMPT_DEPENDS); 

	m_mgrPrompts.SetComboEditPrompt(m_cbAllocBy, IDS_TDC_EDITPROMPT_NAME);
	m_mgrPrompts.SetComboEditPrompt(m_cbAllocTo.GetSafeHwnd(), IDS_TDC_EDITPROMPT_NAME);
	m_mgrPrompts.SetComboEditPrompt(m_cbCategory.GetSafeHwnd(), IDS_TDC_EDITPROMPT_CATEGORY);
	m_mgrPrompts.SetComboEditPrompt(m_cbStatus, IDS_TDC_EDITPROMPT_STATUS);
	m_mgrPrompts.SetComboEditPrompt(m_cbVersion, IDS_TDC_EDITPROMPT_VER);

	m_mgrPrompts.SetEditPrompt(m_ctrlComments, IDS_TDC_EDITPROMPT_COMMENTS);
	m_mgrPrompts.SetPrompt(m_tree, IDS_TDC_TASKLISTPROMPT, TVM_GETCOUNT);
}

void CToDoCtrl::SetTaskCompletedColor(COLORREF color)
{
	if (color != m_crTaskDone)
	{
		m_crTaskDone = color;
		
		if (m_tree.GetSafeHwnd())
			m_tree.Invalidate(FALSE);
	}
}

BOOL CToDoCtrl::SetTreeFont(HFONT hFont)
{
	m_fontTree = hFont;

	if (m_tree.GetSafeHwnd())
	{
		if (!hFont) // set to our font
		{
			// for some reason i can not yet explain, our font
			// is not correctly set so we use our parent's font instead
			// hFont = (HFONT)SendMessage(WM_GETFONT);
			hFont = (HFONT)GetParent()->SendMessage(WM_GETFONT);
		}

		HFONT hTreeFont = (HFONT)m_tree.SendMessage(WM_GETFONT);
		BOOL bChange = (hFont != hTreeFont || !Misc::SameFontNameSize(hFont, hTreeFont));

		if (bChange)
			m_tree.SendMessage(WM_SETFONT, (WPARAM)hFont, TRUE);

		// done font
		if (bChange || m_fontDone == NULL)
		{
			::DeleteObject(m_fontDone);

			if (HasStyle(TDCS_STRIKETHOUGHDONETASKS))
			{
				LOGFONT lf;
				::GetObject(hFont, sizeof(lf), &lf);
				lf.lfStrikeOut = TRUE;
				m_fontDone = CreateFontIndirect(&lf);
			}
			else
				m_fontDone = NULL;

			// bold tree font
			LOGFONT lf;
			::GetObject(hFont, sizeof(lf), &lf);
			lf.lfWeight = FW_BOLD;
			::DeleteObject(m_fontBold);

			m_fontBold = CreateFontIndirect(&lf);
		} 

		// comments font
		if (HasStyle(TDCS_COMMENTSUSETREEFONT))
			m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)hFont, TRUE);
	}
	return TRUE;
}

BOOL CToDoCtrl::SetCommentsFont(HFONT hFont)
{
	m_fontComments = hFont;
	
	if (m_ctrlComments.GetSafeHwnd())
	{
		if (!HasStyle(TDCS_COMMENTSUSETREEFONT))
		{
			if (!hFont)
				hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

			m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)hFont, TRUE);

			// we've had some trouble with plugins using the
			// richedit control so after a font change we always
			// resend the content
			if (m_CustomComments.GetLength())
			{
				if (!m_ctrlComments.SetContent(m_CustomComments))
					m_ctrlComments.SetTextContent(m_sTextComments);
			}
			else
				m_ctrlComments.SetTextContent(m_sTextComments);
		}
	}
	
	return TRUE;
}

TDCCOLUMN* CToDoCtrl::GetColumn(UINT nColID)
{
	int nCol = NUM_COLUMNS;
	
	while (nCol--)
	{
		if (COLUMNS[nCol].nColID == (TDC_COLUMN)nColID)
			return &COLUMNS[nCol];
	}
	
	// else
	return NULL;
}

TDCCOLUMN* CToDoCtrl::GetColumn(TDC_SORTBY nSortBy)
{
	int nCol = NUM_COLUMNS;
	
	while (nCol--)
	{
		if (COLUMNS[nCol].nSortBy == nSortBy)
			return &COLUMNS[nCol];
	}
	
	// else
	return NULL;
}

void CToDoCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CRuntimeDlg::OnSize(nType, cx, cy);
	
	m_tree.TCH().EndLabelEdit(TRUE);
	Resize(cx, cy);
}

void CToDoCtrl::Resize(int cx, int cy)
{
	if (m_tree.GetSafeHwnd())
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

		BOOL bVertComments = HasStyle(TDCS_VERTCOMMENTS);
		
		// written to use DeferWindowPos()
		{
			CDeferWndMove dwm(30);
			
			CRect rComments;
			ReposComments(cx, cy, &dwm, rComments);
			
			// first count up the visible controls
			// so we can allocate the correct amount of space
			BOOL bAllVisible = !HasStyle(TDCS_SHOWCTRLSASCOLUMNS);
			int nVisibleCtrls = VisibleCtrlCount();
			
			// for converting dlus to pixels
			CDlgUnits dlu(*this);
			
			int nCols = 2; // default
			
			// figure out number of ctrl rows
			if (HasStyle(TDCS_AUTOREPOSCTRLS))
			{
				int nAvailWidth = bVertComments ? rComments.left : cx;
				int nCtrlWidth = dlu.ToPixelsX(CTRLENDOFFSET + CTRLHSPACING);
				nCols = max(2, nAvailWidth / nCtrlWidth);
			}
			int nRows = (nVisibleCtrls / nCols) + ((nVisibleCtrls % nCols) ? 1 : 0);

			// calc ctrl rect
			CRect rCtrls(0, 0, 0, 0);

			rCtrls.right = bVertComments ? rComments.left - SPLITSIZE : cx;
			rCtrls.bottom = bVertComments ? cy + CTRLVSPACING : rComments.top;
			rCtrls.top = rCtrls.bottom - nRows * dlu.ToPixelsY(CTRLHEIGHT + CTRLVSPACING);

			// resize tree now that we can calculate where its bottom is
			CRect rProject = ResizeCtrl(IDC_PROJECTNAME); // a get
			CRect rTree(0, rProject.bottom + 4, cx, cy);

			BOOL bMaximize = HasStyle(TDCS_MAXIMIZE);

			if (bMaximize || !HasStyle(TDCS_SHOWPROJECTNAME))
				rTree.top = rProject.top;
			
			if (bMaximize)
			{
				if (HasStyle(TDCS_SHOWCOMMENTSALWAYS))
				{
					if (bVertComments)
						rTree.right = rCtrls.right;
					else
						rTree.bottom = rComments.top - dlu.ToPixelsY(CTRLVSPACING);
				}
			}
			else
			{
				rTree.right = rCtrls.right;
				rTree.bottom = (rCtrls.top - dlu.ToPixelsY(CTRLVSPACING));
			}
			
			dwm.MoveWindow(GetDlgItem(IDC_TASKLIST), rTree);

			// project name
			dwm.ResizeCtrl(this, IDC_PROJECTNAME, rTree.right - rProject.right, 0);
			
			// now iterate the visible controls settings their positions dynamically
			CRect rItem(rCtrls);
			rItem.bottom = rItem.top + dlu.ToPixelsY(CTRLHEIGHT);
		
			int nCol = 0;
			int nRightEdge = bVertComments ? rCtrls.right : rCtrls.right - dlu.ToPixelsY(CTRLHSPACING);
			
			for (int nCtrl = 0; nCtrl < NUM_CTRLITEMS; nCtrl++)
			{
				if (!bAllVisible && !IsColumnShowing(CTRLITEMS[nCtrl].nCol))
					continue;
				
				if (nCol >= nCols)
				{
					nCol = 0;
					rItem.OffsetRect(0, dlu.ToPixelsY(CTRLHEIGHT + CTRLVSPACING));
				}
				
				ReposControl(nCtrl, &dwm, &dlu, nCol, rItem.top, nRightEdge);
				
				nCol++;
			}
		}
		
		UpdateWindow();
	}
}

int CToDoCtrl::GetMinNonCommentSize()
{
	if (HasStyle(TDCS_VERTCOMMENTS))
	{
		// two field widths
		CDlgUnits dlu(*this);
		return dlu.ToPixelsX(2 * (CTRLENDOFFSET + CTRLHSPACING));
	}
	else
		return MINNONCOMMENTHEIGHT;
}

void CToDoCtrl::ReposComments(int cx, int cy, CDeferWndMove* pDWM, CRect& rComments /*out*/) 
{
	// validate comments size but don't change it
	int nCommentSize = HasStyle(TDCS_SHAREDCOMMENTSHEIGHT) ? s_nCommentsSize : m_nCommentsSize;
	
	if (HasStyle(TDCS_VERTCOMMENTS))
	{
		nCommentSize = min(m_nCommentsSize, cx - GetMinNonCommentSize());
		rComments.top = 0;
		rComments.left = cx - nCommentSize;
	}
	else 
	{
		nCommentSize = min(m_nCommentsSize, cy - GetMinNonCommentSize());
		rComments.left = 0;
		rComments.top = cy - nCommentSize;
	}
	rComments.right = cx; // always
	rComments.bottom = cy; // always

	// move label and divider
	CSize sizeLabel = OffsetCtrl(IDC_COMMENTSLABEL).Size(); // get
	CRect rLabel(rComments.left, rComments.top, rComments.left + sizeLabel.cx, rComments.top + sizeLabel.cy);
	rLabel.OffsetRect(2, 0);
	pDWM->MoveWindow(GetDlgItem(IDC_COMMENTSLABEL), rLabel);

	int nDivTop = (rLabel.top + rLabel.bottom) / 2 - 1;
	CRect rDiv(rLabel.right, nDivTop, rComments.right, nDivTop + 2);
	pDWM->MoveWindow(GetDlgItem(IDC_SPLITTER), rDiv);
	
	// comments
	CRect rComField(rComments);
	rComField.top = rLabel.bottom + 4;

	pDWM->MoveWindow(GetDlgItem(IDC_COMMENTS), rComField);
}

void CToDoCtrl::ReposControl(int nCtrl, CDeferWndMove* pDWM, const CDlgUnits* pDLU, int nCol, 
							 int nTop, int nClientRight)
{
	// the bottom of the ctrl is defined by its existing height
	CRect rItem(0, nTop, 0, nTop + ResizeCtrl(CTRLITEMS[nCtrl].nCtrlID).Height());
	
	rItem.left = pDLU->ToPixelsX(nCol * (CTRLENDOFFSET + CTRLHSPACING));
	rItem.right = rItem.left + pDLU->ToPixelsX(CTRLENDOFFSET);
				
	// move label
	CRect rLabel(rItem);
	rLabel.right = rLabel.left + pDLU->ToPixelsX(CTRLSTARTOFFSET - CTRLHSPACING);
				
	pDWM->MoveWindow(GetDlgItem(CTRLITEMS[nCtrl].nLabelID), rLabel);
				
	// move control
	CRect rCtrl(rItem);
	rCtrl.left += pDLU->ToPixelsX(CTRLSTARTOFFSET);
				
	// some special cases
	switch (CTRLITEMS[nCtrl].nCtrlID)
	{
	case IDC_PERCENT:
		{
			CRect rSpin = OffsetCtrl(IDC_PERCENTSPIN); // gets current pos
			rSpin.OffsetRect(rCtrl.right - rSpin.right, 0);
			rSpin.top = rCtrl.top;
			rSpin.bottom = rCtrl.bottom;
			pDWM->MoveWindow(&m_spinPercent, rSpin);
			
			rCtrl.right = rSpin.left + 2;
		}
		break;
		
	case IDC_FILEPATH:
		{
			// file path control can take as much space as is left
			rCtrl.right = nClientRight;
		}
		break;
		
	case IDC_ALLOCTO:
	case IDC_ALLOCBY:
	case IDC_STATUS:
	case IDC_CATEGORY:
	case IDC_PRIORITY:
	case IDC_VERSION:
		rCtrl.bottom += 200; // combo box drop height
		break;
	}
				
	pDWM->MoveWindow(GetDlgItem(CTRLITEMS[nCtrl].nCtrlID), rCtrl);
}

int CToDoCtrl::VisibleCtrlCount()
{
	if (!HasStyle(TDCS_SHOWCTRLSASCOLUMNS))
		return NUM_CTRLITEMS;
	
	// else
	int nVisibleCtrls = 0;
	
	for (int nCtrl = 0; nCtrl < NUM_CTRLITEMS; nCtrl++)
	{
		if (IsColumnShowing(CTRLITEMS[nCtrl].nCol))
			nVisibleCtrls++;
	}
	
	return nVisibleCtrls;
}

int CToDoCtrl::GetMinWidth()
{
	int nMinWidth = CDlgUnits(*this).ToPixelsX(2 * (CTRLENDOFFSET + CTRLHSPACING) + 1);	
	
	return nMinWidth;
}

// wrapper for CTreeCtrl
BOOL CToDoCtrl::SetCheckImageList(HIMAGELIST hImageList)
{
	m_hilDone = hImageList;
	
	if (HasStyle(TDCS_TREECHECKBOXES))
		TreeView_SetImageList(m_tree, m_hilDone, TVSIL_STATE);
	
	return TRUE;
}

void CToDoCtrl::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTV = (LPNMTREEVIEW)pNMHDR;
	HTREEITEM hti = pNMTV->itemNew.hItem;
	
	BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
	BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
	
	// cursor handled here
	// <shift>+cursor handled here
	// <ctrl>+cursor handled in PreTranslateMessage
	// <ctrl>+<shift>+cursor handled in PreTranslateMessage
	if (m_wKeyPress)
	{
		switch (m_wKeyPress)
		{
		case VK_NEXT:  
		case VK_DOWN:
		case VK_UP:
		case VK_PRIOR: 
		case VK_RIGHT:
		case VK_LEFT:
		case VK_HOME:
		case VK_END:
			if (!bCtrl)
			{
				Selection().RemoveAll();
				
				if (bShift)
					Selection().AddItems(Selection().GetAnchor(), hti);
				else
				{
					Selection().SetAnchor(hti);
					Selection().AddItem(hti);
				}
			}
			break;

		default:
			// handle alphanum method of changing selection
			Selection().RemoveAll();
			Selection().SetAnchor(hti);
			Selection().AddItem(hti);
			break;
		}
		
		m_wKeyPress = 0;
	}
	
	UpdateControls(hti); // load newly selected item
	UpdateSelectedTaskPath();
	
	*pResult = 0;
}

void CToDoCtrl::UpdateSelectedTaskPath()
{
	// add the item path to the header
	if (HasStyle(TDCS_SHOWPATHINHEADER))
	{
		CEnString sHeader(IDS_TDC_TASK);
		
		if (GetSelectedCount() == 1)
		{
			CString sPath = GetSelectedTaskPath();
			
			if (!sPath.IsEmpty())
				sHeader.Format("%s [%s]", CEnString(IDS_TDC_TASK), sPath);
		}
		
		m_tree.SetGutterColumnHeaderTitle(TDCC_CLIENT, sHeader);
	}
}

CString CToDoCtrl::GetSelectedTaskPath()
{
	return m_tree.TCH().GetItemPath(GetSelectedItem(), 20);
}

void CToDoCtrl::UpdateControls(HTREEITEM hti)
{
	if (!hti)
		hti = GetSelectedItem();

	int nSelCount = GetSelectedCount();
	
	BOOL bMaximize = HasStyle(TDCS_MAXIMIZE);
	BOOL bEnable = (hti && !bMaximize);
	BOOL bIsParent = Selection().ItemsAreAllParents();
	BOOL bAveSubTaskCompletion = HasStyle(TDCS_AVERAGEPERCENTSUBCOMPLETION) && bIsParent;
	
	BOOL bShowCtrlsAsCols = HasStyle(TDCS_SHOWCTRLSASCOLUMNS);
	BOOL bReadOnly = IsReadOnly();
	
	BOOL bEditTime = !bIsParent || HasStyle(TDCS_ALLOWPARENTTIMETRACKING);
//	BOOL bEditCost = !bIsParent;
	BOOL bEditDue = (nSelCount >= 1);
//	BOOL bEditPriority = (nSelCount > 1 || !(HasStyle(TDCS_USEHIGHESTPRIORITY) && bIsParent));
	BOOL bIsClocking = m_data.IsTaskTimeTrackable(hti) && (GetSelectedTaskID() == m_dwTimeTrackTaskID);
	BOOL bEditPercent = !HasStyle(TDCS_AUTOCALCPERCENTDONE) && (nSelCount > 1 || !bAveSubTaskCompletion);

	// can only edit comments if content ctrl type matches current comments type
	// or content ctrl has no type. ie cannot edit custom comments using different
	// type than original type
	BOOL bEditComments = !m_ctrlComments.HasTypeID() || Misc::GuidIsNull(m_idCommentsType) ||
						m_ctrlComments.IsType(m_idCommentsType);
	
	if (hti)
	{
		m_sTextComments = GetSelectedTaskComments();
		m_CustomComments = GetSelectedTaskCustomComments();
		m_nPriority = (int)GetSelectedTaskPriority();
		m_nRisk = (int)GetSelectedTaskRisk();
		m_sAllocBy = GetSelectedTaskAllocBy();
		m_sStatus = GetSelectedTaskStatus();
		m_sFileRefPath = GetSelectedTaskFileRef();
		m_sExternalID = GetSelectedTaskExtID();
		m_sDepends = GetSelectedTaskDependency();
		m_sVersion = GetSelectedTaskVersion();
		
		// special cases
		GetSelectedTaskCategories(m_aCategory);
		GetSelectedTaskAllocTo(m_aAllocTo);

		if (bEditTime)
		{
			m_dTimeEstimate = GetSelectedTaskTimeEstimate(m_nTimeEstUnits);
			m_dTimeSpent = GetSelectedTaskTimeSpent(m_nTimeSpentUnits);
		}
		else
		{
			m_nTimeEstUnits = m_nTimeSpentUnits = s_tdDefault.nTimeEstUnits;
			m_dTimeEstimate = m_data.CalcTimeEstimate(hti, m_nTimeEstUnits);
			m_dTimeSpent = m_data.CalcTimeSpent(hti, m_nTimeEstUnits);
		}

//		if (bEditCost)
			m_dCost = GetSelectedTaskCost();
//		else
//			m_dCost = m_data.CalcCost(hti);
		
		// chess clock for time spent
		DWORD dwSelTaskID = GetSelectedTaskID();
		
		m_eTimeSpent.CheckButton(ID_TIMEEDITBTN, (dwSelTaskID == m_dwTimeTrackTaskID));
		m_eTimeSpent.EnableButton(ID_TIMEEDITBTN, bEditTime && bEnable && !bReadOnly && 
									nSelCount == 1 && m_data.IsTaskTimeTrackable(hti));

		// dependency link button
		m_eDependency.EnableButton(ID_DEPENDS_LINK, bEnable && !bReadOnly && !m_sDepends.IsEmpty());

		// percent done
		if (IsSelectedTaskDone())
			m_nPercentDone = 100;
		
		else if (bEditPercent)
			m_nPercentDone = GetSelectedTaskPercent();
		else
			m_nPercentDone = m_data.CalcPercentDone(hti);		
		
		// date controls need special handling too
		COleDateTime date;
		
		if (bEditDue)
			date = GetSelectedTaskDate(TDCD_DUE);
		else
			date = m_data.GetEarliestDueDate(hti, HasStyle(TDCS_USEEARLIESTDUEDATE));
				
		SetCtrlDate(m_dateDue, date);
		SetCtrlDate(m_dateDone, GetSelectedTaskDate(TDCD_DONE));
		SetCtrlDate(m_dateStart, GetSelectedTaskDate(TDCD_START));

		// recurrence
		GetSelectedTaskRecurrence(m_tRecurrence);
		m_eRecurrence.SetDefaultDate(date);
	}
	else // clear controls
	{
		m_sTextComments.Empty();
		m_CustomComments.Empty();
		m_nPriority = 0;
		m_nRisk = 0;
		m_sAllocBy.Empty();
		m_sStatus.Empty();
		m_sFileRefPath.Empty();
		m_sExternalID.Empty();
		m_sDepends.Empty();
		m_nPercentDone = 0;
		m_dTimeEstimate = m_dTimeSpent = 0;
		m_dCost = 0;
		m_tRecurrence = TDIRECURRENCE();
		m_sVersion.Empty();

		m_aCategory.RemoveAll();
		m_aAllocTo.RemoveAll();

		COleDateTime date;
		SetCtrlDate(m_dateDue, date);
		SetCtrlDate(m_dateDone, date);
		SetCtrlDate(m_dateStart, date);

		m_eTimeSpent.EnableButton(ID_TIMEEDITBTN, FALSE);
		m_eDependency.EnableButton(ID_DEPENDS_LINK, FALSE);
	}
		
	UpdateData(FALSE);
	UpdateComments(FALSE);
	
	// now enable/disable appropriate controls
	for (int nCtrl = 0; nCtrl < NUM_CTRLITEMS; nCtrl++)
	{
		CWnd* pCtrl = GetDlgItem(CTRLITEMS[nCtrl].nCtrlID);
		CWnd* pLabel = GetDlgItem(CTRLITEMS[nCtrl].nLabelID);
		
		if (!pCtrl || !pLabel)
			continue;
		
		BOOL bCtrlShowing = !bMaximize && (!bShowCtrlsAsCols || IsColumnShowing(CTRLITEMS[nCtrl].nCol));
		int nShowCtrl = bCtrlShowing ? SW_SHOW : SW_HIDE;
		
		// control state
		RT_CTRLSTATE nState = (!bEnable || !bCtrlShowing) ? RTCS_DISABLED : 
								(bReadOnly ? RTCS_READONLY : RTCS_ENABLED);
								
		// some additions and modifications
		switch (CTRLITEMS[nCtrl].nCtrlID)
		{
		case IDC_PERCENT:
			if (!bEditPercent && bEnable)
				nState = RTCS_READONLY;
			
			m_spinPercent.ShowWindow(nShowCtrl);
			SetControlState(m_spinPercent, nState);
			break;
			
		case IDC_TIMEEST:
			if (!bEditTime && bEnable)
				nState = RTCS_READONLY;
			break;
			
		case IDC_TIMESPENT:
			if ((!bEditTime || bIsClocking) && bEnable)
				nState = RTCS_READONLY;
			break;
			
// 		case IDC_COST:
// 			if (!bEditCost && bEnable)
// 				nState = RTCS_READONLY;
		}
		
		pCtrl->ShowWindow(nShowCtrl);
		pLabel->ShowWindow(nShowCtrl);
		
		SetControlState(*pLabel, nState);
		SetControlState(*pCtrl, nState);
	}
	
	// comments
	BOOL bCommentsAlways = HasStyle(TDCS_SHOWCOMMENTSALWAYS);
	RT_CTRLSTATE nState = ((!bCommentsAlways && bMaximize) || !hti) ? RTCS_DISABLED : 
							((bReadOnly || !bEditComments) ? RTCS_READONLY : RTCS_ENABLED);

	switch (nState)
	{
	case RTCS_DISABLED:
		SetControlState(this, IDC_COMMENTS, nState);
		break;

	case RTCS_ENABLED:
		SetControlState(this, IDC_COMMENTS, nState);
		m_ctrlComments.SetReadOnly(FALSE);
		break;

	case RTCS_READONLY:
		SetControlState(this, IDC_COMMENTS, RTCS_ENABLED);
		m_ctrlComments.SetReadOnly(TRUE);
		break;
	}

	SetControlState(this, IDC_COMMENTSLABEL, nState);
	ShowControls(IDC_COMMENTSLABEL, IDC_COMMENTS, (bCommentsAlways || !bMaximize));
	
	// project name
	BOOL bShowProjectName = !bMaximize && HasStyle(TDCS_SHOWPROJECTNAME);

	nState = !bShowProjectName ? RTCS_DISABLED : (bReadOnly ? RTCS_READONLY : RTCS_ENABLED);
	SetControlState(this, IDC_PROJECTLABEL, nState);
	SetControlState(this, IDC_PROJECTNAME, nState);
	ShowControls(this, IDC_PROJECTLABEL, IDC_PROJECTNAME, bShowProjectName);
	
	// tree
	m_tree.ModifyStyle(bReadOnly ? TVS_EDITLABELS : 0, bReadOnly ? 0 : TVS_EDITLABELS);
	m_treeDragDrop.EnableDragDrop(!bReadOnly);
}

void CToDoCtrl::SetCtrlDate(CDateTimeCtrl& ctrl, const COleDateTime& date)
{
	if (date.m_dt)
		ctrl.SetTime(date);
	else
	{
		ctrl.SetTime(COleDateTime::GetCurrentTime());
		ctrl.SendMessage(DTM_SETSYSTEMTIME, GDT_NONE, 0);
	}
}

void CToDoCtrl::SetLabelAlignment(int nStyle)
{
	// now enable/disable appropriate controls
	for (int nCtrl = 0; nCtrl < NUM_CTRLITEMS; nCtrl++)
	{
		CWnd* pLabel = GetDlgItem(CTRLITEMS[nCtrl].nLabelID);
		
		if (!pLabel)
			continue;
		
		pLabel->ModifyStyle(SS_TYPEMASK, nStyle);
	}
}

void CToDoCtrl::UpdateTask(TDC_ATTRIBUTE nAttrib)
{
	if (!m_tree.GetSafeHwnd())
		return;
	
	HTREEITEM hti = GetSelectedItem();
	
	if (!hti)
		return;
	
	// else
	UpdateData();
	
	switch (nAttrib)
	{
		//	case TDCA_TASKNAME:
		//		break;
		
	case TDCA_DONEDATE:
		{
			COleDateTime date;
			m_dateDone.GetTime(date);
			
			if (SetSelectedTaskDate(TDCD_DONE, date, FALSE))
			{
				// check if we need to modify percent done also
				if (!IsSelectedTaskDone())
				{
					int nPercentDone = GetSelectedTaskPercent();
					
					if (nPercentDone == 100)
						nPercentDone = 0;
					
					SetSelectedTaskPercentDone(nPercentDone);
					m_nPercentDone = nPercentDone;
					
					UpdateData(FALSE);
				}
				else if (m_nPercentDone != 100) // make the percent field look right
				{
					m_nPercentDone = 100;
					UpdateData(FALSE);
				}
			}
			else
			{
				UpdateControls(hti);
			}
		}
		break;
		
	case TDCA_STARTDATE:
		{
			COleDateTime date;
			m_dateStart.GetTime(date);
			
			SetSelectedTaskDate(TDCD_START, date);
			RecalcSelectedTimeEstimate();
		}
		break;
		
	case TDCA_DUEDATE:
		{
			COleDateTime date;
			m_dateDue.GetTime(date);
			
			SetSelectedTaskDate(TDCD_DUE, date);
			RecalcSelectedTimeEstimate();
		}
		break;
		
	case TDCA_COST:
		SetSelectedTaskCost(m_dCost);
		break;
		
	case TDCA_RECURRENCE:
		SetSelectedTaskRecurrence(m_tRecurrence);
		break;
		
	case TDCA_DEPENDENCY:
		m_eDependency.EnableButton(ID_DEPENDS_LINK, !m_sDepends.IsEmpty());
		SetSelectedTaskDependency(m_sDepends);
		break;
		
	case TDCA_PRIORITY:
		SetSelectedTaskPriority(m_nPriority);
		break;
		
	case TDCA_RISK:
		SetSelectedTaskRisk(m_nRisk);
		break;
		
		//	case TDCA_COLOR:
		//		break;
		
	case TDCA_EXTERNALID:
		SetSelectedTaskExtID(m_sExternalID);
		break;
		
	case TDCA_ALLOCTO:
		SetSelectedTaskAllocTo(m_aAllocTo);
		break;
		
	case TDCA_ALLOCBY:
		SetSelectedTaskAllocBy(m_sAllocBy);
		break;
		
	case TDCA_STATUS:
		SetSelectedTaskStatus(m_sStatus);
		break;
		
	case TDCA_VERSION:
		SetSelectedTaskVersion(m_sVersion);
		break;
		
	case TDCA_CATEGORY:
		SetSelectedTaskCategories(m_aCategory);
		break;
		
	case TDCA_PERCENT:
		{
			// note: we need to take account of 'done' state too because
			// we maintain the task percent at its pre-done state even
			// if the UI says its '100%'
			BOOL bWasDone = IsSelectedTaskDone();
			SetSelectedTaskPercentDone(m_nPercentDone);
			
			// check if we need to update 'done' state
			BOOL bDoneChange = (bWasDone && m_nPercentDone < 100) || (!bWasDone && m_nPercentDone == 100);
			
			if (bDoneChange)
			{
				SetSelectedTaskDone(m_nPercentDone == 100, FALSE);
				nAttrib = TDCA_DONEDATE;
			}
		}
		break;
		
	case TDCA_TIMEEST:
		SetSelectedTaskTimeEstimate(m_dTimeEstimate, m_nTimeEstUnits);
		break;
		
	case TDCA_TIMESPENT:
		SetSelectedTaskTimeSpent(m_dTimeSpent, m_nTimeSpentUnits);
		break;
		
	case TDCA_FILEREF:
		SetSelectedTaskFileRef(m_sFileRefPath);
		break;
		
	case TDCA_COMMENTS:
		UpdateComments(TRUE); // no longer handled by UpdateData
		SetSelectedTaskComments(m_sTextComments, m_CustomComments, TRUE); // TRUE == internal call
		break;
		
	default:
		return;
	}
}

void CToDoCtrl::RecalcSelectedTimeEstimate()
{
	if (HasStyle(TDCS_AUTOCALCTIMEESTIMATES))
	{
		TODOITEM* pTDI = GetSelectedTask();
		
		if (pTDI)
		{
			double dStart = pTDI->dateStart, dDue = pTDI->dateDue;
			
			if (dStart > 0 && dDue >= dStart) // exists
			{
				int nWeekdays = CDateHelper::CalcWeekdaysFromTo(dStart, dDue, TRUE);
				SetSelectedTaskTimeEstimate((double)nWeekdays, TDITU_DAYS);
			}
			else
				SetSelectedTaskTimeEstimate(0);
		}
	}
}

void CToDoCtrl::OnChangePriority()
{
	UpdateTask(TDCA_PRIORITY);
}

void CToDoCtrl::OnChangeRisk()
{
	UpdateTask(TDCA_RISK);
}

void CToDoCtrl::OnTaskDatechange(NMHDR* pNMHDR, LRESULT* pResult)
{
	switch (pNMHDR->idFrom)
	{
	case IDC_DONEDATE:	UpdateTask(TDCA_DONEDATE);	break;
	case IDC_STARTDATE:	UpdateTask(TDCA_STARTDATE); break;
	case IDC_DUEDATE:	UpdateTask(TDCA_DUEDATE); 	break;
	}
	
	*pResult = 0;
}

BOOL CToDoCtrl::OnEraseBkgnd(CDC* pDC) 
{
	int nDC = pDC->SaveDC();
	
	// clip out all the child controls to reduce flicker
	if (!(GetStyle() & WS_CLIPCHILDREN))
	{
		if (m_tree.GetSafeHwnd())
			ExcludeControls(pDC, IDC_FIRST + 1, IDC_LAST - 1);
	}
	
	BOOL bRes = CRuntimeDlg::OnEraseBkgnd(pDC);
	
	pDC->RestoreDC(nDC);
	
	return bRes;
}

void CToDoCtrl::SetDefaultAllocToNames(const CStringArray& aNames)
{
	m_cbAllocTo.AddUniqueItems(aNames);
}

void CToDoCtrl::SetDefaultAllocByNames(const CStringArray& aNames)
{
	m_cbAllocBy.AddUniqueItems(aNames);
}

void CToDoCtrl::SetDefaultCategoryNames(const CStringArray& aNames)
{
	m_cbCategory.AddUniqueItems(aNames);
}

void CToDoCtrl::SetDefaultStatusNames(const CStringArray& aNames)
{
	m_cbStatus.AddUniqueItems(aNames);
}

void CToDoCtrl::NewList()
{
	BOOL bConfirmDelete = HasStyle(TDCS_CONFIRMDELETE);
	
	if (bConfirmDelete)
		SetStyle(TDCS_CONFIRMDELETE, FALSE);
	
	DeleteAllTasks();
	
	if (bConfirmDelete)
		SetStyle(TDCS_CONFIRMDELETE, TRUE);
	
	m_sProjectName.Empty();
	m_nFileVersion = 0;
	m_bModified = FALSE;
	m_sPassword.Empty();
	
	UpdateData(FALSE);
	UpdateComments(FALSE);
}

BOOL CToDoCtrl::SetSelectedTaskColor(COLORREF color)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskColor(GetTaskID(hti), color);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		UpdateTask(TDCA_COLOR);
		SetModified(TRUE, TDCA_COLOR);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::CanPasteText()
{
	return (!IsReadOnly() && CWinClasses::IsEditControl(::GetFocus()));
}

BOOL CToDoCtrl::PasteText(LPCTSTR szText)
{
	if (!CanPasteText() || !szText || !*szText)
		return FALSE;

	HWND hFocus = ::GetFocus();
	
	::SendMessage(hFocus, EM_REPLACESEL, TRUE, (LPARAM)szText);

	// figure out who changed
	// TODO

	SetModified(TRUE, TDCA_NONE);
	
	return TRUE;
}

// external version
BOOL CToDoCtrl::SetSelectedTaskComments(LPCTSTR szComments, const CString& sCustomComments)
{
	return SetSelectedTaskComments(szComments, sCustomComments, FALSE);
}

BOOL CToDoCtrl::SetSelectedTaskComments(LPCTSTR szComments, const CString& sCustomComments, BOOL bInternal)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;

	// do not change the custom comments unless the comments
	// type matches the current content control type
	BOOL bCanChangeCustom = (!Misc::GuidIsNull(m_idCommentsType) && m_ctrlComments.IsType(m_idCommentsType));
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskComments(GetTaskID(hti), szComments);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;

		if (bCanChangeCustom)
		{
			nItemRes = m_data.SetTaskCustomComments(GetTaskID(hti), sCustomComments);
		
			if (nItemRes == SET_CHANGE)
				nRes = SET_CHANGE;
		}
	}
	
	if (nRes == SET_CHANGE)
	{
		// refresh the comments of the active task if we were called externally
		// note: we don't use SetTextChange because that doesn't handle custom comments
		if (!bInternal)
		{
			// try custom comments if that's what they are
			if (!bCanChangeCustom || 
				!m_ctrlComments.SetContent(sCustomComments) || 
				sCustomComments.IsEmpty())
			{
				// else text comments
				m_ctrlComments.SetTextContent(szComments);
			}
		}

		Selection().InvalidateAll();

		SetModified(TRUE, TDCA_COMMENTS);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskTitle(LPCTSTR szTitle)
{
	if (IsReadOnly() || GetSelectedCount() != 1)
		return FALSE;
	
	int nRes = m_data.SetTaskTitle(GetSelectedTaskID(), szTitle);
	
	if (nRes == SET_CHANGE)
	{
		m_tree.SetItemText(GetSelectedItem(), szTitle);
		
		InvalidateSelectedItem();
		SetModified(TRUE, TDCA_TASKNAME);
	}
	
	return (nRes != SET_FAILED);
}

CString CToDoCtrl::GetSelectedTaskComments() const
{
	if (GetSelectedCount() == 1)
		return m_data.GetTaskComments(GetSelectedTaskID());
	
	// else
	return "";
}

CString CToDoCtrl::GetSelectedTaskCustomComments() const
{
	if (GetSelectedCount() == 1)
		return m_data.GetTaskCustomComments(GetSelectedTaskID());
	
	// else
	return "";
}

HTREEITEM CToDoCtrl::GetSelectedItem() const 
{ 
	if (m_tree.GetSafeHwnd())
		return Selection().GetFirstItem();
	
	// else
	return NULL;
}

CString CToDoCtrl::GetSelectedTaskTitle() const
{
	if (GetSelectedCount() == 1)
		return GetItemText(GetSelectedItem());
	
	// else
	return "";
}

int CToDoCtrl::GetSelectedTaskPriority() const
{
   int nPriority = -1;
   POSITION pos = Selection().GetFirstItemPos();

   while (pos)
   {
      HTREEITEM hti = Selection().GetNextItem(pos);
      int nTaskPriority = m_data.GetTaskPriority(hti);

      if (nPriority == -1)
         nPriority = nTaskPriority;

      else if (nPriority != nTaskPriority)
         return -1;
   }
	
	return nPriority;
}

int CToDoCtrl::GetSelectedTaskRisk() const
{
   int nRisk = -1;
   POSITION pos = Selection().GetFirstItemPos();

   while (pos)
   {
      HTREEITEM hti = Selection().GetNextItem(pos);
      int nTaskRisk = m_data.GetTaskRisk(hti);

      if (nRisk == -1)
         nRisk = nTaskRisk;

      else if (nRisk != nTaskRisk)
         return -1; // == various
   }
	
	return nRisk;
}

COleDateTime CToDoCtrl::GetSelectedTaskDate(TDC_DATE nDate) const
{
	COleDateTime date; // == 0
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		date = m_data.GetTaskDate(hti, nDate);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			COleDateTime dateTask = m_data.GetTaskDate(hti, nDate);
			
			// first check if both date are set or not set
			if (date.m_dt == 0.0 && dateTask.m_dt == 0.0)
				continue;
			
			else if (date.m_dt == 0.0) // means dateTask.m_dt != 0
				return dateTask;
			else 
				return date;
		}

		// if we get here then no dates were set
	}
	
	return date;
}

int CToDoCtrl::IsSelectedTaskFlagged() const
{
	return m_data.IsTaskFlagged(GetSelectedTaskID());
}

double CToDoCtrl::GetSelectedTaskTimeEstimate(int& nUnits) const
{
	double dTime = 0.0;
	nUnits = s_tdDefault.nTimeEstUnits;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		dTime = m_data.GetTaskTimeEstimate(hti, nUnits);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			
			int nTaskUnits;
			double dTaskTime = m_data.GetTaskTimeEstimate(hti, nTaskUnits);
			
			if (dTime != dTaskTime || nUnits != nTaskUnits)
				return 0.0;
		}
	}
	
	return dTime;
}

double CToDoCtrl::CalcSelectedTaskTimeEstimate(int nUnits) const
{
	double dTime = 0.0;
	
	CHTIList selection;
	Selection().CopySelection(selection);
	CTreeSelectionHelper::RemoveChildDuplicates(selection, m_tree);
	
	POSITION pos = selection.GetHeadPosition();
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		dTime += m_data.CalcTimeEstimate(hti, nUnits);
	}

	return dTime;
}

double CToDoCtrl::GetSelectedTaskTimeSpent(int& nUnits) const
{
	double dTime = 0.0;
	nUnits = s_tdDefault.nTimeSpentUnits;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		dTime = m_data.GetTaskTimeSpent(hti, nUnits);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			
			int nTaskUnits;
			double dTaskTime = m_data.GetTaskTimeSpent(hti, nTaskUnits);
			
			if (dTime != dTaskTime || nUnits != nTaskUnits)
				return 0.0;
		}
	}
	
	return dTime;
}

double CToDoCtrl::CalcSelectedTaskTimeSpent(int nUnits) const
{
	double dTime = 0.0;
	
	CHTIList selection;
	Selection().CopySelection(selection);
	CTreeSelectionHelper::RemoveChildDuplicates(selection, m_tree);
	
	POSITION pos = selection.GetHeadPosition();
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		dTime += m_data.CalcTimeSpent(hti, nUnits);
	}

	return dTime;
}

BOOL CToDoCtrl::SetSelectedTaskPriority(int nPriority)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_FAILED;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskPriority(GetTaskID(hti), nPriority);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_nPriority != nPriority)
		{
			m_nPriority = nPriority;
			UpdateDataEx(this, IDC_PRIORITY, m_nPriority, FALSE);
		}
		
		SetModified(TRUE, TDCA_PRIORITY);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskRisk(int nRisk)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_FAILED;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskRisk(GetTaskID(hti), nRisk);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_nRisk != nRisk)
		{
			m_nRisk = nRisk;
			UpdateDataEx(this, TDCA_RISK, m_nRisk, FALSE);
		}
		
		SetModified(TRUE, TDCA_RISK);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskFlag(BOOL bFlagged)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_FAILED;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskFlag(GetTaskID(hti), bFlagged);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		SetModified(TRUE, TDCA_FLAG);
	
		if (IsColumnShowing(TDCC_FLAG))
		{
			if (Selection().GetCount() > 1)
				m_tree.RedrawGutter();
			else
				m_tree.RedrawGutterItem((DWORD)GetSelectedItem());
		}
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::IncrementSelectedTaskPriority(int nAmount)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_FAILED;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		DWORD dwID = GetTaskID(hti);

		int nPriority = m_data.GetTaskPriority(dwID) + nAmount;
		int nItemRes = m_data.SetTaskPriority(dwID, nPriority);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		int nPriority = m_nPriority + nAmount;
		nPriority = max(0, min(10, nPriority));

		if (m_nPriority != nPriority)
		{
			m_nPriority = nPriority;
			UpdateDataEx(this, IDC_PRIORITY, m_nPriority, FALSE);
		}
		
		SetModified(TRUE, TDCA_PRIORITY);
	}
	
	return (nRes != SET_FAILED);
}

void CToDoCtrl::ShowTaskHasDependenciesError()
{
	if (GetSelectedCount() == 1)
	{
		int nRet = AfxMessageBox(IDS_TDC_SELTASKHASDEPENDENCY, MB_YESNO | MB_ICONEXCLAMATION);
		
		if (nRet == IDYES)
		{
			CString sDepends = GetSelectedTaskDependency();
			ShowTaskLink(0, sDepends);
		}
	}
	else
		AfxMessageBox(IDS_TDC_SELTASKSHAVEDEPENDENCIES, MB_OK | MB_ICONEXCLAMATION);

}

BOOL CToDoCtrl::SetSelectedTaskDate(TDC_DATE nDate, const COleDateTime& date, BOOL bPromptCompleteSubtasks)
{
	if (IsReadOnly())
		return FALSE;

	// some pre-checks
	BOOL bDoChildren = FALSE;

	if (nDate == TDCD_DONE && date.m_dt > 0)
	{
		// check for incomplete dependents
		if (SelectionHasIncompleteDependencies())
		{
			// reset ctrl date
			SetCtrlDate(m_dateDone, 0.0);

			// show error msg
			ShowTaskHasDependenciesError();

			return FALSE;
		}

		// check for incomplete subtasks
		if (bPromptCompleteSubtasks)
		{
			int nRet = CheckWantSubtasksCompleted();

			if (nRet == -1) // cancel
				return FALSE;

			// else
			bDoChildren = (nRet == 1);
		}
	}
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);

		// special handling for completion
		int nItemRes = SET_NOCHANGE;
		DWORD dwTaskID = GetTaskID(hti);

		if (nDate == TDCD_DONE)
			nItemRes = SetTaskDoneDate(hti, date, bDoChildren);

		else // due, start, creation
			nItemRes = m_data.SetTaskDate(dwTaskID, nDate, date);

		// post-processing
		if (nItemRes == SET_CHANGE)
		{
			if (nDate == TDCD_DONE)
			{
				if (HasStyle(TDCS_AUTOADJUSTDEPENDENTS))
					FixupTaskDependentsDates(dwTaskID);
			}
			else if (nDate == TDCD_DUE)
				m_eRecurrence.SetDefaultDate(date);

			nRes = SET_CHANGE;
		}

	}
	
	if (nRes == SET_CHANGE)
	{
		switch (nDate)
		{
		case TDCD_CREATE:	SetModified(TRUE, TDCA_CREATIONDATE); break;
		case TDCD_START:	SetModified(TRUE, TDCA_STARTDATE); break;
		case TDCD_DUE:		SetModified(TRUE, TDCA_DUEDATE); break;
		case TDCD_DONE:		SetModified(TRUE, TDCA_DONEDATE); break;
		default:
			ASSERT(0);
			return FALSE;
		}
	}

	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskDate(TDC_DATE nDate, const COleDateTime& date)
{
	return SetSelectedTaskDate(nDate, date, TRUE);
}

int CToDoCtrl::SetTaskDoneDate(HTREEITEM hti, const COleDateTime& date, BOOL bAndChildren)
{
	DWORD dwTaskID = GetTaskID(hti);
	int nRes = 0;

	if (m_data.SetTaskDate(dwTaskID, TDCD_DONE, date) == SET_CHANGE)
		nRes = SET_CHANGE;
		
	m_tree.TCH().SetItemChecked(hti, date.m_dt > 0 ? TRUE : FALSE);
//	m_data.RefreshItemCheckState(hti, TRUE, TRUE);
		
	if (m_dwTimeTrackTaskID == dwTaskID)
	{
		EndTimeTracking();
		UpdateControls();
	}

	if (bAndChildren)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			if (SetTaskDoneDate(htiChild, date, TRUE) == SET_CHANGE)
				nRes = SET_CHANGE;
			
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return nRes;
}

BOOL CToDoCtrl::OffsetSelectedTaskDate(TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks)
{
	if (IsReadOnly())
		return FALSE;

	CHTIList htiSel;
	Selection().CopySelection(htiSel);

	// remove children from tree if bAndSubtasks specified
	// else we might offset them twice
	if (bAndSubtasks)
		CTreeSelectionHelper::RemoveChildDuplicates(htiSel, m_tree);

	POSITION pos = htiSel.GetHeadPosition();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = htiSel.GetNext(pos);

		int nItemRes = m_data.OffsetTaskDate(hti, nDate, nAmount, nUnits, bAndSubtasks);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		switch (nDate)
		{
		case TDCD_CREATE:	SetModified(TRUE, TDCA_CREATIONDATE); break;
		case TDCD_START:	SetModified(TRUE, TDCA_STARTDATE); break;
		case TDCD_DUE:		SetModified(TRUE, TDCA_DUEDATE); break;
		case TDCD_DONE:		SetModified(TRUE, TDCA_DONEDATE); break;
		default:
			ASSERT(0);
			return FALSE;
		}

		UpdateControls();
	}

	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskAttributeAsParent(TDC_ATTRIBUTE nAttrib)
{
	if (m_data.SetTaskAttributeAsParent(GetSelectedItem(), nAttrib) == SET_CHANGE)
	{
		SetModified(TRUE, nAttrib);
		UpdateControls();
	}

	return TRUE;
}

COLORREF CToDoCtrl::GetSelectedTaskColor() const
{
	return m_data.GetTaskColor(GetSelectedItem());
}

BOOL CToDoCtrl::SetSelectedTaskDone(BOOL bDone)
{
	return SetSelectedTaskDone(bDone, TRUE);
}

int CToDoCtrl::CheckWantSubtasksCompleted()
{
	if (m_selection.HasIncompleteSubtasks())
	{
		// ask the user what to do
		UINT nIDMessage = (GetSelectedCount() == 1) ? 
							IDS_TDC_SELTASKHASINCOMPLETE : IDS_TDC_TASKHASINCOMPLETE;
		
		int nRet = AfxMessageBox(nIDMessage, MB_YESNOCANCEL | MB_ICONQUESTION);

		if (nRet == IDYES)
			return 1;

		else if (nRet == IDCANCEL)
			return -1;
	}

	return 0; // no incomplete subtasks or IDNO
}

BOOL CToDoCtrl::SetSelectedTaskDone(BOOL bDone, BOOL bUpdatePercent)
{
	if (IsReadOnly())
		return FALSE;

	// check for incomplete dependents
	if (bDone && SelectionHasIncompleteDependencies())
	{
		ShowTaskHasDependenciesError();
		return FALSE;
	}
	
	// check for incomplete subtasks
	BOOL bDoChildren = FALSE;

	if (bDone)
	{
		int nRet = CheckWantSubtasksCompleted();

		if (nRet == -1) // cancel
			return FALSE;

		// else
		bDoChildren = (nRet == 1);
	}
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	int nPrevPercent = 0;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);

		// is it a recurring task? because if so, there's no point
		DWORD dwTaskID = GetTaskID(hti);
		COleDateTime dtNext;
		BOOL bRecurring = FALSE, bWasDone = bDone;
		
		if (bDone)
		{
			bRecurring = m_data.GetTaskNextOccurrence(dwTaskID, dtNext);

			if (bRecurring)
				bDone = FALSE;
		}

		int nItemRes = SetTaskDone(hti, bDone, nPrevPercent, bDoChildren);

		if (nItemRes == SET_CHANGE || (bRecurring && bWasDone))
		{
			if (bRecurring)
			{
				m_data.SetTaskDate(dwTaskID, TDCD_DUE, dtNext);
/*
				// copy task and it's children
				CTaskFile task;
				TDCFILTER filter(TDCF_ALL, 0);				
				AddItem(hti, task, NULL, filter, 0);

				// add it to the tree
				HTREEITEM htiParent = m_tree.GetParentItem(hti);
				HTREEITEM htiNew = AddItem(task, task.GetFirstTask(), htiParent, hti, TDCR_YES);

				// reset completion state
				int nDummy;
				SetTaskDone(htiNew, FALSE, nDummy, TRUE);

				// and set due date
				TODOITEM* pTDI = m_data.GetTask(htiNew);
				ASSERT (pTDI);
				
				pTDI->dateDue = dtNext;
*/
			}
			nRes = SET_CHANGE;
		}
	}
	
	if (nRes == SET_CHANGE)
	{
		if (bUpdatePercent)
		{
			if (GetSelectedCount() == 1)
			{
				if (!bDone)
				{
					// restore last known percentage unless is 100%
					if (nPrevPercent == 100)
						nPrevPercent = 0;
					
					m_nPercentDone = nPrevPercent;
					UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
				}
				else if (m_nPercentDone != 100) // make it look right
				{
					m_nPercentDone = 100;
					UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
				}
			}
			else
			{
				m_nPercentDone = bDone ? 100 : 0;
				UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
			}
		}
		
		UpdateControls();
		m_tree.UpdateWindow();
		
		SetModified(TRUE, TDCA_DONEDATE);
	}
	
	return (nRes != SET_FAILED);
}

int CToDoCtrl::SetTaskDone(HTREEITEM hti, BOOL bDone, int& nPrevPercent, BOOL bAndChildren)
{
	DWORD dwTaskID = GetTaskID(hti);
	int nRes = 0;

	if (m_data.SetTaskDone(dwTaskID, bDone, nPrevPercent) == SET_CHANGE)
		nRes = SET_CHANGE;
		
	m_tree.TCH().SetItemChecked(hti, bDone);
//	m_data.RefreshItemCheckState(hti, bDone, TRUE);
		
	if (m_dwTimeTrackTaskID == dwTaskID)
	{
		EndTimeTracking();
		UpdateControls();
	}

	if (bAndChildren)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);
		int nTemp;

		while (htiChild)
		{
			if (SetTaskDone(htiChild, bDone, nTemp, TRUE) == SET_CHANGE)
				nRes = SET_CHANGE;
			
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
	
	// update any dependents dates
	FixupTaskDependentsDates(dwTaskID);

	return nRes;
}

BOOL CToDoCtrl::IsSelectedTaskDone() const
{
	return m_data.IsTaskDone(GetSelectedItem());
}

BOOL CToDoCtrl::IsSelectedTaskDue() const
{
	return m_data.IsTaskDue(GetSelectedItem());
}

BOOL CToDoCtrl::SetSelectedTaskPercentDone(int nPercent)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskPercent(GetTaskID(hti), nPercent);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_nPercentDone != nPercent)
		{
			m_nPercentDone = nPercent;
			UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
		}
		
		SetModified(TRUE, TDCA_PERCENT);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskCost(const double& dCost)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);

		// ignore parent tasks
//		if (!m_tree.ItemHasChildren(hti))
		{
			int nItemRes = m_data.SetTaskCost(GetTaskID(hti), dCost);
			
			if (nItemRes == SET_CHANGE)
				nRes = SET_CHANGE;
		}
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_dCost != dCost)
		{
			m_dCost = dCost;
			UpdateDataEx(this, IDC_COST, m_dCost, FALSE);
		}
		
		SetModified(TRUE, TDCA_COST);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskRecurrence(const TDIRECURRENCE& tr)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);

		int nItemRes = m_data.SetTaskRecurrence(GetTaskID(hti), tr);
			
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_tRecurrence != tr)
		{
			m_tRecurrence = tr;
			m_eRecurrence.SetRecurrenceOptions(tr);
		}
		
		SetModified(TRUE, TDCA_RECURRENCE);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::GetSelectedTaskRecurrence(TDIRECURRENCE& tr) const
{
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		m_data.GetTaskRecurrence(hti, tr);
		
		while (pos)
		{
			TDIRECURRENCE trTask;
			HTREEITEM hti = Selection().GetNextItem(pos);
			m_data.GetTaskRecurrence(hti, trTask);
			
			if (tr != trTask)
				tr = TDIRECURRENCE();
		}
	}
	
	return (tr.nRegularity != TDIR_ONCE);
}

BOOL CToDoCtrl::IncrementSelectedTaskPercentDone(int nAmount)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		DWORD dwID = GetTaskID(hti);

		BOOL bDone = m_data.IsTaskDone(dwID);
		int nPercent = m_data.GetTaskPercent(dwID, TRUE) + nAmount, nPrevPercent; 

		// need to handle transition to/from 100% as special case
		if ((bDone && nPercent < 100) || (!bDone && nPercent >= 100))
		{
			m_data.SetTaskDone(dwID, !bDone, nPrevPercent);
			m_tree.TCH().SetItemChecked(hti, bDone);
//			m_data.RefreshItemCheckState(hti, bDone);
		}

		int nItemRes = m_data.SetTaskPercent(dwID, nPercent);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	if (nRes == SET_CHANGE)
	{
		// don't update m_nPercentDone for multiple selection
		// else all items else they all end up as the same value
		if (GetSelectedCount() == 1)
		{
			int nPercent = m_nPercentDone + nAmount; 
			if (m_nPercentDone != nPercent)
			{
				m_nPercentDone = nPercent;
				UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
			}
		}
		
		SetModified(TRUE, TDCA_PERCENT);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskTimeEstimate(const double& dHours, int nUnits)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);

		// ignore parent tasks
		if (!m_tree.ItemHasChildren(hti) || HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
		{
			int nItemRes = m_data.SetTaskTimeEstimate(GetTaskID(hti), dHours, nUnits);
			
			if (nItemRes == SET_CHANGE)
				nRes = SET_CHANGE;
		}
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_dTimeEstimate != dHours || m_nTimeEstUnits != nUnits)
		{
			m_dTimeEstimate = dHours;
			m_nTimeEstUnits = nUnits;
			
			UpdateDataEx(this, IDC_TIMEEST, m_dTimeEstimate, FALSE);
			m_eTimeEstimate.SetUnits(m_nTimeEstUnits);
		}
		
		// update % complete?
		if (HasStyle(TDCS_AUTOCALCPERCENTDONE) && GetSelectedCount() == 1)
		{
			m_nPercentDone = m_data.CalcPercentDone(GetSelectedItem());		
			UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
		}
		
		SetModified(TRUE, TDCA_TIMEEST);
	}
	
	return (nRes != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskTimeSpent(const double& dHours, int nUnits)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);

		// ignore parent tasks
		if (!m_tree.ItemHasChildren(hti) || HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
		{
			int nItemRes = m_data.SetTaskTimeSpent(GetTaskID(hti), dHours, nUnits);
			
			if (nItemRes == SET_CHANGE)
				nRes = SET_CHANGE;
		}
	}
	
	if (nRes == SET_CHANGE)
	{
		if (m_dTimeSpent != dHours || m_nTimeSpentUnits != nUnits)
		{
			m_dTimeSpent = dHours;
			m_nTimeSpentUnits = nUnits;
			
			UpdateDataEx(this, IDC_TIMESPENT, m_dTimeSpent, FALSE);
			m_eTimeSpent.SetUnits(m_nTimeSpentUnits);
		}
		
		// update % complete?
		if (HasStyle(TDCS_AUTOCALCPERCENTDONE) && GetSelectedCount() == 1)
		{
			m_nPercentDone = m_data.CalcPercentDone(GetSelectedItem());		
			UpdateDataEx(this, IDC_PERCENT, m_nPercentDone, FALSE);
		}
		
		SetModified(TRUE, TDCA_TIMESPENT);
	}
	
	return (nRes != SET_FAILED);
}

int CToDoCtrl::GetSelectedTaskPercent() const
{
	int nPercent = 0;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		nPercent = m_data.GetTaskPercent(hti);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			int nTaskPercent = m_data.GetTaskPercent(hti);
			
			if (nPercent != nTaskPercent)
				return -1;
		}
	}
	
	return nPercent;
}

double CToDoCtrl::GetSelectedTaskCost() const
{
	double dCost = 0.0;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		dCost = m_data.GetTaskCost(hti);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			double dTaskCost = m_data.GetTaskCost(hti);

			if (dCost != dTaskCost)
				return 0.0;
		}
	}
	
	return dCost;
}

double CToDoCtrl::CalcSelectedTaskCost() const
{
	double dCost = 0.0;
	
	CHTIList selection;
	Selection().CopySelection(selection);
	CTreeSelectionHelper::RemoveChildDuplicates(selection, m_tree);

	POSITION pos = selection.GetHeadPosition();
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		dCost += m_data.CalcCost(hti);
	}
	
	return dCost;
}

BOOL CToDoCtrl::SetTextChange(int nChange, CString& sItem, LPCTSTR szNewItem, TDC_ATTRIBUTE nAttrib, UINT nIDC)
{
	ASSERT(!IsReadOnly());
	
	if (nChange == SET_CHANGE)
	{
		if (sItem != szNewItem)
		{
			sItem = szNewItem;
			UpdateDataEx(this, nIDC, sItem, FALSE);
		}
		
		SetModified(TRUE, nAttrib);
	}
	
	return (nChange != SET_FAILED);
}

BOOL CToDoCtrl::SetSelectedTaskAllocTo(const CStringArray& aAllocTo)
{
	if (IsReadOnly())
		return FALSE;

	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskAllocTo(GetTaskID(hti), aAllocTo);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	SetModified(TRUE, TDCA_ALLOCTO);
	return TRUE;
}

BOOL CToDoCtrl::SetSelectedTaskAllocBy(LPCTSTR szAllocBy)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskAllocBy(GetTaskID(hti), szAllocBy);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	return SetTextChange(nRes, m_sAllocBy, szAllocBy, TDCA_ALLOCBY, IDC_ALLOCBY);
}

BOOL CToDoCtrl::SetSelectedTaskVersion(LPCTSTR szVersion)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskVersion(GetTaskID(hti), szVersion);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	return SetTextChange(nRes, m_sVersion, szVersion, TDCA_VERSION, IDC_VERSION);
}

BOOL CToDoCtrl::SetSelectedTaskStatus(LPCTSTR szStatus)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskStatus(GetTaskID(hti), szStatus);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	return SetTextChange(nRes, m_sStatus, szStatus, TDCA_STATUS, IDC_STATUS);
}

BOOL CToDoCtrl::SetSelectedTaskCategories(const CStringArray& aCats)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes = m_data.SetTaskCategories(GetTaskID(hti), aCats);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	SetModified(TRUE, TDCA_CATEGORY);
	return TRUE;
}

BOOL CToDoCtrl::TimeTrackSelectedTask()
{
	if (!CanTimeTrackSelectedTask())
		return FALSE;
	
	TimeTrackTask(GetSelectedItem());
	
	return FALSE;
}

BOOL CToDoCtrl::CanTimeTrackSelectedTask() const
{
	if (IsReadOnly() || GetSelectedCount() != 1)
		return FALSE;
	
	return (m_data.IsTaskTimeTrackable(GetSelectedItem()));
}

BOOL CToDoCtrl::IsSelectedTaskBeingTimeTracked() const
{
	HTREEITEM hti = GetSelectedItem();
	
	return (m_data.IsTaskTimeTrackable(hti) && m_dwTimeTrackTaskID == GetTaskID(hti));
}

BOOL CToDoCtrl::IsActivelyTimeTracking() const
{
	if (!m_dwTimeTrackTaskID || m_bTimeTrackingPaused)
		return FALSE;

	DWORD dwSelTaskID = GetSelectedTaskID();
	BOOL bTrackingSelTask = (dwSelTaskID == m_dwTimeTrackTaskID);

	if (!bTrackingSelTask && HasStyle(TDCS_TRACKSELECTEDTASKONLY))
		return FALSE;
	
	// does it permit tracking
	HTREEITEM htiTrack = bTrackingSelTask ? GetSelectedItem() : m_data.GetItem(m_dwTimeTrackTaskID);

	if (!m_data.IsTaskTimeTrackable(htiTrack))
		return FALSE;

	if (HasStyle(TDCS_PAUSETIMETRACKINGONSCRNSAVER))
	{
		if (Misc::IsScreenSaverActive() || Misc::IsWorkStationLocked())
			return FALSE;
	}

	// yeah!
	return TRUE;
}

int CToDoCtrl::GetSelectedTaskAllocTo(CStringArray& aAllocTo) const
{
	aAllocTo.RemoveAll();

	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		
		m_data.GetTaskAllocTo(hti, aAllocTo);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);

			CStringArray aTaskAllocTo;
			m_data.GetTaskAllocTo(hti, aTaskAllocTo);
			
			if (!Misc::ArraysMatch(aAllocTo, aTaskAllocTo))
			{
				aAllocTo.RemoveAll();
				break;
			}
		}
	}
	
	return aAllocTo.GetSize();
}

CString CToDoCtrl::GetSelectedTaskAllocBy() const
{
	CString sAllocBy;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		sAllocBy = m_data.GetTaskAllocBy(hti);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			CString sTaskAllocBy = m_data.GetTaskAllocBy(hti);
			
			if (sAllocBy != sTaskAllocBy)
				return "";
		}
	}
	
	return sAllocBy;
}

CString CToDoCtrl::GetSelectedTaskVersion() const
{
	CString sVersion;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		sVersion = m_data.GetTaskVersion(hti);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			CString sTaskVersion = m_data.GetTaskVersion(hti);
			
			if (sVersion != sTaskVersion)
				return "";
		}
	}
	
	return sVersion;
}

CString CToDoCtrl::GetSelectedTaskStatus() const
{
	CString sStatus;
	
	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		sStatus = m_data.GetTaskStatus(hti);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			CString sTaskStatus = m_data.GetTaskStatus(hti);
			
			if (sStatus != sTaskStatus)
				return "";
		}
	}
	
	return sStatus;
}

int CToDoCtrl::GetSelectedTaskCategories(CStringArray& aCats) const
{
	aCats.RemoveAll();

	if (GetSelectedCount())
	{
		// get first item's value as initial
		POSITION pos = Selection().GetFirstItemPos();
		HTREEITEM hti = Selection().GetNextItem(pos);
		
		m_data.GetTaskCategories(hti, aCats);
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);

			CStringArray aTaskCats;
			m_data.GetTaskCategories(hti, aTaskCats);
			
			if (!Misc::ArraysMatch(aCats, aTaskCats))
			{
				aCats.RemoveAll();
				break;
			}
		}
	}
	
	return aCats.GetSize();
}

BOOL CToDoCtrl::SetSelectedTaskFileRef(LPCTSTR szFilePath)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes =	m_data.SetTaskFileRef(GetTaskID(hti), szFilePath);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	return SetTextChange(nRes, m_sFileRefPath, szFilePath, TDCA_FILEREF, IDC_FILEPATH);
}

BOOL CToDoCtrl::SetSelectedTaskDependency(LPCTSTR szDepends)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
      	DWORD dwTaskID = GetTaskID(hti);
		int nItemRes =	m_data.SetTaskDependency(dwTaskID, szDepends);
		
		if (nItemRes == SET_CHANGE)
      	{
         	// make sure our start date matches our dependents due date
			if (HasStyle(TDCS_AUTOADJUSTDEPENDENTS))
			{
         		DWORD dwIDDepends = atoi(szDepends);

         		if (dwIDDepends && AdjustTaskDates(dwTaskID, dwIDDepends))
				{
					// and then fix up our dependents
					FixupTaskDependentsDates(dwTaskID);
				}
			}

			nRes = SET_CHANGE;
      	}
	}
	
	return SetTextChange(nRes, m_sDepends, szDepends, TDCA_DEPENDENCY, IDC_DEPENDS);
}

BOOL CToDoCtrl::SetSelectedTaskExtID(LPCTSTR szID)
{
	if (IsReadOnly())
		return FALSE;
	
	POSITION pos = Selection().GetFirstItemPos();
	int nRes = SET_NOCHANGE;
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		int nItemRes =	m_data.SetTaskExtID(GetTaskID(hti), szID);
		
		if (nItemRes == SET_CHANGE)
			nRes = SET_CHANGE;
	}
	
	return SetTextChange(nRes, m_sExternalID, szID, TDCA_EXTERNALID, IDC_EXTERNALID);
}

CString CToDoCtrl::GetSelectedTaskFileRef() const
{
	if (GetSelectedCount() == 1)
		return m_data.GetTaskFileRef(GetSelectedTaskID());
	
	// else
	return "";
}

CString CToDoCtrl::GetSelectedTaskExtID() const
{
	if (GetSelectedCount() == 1)
		return m_data.GetTaskExtID(GetSelectedTaskID());
	
	// else
	return "";
}

CString CToDoCtrl::GetSelectedTaskDependency() const
{
	if (GetSelectedCount() == 1)
		return m_data.GetTaskDependency(GetSelectedTaskID());
	
	// else
	return "";
}

BOOL CToDoCtrl::GotoSelectedTaskFileRef()
{
	return GotoFile(GetSelectedTaskFileRef(), TRUE);
}

HTREEITEM CToDoCtrl::NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere, BOOL bSelect, 
							 BOOL bEditText)
{
	if (IsReadOnly())
		return NULL;
	
	if (!szText || !*szText)
		return NULL;

	// are we an archive and should we warn user if we are
	if (m_bArchive && HasStyle(TDCS_WARNADDDELETEARCHIVE))
	{
		if (MessageBoxEx(this, IDS_TDC_WARNADDTOARCHIVE, IDS_TDC_CONFIRMADD_TITLE, 
						MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return NULL;
		}
	}

	HTREEITEM htiSel = GetSelectedItem(), htiParent = NULL, htiAfter = NULL;
	
	// handle no selected item
	if (!htiSel)
	{
		switch (nWhere)
		{
		case TDC_INSERTATTOP:
		case TDC_INSERTATTOPOFSELTASKPARENT:
		case TDC_INSERTATTOPOFSELTASK:
		case TDC_INSERTBEFORESELTASK:
			htiParent = TVI_ROOT;
			htiAfter = TVI_FIRST;
			break;
			
		case TDC_INSERTATBOTTOM:
		case TDC_INSERTATBOTTOMOFSELTASKPARENT:
		case TDC_INSERTATBOTTOMOFSELTASK:
		case TDC_INSERTAFTERSELTASK:
			htiParent = TVI_ROOT;
			htiAfter = TVI_LAST; 
			break;
		}
	}
	else // determine the actual pos to insert
	{
		switch (nWhere)
		{
		case TDC_INSERTATTOP:
			htiParent = TVI_ROOT;
			htiAfter = TVI_FIRST;
			break;
			
		case TDC_INSERTATBOTTOM:
			htiParent = TVI_ROOT;
			htiAfter = TVI_LAST; 
			break;
			
		case TDC_INSERTATTOPOFSELTASKPARENT:
			htiParent = m_tree.GetParentItem(htiSel);
			htiAfter = TVI_FIRST;
			break;
			
		case TDC_INSERTATBOTTOMOFSELTASKPARENT:
			htiParent = m_tree.GetParentItem(htiSel);
			htiAfter = TVI_LAST;
			break;
			
		case TDC_INSERTAFTERSELTASK:
			htiParent = m_tree.GetParentItem(htiSel);
			htiAfter = htiSel;
			
			if (!htiAfter)
				htiAfter = TVI_LAST;
			break;
			
		case TDC_INSERTBEFORESELTASK:
			htiParent = m_tree.GetParentItem(htiSel);
			htiAfter = m_tree.GetNextItem(htiSel, TVGN_PREVIOUS);
			
			if (!htiAfter)
				htiAfter = TVI_FIRST;
			break;
			
		case TDC_INSERTATTOPOFSELTASK: // subtask
			htiParent = htiSel;
			htiAfter = TVI_FIRST;
			break;
			
		case TDC_INSERTATBOTTOMOFSELTASK: // subtask
			htiParent = htiSel;
			htiAfter = TVI_LAST; 
			break;
		}
	}
		
	return InsertItem(szText, htiParent, htiAfter, bSelect, bEditText);
}

TODOITEM* CToDoCtrl::NewTask()
{
	return m_data.NewTask(&s_tdDefault);
}

HTREEITEM CToDoCtrl::InsertItem(LPCTSTR szText, HTREEITEM htiParent, HTREEITEM htiAfter, 
								BOOL bSelect, BOOL bEdit)
{
	m_htiLastAdded = NULL;
	
	if (IsReadOnly())
		return NULL;
	
	if (!szText || !*szText)
		return NULL;
	
	// must create the new task item first
	TODOITEM* pTDINew = NewTask();
	
	// and initialize
	if (szText)
		pTDINew->sTitle = szText;

	if (pTDINew->dateStart.m_dt == -1)
		pTDINew->dateStart = floor(COleDateTime::GetCurrentTime());
	
	// and map it
	m_data.AddTask(m_dwNextUniqueID, pTDINew);
	m_data.ResetCachedCalculations();
	
	// bold state
    BOOL bRootParent = (!htiParent || htiParent == TVI_ROOT);
	BOOL bBold = bRootParent;
	
	HTREEITEM htiNew = m_tree.InsertItem(
		TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
		LPSTR_TEXTCALLBACK, 
		I_IMAGECALLBACK, 
		I_IMAGECALLBACK, 
		(bBold ? TVIS_BOLD : 0),
		TVIS_BOLD, // state mask
		m_dwNextUniqueID, // lParam
		htiParent, 
		htiAfter);
	
	if (htiNew)
	{
		// check state
		m_tree.TCH().SetItemChecked(htiNew, pTDINew->IsDone());

		//	BOOL bParentHasChildren = (!bRootParent && m_tree.ItemHasChildren(htiParent));
		//m_data.RefreshItemCheckState(htiNew, FALSE, bParentHasChildren);

		// if the parent was being clocked then stop it now
		// since having children makes a task unclockable
		if (!bRootParent && m_dwTimeTrackTaskID && m_dwTimeTrackTaskID == GetTaskID(htiParent))
			EndTimeTracking();

		// if the parent was marked as done and this task is not
		// then mark the parent as incomplete too
		if (!pTDINew->IsDone() && htiParent != TVI_ROOT && m_data.IsTaskDone(htiParent))
		{
			int nPercent;

			m_data.SetTaskDone(m_data.GetTaskID(htiParent), FALSE, nPercent);
			m_tree.TCH().SetItemChecked(htiParent, FALSE);
		}
		
		m_dwNextUniqueID++;
		SetModified(TRUE, TDCA_NONE); 
		
		// resort if necessary because TDCA_NONE in the call to SetModified 
		// will not cause a resort
		if (HasStyle(TDCS_RESORTONMODIFY) && IsSortable())
			m_data.Sort(m_nSortBy, m_bSortAscending, htiParent);

		m_tree.SelectItem(htiNew);
		m_tree.Invalidate(TRUE);
		m_tree.UpdateWindow();

		if (bSelect)
		{
			SelectItem(htiNew);
			
			if (bEdit)
			{
				m_htiLastAdded = htiNew;
				m_tree.PostMessage(TVM_EDITLABEL, 0, (LPARAM)htiNew); 

/*
				CEdit* pEdit = m_tree.EditLabel(htiNew);
				
				// workaround because if the user presses <enter> without
				// editing the treectrl reports this as a non-edit and
				// the new items gets deleted!
				if (pEdit)
				{
					pEdit->SetWindowText(szText);
					pEdit->SetSel(0, -1);
				}
*/
			}
			else
				m_tree.SetFocus();
		}
	}
	else // cleanup
		m_data.DeleteTask(m_dwNextUniqueID);
	
	return htiNew;
}

BOOL CToDoCtrl::IsSortable() const
{
	return (m_nSortBy != TDC_SORTDISABLED && m_nSortBy != TDC_SORTUNDEF);
}

BOOL CToDoCtrl::CanSplitSelectedTask()
{
	int nSelCount = GetSelectedCount();
	
	return (!IsReadOnly() && (nSelCount > 1 || (nSelCount == 1 && !IsSelectedTaskDone() && 
								!ItemHasChildren(GetSelectedItem()))));
}

BOOL CToDoCtrl::SplitSelectedTask(int nNumSubtasks)
{
	if (nNumSubtasks < 2)
		return FALSE;
	
	if (!GetSelectedCount())
		return FALSE;
	
	CHoldRedraw hr(m_tree);
	
	POSITION pos = Selection().GetFirstItemPos();
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
				
		if (ItemHasChildren(hti))
			continue;
		
		TODOITEM* pTDI = GetTask(GetTaskID(hti));
		ASSERT(pTDI);
		
		if (pTDI->IsDone())
			continue;
		
		// calculate how much time to allocate to each
		double dSubTime = 0, dSubEst = pTDI->dTimeEstimate / nNumSubtasks;
		
		if (pTDI->HasStart() && pTDI->dateDue > pTDI->dateStart)
			dSubTime = (pTDI->dateDue - pTDI->dateStart) / nNumSubtasks;
		
		for (int nSubtask = 0; nSubtask < nNumSubtasks; nSubtask++)
		{
			TODOITEM* tdiSub = m_data.NewTask(pTDI); // copy parent
			
			// allocate time slice and dates
			tdiSub->dTimeEstimate = dSubEst;
			
			if (dSubTime)
			{
				tdiSub->dateStart += (nSubtask * dSubTime);
				tdiSub->dateDue = tdiSub->dateStart + COleDateTime(dSubTime);
			}
			else if (nSubtask) // not the first
			{
				tdiSub->ClearStart();
				tdiSub->ClearDue();
			}

			// clear time spent from all but first task
			if (nSubtask)
				tdiSub->dTimeSpent = 0;

			// map it
			DWORD dwTaskID = m_dwNextUniqueID++;
			m_data.AddTask(dwTaskID, tdiSub);
			
			// create tree item
			HTREEITEM htiSub = m_tree.InsertItem(LPSTR_TEXTCALLBACK, I_IMAGECALLBACK, I_IMAGECALLBACK, hti);
			
			m_tree.SetItemData(htiSub, dwTaskID);
			m_tree.TCH().SetItemChecked(htiSub, TCHC_UNCHECKED);
		}

		// clear parent time spent/est
		pTDI->dTimeSpent = pTDI->dTimeEstimate = 0;

		// show new subtasks
		m_tree.Expand(hti, TVE_EXPAND);
	}
	
	SetModified(TRUE, TDCA_NONE);
	
	return TRUE;
}

BOOL CToDoCtrl::SelectedTasksHaveChildren() const
{
	return Selection().AnyItemsHaveChildren();
}

BOOL CToDoCtrl::DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel)
{
	if (IsReadOnly())
		return FALSE;
	
	int nSelCount = GetSelectedCount();
	
	if (!nSelCount)
		return FALSE;
	
	// if need confirmation then make sure the user knows that child items will get removed too
	if (bWarnUser && HasStyle(TDCS_CONFIRMDELETE))
	{
		CEnString sMessage(IDS_TDC_CONFIRMDELETESEL);
		UINT nFlags = MB_ICONEXCLAMATION | MB_YESNO;
		
		if (SelectedTasksHaveChildren())
		{
			sMessage += (nSelCount > 1) ? CEnString(IDS_TDC_CONFIRMDELETEMOREWARNSUBTASKS) : 
										CEnString(IDS_TDC_CONFIRMDELETEONEWARNSUBTASKS);
			nFlags |= MB_DEFBUTTON2;
		}
		else
			nFlags |= MB_DEFBUTTON1;
		
		if (MessageBoxEx(this, sMessage, IDS_TDC_CONFIRMDELETE_TITLE, nFlags) != IDYES)
			return FALSE;
	}
	
	// snapshot selection against changes
	CHTIList selection;
	Selection().CopySelection(selection);
	
	// reset anchor if we're going to delete it
	if (Selection().AnchorHasSelectedParent())
		Selection().SetAnchor(NULL);
	
	// remove real tree selection first if its going to be deleted
	// but before we do, lets figure out where the selection is
	// going to go _after_ the deletion, but only for single items.
	HTREEITEM htiNextSel = NULL;
	CTreeCtrlHelper tch(m_tree);
	
	if (bResetSel)
	{
		HTREEITEM hti = Selection().GetFirstItem();
		
		if (nSelCount == 1)
		{
			htiNextSel = tch.GetNextVisibleItem(hti, FALSE);
			
			// don't include items we're about to delete!
			while (htiNextSel && Selection().HasItem(htiNextSel))
				htiNextSel = tch.GetNextVisibleItem(htiNextSel);
			
			// else try previous item
			if (!htiNextSel)
			{
				htiNextSel = tch.GetPrevVisibleItem(hti, FALSE);
				
				// don't include items we're about to delete!
				while (htiNextSel && Selection().HasItem(htiNextSel))
					htiNextSel = tch.GetPrevVisibleItem(htiNextSel);
			}
		}
	}
	
	if (Selection().HasItem(m_tree.GetSelectedItem()))
		m_tree.SelectItem(NULL);
	
	// because of the free-form nature of the selection list
	// it can contain children and their parents in the same list
	// which can be a problem if we delete the parent first.
	// so we first remove all items from the list if their
	// parent is in the list because they'll be deleted anyhow
	Selection().RemoveChildDuplicates(selection, m_tree);
	
	// do it
	BOOL bRemoveAll = Selection().ContainsAllItems();

	if (!bRemoveAll)
		m_tree.SetRedraw(FALSE);
	
	// now we can delete with impunity
	POSITION pos = selection.GetHeadPosition();

	// note: we also keep track of the parent items so we can 
	// update their check states afterwards
	CHTIList lstParents;
	
	while (pos)
	{
		HTREEITEM hti = selection.GetNext(pos);
		HTREEITEM htiParent = m_tree.GetParentItem(hti);

		// save off the parent so long as its not going to be deleted
		if (!selection.Find(htiParent) && !lstParents.Find(htiParent))
			lstParents.AddTail(htiParent);

		m_data.DeleteTask(hti);
	}
	
	// clear selection
	Selection().RemoveAll(TRUE);
	
	// refresh parent check states
/*	pos = lstParents.GetHeadPosition();

	while (pos)
		m_data.RefreshItemCheckState(lstParents.GetNext(pos), -1, TRUE);
*/
	// refresh rest of UI
	SetModified(TRUE, TDCA_NONE);
	UpdateControls();
	
	// set next selection
	if (htiNextSel)
		SelectItem(htiNextSel);

	if (!bRemoveAll)
		m_tree.SetRedraw(TRUE);
		
	return TRUE;
}

BOOL CToDoCtrl::DeleteSelectedTask()
{
	// are we an archive and should we warn user if we are
	if (m_bArchive && HasStyle(TDCS_WARNADDDELETEARCHIVE))
	{
		if (MessageBoxEx(this, IDS_TDC_WARNDELETEFROMARCHIVE, IDS_TDC_CONFIRMDELETE_TITLE, 
						MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return FALSE;
		}
		else
			return DeleteSelectedTask(FALSE, TRUE); // FALSE ==  don't warn again
	}
	
	// else do the standard warning against deletion
	return DeleteSelectedTask(TRUE, TRUE);
}

BOOL CToDoCtrl::EditSelectedTask(BOOL bTaskIsNew)
{
	if (IsReadOnly() || GetSelectedCount() != 1)
		return FALSE;
	
	HTREEITEM htiSel = GetSelectedItem();
	ASSERT (htiSel);

	if (bTaskIsNew)
		m_htiLastAdded = htiSel;
	
	return (NULL != m_tree.EditLabel(htiSel));
}

BOOL CToDoCtrl::IsTreeLabelEditing() 
{ 
	if (IsReadOnly() || GetSelectedCount() != 1)
		return FALSE;
	
	CEdit* pEdit = m_tree.GetEditControl();
	
	return (pEdit && GetFocus() == pEdit); 
}

void CToDoCtrl::OnTreeEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	BOOL bCancelled = (pTVDispInfo->item.pszText == NULL);
	
	if (!bCancelled) // not cancelled
	{
		DWORD dwID = GetTaskID(pTVDispInfo->item.hItem);
		TODOITEM* pTDI = GetTask(dwID);
		
		if (pTDI)
		{
			if (pTDI->sTitle != pTVDispInfo->item.pszText)
			{
				pTDI->sTitle = pTVDispInfo->item.pszText;
				pTDI->SetModified();
				
				SetModified(TRUE, TDCA_TASKNAME);
			}

			m_tree.TCH().InvalidateItem(pTVDispInfo->item.hItem);
		}
	}
	else // cancelled
	{
		// if the item was just added and cancelled then delete it
		// There's something funny going on under Vista that causes new subtasks
		// to be auto cancelled resulting in them being deleted. So for now we
		// disable the auto-delete-on-cancel feature of new tasks under Vista
		if (COSVersion() <= OSV_XP && pTVDispInfo->item.hItem == m_htiLastAdded)
		{
			CHoldRedraw hr(this);
			
			// make sure this item is not selected
			Selection().RemoveItem(m_htiLastAdded, TRUE);
			m_data.DeleteTask(m_htiLastAdded);
			
			// try selecting whatever was previous selected
			// note: its safe to say that whatever was previously
			// selected was a single item so we can select it explicitly
			if (Selection().PrevSelection(FALSE))
				SelectItem(Selection().GetFirstItem());
			
			else // else reselect what the tree control thinks is selected
				SelectItem(m_tree.GetSelectedItem());
			
			// reset cached calcs because we're not calling SetModified
			m_data.ResetCachedCalculations();

			// resync fields for selected task
			UpdateControls();
		}
		else
			m_tree.TCH().InvalidateItem(pTVDispInfo->item.hItem);
	}
	
	m_htiLastAdded = m_htiEditing = NULL; // reset
	*pResult = !bCancelled; // accept text
}

void CToDoCtrl::OnTreeBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	// can't edit multiple items. return TRUE to stop
	*pResult = (IsReadOnly() || GetSelectedCount() != 1);

	if (!(*pResult)) // good to edit
	{
		// set the edit text to only the task title
		HWND hEdit = (HWND)m_tree.SendMessage(TVM_GETEDITCONTROL);
		ASSERT (hEdit);

		TODOITEM* pTDI = GetTask(pTVDispInfo->item.lParam);
		ASSERT (pTDI && pTDI == GetSelectedTask());

		::SetWindowText(hEdit, (LPCSTR)(LPCTSTR)pTDI->sTitle);

		m_htiEditing = pTVDispInfo->item.hItem;
	}
}

BOOL CToDoCtrl::DeleteAllTasks()
{
	if (!GetTaskCount() || IsReadOnly())
		return FALSE;

	if (HasStyle(TDCS_CONFIRMDELETE))
	{
		if (MessageBoxEx(this, IDS_TDC_CONFIRMDELETEALL, IDS_TDC_CONFIRMDELETE_TITLE, 
			MB_YESNO | MB_DEFBUTTON2) == IDNO)
			return FALSE;
	}
	
	CHoldRedraw hr(this);
		
	// must do these first
	Selection().ClearHistory();
	Selection().RemoveAll(); 
	
	m_data.DeleteAllTasks();
	
	SetModified(TRUE, TDCA_NONE);
	UpdateControls();
	
	return TRUE;
}

void CToDoCtrl::SetStyle(TDC_STYLE nStyle, BOOL bOn)
{
	ASSERT (GetSafeHwnd());
	
	bOn = bOn ? TRUE : FALSE; // normalize
	
	if (bOn != HasStyle(nStyle))
	{
        m_aStyles[(int)nStyle] = (WORD)bOn;
		
		switch (nStyle)
		{
		case TDCS_SHOWDATESINISO:
			m_tree.RecalcGutter();
			break;
			
		case TDCS_SHOWINFOTIPS:
			m_tree.ModifyStyle(bOn ? 0 : TVS_INFOTIP, bOn ? TVS_INFOTIP : 0);

			if (bOn)
			{
				// make tooltips remain for as long as possible
				CToolTipCtrl* pTT = m_tree.GetToolTips();

				if (pTT)
					pTT->SetDelayTime(TTDT_AUTOPOP, 30000);
			}
			break;
			
		case TDCS_COLORTEXTBYPRIORITY:
		case TDCS_COLORTEXTBYCATEGORY:
		case TDCS_SHOWCOMMENTSINLIST:
		case TDCS_SHOWFIRSTCOMMENTLINEINLIST:
		case TDCS_USEEARLIESTDUEDATE:
		case TDCS_STRIKETHOUGHDONETASKS:
		case TDCS_TASKCOLORISBACKGROUND:
		case TDCS_SHOWSUBTASKCOMPLETION:
			Invalidate(FALSE);
			break;
			
		case TDCS_SORTDONETASKSATBOTTOM:
			m_bModSinceLastSort = TRUE;
			Resort();
			break;
			
		case TDCS_DISPLAYHMSTIMEFORMAT:
			Invalidate(FALSE);
			m_tree.RecalcGutter();
			break;

		case TDCS_TREATSUBCOMPLETEDASDONE:
//			m_data.RefreshAllCheckStates();
			Invalidate(FALSE);
			m_tree.RecalcGutter();
			break;
			
		case TDCS_USEHIGHESTPRIORITY:
		case TDCS_INCLUDEDONEINPRIORITYCALC:
		case TDCS_HIDEPRIORITYNUMBER:
			if (IsColumnShowing(TDCC_PRIORITY))
				m_tree.RedrawGutter();
			break;
			
		case TDCS_USEHIGHESTRISK:
			if (IsColumnShowing(TDCC_RISK))
				m_tree.RedrawGutter();
			break;
			
		case TDCS_SHOWNONFILEREFSASTEXT:
			if (IsColumnShowing(TDCC_FILEREF))
				m_tree.RecalcGutter();
			break;
			
		case TDCS_USEPERCENTDONEINTIMEEST:
			if (IsColumnShowing(TDCC_TIMEEST))
				m_tree.RecalcGutter();
			break;
			
		case TDCS_HIDEZEROTIMECOST:
			if (IsColumnShowing(TDCC_TIMEEST) || IsColumnShowing(TDCC_TIMESPENT) ||
				IsColumnShowing(TDCC_COST))
				m_tree.RecalcGutter();
			break;
			
		case TDCS_ROUNDTIMEFRACTIONS:
			if (IsColumnShowing(TDCC_TIMEEST) || IsColumnShowing(TDCC_TIMESPENT))
				m_tree.RecalcGutter();
			break;
			
		case TDCS_HIDEPERCENTFORDONETASKS:
		case TDCS_INCLUDEDONEINAVERAGECALC:
		case TDCS_WEIGHTPERCENTCALCBYTIMEEST:
		case TDCS_WEIGHTPERCENTCALCBYPRIORITY:
		case TDCS_WEIGHTPERCENTCALCBYNUMSUB:
		case TDCS_SHOWPERCENTASPROGRESSBAR:
			if (IsColumnShowing(TDCC_PERCENT))
				m_tree.RedrawGutter();
			break;
			
		case TDCS_AVERAGEPERCENTSUBCOMPLETION:
		case TDCS_AUTOCALCPERCENTDONE:
			UpdateControls(); // disable/enable some controls
			
			if (IsColumnShowing(TDCC_PERCENT))
				m_tree.RecalcGutter();
			else
				m_tree.RedrawGutter();
			break;
			
		case TDCS_HIDESTARTDUEFORDONETASKS:
			if (IsColumnShowing(TDCC_STARTDATE) || IsColumnShowing(TDCC_DUEDATE))
				m_tree.RedrawGutter();
			break;
			
		case TDCS_SHOWWEEKDAYINDATES:
			if (IsColumnShowing(TDCC_STARTDATE) || IsColumnShowing(TDCC_DUEDATE) ||
				IsColumnShowing(TDCC_DONEDATE))
				m_tree.RecalcGutter();
			break;
			
		case TDCS_MAXIMIZE:
			{
				// ensure focus is ok
				if (bOn)
					m_tree.SetFocus();
				
				CHoldRedraw hr(this);
				Resize();
			}
			break;
			
		case TDCS_SHOWPROJECTNAME:
			{
				// ensure focus is ok
				if (!bOn && GetFocus() == GetDlgItem(IDC_PROJECTNAME))
					m_tree.SetFocus();
				
				CHoldRedraw hr(this);
				Resize();
			}
			break;
			
		case TDCS_SHAREDCOMMENTSHEIGHT:
		case TDCS_VERTCOMMENTS:
//		case TDCS_RIGHTCOMMENTS:
//		case TDCS_BOTTOMRIGHTCOMMENTS:
//		case TDCS_BOTTOMCOMMENTS:
		case TDCS_SHOWCTRLSASCOLUMNS:
		case TDCS_SHOWCOMMENTSALWAYS:
		case TDCS_AUTOREPOSCTRLS:
			Resize();
			break;

		case TDCS_READONLY:
		case TDCS_CONFIRMDELETE:
		case TDCS_CHECKOUTONLOAD:
		case TDCS_ENABLESOURCECONTROL:
		case TDCS_SORTVISIBLETASKSONLY:
		case TDCS_AUTOCALCTIMEESTIMATES:
		case TDCS_FOCUSTREEONENTER:
		case TDCS_PAUSETIMETRACKINGONSCRNSAVER:
		case TDCS_TRACKSELECTEDTASKONLY:
		case TDCS_LOGTIMETRACKING:
		case TDCS_LOGTASKTIMESEPARATELY:
		case TDCS_WARNADDDELETEARCHIVE:
		case TDCS_RESORTONMODIFY:
		case TDCS_REFILTERONMODIFY:
		case TDCS_ALLOWPARENTTIMETRACKING:
		case TDCS_AUTOADJUSTDEPENDENTS:
			break;
			
		case TDCS_SHOWPATHINHEADER:
			if (!bOn)
				m_tree.SetGutterColumnHeaderTitle(TDCC_CLIENT, TDL_TASK);
			else
				UpdateSelectedTaskPath();
			break;
			
		case TDCS_FULLROWSELECTION:
			if (bOn)
				m_tree.ModifyStyle(TVS_HASLINES, TVS_FULLROWSELECT);
			else
				m_tree.ModifyStyle(TVS_FULLROWSELECT, TVS_HASLINES);
			break;
			
		case TDCS_TREECHECKBOXES:
			if (bOn)
				TreeView_SetImageList(m_tree, m_hilDone, TVSIL_STATE);
			else
				TreeView_SetImageList(m_tree, NULL, TVSIL_STATE);
			
			if (IsColumnShowing(TDCC_DONE))
				m_tree.RecalcGutter(); // to hide/display the 'done' column
			break;
			
		case TDCS_COLUMNHEADERCLICKING:
			UpdateColumnHeaderClicking();
			break;
			
		case TDCS_COMMENTSUSETREEFONT:
			if (bOn && m_fontTree)
				m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)m_fontTree, TRUE);
			
			else if (!bOn && m_fontComments)
				m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)m_fontComments, TRUE);
			else
				m_ctrlComments.SendMessage(WM_SETFONT, (WPARAM)CDialogHelper::GetFont(this), TRUE);
			break;
			
		case TDCS_RIGHTALIGNLABELS:
			SetLabelAlignment(bOn ? SS_RIGHT : SS_LEFT);
			break;
			
		case TDCS_SHOWPARENTSASFOLDERS:
			if (bOn)
				TreeView_SetImageList(m_tree, m_ilFileRef.GetHImageList(), TVSIL_NORMAL);
			else
				TreeView_SetImageList(m_tree, NULL, TVSIL_NORMAL);
			break;

/*		case TDCS_RTLCOMMENTS:
			m_ctrlComments.ModifyStyleEx(bOn ? 0 : WS_EX_RTLREADING, bOn ? WS_EX_RTLREADING : 0);
			break;
*/			
		default:
			ASSERT(0); // just to help catch forgotten styles
			break;
		}
		
		m_data.ResetCachedCalculations();
		
		UpdateControls();
	}
}

void CToDoCtrl::SetStyles(const CTDCStyles& styles)
{
	// first evaluate whether there any changes or not
	// without changing anything
	BOOL bChange = FALSE;
	int nStyle;
	
	for (nStyle = TDCS_FIRST; nStyle < TDCS_LAST && !bChange; nStyle++)
	{
		BOOL bWantOn = -1, bIsOn = HasStyle((TDC_STYLE)nStyle); // undefined
		
		if (styles.Lookup((TDC_STYLE)nStyle, bWantOn))
		{
			bWantOn = bWantOn ? 1 : 0; // normalize
			bChange = (bWantOn != bIsOn);
		}
	}
	
	if (!bChange)
		return;
	
	CHoldRedraw hr(this);
	
	for (nStyle = TDCS_FIRST; nStyle < TDCS_LAST; nStyle++)
	{
		BOOL bOn = -1; // undefined
		
		if (styles.Lookup((TDC_STYLE)nStyle, bOn))
			SetStyle((TDC_STYLE)nStyle, bOn);
	}
	
	m_tree.RecalcGutter();
}

BOOL CToDoCtrl::HasStyle(TDC_STYLE nStyle) const 
{ 
	// some special handling for dependent styles
	switch (nStyle)
	{
	case TDCS_CHECKOUTONLOAD:
		if (!HasStyle(TDCS_ENABLESOURCECONTROL))
            return FALSE;
		break;
		
	case TDCS_SHOWFIRSTCOMMENTLINEINLIST:
		if (!HasStyle(TDCS_SHOWCOMMENTSINLIST))
            return FALSE;
		break;

	case TDCS_LOGTASKTIMESEPARATELY:
		if (!HasStyle(TDCS_LOGTIMETRACKING))
            return FALSE;
		break;
	}
	
	return m_aStyles[nStyle] ? TRUE : FALSE; 
}

void CToDoCtrl::SetVisibleColumns(const CTDCColumnArray& aColumns)
{
	DWORD dwPrevCols = m_dwVisibleColumns;
	
	if (m_tree.IsGutterPosColumnShowing())
		dwPrevCols |= (2 << (int)TDCC_POSITION);

	m_dwVisibleColumns = 0;

	int nItem = (int)aColumns.GetSize();
	
	while (nItem--)
	{
		TDC_COLUMN nCol = aColumns[nItem];
		DWORD dwColumn = (2 << (int)nCol);

		m_dwVisibleColumns |= dwColumn;
	}

	if (m_dwVisibleColumns != dwPrevCols)
	{
		if (!m_tree.ShowGutterPosColumn(IsColumnShowing(TDCC_POSITION)))
			m_tree.RecalcGutter();
		
		// hide/show controls which may have been affected
		if (HasStyle(TDCS_SHOWCTRLSASCOLUMNS))
		{
			UpdateControls();
			
			// and resize to allow for control realignment
			Resize();
		}
	}
}


int CToDoCtrl::GetVisibleColumns(CTDCColumnArray& aColumns) const
{
	aColumns.RemoveAll();

	for (int nCol = TDCC_FIRST; nCol < TDCC_LAST; nCol++)
	{
		DWORD dwColumn = (2 << (int)nCol);

		if ((m_dwVisibleColumns & dwColumn) == dwColumn)
			aColumns.Add((TDC_COLUMN&)nCol);
	}

	return aColumns.GetSize();
}

BOOL CToDoCtrl::IsColumnShowing(TDC_COLUMN nColumn) const
{
	DWORD dwColumn = (2 << (int)nColumn);
	
	return ((m_dwVisibleColumns & dwColumn) == dwColumn);
}

TDC_FILE CToDoCtrl::Save(LPCTSTR szFilePath, BOOL bCheckforLaterChanges)
{
	CTaskFile tasks;

	return Save(tasks, szFilePath, bCheckforLaterChanges);
}

TDC_FILE CToDoCtrl::Save(CTaskFile& tasks/*out*/, LPCTSTR szFilePath, BOOL bCheckforLaterChanges)
{
	ASSERT (GetSafeHwnd());
	
	if (!GetSafeHwnd())
		return TDCO_OTHER;

	// can't save if file is readonly
	if (CDriveInfo::IsReadonlyPath(szFilePath) > 0)
		return TDCO_NOTALLOWED;
	
	// can't save if not checked-out
	// unless we're saving to another filename or this is our first save
	BOOL bFirstSave = (m_sLastSavePath.IsEmpty() || 
						(szFilePath && m_sLastSavePath.CompareNoCase(szFilePath) != 0));
	
	if (HasStyle(TDCS_ENABLESOURCECONTROL) && !m_bCheckedOut && !bFirstSave)
		return TDCO_OTHER;
	
	if (!szFilePath)
	{
		if (m_sLastSavePath.IsEmpty())
			return TDCO_OTHER;
		else
			szFilePath = m_sLastSavePath;
	}
	
	UpdateData(TRUE);
	UpdateComments(TRUE);
	
	// check for later changes (multi-user usage)
	if (bCheckforLaterChanges && m_nFileVersion > 0) // else its newly created
	{
		if (FileMisc::FileExists(szFilePath)) // file exists (sanity check)
		{
			// i was going to use filetimes but these are too unreliable
			// instead we open the xml file and look at its internal version
			CTaskFile temp;
			CXmlParseController xpc(TDL_FILEVERSION);
			
			if (temp.Load(szFilePath, &xpc, FALSE)) // FALSE => don't decrypt
			{
				if (temp.GetFileVersion() > m_nFileVersion)
				{
					CEnString sMessage(IDS_TDC_CONFIRMOVERWRITE, szFilePath);
					
					if (MessageBoxEx(this, sMessage, IDS_TDC_CONFIRMOVERWRITE_TITLE, 
						MB_ICONWARNING | MB_YESNO) == IDNO)
						return TDCO_CANCELLED;
				}
			}
		}
	}
	
	// get tasklist
	GetAllTasks(tasks);
	
	// sort tasks by ID to help users merge changes into 3rd party source control systems
	if (IsSortable())
		tasks.SortTasksByID();
	
	// file header info
	if (IsModified())
		m_nFileVersion++;
	
	tasks.SetHeader(m_sXmlHeader);
	tasks.SetLastModified(CDateHelper::FormatCurrentDate(TRUE));
	tasks.SetProjectName(m_sProjectName);
	tasks.SetArchive(m_bArchive);
	tasks.SetCustomCommentsType(m_idCommentsType);

	// encrypt prior to setting checkout status and file info (so these are visible without decryption)
	// this simply fails if password is empty
	tasks.Encrypt(m_sPassword);
	
	tasks.SetFileFormat(FILEFORMAT);
	tasks.SetFileVersion(m_nFileVersion);
	tasks.SetEarliestDueDate(m_data.GetEarliestDueDate());
	
	// checkout status
	// if this is a first time save and source control is enabled
	// then check it out
	if (HasStyle(TDCS_ENABLESOURCECONTROL) && (m_bCheckedOut || bFirstSave))
		tasks.SetCheckedOutTo(m_sMachineName);

	ASSERT (tasks.GetNextUniqueID() == m_dwNextUniqueID);

	// backup the file if opening in read-write
	CFileBackup backup(szFilePath);

	// do the save
	if (tasks.Save(szFilePath))
	{
		m_sLastSavePath = szFilePath;
		m_bModified = FALSE;
		m_bCheckedOut = tasks.IsCheckedOut();
		
		// update fileref with the todolist's folder
		m_eFileRef.SetCurrentFolder(FileMisc::GetFolderFromFilePath(szFilePath));

		// restore task positions if necessary
		if (IsSortable())
			tasks.SortTasksByPos();
		
		return TDCO_SUCCESS;
	}

	// restore and delete the backup
	backup.RestoreBackup();
	
	// else error handling
	int nFileErr = tasks.GetLastFileError();

	switch (nFileErr)
	{
	case XFL_BADMSXML:
		return TDCO_BADMSXML;

	default:
		// if nFileErr is greater than zero then it represents GetLastError
		// so we append this to TDCO_OTHER
		if (nFileErr > 0)
		{
			switch (nFileErr)
			{
			case ERROR_ACCESS_DENIED:		return TDCO_NOTALLOWED;
			case ERROR_SHARING_VIOLATION:	return TDCO_INUSE;

			default: return (TDC_FILE)(TDCO_OTHER + nFileErr);
			}
		}
		
		// all else
		return TDCO_OTHER;
	}
}

BOOL CToDoCtrl::WantExportColumn(TDC_COLUMN nColumn, BOOL bVisibleColsOnly) const
{
	return (!bVisibleColsOnly || IsColumnShowing(nColumn));
}

BOOL CToDoCtrl::CheckRestoreBackupFile(LPCTSTR szFilePath)
{
	// check for the existence of a backup file
	CString sBackup = CFileBackup::BuildBackupPath(szFilePath);

	if (FileMisc::FileExists(sBackup))
	{
		double dBackupSize = FileMisc::GetFileSize(sBackup);
		double dSize = FileMisc::GetFileSize(szFilePath);

		if (dSize == 0 && dBackupSize > 0) // definitely a bad save
		{
			::CopyFile(sBackup, szFilePath, FALSE);
		}
		else if (dBackupSize > 0 && dSize == dBackupSize) // same size == same file
		{
			::DeleteFile(sBackup);
		}
		else if (dBackupSize > dSize) // prompt
		{
			CEnString sMessage(IDS_BACKUPFILEFOUND, szFilePath);
			int nRet = AfxMessageBox(sMessage, MB_YESNOCANCEL);

			switch (nRet)
			{
			case IDYES:
				::CopyFile(sBackup, szFilePath, FALSE);
				break;

			case IDNO: // keep the backup just in case
				{
					CString sRename = CFileBackup::BuildBackupPath(sBackup);
					::CopyFile(sBackup, sRename, FALSE);
					::DeleteFile(sBackup);
				}

				break;

			case IDCANCEL:
				return FALSE; // do nothing
			}
		}
	}

	// all else
	return TRUE;
}

TDC_FILE CToDoCtrl::Load(LPCTSTR szFilePath, LPCTSTR szArchivePath, TDC_ARCHIVE nRemove, BOOL bRemoveFlagged)
{
	ASSERT (GetSafeHwnd());
	
	if (!GetSafeHwnd())
		return TDCO_OTHER;
	
	if (!CheckRestoreBackupFile(szFilePath))
		return TDCO_CANCELLED;

	if (!FileMisc::FileExists(szFilePath))
		return TDCO_NOTEXIST;

	CTaskFile file(m_sPassword);
	
	// work out whether we're going to write to the file
	// as part of the open -> checkout
	BOOL bWantCheckOut = HasStyle(TDCS_ENABLESOURCECONTROL) && HasStyle(TDCS_CHECKOUTONLOAD);
	BOOL bReadOnly = (CDriveInfo::IsReadonlyPath(szFilePath) > 0);

	XF_OPEN nMode = (bReadOnly || !bWantCheckOut) ? XF_READ : XF_READWRITE;

	// backup the file if opening in read-write
	CFileBackup backup;

	if (nMode == XF_READWRITE)
		backup.MakeBackup(szFilePath);

	if (!file.Open(szFilePath, nMode, FALSE))
	{
		// if opening in readwrite mode failed then try readonly
		if (nMode == XF_READWRITE && file.Open(szFilePath, XF_READ, FALSE))
		{
			// probably a share error
			bReadOnly = TRUE;
			bWantCheckOut = FALSE;
		}
		else
		{
			// do minor error handling
			switch (GetLastError())
			{
			case ERROR_ACCESS_DENIED:		return TDCO_NOTALLOWED;
			case ERROR_SHARING_VIOLATION:	return TDCO_INUSE;

			default:						return TDCO_OTHER;
			}
		}
	}
	
	if (file.LoadEx())
	{
		m_bSourceControlled = file.IsSourceControlled();

		// this is going to seem counter-intuitive but we check-out
		// the tasklist before decrypting it else we'll need to 
		// encrypt and resave it when we check it out. so we check it 
		// out first and if the decryption fails we must check it back in

		if (!bReadOnly && HasStyle(TDCS_ENABLESOURCECONTROL))
		{
			if (bWantCheckOut)
				m_bCheckedOut = file.CheckOut();
			else
				m_bCheckedOut = file.IsCheckedOut();
		}

		m_bSourceControlled = file.IsSourceControlled();
		
		if (file.Decrypt())
		{
			// save off password
			m_sPassword = file.GetPassword();
			
			// restore unsorted order
			file.SortTasksByPos();
			
			// custom comments
			GUID idType;
			
			if (file.GetCustomCommentsType(idType))
			{
				m_idCommentsType = idType;
				CreateContentControl();
			}
			
			Load(file, szArchivePath, nRemove, bRemoveFlagged);
			
			LoadSplitPos();
			
			// init fileref with the todolist's folder
			m_eFileRef.SetCurrentFolder(FileMisc::GetFolderFromFilePath(szFilePath));

			// lastly, if the tasklist does not have the 'earliest due date'
			// attribute in its header then we resave the tasklist to update
			// the format (relates to delayed loading)
			COleDateTime dtDummy;

			if (!file.GetEarliestDueDate(dtDummy))
			{
				file.Close(); // else file won't be writable
				Save(szFilePath, FALSE);
			}
			
			return TDCO_SUCCESS;
		}

		// else check back in if necessary
		if (HasStyle(TDCS_CHECKOUTONLOAD) && m_bCheckedOut)
		{
			file.SetCheckedOutTo("");
			file.Save(szFilePath);
		}
	}
	else if (nMode == XF_READWRITE) // restore the backup
		backup.RestoreBackup();

	// else do error handling
	int nFileErr = file.GetLastFileError();
	
	switch (nFileErr)
	{
	case XFL_CANCELLED:
		return TDCO_CANCELLED;
		
	case XFL_MISSINGROOT:
		return TDCO_NOTTASKLIST;
		
	case XFL_BADMSXML:
		return TDCO_BADMSXML;
		
	case XFL_NOENCRYPTIONDLL:
		return TDCO_NOENCRYPTIONDLL;
		
	case XFL_UNKNOWNENCRYPTION:
		return TDCO_UNKNOWNENCRYPTION;
	}

	// if nFileErr is greater than zero then it represents GetLastError
	// so we append this to TDCO_OTHER
	if (nFileErr > 0)
		return (TDC_FILE)(TDCO_OTHER + nFileErr);
	
	// all else
	return TDCO_OTHER;
}

BOOL CToDoCtrl::DelayLoad(LPCTSTR szFilePath, COleDateTime& dtEarliestDue)
{
	CTaskFile temp;
	
	if (temp.LoadHeader(szFilePath) && temp.GetEarliestDueDate(dtEarliestDue))
	{
		// save off some of the header info
		SetFilePath(szFilePath);
		SetProjectName(temp.GetProjectName());
		SetModified(FALSE);

		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CToDoCtrl::Load(const CTaskFile& file, LPCTSTR szArchivePath, TDC_ARCHIVE nRemove, BOOL bRemoveFlagged)
{
	ASSERT (GetSafeHwnd());
	
	if (!GetSafeHwnd())
		return FALSE;

	m_tree.ShowWindow(SW_HIDE);
	SaveExpandedState();
	
	// file header info
	m_sXmlHeader = file.GetHeader();
	m_nFileFormat = file.GetFileFormat();
	m_nFileVersion = file.GetFileVersion();
	m_sProjectName = file.GetProjectName();
	m_bArchive = file.IsArchive();
	
	// next unique ID
	m_dwNextUniqueID = file.GetNextUniqueID();
	
	// backwards compatibility
	if (!m_dwNextUniqueID)
		m_dwNextUniqueID = 1;
	
	m_sLastSavePath = file.GetFilePath();
	
	// sorting
	LoadSortState(file.GetFilePath());
	m_tree.PressGutterColumnHeader((UINT)-1);

	if (file.GetTaskCount())
	{
		CHoldRedraw hr(m_tree);
	
		HTREEITEM htiFirst = SetAllTasks(file);
		
		if (szArchivePath && *szArchivePath && 
			ArchiveDoneTasks(szArchivePath, nRemove, bRemoveFlagged, FALSE))
		{
			m_bModified = TRUE;
		}
		else
			m_bModified = FALSE;

		// reset cached calcs after loading but before sorting
		m_data.ResetCachedCalculations();

		// redo last sort
		if (IsSortable())
		{
			TDCCOLUMN* pTDCC = GetColumn(m_nSortBy);
			ASSERT (pTDCC);
			
			m_tree.SetGutterColumnSort(pTDCC->nColID, m_bSortAscending ? NCGSORT_UP : NCGSORT_DOWN);
			m_data.Sort(m_nSortBy, m_bSortAscending);

			m_bModSinceLastSort = FALSE;
		}

		// restore last expanded state
		HTREEITEM htiSel = LoadExpandedState(FALSE);
		
		if (!htiSel)
			htiSel = htiFirst;
		
		if (htiSel)
		{
			SelectItem(htiSel);
			m_tree.TCH().EnsureVisibleEx(htiSel, FALSE);
		}
	}
	else
		m_bModified = FALSE;

	// reset last taskedit
	m_tLastTaskMod = COleDateTime::GetCurrentTime();

	m_tree.ShowWindow(SW_SHOW);

	if (IsWindowVisible())
		m_tree.SetFocus();

	UpdateData(FALSE);
	UpdateComments(FALSE);

	return TRUE;
}

int CToDoCtrl::ArchiveDoneTasks(LPCTSTR szArchivePath, TDC_ARCHIVE nRemove, BOOL bRemoveFlagged, BOOL bHoldRedraw)
{
	// can't archive archives
	if (m_bArchive)
		return 0;
	
	if (!GetTaskCount() || !szArchivePath || !*szArchivePath)
		return 0;
	
	// 1. first see if there's anything to archive
	CTaskFile xiDone;
	
	if (!GetTasks(xiDone, TDCF_DONE))
		return 0;
	
	// 2. load existing archive if present
	CTaskFile file(m_sPassword);
	
	if (file.Load(szArchivePath))
	{
		VERIFY(file.IsArchive());
		
		int nFileVer = file.GetFileVersion() + 1;
		file.SetFileVersion(max(nFileVer, 1));
	}
	else // or initialize first time
	{
		file.SetArchive(true);
		file.SetFileFormat(FILEFORMAT);
		file.SetFileVersion(1);
		file.SetLastModified(CDateHelper::FormatCurrentDate(TRUE));
		file.SetProjectName(CEnString("%s (%s)", m_sProjectName, CEnString(IDS_TDC_ARCHIVEPROJECT)));
		file.SetNextUniqueID(1);
	}
	
	// 3. merge in new done items
	file.Merge(xiDone, TRUE, TRUE);
	
	// 4. save archive
	file.Encrypt();
	file.Save(szArchivePath);
	
	// 5. remove archived tasks from current list
	if (!IsReadOnly() && nRemove != TDC_REMOVENONE)
	{
		file.Reset();
		
		// filter
		TDC_FILTER nFilter = TDCF_ALL;
		
		switch (nRemove)
		{
		case TDC_REMOVEALL:
			nFilter = TDCF_NOTDONE;
			break;
			
		case TDC_REMOVEIFSIBLINGSANDSUBTASKSCOMPLETE:
			nFilter = TDCF_NOTFULLYDONE;
			break;
			
		default:
			ASSERT (0);
		}
		
		// clear selection
		Selection().RemoveAll();
		
		BOOL bHasTasks = GetTasks(file, nFilter, bRemoveFlagged ? 0 : TDCGT_KEEPFLAGGED);

		if (bHasTasks)
		{
			if (bHoldRedraw)
				m_tree.SetRedraw(FALSE);
			
			m_data.DeleteAllTasks();
			AddItem(file, NULL);
		
			// we only need to this if we've rebuilt the tree
			LoadExpandedState();

			if (bHoldRedraw)
			{
				m_tree.SetRedraw(TRUE);
				CRedrawAll ra(m_tree);
			}
		}
		else
			m_data.DeleteAllTasks();

		SetModified(TRUE);
	}
	
	return (nRemove == TDC_REMOVEALL);
}

int CToDoCtrl::ArchiveDoneTasks(LPCTSTR szFilePath, TDC_ARCHIVE nRemove, BOOL bRemoveFlagged)
{
	SaveExpandedState();

	// this is only called externally so we need to specify flicker free
	int nRes = ArchiveDoneTasks(szFilePath, nRemove, bRemoveFlagged, TRUE);
	
	if (nRes)
		SetModified(TRUE, TDCA_NONE);
	
	return nRes;
}

BOOL CToDoCtrl::Import(LPCTSTR szFilePath, BOOL bAppend)
{
	if (IsReadOnly())
		return FALSE;
	
	CTaskFile fileSrc(m_sPassword);
	
	if (!fileSrc.Load(szFilePath))
		return FALSE;

	return SetTasks(fileSrc, bAppend);
}

HTREEITEM CToDoCtrl::AddItem(const CTaskFile& file, HTASKITEM hTask, HTREEITEM htiParent, HTREEITEM htiAfter, TDC_RESETIDS nResetID)
{
	HTREEITEM hti = TVI_ROOT; // default for root item
	TODOITEM* pTDI = NULL;
	
	if (hTask)
	{
		// add new task to map
		pTDI = m_data.NewTask();
		
		DWORD dwUniqueID = file.GetTaskID(hTask);
		
		pTDI->tLastMod = file.GetTaskLastModifiedOle(hTask);
		pTDI->sTitle = file.GetTaskTitle(hTask);
		pTDI->sComments = file.GetTaskComments(hTask);
		file.GetTaskCustomComments(hTask, pTDI->sCustomComments);
//		pTDI->sAllocTo = file.GetTaskAllocatedTo(hTask);
		pTDI->sAllocBy = file.GetTaskAllocatedBy(hTask);
		pTDI->sStatus = file.GetTaskStatus(hTask);
		pTDI->sCreatedBy = file.GetTaskCreatedBy(hTask);
		pTDI->bFlagged = file.IsTaskFlagged(hTask);
		pTDI->color = (COLORREF)file.GetTaskColor(hTask);
		pTDI->nPercentDone = (int)file.GetTaskPercentDone(hTask, FALSE);
		pTDI->sFileRefPath = file.GetTaskFileReferencePath(hTask);
		pTDI->dTimeEstimate = file.GetTaskTimeEstimate(hTask, (char&)pTDI->nTimeEstUnits, FALSE);
		pTDI->dTimeSpent = file.GetTaskTimeSpent(hTask, (char&)pTDI->nTimeSpentUnits, FALSE);
		pTDI->nPriority = (int)file.GetTaskPriority(hTask, FALSE);
		pTDI->dateDue = file.GetTaskDueDateOle(hTask);
		pTDI->dateStart = file.GetTaskStartDateOle(hTask);
		pTDI->dateDone = file.GetTaskDoneDateOle(hTask);
		pTDI->dateCreated = file.GetTaskCreationDateOle(hTask);
		pTDI->nRisk = file.GetTaskRisk(hTask, FALSE);
		pTDI->sExternalID = file.GetTaskExternalID(hTask);
		pTDI->dCost = file.GetTaskCost(hTask, FALSE);
		pTDI->sDependency = file.GetTaskDependency(hTask);
		pTDI->sVersion = file.GetTaskVersion(hTask);

		file.GetTaskCategories(hTask, pTDI->aCategories);
		file.GetTaskAllocatedTo(hTask, pTDI->aAllocTo);
		file.GetTaskRecurrence(hTask, pTDI->trRecurrence);
		
		// make sure that m_dwUniqueID exceeds this ID
		if (dwUniqueID && nResetID == TDCR_NO)
			m_dwNextUniqueID = max(m_dwNextUniqueID, dwUniqueID + 1);

		else if (dwUniqueID && nResetID == TDCR_CHECK)
		{
			// see if it already exist
			if (m_data.GetTask(dwUniqueID))
				dwUniqueID = m_dwNextUniqueID++; // provide a new unique ID
			else // make sure that m_dwUniqueID exceeds this ID
				m_dwNextUniqueID = max(m_dwNextUniqueID, dwUniqueID + 1);
		}
		else 
			dwUniqueID = m_dwNextUniqueID++;
		
		// add items to map
		m_data.AddTask(dwUniqueID, pTDI);
		
		// add this item to tree
		BOOL bBold = (htiParent == TVI_ROOT);
		hti = m_tree.InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
								LPSTR_TEXTCALLBACK,
								I_IMAGECALLBACK, 
								I_IMAGECALLBACK, 
								(bBold ? TVIS_BOLD : 0),
								TVIS_BOLD, // state mask
								dwUniqueID, // lParam
								htiParent, 
								htiAfter);
		
	
		// add unique items to comboboxes
//		m_cbAllocTo.AddUniqueItem(pTDI->sAllocTo);
		m_cbAllocTo.AddUniqueItems(pTDI->aAllocTo);
		m_cbAllocBy.AddUniqueItem(pTDI->sAllocBy);
		m_cbStatus.AddUniqueItem(pTDI->sStatus);
		m_cbCategory.AddUniqueItems(pTDI->aCategories);
		m_cbVersion.AddUniqueItem(pTDI->sVersion);
	}
	
	// then children
	HTREEITEM htiFirstItem = (hti == TVI_ROOT) ? NULL : hti;
	HTASKITEM hChildTask = file.GetFirstTask(hTask);
	
	while (hChildTask)
	{
		HTREEITEM htiChild = AddItem(file, hChildTask, hti, TVI_LAST, nResetID);
		
		if (!htiFirstItem)
			htiFirstItem = htiChild;
		
		hChildTask = file.GetNextTask(hChildTask); // next
	}

	// check state (may depend on children)
    if (pTDI)
		m_tree.TCH().SetItemChecked(hti, (pTDI->dateDone > 0));
//		m_data.RefreshItemCheckState(hti, (pTDI->dateDone > 0));
		
	return htiFirstItem;
}

void CToDoCtrl::OnTreeGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO lptvdi = (LPNMTVDISPINFO)pNMHDR;
	*pResult = 0;
	
	UINT nMask = lptvdi->item.mask;
	HTREEITEM hti = lptvdi->item.hItem;
	
	// image
	if (nMask & (TVIF_SELECTEDIMAGE | TVIF_IMAGE))
	{
		BOOL bHasChildren = m_tree.ItemHasChildren(hti);
		int nImage = (bHasChildren ? m_ilFileRef.GetFolderImageIndex() : -1);
		
		lptvdi->item.iImage = lptvdi->item.iSelectedImage = nImage;
	}
	
	// title text (unless it's being edited)
	if ((nMask & TVIF_TEXT) &&  m_htiEditing != hti)
	{
		TODOITEM* pTDI = GetTask(lptvdi->item.lParam);
		ASSERT (pTDI);
		
		if (HasStyle(TDCS_SHOWSUBTASKCOMPLETION))
		{
			int nSubtasksCount, nSubtasksDone;
			
			if (m_data.GetSubtaskTotals(hti, pTDI, nSubtasksCount, nSubtasksDone))
			{
				static CString sText;
				sText.Format("(%d/%d) %s", nSubtasksDone, nSubtasksCount, pTDI->sTitle);
				
				lptvdi->item.pszText = (LPSTR)(LPCTSTR)sText;
				return;
			}
		}

		// all else
		lptvdi->item.pszText = (LPSTR)(LPCTSTR)pTDI->sTitle;
	}
}

void CToDoCtrl::OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCUSTOMDRAW* pNMCD = (NMCUSTOMDRAW*)pNMHDR;
	
	if (pNMCD->dwDrawStage == CDDS_PREPAINT)
		*pResult |= CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;	
	
	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		NMTVCUSTOMDRAW* pTVCD = (NMTVCUSTOMDRAW*)pNMCD;
		
		DWORD dwItemID = (DWORD)pTVCD->nmcd.lItemlParam;
		TODOITEM* pTDI = GetTask(dwItemID);
		
		// set fonts and colors
		if (pTDI)
		{
			HTREEITEM hti = (HTREEITEM)pTVCD->nmcd.dwItemSpec;

			TDI_STATE nState = GetItemState(hti);
			BOOL bDone = m_data.IsTaskDone(hti);
			BOOL bGoodAsDone = m_data.IsTaskDone(hti, TDCCHECKALL);
			
			switch (nState)
			{
			case TDIS_DROPHILITED:
			case TDIS_SELECTEDNOTFOCUSED:
				pTVCD->clrText = GetSysColor(COLOR_WINDOWTEXT);
				pTVCD->clrTextBk = GetSysColor(COLOR_3DFACE);
				
				*pResult |= CDRF_NEWFONT;
				break;
				
			case TDIS_SELECTED:
				pTVCD->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
				pTVCD->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
				
				*pResult |= CDRF_NEWFONT;
				break;
				
			case TDIS_NONE:
			default:
				if (GetTaskTextColors(hti, pTDI, (bDone || bGoodAsDone), pTVCD->clrText, pTVCD->clrTextBk))
					*pResult |= CDRF_NEWFONT;
				break;
			}
			
			// also
			if (bDone && HasStyle(TDCS_STRIKETHOUGHDONETASKS))
			{
				::SelectObject(pNMCD->hdc, m_fontDone);
				*pResult |= CDRF_NEWFONT;
			}

			*pResult |= CDRF_NOTIFYPOSTPAINT;	
		}
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPOSTPAINT)
	{
		NMTVCUSTOMDRAW* pTVCD = (NMTVCUSTOMDRAW*)pNMCD;

		HTREEITEM hti = (HTREEITEM)pTVCD->nmcd.dwItemSpec;
		TODOITEM* pTDI = GetTask((DWORD)pTVCD->nmcd.lItemlParam);

		if (!pTDI)
			return;

		CDC* pDC = CDC::FromHandle(pNMCD->hdc);
		int nDC = pDC->SaveDC();

		CRect rText;
		m_tree.GetItemRect((HTREEITEM)pTVCD->nmcd.dwItemSpec, rText, TRUE);
		
		// render (num done / total)
		if (HasStyle(TDCS_SHOWSUBTASKCOMPLETION) && m_tree.ItemHasChildren(hti) && 	
			GetItemState(hti) != TDIS_SELECTED)
		{
			int nSubtasksCount, nSubtasksDone;
		
			if (m_data.GetSubtaskTotals(hti, pTDI, nSubtasksCount, nSubtasksDone))
			{
				// only need to redraw if not complete because it'll
				// already be the right colour
				if (!m_data.IsTaskDone(hti, TDCCHECKALL))
				{
					CString sText;
					sText.Format("(%d/%d)", nSubtasksDone, nSubtasksCount);

					HFONT hFontOld = NULL;

					if (m_tree.GetParentItem(hti) == NULL)
						hFontOld = (HFONT)::SelectObject(pNMCD->hdc, m_fontBold);

					if (HasStyle(TDCS_TASKCOLORISBACKGROUND))
					{
						COLORREF crText, crBack;
						GetTaskTextColors(hti, pTDI, FALSE, crText, crBack);
						pDC->SetTextColor(crText);
					}
					else
						pDC->SetTextColor(COMMENTSCOLOR);
					
					pDC->SetBkMode(TRANSPARENT);
					pDC->ExtTextOut(rText.left + 2, rText.top + 1, ETO_CLIPPED, rText, sText, NULL);

					::SelectObject(pNMCD->hdc, hFontOld);
				}
			}
		}
		
		// render comment text
		if (!pTDI->sComments.IsEmpty())
		{
			BOOL bSelected = IsItemSelected(hti);
			BOOL bFullRow = (m_tree.GetStyle() & TVS_FULLROWSELECT);
			
			CString sTemp;
			CEnString sComments(IDS_TDC_SEECOMMENTS);
			
			// if we're showing the comments but not the first line
			// and the first line is empty then scoot
			if (HasStyle(TDCS_SHOWCOMMENTSINLIST))
			{
				if (HasStyle(TDCS_SHOWFIRSTCOMMENTLINEINLIST))
				{
					int nFind = pTDI->sComments.FindOneOf("\n\r");
					
					if (nFind != -1)
						sTemp = pTDI->sComments.Left(nFind);
					else
						sTemp = pTDI->sComments;
					
					if (sTemp.IsEmpty())
						return;
				}
				else
					sTemp = pTDI->sComments;
				
				sComments.Format("[%s]", sTemp);
				
				if (sComments.Find('\r') >= 0 || sComments.Find('\n') >= 0)
				{
					sComments.Replace("\r\n", " ");
					sComments.Replace('\n', ' ');
					sComments.Replace('\r', ' ');
				}
			}
			
			BOOL bFullRowSelected = (bSelected && bFullRow);
			
			if (!bFullRowSelected)
			{
				// use 'done' color if done, else use grey unless the window
				// background is a mid-grey
				COLORREF crComments = COMMENTSCOLOR;

				if (m_data.IsTaskDone(hti, TDCCHECKALL))
					crComments = m_crTaskDone;
				else
				{
					RGBX rgbBkGnd(GetSysColor(COLOR_WINDOW));

					if (rgbBkGnd.IsGray(2) && rgbBkGnd.Luminance() > 48 && rgbBkGnd.Luminance() < 208)
						crComments = (rgbBkGnd.Luminance() < 128) ? RGB(255, 255, 255) : 0;
				}

				pDC->SetTextColor(crComments);
			}
			else
			{
				if (pNMCD->uItemState & CDIS_FOCUS)
					pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				else
					pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			}
			
			if (m_data.IsTaskDone(hti) && m_fontDone)
				pDC->SelectObject(m_fontDone);
			
			CRect rText;
			m_tree.GetItemRect((HTREEITEM)pTVCD->nmcd.dwItemSpec, rText, TRUE);

			rText.top++;
			rText.left = rText.right + 4;
			rText.right = pTVCD->nmcd.rc.right;

			pDC->SetBkMode(TRANSPARENT);
			pDC->ExtTextOut(rText.left, rText.top, ETO_CLIPPED, &rText, sComments, NULL);
		}

		pDC->RestoreDC(nDC);
	}
}

BOOL CToDoCtrl::GetTaskTextColors(HTREEITEM hti, const TODOITEM* pTDI, BOOL bDone,
								 COLORREF& crText, COLORREF& crBack)
{
	COLORREF crDefText = GetSysColor(COLOR_WINDOWTEXT);

	crText = crDefText;
	crBack = m_tree.GetItemLineColor(hti);

	if (bDone)
		crText = m_crTaskDone; // parent and/or item is done
	
	else if (m_data.IsTaskDue(hti, pTDI)) // due today or overdue
	{
		BOOL bDueToday = m_data.IsTaskDue(hti, pTDI, TRUE);

		if (bDueToday)
		{
			// do we have a custom 'due today' color
			if (m_crDueToday != NOCOLOR)
				crText = m_crDueToday;

			else if (HasStyle(TDCS_COLORTEXTBYPRIORITY))
			{
				int nPriority = m_data.GetHighestPriority(hti, pTDI, FALSE);

				if (nPriority != FT_NOPRIORITY)
					crText = GetPriorityColor(nPriority); 
			}
		}
		else // over due
		{
			if (m_crDue != NOCOLOR)
				crText = m_crDue;

			else if (HasStyle(TDCS_COLORTEXTBYPRIORITY))
				crText = GetPriorityColor(10);
			else
				crText = 255; // red
		}
	}
	else if (HasStyle(TDCS_COLORTEXTBYPRIORITY))
	{
		int nPriority = m_data.GetHighestPriority(hti, pTDI);

		if (nPriority != FT_NOPRIORITY)
			crText = GetPriorityColor(nPriority); 
	}
	else if (HasStyle(TDCS_COLORTEXTBYCATEGORY))
	{
		GetCategoryColor(pTDI->GetFirstCategory(), crText);
	}
	else if (pTDI->color)
		crText = pTDI->color; 

	if (crText != crDefText)
	{
		if (HasStyle(TDCS_TASKCOLORISBACKGROUND) && !bDone)
		{
			crBack = crText;
			
			// base text color on luminance
			crText = (RGBX(crBack).Luminance() < 128) ? RGB(255, 255, 255) : 0;
		}

		return TRUE;
	}

	return FALSE; // noc change
}

CToDoCtrl::TDI_STATE CToDoCtrl::GetItemState(HTREEITEM hti)
{
	if (m_tree.GetItemState(hti, TVIS_DROPHILITED) & TVIS_DROPHILITED)
		return TDIS_DROPHILITED;
	
	else if (IsItemSelected(hti))
		return (m_tree.TCH().HasFocus() ? TDIS_SELECTED : TDIS_SELECTEDNOTFOCUSED);
	
	return TDIS_NONE;
}

void CToDoCtrl::OnTreeChangeFocus(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	Selection().InvalidateAll(FALSE);
	*pResult = 0;
}

void CToDoCtrl::OnTreeClick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	*pResult = 0;
}

void CToDoCtrl::OnTreeDblClk(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	CPoint point(::GetMessagePos());
	m_tree.ScreenToClient(&point);
	
	UINT nHitFlags = 0;
	HTREEITEM htiHit = m_tree.HitTest(point, &nHitFlags);
	
	if (!IsReadOnly() && htiHit && (nHitFlags & TVHT_ONITEMLABEL) && !m_tree.ItemHasChildren(htiHit))
	{
		// we have to post this for reasons i haven't been able to figure.
		// if we send it, the edit finishes immediately
		// (later) i now think that this may be related to the lbuttonup which follows this
		m_tree.PostMessage(TVM_EDITLABEL, 0, (LPARAM)htiHit);
	}
	
	*pResult = 0;
}

void CToDoCtrl::OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVKEYDOWN pTVKey = (LPNMTVKEYDOWN)pNMHDR;
	m_wKeyPress = 0;
	
	// cursor handled in OnTreeSelChanged
	// <shift>+cursor handled in OnTreeSelChanged
	// <ctrl>+cursor handled in PreTranslateMessage
	// <ctrl>+<shift>+cursor handled in PreTranslateMessage
	
	// all we do is get a copy of the key pressed for reference
	switch (pTVKey->wVKey)
	{
	case VK_NEXT:  
	case VK_DOWN:
	case VK_UP:
	case VK_PRIOR: 
	case VK_RIGHT:
	case VK_LEFT:
	case VK_HOME:
	case VK_END:
		m_wKeyPress = pTVKey->wVKey;
		break;
		
	default:
		// handle alphanum method of changing selection
		{
			// convert to char because its easier to work out what
			// are valid chars
			UINT nChar = MapVirtualKey(pTVKey->wVKey, 2);

			if (nChar >= 0x20 && nChar <= 0xFF)
				m_wKeyPress = pTVKey->wVKey;
		}
		break;
	}

	*pResult = 0;
}

void CToDoCtrl::PreSubclassWindow() 
{
	if (!m_fontDone && HasStyle(TDCS_STRIKETHOUGHDONETASKS)) // create first time
		m_fontDone = Misc::CreateFont(NULL, -1, Misc::STRIKETHRU);

	CRuntimeDlg::PreSubclassWindow();
}

void CToDoCtrl::OnDestroy() 
{
	// save state
	SaveExpandedState();
	SaveSplitPos();
	SaveSortState();
	
	// reset copied item if its us
	if (s_clipboard.hwndToDoCtrl == GetSafeHwnd())
		s_clipboard.hwndToDoCtrl = NULL;
	
	CRuntimeDlg::OnDestroy();
}

void CToDoCtrl::SetModified(BOOL bMod)
{
	if (!IsReadOnly())
	{
		m_bModified = bMod;
		m_bModSinceLastSort = TRUE; // always

		if (bMod)
			m_tLastTaskMod = COleDateTime::GetCurrentTime();
	}
}

// internal version
void CToDoCtrl::SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib)
{
	if (IsReadOnly())
		return;
	
	SetModified(bMod);
	
	if (bMod)
	{
		// redraw the gutter if the associated column is visible
		BOOL bRedraw = FALSE, bResort = ModNeedsResort(nAttrib);
		UINT nRecalcColID = 0;
		
		switch (nAttrib)
		{
		case TDCA_DONEDATE:
			bRedraw = IsColumnShowing(TDCC_DONEDATE);
			Invalidate(); // text color can be affected
			
			if (IsColumnShowing(TDCC_TIMEEST) && HasStyle(TDCS_USEPERCENTDONEINTIMEEST))
				nRecalcColID = TDCC_TIMEEST;
			break;
			
		case TDCA_DUEDATE:
			bRedraw = IsColumnShowing(TDCC_DUEDATE) || IsColumnShowing(TDCC_PRIORITY);
			Invalidate(); // text color can be affected 
			break;
			
		case TDCA_STARTDATE:
			bRedraw = IsColumnShowing(TDCC_STARTDATE);
			break;
			
		case TDCA_EXTERNALID:
			if (IsColumnShowing(TDCC_EXTERNALID))
				nRecalcColID = TDCC_EXTERNALID;
			break;
			
		case TDCA_RISK:
			bRedraw = IsColumnShowing(TDCC_RISK);
			break;
			
		case TDCA_RECURRENCE:
			bRedraw = IsColumnShowing(TDCC_RECURRENCE);
			break;
			
		case TDCA_DEPENDENCY:
			bRedraw = IsColumnShowing(TDCC_DEPENDENCY);
			break;
			
		case TDCA_PRIORITY:
			bRedraw = IsColumnShowing(TDCC_PRIORITY);
			
			// text color can be affected 
			if (HasStyle(TDCS_COLORTEXTBYPRIORITY))
				Invalidate(); 
			break;
			
		case TDCA_COLOR:
			// text color can be affected 
			if (!HasStyle(TDCS_COLORTEXTBYPRIORITY))
				Invalidate(); 
			break;
			
		case TDCA_ALLOCTO:
			if (IsColumnShowing(TDCC_ALLOCTO))
				nRecalcColID = TDCC_ALLOCTO;
			break;
			
		case TDCA_ALLOCBY:
			if (IsColumnShowing(TDCC_ALLOCBY))
				nRecalcColID = TDCC_ALLOCBY;
			break;
			
		case TDCA_STATUS:
			if (IsColumnShowing(TDCC_STATUS))
				nRecalcColID = TDCC_STATUS;
			break;
			
		case TDCA_VERSION:
			if (IsColumnShowing(TDCC_VERSION))
				nRecalcColID = TDCC_VERSION;
			break;
			
		case TDCA_CATEGORY:
			if (IsColumnShowing(TDCC_CATEGORY))
				nRecalcColID = TDCC_CATEGORY;
			break;
			
		case TDCA_PERCENT:
			bRedraw = IsColumnShowing(TDCC_PERCENT);
			Invalidate(); // text color can be affected 
			break;
			
		case TDCA_TIMEEST:
			if (IsColumnShowing(TDCC_TIMEEST))
				nRecalcColID = TDCC_TIMEEST;
			break;
			
		case TDCA_TIMESPENT:
			if (IsColumnShowing(TDCC_TIMESPENT))
				nRecalcColID = TDCC_TIMESPENT;
			break;
			
		case TDCA_COST:
			if (IsColumnShowing(TDCC_COST))
				nRecalcColID = TDCC_COST;
			break;
			
		case TDCA_FILEREF:
			bRedraw = IsColumnShowing(TDCC_FILEREF);
			break;

		case TDCA_FLAG:
		case TDCA_TASKNAME:
		case TDCA_PROJNAME:
		case TDCA_COMMENTS:
		case TDCA_NONE:
			break;

		default:
			ASSERT(0);
			break;
		}
		
		m_data.ResetCachedCalculations();
		
		if (HasStyle(TDCS_SHOWSUBTASKCOMPLETION))
			UpdateSelectedTaskPath();
		
		if (nRecalcColID)
		{
			if (!m_tree.RecalcGutterColumn(nRecalcColID))
				m_tree.RedrawGutter();
		}
		else if (bRedraw)
			m_tree.RedrawGutter();

		if (bResort)
			Resort();
		
		UpdateWindow();
		GetParent()->SendMessage(WM_TDCN_MODIFY, GetDlgCtrlID(), (LPARAM)nAttrib);

		// special case: if this was the project name being edited make sure
		// the focus is set back to the name
		if (nAttrib == TDCA_PROJNAME)
			GetDlgItem(IDC_PROJECTNAME)->SetFocus();
	}
}

BOOL CToDoCtrl::ModNeedsResort(TDC_ATTRIBUTE nModType)
{
	if (HasStyle(TDCS_RESORTONMODIFY))
	{
		switch (nModType)
		{
		case TDCA_TASKNAME:		return (TDC_SORTBYNAME == m_nSortBy);
		case TDCA_STARTDATE:	return (TDC_SORTBYSTARTDATE == m_nSortBy);
		case TDCA_PRIORITY:		return (TDC_SORTBYPRIORITY == m_nSortBy);
		case TDCA_COLOR:		return (TDC_SORTBYCOLOR == m_nSortBy);
		case TDCA_ALLOCTO:		return (TDC_SORTBYALLOCTO == m_nSortBy);
		case TDCA_ALLOCBY:		return (TDC_SORTBYALLOCBY == m_nSortBy);
		case TDCA_PERCENT:		return (TDC_SORTBYPERCENT == m_nSortBy);
		case TDCA_STATUS:		return (TDC_SORTBYSTATUS == m_nSortBy);
		case TDCA_CATEGORY:		return (TDC_SORTBYCATEGORY == m_nSortBy);
		case TDCA_TIMEEST:		return (TDC_SORTBYTIMEEST == m_nSortBy);
		case TDCA_RISK:			return (TDC_SORTBYRISK == m_nSortBy);
		case TDCA_EXTERNALID:	return (TDC_SORTBYEXTERNALID == m_nSortBy);
		case TDCA_TIMESPENT:	return (TDC_SORTBYTIMESPENT == m_nSortBy);
		case TDCA_VERSION:		return (TDC_SORTBYVERSION == m_nSortBy);
		case TDCA_RECURRENCE:	return (TDC_SORTBYRECURRENCE == m_nSortBy);

		case TDCA_DONEDATE:
			if (HasStyle(TDCS_SORTDONETASKSATBOTTOM))
				return TRUE;
			
			// else also check other attribs affected by completed items
			return (TDC_SORTBYPERCENT == m_nSortBy || 
					TDC_SORTBYDONEDATE == m_nSortBy ||
					TDC_SORTBYDONE == m_nSortBy ||
					TDC_SORTBYRISK == m_nSortBy ||
					TDC_SORTBYPRIORITY == m_nSortBy ||
					TDC_SORTBYSTARTDATE == m_nSortBy ||
					TDC_SORTBYDUEDATE == m_nSortBy);
			
		case TDCA_DUEDATE:
			// also check priority
			return (TDC_SORTBYDUEDATE == m_nSortBy || TDC_SORTBYPRIORITY == m_nSortBy);
		}
	}

	// all else
	return FALSE;
}

LRESULT CToDoCtrl::OnChangeComments(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (!m_ctrlComments.SettingContent())
		UpdateTask(TDCA_COMMENTS);

	return 0L;
}

void CToDoCtrl::OnChangeFileRefPath()
{
	UpdateTask(TDCA_FILEREF);
}

void CToDoCtrl::OnChangeProjectName()
{
	UpdateData();
	SetModified(TRUE, TDCA_PROJNAME);
}

void CToDoCtrl::InvalidateSelectedItem()
{
	InvalidateItem(GetSelectedItem());
}

void CToDoCtrl::InvalidateItem(HTREEITEM hti)
{
	CRect rItem;
	
	if (m_tree.GetItemRect(hti, rItem, FALSE))
		m_tree.InvalidateRect(rItem, FALSE);
}

void CToDoCtrl::Sort(TDC_SORTBY nBy)
{
	if (nBy == TDC_SORTDISABLED || nBy == TDC_SORTUNDEF)
	{
		TDC_SORTBY nPrevSortBy = m_nSortBy;

		m_tree.SetGutterColumnSort(0, NCGSORT_NONE);
		m_nSortBy = nBy;

		// disable column header clicking
		UpdateColumnHeaderClicking();

		// mark as modified so that current 'POS' gets saved
		// if the sort has actually changed
		if (nPrevSortBy != nBy)
			SetModified();

		return;
	}

	// was sorting previously disabled?
	BOOL bWasDisabled = !IsSortable();
	
	TDCCOLUMN* pTDCC = GetColumn(nBy);
	ASSERT (pTDCC);

	if (!pTDCC)
		return;
		
	if (m_bSortAscending == -1 || nBy != m_nSortBy)
		m_bSortAscending = pTDCC->bSortAscending;
	
	// if there's been a mod since last sorting then its reasonable to assume
	// that the user is not toggling direction but wants to simply resort
	// in the same direction.
	else if (!m_bModSinceLastSort)
		m_bSortAscending = !m_bSortAscending;
	
	// update the column header
	m_tree.SetGutterColumnSort(pTDCC->nColID, m_bSortAscending ? NCGSORT_UP : NCGSORT_DOWN);
	
	// note: without this check new tasklists with a single
	// task demonstrate a display bug where the task name 
	// does not appear. and we don't want to get rid of the
	// CHoldRedraw (the culprit) because it speeds up sorting
	if (m_data.GetTaskCount() > 1)
	{
		CHoldRedraw hr(m_tree);
		m_data.Sort(nBy, m_bSortAscending);
	}
	
	m_nSortBy = nBy;
	m_bModSinceLastSort = FALSE;

	// update column header clicking
	if (bWasDisabled)
		UpdateColumnHeaderClicking();

	// update registry
	SaveSortState();
}

LRESULT CToDoCtrl::OnTreeDragDrop(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	const MSG* pMsg = GetCurrentMessage();
	
	UINT nRes = m_treeDragDrop.ProcessMessage(pMsg);
	
	// handle WM_DD_DRAGDROP
	if (nRes && pMsg->message == WM_DD_DRAGDROP)
	{
		HTREEITEM htiDrop, htiAfter;
		
		if (m_treeDragDrop.GetDropTarget(htiDrop, htiAfter))
		{
			// copy selection as xml
			CHoldRedraw hr(GetSafeHwnd());
			CTaskFile tasks;
			
			if (GetSelectedTasks(tasks))
			{
				BOOL bCopy = (GetKeyState(VK_CONTROL) & 0x8000);
				
				if (!bCopy)
					DeleteSelectedTask(FALSE);
				
				AddToItem(tasks, htiDrop, htiAfter, bCopy ? TDCR_YES : TDCR_NO);

				// update check state of parent
//				m_data.RefreshItemCheckState(htiDrop, -1, TRUE);
			}
		}
		else
			ASSERT(0);
	}
	
	return nRes;
}

BOOL CToDoCtrl::IsReservedShortcut(DWORD dwShortcut)
{
	// check this is not a reserved shortcut used by the tree or a.n.other ctrl
	switch (dwShortcut)
	{
	case MAKELONG(VK_UP, HOTKEYF_EXT):
	case MAKELONG(VK_PRIOR, HOTKEYF_EXT):
	case MAKELONG(VK_DOWN, HOTKEYF_EXT):
	case MAKELONG(VK_NEXT, HOTKEYF_EXT):

	case MAKELONG(VK_UP, HOTKEYF_CONTROL | HOTKEYF_EXT):
	case MAKELONG(VK_PRIOR, HOTKEYF_CONTROL | HOTKEYF_EXT):
	case MAKELONG(VK_DOWN, HOTKEYF_CONTROL | HOTKEYF_EXT):
	case MAKELONG(VK_NEXT, HOTKEYF_CONTROL | HOTKEYF_EXT):

	case MAKELONG(VK_UP, HOTKEYF_SHIFT | HOTKEYF_EXT):
	case MAKELONG(VK_PRIOR, HOTKEYF_SHIFT | HOTKEYF_EXT):
	case MAKELONG(VK_DOWN, HOTKEYF_SHIFT | HOTKEYF_EXT):
	case MAKELONG(VK_NEXT, HOTKEYF_SHIFT | HOTKEYF_EXT):

	case MAKELONG(VK_UP, HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_EXT):
	case MAKELONG(VK_PRIOR, HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_EXT):
	case MAKELONG(VK_DOWN, HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_EXT):
	case MAKELONG(VK_NEXT, HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_EXT):

	case MAKELONG(VK_DELETE, HOTKEYF_CONTROL | HOTKEYF_EXT):
		return TRUE;
	}

	// all else
	return FALSE;
}

BOOL CToDoCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// handle plain 'TAB' key when the comments field has the focus
	// because certain types of windows will eat it
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB &&
		IsChildOrSame(m_ctrlComments, pMsg->hwnd))
	{
		BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);

		if (!bCtrl)
		{
			BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
			::SetFocus(::GetNextDlgTabItem(*this, pMsg->hwnd, bShift));

			return TRUE;
		}
	}

	if (m_ctrlComments.PreTranslateMessage(pMsg))
		return TRUE;

	if (m_treeDragDrop.ProcessMessage(pMsg))
		return TRUE;
	
	switch (pMsg->message)
	{
	case WM_NCMOUSEMOVE:
		// make sure tree tooltips go away
		if (HasStyle(TDCS_SHOWINFOTIPS))
		{
			if (pMsg->hwnd != m_tree || pMsg->wParam != HTCLIENT)
			{
				CWnd* pTT = m_tree.GetToolTips();
				
				if (pTT)
					pTT->SendMessage(TTM_POP); 
			}
		}
		break;

	case WM_KEYDOWN:
		{
			if (pMsg->wParam == VK_TAB)
				return CRuntimeDlg::PreTranslateMessage(pMsg);

			// handle <return> and <escape> for tree label editing
			else if (pMsg->hwnd == (HWND)m_tree.SendMessage(TVM_GETEDITCONTROL))
			{
				switch (pMsg->wParam)
				{
				case VK_ESCAPE:
					m_tree.SendMessage(TVM_ENDEDITLABELNOW, TRUE);
					return TRUE;
					
				case VK_RETURN:
					m_tree.SendMessage(TVM_ENDEDITLABELNOW, FALSE);
					return TRUE;
				}
			}
			// handle <ctrl>/Shift>+cursor for tree.
			// we have to handle these here before the tree gets hold of them
			// because its default behaviour does not suit our multiple
			// selection implementation
			else if (pMsg->hwnd == (HWND)m_tree)
			{
				// cursor handled in OnTreeSelChanged
				// <shift>+cursor handled in OnTreeSelChanged
				// <ctrl>+cursor handled here
				// <ctrl>+<shift>+cursor here
				BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
				BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
				
				// get the real currently selected item
				HTREEITEM hti = m_tree.GetSelectedItem();
				CTreeCtrlHelper tch(m_tree);
				
				switch (pMsg->wParam)
				{
				case VK_NEXT:  
				case VK_DOWN:
					if (bCtrl)
					{
						HTREEITEM htiNext = NULL;
						
						if (pMsg->wParam == VK_NEXT)
							htiNext = tch.GetNextPageVisibleItem(hti);
						else
							htiNext = tch.GetNextVisibleItem(hti);
						
						if (htiNext)
						{
							m_tree.SelectItem(htiNext);
							
							// toggle items if shift is also down, but not the one 
							// we're just moving on to
							if (bShift)
							{
								HTREEITEM htiPrev = tch.GetPrevVisibleItem(htiNext, FALSE);
								MultiSelectItems(htiPrev, hti, -1);
							}
						}
						
						return TRUE;
					}
					break;
					
				case VK_UP:
				case VK_PRIOR: 
					if (bCtrl)
					{
						HTREEITEM htiPrev = NULL;
						
						if (pMsg->wParam == VK_PRIOR)
							tch.GetPrevPageVisibleItem(hti);
						else
							tch.GetPrevVisibleItem(hti);
						
						if (htiPrev)
						{
							m_tree.SelectItem(htiPrev);
							
							// toggle items if shift is also down, but not the one 
							// we're just moving on to
							if (bShift)
							{
								HTREEITEM htiNext = tch.GetNextVisibleItem(htiPrev, FALSE);
								MultiSelectItems(htiNext, hti, -1);
							}
						}
						
						return TRUE;
					}
					break;
					
				case VK_SPACE:
					if (bCtrl)
					{
						// toggle real selected item state
						MultiSelectItem(hti, -1);
						return TRUE;
					}
					break;
				}
				
				// see if the tree wants it
				if (((CWnd*)&m_tree)->PreTranslateMessage(pMsg))
					return TRUE;
			}
			// handle <enter> and TDCS_FOCUSTREEONENTER
			else if (HasStyle(TDCS_FOCUSTREEONENTER) && pMsg->wParam == VK_RETURN)
			{
				// have to be careful here because dropped-down comboboxes should
				// take precedence. so we have to check explicitly for them
				// and make sure they're not in a dropped state
				CWnd* pFocus = GetFocus();
				
				if (pFocus && IsChild(pFocus))
				{
					CString sClass = CWinClasses::GetClass(*pFocus);
					
					if (CWinClasses::IsClass(sClass, WC_COMBOBOX))
					{
						if (pFocus->SendMessage(CB_GETDROPPEDSTATE))
							return FALSE;
					}
					// also check for combo's edit box and edits with ES_WANTRETURN
					else if (CWinClasses::IsClass(sClass, WC_EDIT))
					{
						if (pFocus->GetStyle() & ES_WANTRETURN)
							return FALSE;
						
						CWnd* pParent = pFocus->GetParent();
						ASSERT (pParent);
						
						if (CWinClasses::IsClass(*pParent, WC_COMBOBOX) && 
							pParent->SendMessage(CB_GETDROPPEDSTATE))
							return FALSE;
					}
					// and also richedit with ES_WANTRETURN
					else if (CWinClasses::IsRichEditControl(sClass))
					{
						if (pFocus->GetStyle() & ES_WANTRETURN)
							return FALSE;
					}
					
					// else move focus to tree
					SetFocusToTree();
					return TRUE;
				}
			}
		}
		break;
	}
	
	return CRuntimeDlg::PreTranslateMessage(pMsg);
}

BOOL CToDoCtrl::MoveSelectedTask(TDDH_MOVE nDirection)
{
	if (!CanMoveSelectedTask(nDirection))
		return FALSE;
	
	// else
	Flush();

	{
		CHoldRedraw hr(*this);

		m_treeDragDrop.MoveSelection(nDirection);
	}

	if (nDirection == MOVE_UP || nDirection == MOVE_DOWN)
	{
		// if moving up or down make sure we scroll ahead a bit
		m_tree.TCH().SetMinDistanceToEdge(Selection().GetFirstItem(), TCHE_TOP, 2);

		if (GetSelectedCount() > 1)
			m_tree.TCH().SetMinDistanceToEdge(Selection().GetLastItem(), TCHE_BOTTOM, 2);
	}
	else
	{
		// update bold states
		RefreshBoldStates();
	}

	SetModified(TRUE, TDCA_NONE);
		  
	return TRUE;
}

void CToDoCtrl::RefreshBoldStates(HTREEITEM hti)
{
	if (hti && hti != TVI_ROOT)
		m_tree.TCH().SetItemBold(hti, m_tree.GetParentItem(hti) == NULL);

	// children
	HTREEITEM htiChild = m_tree.GetChildItem(hti);

	while (htiChild)
	{
		RefreshBoldStates(htiChild);
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

BOOL CToDoCtrl::CanMoveSelectedTask(TDDH_MOVE nDirection) const
{
	return (!IsReadOnly() && m_treeDragDrop.CanMoveSelection(nDirection));
}

void CToDoCtrl::GotoTopLevelTask(TDC_GOTO nDirection)
{
	HTREEITEM htiNext = m_tree.TCH().GetNextTopLevelItem(GetSelectedItem(), nDirection == TDCG_NEXT);
	
	if (htiNext)
		SelectItem(htiNext);
}

BOOL CToDoCtrl::CanGotoTopLevelTask(TDC_GOTO nDirection) const
{
	return (NULL != m_tree.TCH().GetNextTopLevelItem(GetSelectedItem(), nDirection == TDCG_NEXT));
}

LRESULT CToDoCtrl::OnGutterDrawItem(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGDRAWITEM* pNCGDI = (NCGDRAWITEM*)lParam;
	
	// convert index to TDC_COLUMN
	TDCCOLUMN* pTDCC = GetColumn(pNCGDI->nColID);
	
	if (!pTDCC) // not one of ours
		return FALSE;
	
	// is it visible
	if (!CRect().IntersectRect(pNCGDI->rItem, pNCGDI->rWindow))
		return TRUE;
	
	if (!IsColumnShowing(pTDCC->nColID))
		return TRUE;
	
	HTREEITEM hti = (HTREEITEM)pNCGDI->dwItem;
	DWORD dwUniqueID = GetTaskID(hti);
	TODOITEM* pTDI = GetTask(dwUniqueID);

	if (pTDI)
	{
		CRect rItem(pNCGDI->rItem);
		TDI_STATE nState = GetItemState(hti);
		
		// colors and vertical divider
		COLORREF crTextColor = GetSysColor(COLOR_WINDOWTEXT); // default
		
		switch (nState)
		{
		case TDIS_DROPHILITED:
		case TDIS_SELECTEDNOTFOCUSED:
			pNCGDI->pDC->FillSolidRect(rItem, GetSysColor(COLOR_3DFACE));
			break;
			
		case TDIS_SELECTED:
			pNCGDI->pDC->FillSolidRect(rItem, GetSysColor(COLOR_HIGHLIGHT));
			crTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
			break;
			
		case TDIS_NONE:
		default:
			pNCGDI->pDC->FillSolidRect(rItem, m_tree.GetItemLineColor(hti));

			if (m_tree.GetGridlineColor() !=-1)
				pNCGDI->pDC->FillSolidRect(rItem.right - 1, rItem.top, 1, rItem.Height(), 
											m_tree.GetGridlineColor());
			break; // no color change
		}
		
		switch (pTDCC->nColID)
		{
		case TDCC_PRIORITY:
			{
				// priority color
				if (!m_data.IsTaskDone(hti, TDCCHECKALL))
				{
					rItem.DeflateRect(2, 2, 3, 2);
					
					// first draw the priority colour
					int nPriority = m_data.GetHighestPriority(hti, pTDI, FALSE);
					BOOL bHasPriority = (nPriority != FT_NOPRIORITY);

					if (bHasPriority)
					{
						COLORREF color = GetPriorityColor(nPriority);
						pNCGDI->pDC->FillSolidRect(rItem, color);
					}
					
					// then, if the task is also due, draw a small tag
					// to indicate this
					if (m_data.IsTaskDue(hti, pTDI))
					{
						BOOL bDueToday = m_data.IsTaskDue(hti, pTDI, TRUE);

						// unless it's due today and the user doesn't want today's tasks marked as due
						if (!bDueToday || m_crDueToday != NOCOLOR)
						{
							POINT pt[3] = { { rItem.left, rItem.top + 7 }, 
											{ rItem.left, rItem.top }, 
											{ rItem.left + 7, rItem.top } };

							CBrush* pOldBr = pNCGDI->pDC->SelectObject(bDueToday ? &m_brDueToday : &m_brDue);
							pNCGDI->pDC->SelectStockObject(NULL_PEN);

							pNCGDI->pDC->Polygon(pt, 3);
							pNCGDI->pDC->SelectObject(pOldBr);

							// draw a black line between the two
							pNCGDI->pDC->SelectStockObject(BLACK_PEN);
							pNCGDI->pDC->MoveTo(rItem.left, rItem.top + 6);
							pNCGDI->pDC->LineTo(rItem.left + 7, rItem.top - 1);
						}
					}

					// draw priority number over the top
					if (bHasPriority && !HasStyle(TDCS_HIDEPRIORITYNUMBER))
					{
						COLORREF color = GetPriorityColor(nPriority);
						CString sPriority;

						sPriority.Format("%d", nPriority);
						
						rItem.left++; // centres it properly
						rItem.top--;
						
						// pick best colour for text
						crTextColor = (RGBX(color).Luminance() < 128) ? RGB(255, 255, 255) : 0;
						DrawGutterItemText(pNCGDI->pDC, sPriority, rItem, DT_CENTER, crTextColor);
					}
				}
			}
			break;
			
		case TDCC_RISK:
			{
				int nRisk = m_data.GetHighestRisk(hti, pTDI);

				if (nRisk != FT_NORISK)
				{
					CString sRisk;
					sRisk.Format("%d", nRisk);
					DrawGutterItemText(pNCGDI->pDC, sRisk, rItem, DT_CENTER, crTextColor);
				}
			}
			break;

		case TDCC_RECURRENCE:
			if (pTDI->trRecurrence.nRegularity != TDIR_ONCE)
			{
				CString sRecurrence = CRecurringTaskEdit::GetRegularity(pTDI->trRecurrence.nRegularity);
				DrawGutterItemText(pNCGDI->pDC, sRecurrence, rItem, DT_LEFT, crTextColor);
			}
			break;

		case TDCC_ID:
			{
				CString sID;
				sID.Format("%u", dwUniqueID);
				
				DrawGutterItemText(pNCGDI->pDC, sID, rItem, DT_RIGHT, crTextColor);
			}
			break;
			
		case TDCC_COST:
			{
				double dCost = m_data.CalcCost(hti, pTDI);

				if (dCost > 0 || !HasStyle(TDCS_HIDEZEROTIMECOST))
					DrawGutterItemText(pNCGDI->pDC, Misc::Format(dCost), rItem, DT_RIGHT, crTextColor);
			}
			break;
			
		case TDCC_EXTERNALID:
			DrawGutterItemText(pNCGDI->pDC, pTDI->sExternalID, rItem, pNCGDI->nTextAlign, crTextColor);
			break;
			
		case TDCC_VERSION:
			DrawGutterItemText(pNCGDI->pDC, pTDI->sVersion, rItem, pNCGDI->nTextAlign, crTextColor);
			break;
			
		case TDCC_ALLOCTO:
//			DrawGutterItemText(pNCGDI->pDC, pTDI->sAllocTo, rItem, pNCGDI->nTextAlign, crTextColor);
			{
				CString sAllocTo = Misc::FormatArray(pTDI->aAllocTo);

				// if this is empty and this is a selected task 
				// then use whatever is in the category edit field
				if (sAllocTo.IsEmpty() && Selection().HasItem(hti))
					m_cbAllocTo.GetWindowText(sAllocTo);

				DrawGutterItemText(pNCGDI->pDC, sAllocTo, rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
			
		case TDCC_ALLOCBY:
			DrawGutterItemText(pNCGDI->pDC, pTDI->sAllocBy, rItem, pNCGDI->nTextAlign, crTextColor);
			break;
			
		case TDCC_STATUS:
			DrawGutterItemText(pNCGDI->pDC, pTDI->sStatus, rItem, pNCGDI->nTextAlign, crTextColor);
			break;
			
		case TDCC_CATEGORY:
			{
				CString sCats = Misc::FormatArray(pTDI->aCategories);

				// if this is empty and this is a selected task 
				// then use whatever is in the category edit field
				if (sCats.IsEmpty() && Selection().HasItem(hti))
					m_cbCategory.GetWindowText(sCats);

				DrawGutterItemText(pNCGDI->pDC, sCats, rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
			
		case TDCC_CREATEDBY:
			DrawGutterItemText(pNCGDI->pDC, pTDI->sCreatedBy, rItem, pNCGDI->nTextAlign, crTextColor);
			break;
			
		case TDCC_PERCENT:
			{
				BOOL bDone = -1; // so we know when it's set
				BOOL bHidePercent = HasStyle(TDCS_HIDEPERCENTFORDONETASKS);

				if (bHidePercent)
					bDone = m_data.IsTaskDone(hti, TDCCHECKALL);

				if (!bHidePercent || !bDone)
				{
					int nPercent = m_data.CalcPercentDone(hti, pTDI);

					CString sPercent;
					sPercent.Format("%d%%", nPercent);
					
					// draw progressbar?
					if (HasStyle(TDCS_SHOWPERCENTASPROGRESSBAR))
					{
						CRect rProgress(rItem);
						rProgress.DeflateRect(2, 2, 3, 2);
						rProgress.right = rProgress.left + MulDiv(rProgress.Width(), nPercent, 100);
						
						// if the task is done then draw in 'done' colour else priority colour
						if (bDone == -1) // not yet determined
							bDone = m_data.IsTaskDone(hti, TDCCHECKALL);

						COLORREF crBar;

						if (bDone)
							crBar = m_crTaskDone;

						else // determine appropriate priority
						{
							int nPriority = m_data.GetHighestPriority(hti, pTDI);
							crBar = GetPriorityColor(nPriority);
						}
						
						pNCGDI->pDC->FillSolidRect(rProgress, crBar); 
					}
					
					DrawGutterItemText(pNCGDI->pDC, sPercent, rItem, DT_RIGHT, crTextColor);
				}
			}
			break;
			
		case TDCC_TIMEEST:
		case TDCC_TIMESPENT:
			{
				BOOL bTimeEst = (pTDCC->nColID == TDCC_TIMEEST);
				
				int nUnits = s_tdDefault.nTimeEstUnits; // good default value
				
				// get actual task time units
				if (!m_tree.ItemHasChildren(hti))
					nUnits = bTimeEst ? pTDI->nTimeEstUnits : pTDI->nTimeSpentUnits;
				
				// draw time
				double dTime = bTimeEst ? m_data.CalcTimeEstimate(hti, pTDI, nUnits) :
										  m_data.CalcTimeSpent(hti, pTDI, nUnits);
				
				if (dTime > 0 || !HasStyle(TDCS_HIDEZEROTIMECOST))
				{
					int nDecPlaces = HasStyle(TDCS_ROUNDTIMEFRACTIONS) ? 0 : 2;
					CString sTime;
					
					if (HasStyle(TDCS_DISPLAYHMSTIMEFORMAT))
						sTime = CTimeHelper::FormatTimeHMS(dTime, nUnits, (BOOL)nDecPlaces);
					else
					{
						sTime = CTimeHelper::FormatTime(dTime, nUnits, nDecPlaces);
						sTime.MakeLower();
					}

					// show text in red if we're currently tracking
					BOOL bTimeTracking = (m_dwTimeTrackTaskID == dwUniqueID) &&
											(nState == TDIS_SELECTED || 
											nState == TDIS_SELECTEDNOTFOCUSED ||
											!HasStyle(TDCS_TRACKSELECTEDTASKONLY));

					if (!bTimeEst && bTimeTracking)
						crTextColor = 255;
					
					DrawGutterItemText(pNCGDI->pDC, sTime, rItem, pNCGDI->nTextAlign, crTextColor);
				}
			}
			break;
			
		case TDCC_TRACKTIME:
			if (m_dwTimeTrackTaskID == dwUniqueID)
			{
				// show icon in red if we're currently tracking
				if (nState == TDIS_SELECTED || nState == TDIS_SELECTEDNOTFOCUSED ||
					!HasStyle(TDCS_TRACKSELECTEDTASKONLY))
					crTextColor = 255;

				CFont& font = Misc::WingDings();
				
				if (font.GetSafeHandle())
				{
					CFont* pOldFont = pNCGDI->pDC->SelectObject(&font);
					DrawGutterItemText(pNCGDI->pDC, "¹", rItem, pNCGDI->nTextAlign, crTextColor);
					pNCGDI->pDC->SelectObject(pOldFont);
				}
				else
					DrawGutterItemText(pNCGDI->pDC, "T", rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
			
		case TDCC_FLAG:
			if (pTDI->bFlagged)
			{
				CFont& font = Misc::WingDings();
				
				if (font.GetSafeHandle())
				{
					CFont* pOldFont = pNCGDI->pDC->SelectObject(&font);
					DrawGutterItemText(pNCGDI->pDC, "O", rItem, pNCGDI->nTextAlign, crTextColor);
					pNCGDI->pDC->SelectObject(pOldFont);
				}
				else
					DrawGutterItemText(pNCGDI->pDC, "F", rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
			
		case TDCC_STARTDATE:
			if (pTDI->HasStart())
			{
				if (!HasStyle(TDCS_HIDESTARTDUEFORDONETASKS) ||	!m_data.IsTaskDone(hti, TDCCHECKALL))
				{
					BOOL bShowWeekday = HasStyle(TDCS_SHOWWEEKDAYINDATES);
					BOOL bISO = HasStyle(TDCS_SHOWDATESINISO);
					
					CString sDate = CDateHelper::FormatDate(pTDI->dateStart, bISO, bShowWeekday);
					
					DrawGutterItemText(pNCGDI->pDC, sDate, rItem, pNCGDI->nTextAlign, crTextColor);
				}
			}
			break;
			
		case TDCC_DUEDATE:
			{
				COleDateTime date;
				BOOL bDone = m_data.IsTaskDone(hti, TDCCHECKALL);

				if (bDone && !HasStyle(TDCS_HIDESTARTDUEFORDONETASKS))
					date = pTDI->dateDue;

				else if (!bDone)
					date = m_data.GetEarliestDueDate(hti, pTDI, HasStyle(TDCS_USEEARLIESTDUEDATE));
				
				if (date.m_dt > 0) 
				{
					BOOL bShowWeekday = HasStyle(TDCS_SHOWWEEKDAYINDATES);
					BOOL bISO = HasStyle(TDCS_SHOWDATESINISO);
					
					CString sDate = CDateHelper::FormatDate(date, bISO, bShowWeekday);
					
					DrawGutterItemText(pNCGDI->pDC, sDate, rItem, pNCGDI->nTextAlign, crTextColor);
				}
			}
			break;
			
		case TDCC_DONEDATE:
			if (pTDI->IsDone())
			{
				BOOL bShowWeekday = HasStyle(TDCS_SHOWWEEKDAYINDATES);
				BOOL bISO = HasStyle(TDCS_SHOWDATESINISO);
				
				CString sDate = CDateHelper::FormatDate(pTDI->dateDone, bISO, bShowWeekday);
				
				DrawGutterItemText(pNCGDI->pDC, sDate, rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
			
		case TDCC_CREATIONDATE:
			if (pTDI->HasCreation())
			{
				BOOL bShowWeekday = HasStyle(TDCS_SHOWWEEKDAYINDATES);
				BOOL bISO = HasStyle(TDCS_SHOWDATESINISO);
				
				CString sDate = CDateHelper::FormatDate(pTDI->dateCreated, bISO, bShowWeekday);
				
				//sDate = "111";
				
				DrawGutterItemText(pNCGDI->pDC, sDate, rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
		
		case TDCC_LASTMOD:
			if (pTDI->HasLastMod())
			{
				BOOL bShowWeekday = HasStyle(TDCS_SHOWWEEKDAYINDATES);
				BOOL bISO = HasStyle(TDCS_SHOWDATESINISO);
				
				CString sDate = CDateHelper::FormatDate(pTDI->tLastMod, bISO, bShowWeekday);
				
				DrawGutterItemText(pNCGDI->pDC, sDate, rItem, pNCGDI->nTextAlign, crTextColor);
			}
			break;
			
		case TDCC_DEPENDENCY:
			if (!pTDI->sDependency.IsEmpty())
			{
				// draw our app icon 
				HICON hIcon = (HICON)AfxGetMainWnd()->SendMessage(WM_GETICON, ICON_SMALL);
					
				if (hIcon)
				{
					rItem.DeflateRect(0, (rItem.Height() + 1 - 16) / 2);
					rItem.left += NCG_COLPADDING;
					::DrawIconEx(pNCGDI->pDC->GetSafeHdc(), rItem.left, rItem.top, hIcon,
						16, 16, 0, NULL, DI_NORMAL);
				}
			}
			break;
			
		case TDCC_FILEREF:
			if (!pTDI->sFileRefPath.IsEmpty())
			{
				// if it coulsn't be found then check for a tdl://
				if (pTDI->sFileRefPath.Find(TDL_PROTOCOL) != -1)
				{
					HICON hIcon = (HICON)AfxGetMainWnd()->SendMessage(WM_GETICON, ICON_SMALL);
					
					// draw our app icon 
					if (hIcon)
					{
						rItem.DeflateRect(0, (rItem.Height() + 1 - 16) / 2);
						rItem.left += NCG_COLPADDING;
						::DrawIconEx(pNCGDI->pDC->GetSafeHdc(), rItem.left, rItem.top, hIcon,
							16, 16, 0, NULL, DI_NORMAL);
					}
				}
				else
				{
					// get the associated image failing if necessary
					CString sFileRef = pTDI->sFileRefPath;
					sFileRef.Remove('\"'); // remove quotes

					int nImage = m_ilFileRef.GetFileImageIndex(sFileRef, TRUE);
					
					if (nImage == -1 && HasStyle(TDCS_SHOWNONFILEREFSASTEXT))
						DrawGutterItemText(pNCGDI->pDC, sFileRef, rItem, DT_LEFT, crTextColor);
					else
					{
						if (nImage == -1)
							nImage = m_ilFileRef.GetFileImageIndex(sFileRef, FALSE);
						
						if (nImage >= 0)
						{
							rItem.DeflateRect(0, (rItem.Height() + 1 - 16) / 2);
							rItem.left += NCG_COLPADDING;
							m_ilFileRef.GetImageList()->Draw(pNCGDI->pDC, nImage, rItem.TopLeft(), ILD_TRANSPARENT);
						}
					}
				}
			}
			break;
			
		case TDCC_DONE:
			{
				BOOL bDone = pTDI->IsDone();
				rItem.DeflateRect((rItem.Width() - 16) / 2, (rItem.Height() - 16) / 2);
				
				if (m_hilDone)
				{
					int nImage = (int)m_tree.TCH().GetItemCheckState(hti) + 1; // first image is blank
					ImageList_Draw(m_hilDone, nImage, pNCGDI->pDC->GetSafeHdc(), 
						rItem.left, rItem.top, ILD_TRANSPARENT);
				}
				else
				{
					rItem.right--;
					rItem.bottom--;
					CThemed::DrawFrameControl(this, pNCGDI->pDC, rItem, DFC_BUTTON, 
						DFCS_BUTTONCHECK | (bDone ? DFCS_CHECKED : 0));
				}
			}
		}
		
		return TRUE; // we handled it
	}
	
	return FALSE;
}

LRESULT CToDoCtrl::OnGutterPostDrawItem(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
//	NCGDRAWITEM* pNCGDI = (NCGDRAWITEM*)lParam;

	return FALSE;
}

LRESULT CToDoCtrl::OnGutterWantRedraw(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	ASSERT (wParam == IDC_TASKLIST);
	
	BOOL bCancel = FALSE;
	const MSG* pMsg = (MSG*)lParam;
	
	switch (pMsg->message)
	{
	case TVM_SELECTITEM:
		// we deal with drop highlight changes ourselves
		if (pMsg->wParam == TVGN_DROPHILITE)
		{
			static HTREEITEM htiPrevDropHilite = NULL;
			
			// we do it ourselves
			if (htiPrevDropHilite != (HTREEITEM)pMsg->lParam)
			{
				m_tree.RedrawGutterItem((DWORD)htiPrevDropHilite);
				m_tree.RedrawGutterItem((DWORD)pMsg->lParam);
				
				htiPrevDropHilite = (HTREEITEM)pMsg->lParam;
			}
			
			bCancel = TRUE;
		}
		break;
	}
	
	return bCancel;
}

void CToDoCtrl::DrawGutterItemText(CDC* pDC, const CString& sText, const CRect& rect, int nAlign, COLORREF crText)
{
	if (sText.IsEmpty())
		return;
	
	CRect rText(rect);
	
	switch (nAlign)
	{
	case DT_LEFT:
		rText.left += NCG_COLPADDING;
		break;
		
	case DT_RIGHT:
		rText.right -= NCG_COLPADDING + 1;
		break;
	}
	
	pDC->SetTextColor(crText);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(sText, rText, DT_SINGLELINE | DT_VCENTER | nAlign);
}

BOOL CToDoCtrl::SetPriorityColors(const CDWordArray& aColors)
{
	ASSERT (aColors.GetSize() == 11);
	
	m_aPriorityColors.RemoveAll();
	
	if (aColors.GetSize() == 11)
	{
		m_aPriorityColors.Copy(aColors);
        m_cbPriority.SetColors(aColors);

		if (GetSafeHwnd())
			Invalidate();
		
		return TRUE;
	}
	
	// else
	return FALSE; // invalid combination
}

void CToDoCtrl::SetDueTaskColors(COLORREF crDue, COLORREF crDueToday)
{
	ASSERT (crDue != NOCOLOR);

	if (m_crDue != crDue)
	{
		m_brDue.DeleteObject();

		if (crDue != NOCOLOR)
			m_brDue.CreateSolidBrush(crDue);

		m_crDue = crDue;
	}

	if (m_crDueToday != crDueToday)
	{
		m_brDueToday.DeleteObject();

		if (crDueToday != NOCOLOR)
			m_brDueToday.CreateSolidBrush(crDueToday);

		m_crDueToday = crDueToday;
	}

	// we only sort the due today tasks high if the user has assigned
	// them a colour
	m_data.SetSortDueTodayHigh(crDueToday != NOCOLOR);
}

void CToDoCtrl::SetCategoryColors(const CTDCColorMap& colors)
{
	m_mapCatColors.RemoveAll();

	CString sCategory;
	COLORREF color;
	POSITION pos = colors.GetStartPosition();

	while (pos)
	{
		colors.GetNextAssoc(pos, sCategory, color);
		sCategory.MakeUpper();

		m_mapCatColors[sCategory] = color;
	}

	if (GetSafeHwnd() && HasStyle(TDCS_COLORTEXTBYCATEGORY))
		Invalidate();
}

BOOL CToDoCtrl::GetCategoryColor(LPCTSTR szCategory, COLORREF& color) const
{
	if (szCategory && *szCategory)
	{
		CString sCategory(szCategory);
		sCategory.MakeUpper();

		if (m_mapCatColors.Lookup(sCategory, color))
			return TRUE; 
	}
	
	// else
	return FALSE;
}

COLORREF CToDoCtrl::GetPriorityColor(int nPriority) const
{
	if (nPriority < 0 || nPriority >= m_aPriorityColors.GetSize())
		return 0;

	return (COLORREF)m_aPriorityColors[nPriority];
}

void CToDoCtrl::SetProjectName(LPCTSTR szProjectName)
{
	if (m_sProjectName != szProjectName)
	{
		m_sProjectName = szProjectName;

		if (GetSafeHwnd())
			UpdateDataEx(this, IDC_PROJECTNAME, m_sProjectName, FALSE);
	}
}

CString CToDoCtrl::GetFriendlyProjectName(int nUntitledIndex) const
{
	CString sProjectName(m_sProjectName);
	sProjectName.TrimRight();
	
	if (sProjectName.IsEmpty())
	{
		if (!m_sLastSavePath.IsEmpty())
			sProjectName = FileMisc::GetFileNameFromPath(m_sLastSavePath);
		else
		{
			if (nUntitledIndex > 0)
				sProjectName.Format("(%s%d)", CEnString(IDS_TDC_UNTITLEDFILE), nUntitledIndex);
			else
				sProjectName.Format("(%s)", CEnString(IDS_TDC_UNTITLEDFILE));
		}
	}
	
	return sProjectName;
}

void CToDoCtrl::InitExportFilter(TDCFILTER& filter, TDC_FILTER nFilter, DWORD dwFlags, LPCTSTR szAllocatedTo)
{
	filter.nFilter = nFilter;
	filter.dwFlags = dwFlags;
	filter.sAllocTo = szAllocatedTo;
	
	// initialize filter:dateDueBy if necessary
	switch (nFilter)
	{
	case TDCF_DUE:
		filter.dateDueBy = CDateHelper::GetDate(DHD_TODAY);
		break; // done
		
	case TDCF_DUETOMORROW:
		filter.dateDueBy += CDateHelper::CalcDaysFromTo(filter.dateDueBy, DHD_TOMORROW, 0);
		break;
		
	case TDCF_DUETHISWEEK:
		filter.dateDueBy += CDateHelper::CalcDaysFromTo(filter.dateDueBy, DHD_ENDTHISWEEK, 0);
		break;
		
	case TDCF_DUENEXTWEEK:
		filter.dateDueBy += CDateHelper::CalcDaysFromTo(filter.dateDueBy, DHD_ENDNEXTWEEK, 0);
		break;
		
	case TDCF_DUETHISMONTH:
		filter.dateDueBy += CDateHelper::CalcDaysFromTo(filter.dateDueBy, DHD_ENDTHISMONTH, 0);
		break;
		
	case TDCF_DUENEXTMONTH:
		filter.dateDueBy += CDateHelper::CalcDaysFromTo(filter.dateDueBy, DHD_ENDNEXTMONTH, 0);
		break;
	}
}

void CToDoCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// eat it if it came from us
	if (pWnd == this)
	{
		return;
	}
	else if (pWnd == &m_tree)
	{
		// check for (-1, -1)
		if (point.x == -1 && point.y == -1)
		{
			HTREEITEM htiSel = GetSelectedItem();
			CRect rItem;
			
			if (htiSel)
			{
				m_tree.TCH().EnsureVisibleEx(htiSel, FALSE);
				m_tree.GetItemRect(htiSel, rItem, TRUE);
				point = rItem.CenterPoint();
			}
			else
				point.x = point.y = 10;
			
			m_tree.ClientToScreen(&point);
		}
		else
		{
			CPoint ptTree(point);
			m_tree.ScreenToClient(&ptTree);
			
			if (ptTree.y >= 0) // == below header
			{
				if (ptTree.x < 0)
					ptTree.x = 0; // because point may be in the gutter
				
				HTREEITEM htiHit = m_tree.HitTest(ptTree);
				
				if (!Selection().HasItem(htiHit))
					SelectItem(htiHit);
				else
					m_tree.SelectItem(htiHit);
			}
		}
		
		// forward to our parent for handling
		GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)GetSafeHwnd(), MAKELPARAM(point.x, point.y));
		return;
	}
	// don't forward it on if it came from the content ctrl
	else if (IsChildOrSame(m_ctrlComments, pWnd->GetSafeHwnd()))
		return;

	// else
	CRuntimeDlg::OnContextMenu(pWnd, point);
}

BOOL CToDoCtrl::PtInHeader(CPoint ptScreen) const
{
	return m_tree.PtInHeader(ptScreen);
}

LRESULT CToDoCtrl::OnAutoComboChange(WPARAM wp, LPARAM /*lp*/)
{
	UINT nCtrlID = LOWORD(wp);

	switch (nCtrlID)
	{
	case IDC_CATEGORY:
		UpdateTask(TDCA_CATEGORY);
		GetParent()->SendMessage(WM_TDCN_LISTCHANGE, 0, TDCA_CATEGORY);
		break;

	case IDC_STATUS:
		GetParent()->SendMessage(WM_TDCN_LISTCHANGE, 0, TDCA_STATUS);
		break;

	case IDC_ALLOCTO:
		UpdateTask(TDCA_ALLOCTO);
		GetParent()->SendMessage(WM_TDCN_LISTCHANGE, 0, TDCA_ALLOCTO);
		break;

	case IDC_ALLOCBY:
		GetParent()->SendMessage(WM_TDCN_LISTCHANGE, 0, TDCA_ALLOCBY);
		break;
	}

	return 0L;
}

void CToDoCtrl::OnEditChangeAllocTo()
{
	UpdateTask(TDCA_ALLOCTO);
}

void CToDoCtrl::OnSelChangeAllocTo()
{
	// we work off the edit field which will not have been 
	// updated yet. so we just post a CBN_EDITCHANGE to handle it
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_ALLOCTO, CBN_EDITCHANGE), (LPARAM)m_cbAllocTo.GetSafeHwnd());
}

void CToDoCtrl::OnEditChangeAllocBy()
{
	UpdateTask(TDCA_ALLOCBY);
}

void CToDoCtrl::OnSelChangeAllocBy()
{
	// we work off the edit field which will not have been 
	// updated yet. so we just post a CBN_EDITCHANGE to handle it
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_ALLOCBY, CBN_EDITCHANGE), (LPARAM)(HWND)m_cbAllocBy);
}

void CToDoCtrl::OnEditChangeStatus()
{
	UpdateTask(TDCA_STATUS);
}

void CToDoCtrl::OnSelChangeStatus()
{
	// we work off the edit field which will not have been 
	// updated yet. so we just post a CBN_EDITCHANGE to handle it
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_STATUS, CBN_EDITCHANGE), (LPARAM)(HWND)m_cbStatus);
}

void CToDoCtrl::OnEditChangeVersion()
{
	UpdateTask(TDCA_VERSION);
}

void CToDoCtrl::OnSelChangeVersion()
{
	// we work off the edit field which will not have been 
	// updated yet. so we just post a CBN_EDITCHANGE to handle it
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_VERSION, CBN_EDITCHANGE), (LPARAM)(HWND)m_cbVersion);
}

void CToDoCtrl::OnEditChangeCategory()
{
	UpdateTask(TDCA_CATEGORY);
}

void CToDoCtrl::OnSelChangeCategory()
{
	// we work off the edit field which will not have been 
	// updated yet. so we just post a CBN_EDITCHANGE to handle it
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CATEGORY, CBN_EDITCHANGE), (LPARAM)m_cbCategory.GetSafeHwnd());
}

void CToDoCtrl::OnChangeTimeEstimate()
{
	UpdateTask(TDCA_TIMEEST); 
}

void CToDoCtrl::OnChangeTimeSpent()
{
	UpdateTask(TDCA_TIMESPENT); 
}

void CToDoCtrl::OnChangeExternalID()
{
	UpdateTask(TDCA_EXTERNALID); 
}

void CToDoCtrl::OnChangeCost()
{
	UpdateTask(TDCA_COST); 
}

void CToDoCtrl::OnChangeDependency()
{
	UpdateTask(TDCA_DEPENDENCY); 
}

void CToDoCtrl::OnChangeRecurrence()
{
	UpdateTask(TDCA_RECURRENCE); 
}

void CToDoCtrl::OnChangePercent()
{
	// don't handle if this was an 'auto' change
	if (!HasStyle(TDCS_AUTOCALCPERCENTDONE))
		UpdateTask(TDCA_PERCENT);
}

void CToDoCtrl::OnTreeGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVGETINFOTIP* pTVGIT = (NMTVGETINFOTIP*)pNMHDR;
	TODOITEM* pTDI = GetTask((DWORD)pTVGIT->lParam);
	
	if (pTDI)
	{
//		pTVGIT->pszText = 0;
//		TRACE("CToDoCtrl::OnTreeGetInfoTip(%d)\n", pTVGIT->lParam);
		//fabio_2005
#if _MSC_VER >= 1400
		strncpy_s(pTVGIT->pszText, pTVGIT->cchTextMax, FormatInfoTip(pTVGIT->hItem, pTDI), pTVGIT->cchTextMax);
#else
		strncpy(pTVGIT->pszText, FormatInfoTip(pTVGIT->hItem, pTDI), pTVGIT->cchTextMax);
#endif

	}
	
	*pResult = 0;
}

CString CToDoCtrl::FormatInfoTip(const HTREEITEM hti, const TODOITEM* pTDI) const
{
	// format text multilined
	CString sTip;
	
	sTip.Format("Task:\t\t %s",	pTDI->sTitle);

	CString sComments = pTDI->sComments;
	int nLen = sComments.GetLength();
	
	if (nLen && m_nMaxInfotipCommentsLength != 0)
	{
		if (m_nMaxInfotipCommentsLength > 0 && nLen > m_nMaxInfotipCommentsLength)
			sComments = sComments.Left(m_nMaxInfotipCommentsLength) + MISC_ELLIPSIS;

		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_COMMENTS), sComments);
	}
	
	if (IsColumnShowing(TDCC_STATUS) && !pTDI->sStatus.IsEmpty())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_STATUS), pTDI->sStatus);
	
	if (IsColumnShowing(TDCC_CATEGORY) && pTDI->aCategories.GetSize())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_CATEGORY), Misc::FormatArray(pTDI->aCategories));
	
//	if (IsColumnShowing(TDCC_ALLOCTO) && !pTDI->sAllocTo.IsEmpty())
//		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_ALLOCTO), pTDI->sAllocTo);

	if (IsColumnShowing(TDCC_ALLOCTO) && pTDI->aAllocTo.GetSize())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_ALLOCTO), Misc::FormatArray(pTDI->aAllocTo));
	
	if (IsColumnShowing(TDCC_ALLOCBY) && !pTDI->sAllocBy.IsEmpty())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_ALLOCBY), pTDI->sAllocBy); 
	
	if (IsColumnShowing(TDCC_VERSION) && !pTDI->sVersion.IsEmpty())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_VERSION), pTDI->sVersion); 
	
	if (pTDI->IsDone())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_DONEDATE), CDateHelper::FormatDate(pTDI->dateDone, HasStyle(TDCS_SHOWDATESINISO)));
	else
	{
		if (IsColumnShowing(TDCC_PRIORITY) && pTDI->nPriority)
			sTip += CEnString("\n%s %d", CEnString(IDS_TDCTIP_PRIORITY), pTDI->nPriority);
		
		if (IsColumnShowing(TDCC_STARTDATE) && pTDI->HasStart())
			sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_STARTDATE), CDateHelper::FormatDate(pTDI->dateStart, HasStyle(TDCS_SHOWDATESINISO)));
		
		if (IsColumnShowing(TDCC_DUEDATE) && pTDI->HasDue())
			sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_DUEDATE), CDateHelper::FormatDate(pTDI->dateDue, HasStyle(TDCS_SHOWDATESINISO)));
		
		if (IsColumnShowing(TDCC_PERCENT))
			sTip += CEnString("\n%s %d", CEnString(IDS_TDCTIP_PERCENT), m_data.CalcPercentDone(hti, pTDI));
		
		if (IsColumnShowing(TDCC_TIMEEST))
			sTip += CEnString("\n%s %.2f %c", CEnString(IDS_TDCTIP_TIMEEST), m_data.CalcTimeEstimate(hti, pTDI, TDITU_HOURS), TIME_HOUR_ABBREV);
		
		if (IsColumnShowing(TDCC_TIMESPENT))
			sTip += CEnString("\n%s %.2f %c", CEnString(IDS_TDCTIP_TIMESPENT), m_data.CalcTimeSpent(hti, pTDI, TDITU_HOURS), TIME_HOUR_ABBREV);
	}
	
	if (IsColumnShowing(TDCC_COST))
		sTip += CEnString("\n%s %.2f", CEnString(IDS_TDCTIP_COST), m_data.CalcCost(hti, pTDI));
	
	if (IsColumnShowing(TDCC_DEPENDENCY) && !pTDI->sDependency.IsEmpty())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_DEPENDS), pTDI->sDependency);
	
	if (IsColumnShowing(TDCC_FILEREF) && !pTDI->sFileRefPath.IsEmpty())
		sTip += CEnString("\n%s %s", CEnString(IDS_TDCTIP_FILEREF), pTDI->sFileRefPath);
	
	if (pTDI->tLastMod > 0)
		sTip += CEnString("\n%s %s (%s)", CEnString(IDS_TDCTIP_LASTMOD), pTDI->tLastMod.Format(VAR_DATEVALUEONLY), pTDI->tLastMod.Format(VAR_TIMEVALUEONLY));

	return sTip;
}

BOOL CToDoCtrl::CopyCurrentSelection(BOOL /*bCut*/) const
{
	ClearCopiedItem();

	if (!GetSelectedTasks(s_clipboard.tasks))
		return FALSE;
	
	// tag with our window handle
	s_clipboard.hwndToDoCtrl = GetSafeHwnd();

	// and filename
	s_clipboard.tasks.SetFileName(GetFilePath());
	
	// also copy selected task titles to clipboard
	CString sTitles;
	POSITION pos = Selection().GetFirstItemPos();
	
	while (pos)
	{
		HTREEITEM hti = Selection().GetNextItem(pos);
		
		sTitles += GetItemText(hti);
		sTitles += "\n";
	}
	
	Misc::CopyTexttoClipboard(sTitles, *this);
	
	return TRUE;
}

BOOL CToDoCtrl::IsClipboardEmpty() const
{
	return (!s_clipboard.tasks.GetTaskCount());
}

BOOL CToDoCtrl::CopySelectedTask() const
{
	return CopyCurrentSelection(FALSE);
}

BOOL CToDoCtrl::CutSelectedTask()
{
	if (CopyCurrentSelection(TRUE))
	{
		DeleteSelectedTask(FALSE, TRUE);
		return TRUE;
	}
	
	return FALSE;
}

BOOL CToDoCtrl::CanPaste() const
{
	return (!IsReadOnly() && !IsClipboardEmpty() && GetSelectedCount() <= 1);
}

void CToDoCtrl::ClearCopiedItem() const
{
	s_clipboard.hwndToDoCtrl = NULL;
	s_clipboard.tasks.Reset();
}

BOOL CToDoCtrl::PasteOnSelectedTask()
{
	if (GetSelectedCount() > 1 || !CanPaste())
		return FALSE;
	
	HTREEITEM htiDest = m_tree.GetSelectedItem();
	
	if (!htiDest)
		htiDest = TVI_ROOT;
	
	// reset the task IDs if the tasks did *not* originate from us
	TDC_RESETIDS nResetID = (s_clipboard.hwndToDoCtrl == *this) ? TDCR_CHECK : TDCR_YES;
	
	return AddToItem(s_clipboard.tasks, htiDest, TVI_FIRST, nResetID);
}

BOOL CToDoCtrl::PasteAfterSelectedTask()
{
	if (GetSelectedCount() > 1 || !CanPaste())
		return FALSE;
	
	HTREEITEM htiDest = NULL, htiDestAfter = m_tree.GetSelectedItem();
	
	if (!htiDestAfter)
		htiDestAfter = TVI_LAST;
	else
	{
		htiDest = m_tree.GetParentItem(htiDestAfter);
		
		if (!htiDest)
			htiDest = TVI_ROOT;
	}

	// reset the task IDs if the tasks did *not* originate from us
	TDC_RESETIDS nResetID = (s_clipboard.hwndToDoCtrl == *this) ? TDCR_CHECK : TDCR_YES;
	
	return AddToItem(s_clipboard.tasks, htiDest, htiDestAfter, nResetID);
}

BOOL CToDoCtrl::AddToItem(const CTaskFile& tasks, HTREEITEM htiDest, HTREEITEM htiDestAfter, TDC_RESETIDS nResetIDs)
{
	if (!htiDest)
		return FALSE;
	
	HTASKITEM hTask = tasks.GetFirstTask();
	
	if (!hTask)
		return FALSE;
	
	// clear current selection
	Selection().RemoveAll();
	
	CHoldRedraw hr(this);
	CHoldRedraw hr2(&m_tree);
	
	while (hTask)
	{
		htiDestAfter = AddItem(tasks, hTask, htiDest, htiDestAfter, nResetIDs);
		
		// add item to selection
		Selection().AddItem(htiDestAfter);
		
		// next task
		hTask = tasks.GetNextTask(hTask);
	}
	
	m_tree.Expand(htiDest, TVE_EXPAND);
	
	if (GetSelectedCount())
		m_tree.SelectItem(Selection().GetFirstItem());
	
	SetModified(TRUE, TDCA_NONE);
	UpdateControls();
	
	return TRUE;
}

LRESULT CToDoCtrl::OnGutterNotifyItemClick(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGITEMCLICK* pNGIC = (NCGITEMCLICK*)lParam;
	HTREEITEM htiHit = (HTREEITEM)pNGIC->dwItem;
	HTREEITEM htiPrev = (HTREEITEM)pNGIC->dwPrevItem;

	LRESULT lr = 0;

	if (htiHit)
	{
		UINT nHitFlags = 0;
		m_tree.HitTest(pNGIC->ptClick, &nHitFlags);
		
		switch (pNGIC->nMsgID)
		{
		case WM_LBUTTONDOWN:
			{
				ProcessItemLButtonDown(htiHit, nHitFlags, (UINT)-1);

				m_selection.nLBtnDownFlags = nHitFlags;
				m_selection.nNcLBtnDownColID = -1;

				lr = 1L;  // we handled it
			}
			break;
			
		case WM_NCLBUTTONDOWN:
			{
				ProcessItemLButtonDown(htiHit, 0, pNGIC->nColID);
	
				m_selection.nNcLBtnDownColID = pNGIC->nColID;
				m_selection.nLBtnDownFlags = 0;

				lr = 1L;  // we handled it
			}
			break;

			// we handle these in the event that they weren't handled in the 
			// DOWN message handlers above and so long as we're still over
			// the same pos
		case WM_LBUTTONUP:
			{
				if (htiHit == htiPrev && nHitFlags == m_selection.nLBtnDownFlags)
					ProcessItemLButtonUp(htiHit, nHitFlags, (UINT)-1);

				m_selection.nLBtnDownFlags = 0;
				lr = 1L;  // we handled it
			}
			break;
			
		case WM_NCLBUTTONUP:
			{
				if (htiHit == htiPrev && pNGIC->nColID == m_selection.nNcLBtnDownColID)
				{
					ProcessItemLButtonUp(htiHit, 0, pNGIC->nColID);
				}
				// else if we were dragging but we're not anymore and
				// the items are different then shift the focus back to the prev item
				else if (htiHit != htiPrev && !m_treeDragDrop.IsDragging())
				{
					// post this message so that the tree's finished its stuff first
					PostMessage(WM_TDC_RESTOREFOCUSEDITEM, m_tree.GetDlgCtrlID(), (LPARAM)htiPrev);
					
					// update anchor
					Selection().SetAnchor(htiPrev);
				}

				m_selection.nNcLBtnDownColID = -1;
				lr = 1L;  // we handled it
			}
			break;
			
		case WM_RBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
			// if the hit item is not selected then 
			// select it first
			if (!IsItemSelected(htiHit))
			{
				SelectItem(htiHit);
				lr = 1L;  // we handled it
			}
			break;
		}
	}
	// if nowt clicked then clear selection
/*	else if (Selection().RemoveAll()) // !hti
	{
		m_tree.SelectItem(NULL);
		m_tree.RedrawGutter();
		lr = 1L;  // we handled it
	}
*/	
	m_treeDragDrop.EnableDragDrop(!IsReadOnly() && GetSelectedCount());
	
	return lr;
}

void CToDoCtrl::ProcessItemLButtonDown(HTREEITEM htiHit, int nHitFlags, UINT nColID)
{
	BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000); // ctrl key down
	BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000); // shift key down
	
	HTREEITEM htiAnchor = Selection().GetAnchor();
	
	if (!htiAnchor && bShift)
		htiAnchor = htiHit;
	
	if (bCtrl)
	{
		// don't change the selection if the user is clicking on
		// an item's fileref
		if (nColID == TDCC_FILEREF)
			return;

		if (bShift)
			MultiSelectItems(htiAnchor, htiHit, 1);
		else
		{
			// if the item clicked is already selected and there are no
			// other selected items then assume (for now) that the user
			// is going to perform a drag rather than a toggle
			if (GetSelectedCount() != 1 || !Selection().HasItem(htiHit))
				MultiSelectItem(htiHit, -1);
		}
		
		// save item handle so we don't rehandle in LButtonUp handler
		m_selection.htiLastHandledLBtnDown = htiHit;
	}
	else if (bShift) 
	{
		Selection().RemoveAll();
		MultiSelectItems(htiAnchor, htiHit, 1);
		
		// save item handle so we don't rehandle in LButtonUp handler
		m_selection.htiLastHandledLBtnDown = htiHit;
	}
	else if (!Selection().HasItem(htiHit))
	{
		// if the user clicked on the tree item itself then
		// don't call SelectItem here else in combination with the 
		// tree's own handling of this message it can trigger a label edit
		if (nHitFlags & TVHT_ONITEM)
		{
			Selection().RemoveAll();
			Selection().AddItem(htiHit);
		}
		else
			SelectItem(htiHit);
	
		// save item handle so we don't rehandle in LButtonUp handler
		m_selection.htiLastHandledLBtnDown = htiHit;
	}
	
	// update anchor
	if (!bShift)
		Selection().SetAnchor(htiHit);
}

void CToDoCtrl::ProcessItemLButtonUp(HTREEITEM htiHit, int nHitFlags, UINT nColID)
{
	BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000); // ctrl key down
	BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000); // shift key down
	
	// if control is down and we're clicking on the file ref
	// then always display the file ref if its got one
	if (bCtrl && nColID == TDCC_FILEREF)
	{
		CString sFile = m_data.GetTaskFileRef(GetTaskID(htiHit));
		
		if (!sFile.IsEmpty())
			GotoFile(sFile, TRUE);
	}
	if (bCtrl && nColID == TDCC_DEPENDENCY)
	{
		CString sDepends = m_data.GetTaskDependency(GetTaskID(htiHit));
		
		if (!sDepends.IsEmpty())
			ShowTaskLink(0, sDepends);
	}
	else if (!IsReadOnly())
	{
		// if the button was over the checkbox and the button down
		// was too then set the appropriate done state
		if ((nHitFlags & TVHT_ONITEMSTATEICON) || nColID == TDCC_DONE)
		{
			SetSelectedTaskDone(!m_data.IsTaskDone(htiHit));
			return;
		}
		else if (nColID == TDCC_TRACKTIME)
		{
			if (GetSelectedCount() == 1 && IsItemSelected(htiHit) && 
				m_data.IsTaskTimeTrackable(htiHit))
				TimeTrackTask(htiHit);
			
			return;
		}
		else if (nColID == TDCC_FLAG)
		{
			BOOL bFlagged = m_data.IsTaskFlagged(GetTaskID(htiHit));
			SetSelectedTaskFlag(!bFlagged);
			return;
		}
	}
	
	// don't reprocess clicks that were processes in the button down handler
	if (!htiHit || htiHit == m_selection.htiLastHandledLBtnDown)
	{
		m_selection.htiLastHandledLBtnDown = NULL;
		return; // handled in button down
	}

	// if ctrl or shift are not pressed and we've 
	// only got one item selected and its the same 
	// as htiHit and htiPrev then we've nothing to do
	// else clear all and select the hit item
	if (!bCtrl && !bShift)
	{
		ASSERT (GetSelectedCount());
		
		if (GetSelectedCount() > 1 || GetSelectedItem() != htiHit)
		{
			// if the user clicked on the tree item itself then
			// don't call SelectItem here else in combination with the 
			// tree's own handling of this message it can trigger a label edit
			if (nHitFlags & TVHT_ONITEM)
			{
				Selection().RemoveAll();
				Selection().AddItem(htiHit);

				m_tree.RedrawGutter();

				UpdateControls(htiHit);
			}
			else
				SelectItem(htiHit);
		}
		
		// update anchor
		Selection().SetAnchor(htiHit);
	}
}

void CToDoCtrl::TimeTrackTask(HTREEITEM hti)
{
	ASSERT (GetSelectedCount() == 1); // sanity check
	
	DWORD dwTaskID = GetTaskID(hti), dwPrevTaskID = 0;

	if (dwTaskID == m_dwTimeTrackTaskID)
		EndTimeTracking(); // toggle off
	else
	{
		dwPrevTaskID = m_dwTimeTrackTaskID;
	
		if (m_data.IsTaskTimeTrackable(hti))
		{
			// if there's a current task being tracked then end it
			EndTimeTracking();

			m_dwTimeTrackTaskID = dwTaskID;
			m_dwTickLast = GetTickCount();
			
			// if the task's start date has not been set then set it now
			TODOITEM* pTDI = GetTask(dwTaskID);
			ASSERT (pTDI);
			
			if (pTDI->dateStart == 0)
				pTDI->dateStart = floor(COleDateTime::GetCurrentTime());

			// notify parent
			GetParent()->SendMessage(WM_TDCN_TIMETRACK, GetDlgCtrlID(), TRUE);
		}
	}
	
	if (hti == GetSelectedItem())
		UpdateControls();
	
	if (IsColumnShowing(TDCC_TRACKTIME) || IsColumnShowing(TDCC_TIMESPENT))
	{
		if (dwPrevTaskID)
			m_tree.RedrawGutter();
		else
			m_tree.RedrawGutterItem((DWORD)hti);
	}
}

void CToDoCtrl::EndTimeTracking()
{
	// if there's a current task being tracked then log it
	if (m_dwTimeTrackTaskID)
	{
		// add whatever time is still waiting
		IncrementTrackedTime();
		
		// log it
		if (HasStyle(TDCS_LOGTIMETRACKING))
		{
			CTaskTimeLog log(GetFilePath());

			log.LogTime(m_dwTimeTrackTaskID, m_dLogTime, HasStyle(TDCS_LOGTASKTIMESEPARATELY));
		}

		// reset taskID and time
		m_dwTickLast = 0;
		m_dwTimeTrackTaskID = 0;
		m_dLogTime = 0;

		// notify parent
		if (GetSafeHwnd())
			GetParent()->SendMessage(WM_TDCN_TIMETRACK, GetDlgCtrlID(), FALSE);
	}
}

CString CToDoCtrl::GetSelectedTaskTimeLogPath() const
{
	DWORD dwTaskID = GetSelectedTaskID();

	if (dwTaskID)
		return CTaskTimeLog(GetFilePath()).GetLogPath(dwTaskID, HasStyle(TDCS_LOGTASKTIMESEPARATELY));

	// else
	return "";
}

LRESULT CToDoCtrl::OnTreeRestoreFocusedItem(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_TASKLIST)
		m_tree.SelectItem((HTREEITEM)lParam);
	
	return 0L;
}

LRESULT CToDoCtrl::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return (LRESULT)::SendMessage(::GetParent(*this), WM_GETFONT, 0, 0);
}

BOOL CToDoCtrl::MultiSelectItems(const CDWordArray& aTasks, int nState, BOOL bRedraw)
{
	// select tasks in one hit
	int nTask = aTasks.GetSize();
	BOOL bSel = FALSE;

	while (nTask--)
	{
		HTREEITEM hti = m_data.GetItem(aTasks[nTask]);
		
		if (hti)
			bSel |= Selection().SetItem(hti, nState, bRedraw);
	}

	if (bSel)
	{
		if (bRedraw)
			m_tree.RedrawGutter();
		
		UpdateControls(); // load newly selected item
		UpdateSelectedTaskPath();
		
		// re-enable dragdrop
		m_treeDragDrop.EnableDragDrop(!IsReadOnly() && GetSelectedCount());
	}

	// else
	return bSel;
}

BOOL CToDoCtrl::MultiSelectItem(HTREEITEM hti, int nState, BOOL bRedraw) 
{ 
	if (Selection().SetItem(hti, nState, bRedraw))
	{
		if (bRedraw)
			m_tree.RedrawGutter();
		
		UpdateControls(); // load newly selected item
		UpdateSelectedTaskPath();
		
		// re-enable dragdrop
		m_treeDragDrop.EnableDragDrop(!IsReadOnly() && GetSelectedCount());
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CToDoCtrl::MultiSelectItems(HTREEITEM htiFrom, HTREEITEM htiTo, int nState, BOOL bRedraw)
{
	if (Selection().SetItems(htiFrom, htiTo, nState, bRedraw))
	{
		if (bRedraw)
			m_tree.RedrawGutter();
		
		UpdateControls(); // load newly selected items
		UpdateSelectedTaskPath();
		
		// re-enable dragdrop
		m_treeDragDrop.EnableDragDrop(!IsReadOnly() && GetSelectedCount());
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CToDoCtrl::HasTask(DWORD dwTaskID) 
{ 
	return (NULL != m_data.GetItem(dwTaskID));
}

BOOL CToDoCtrl::SelectTask(DWORD dwTaskID) 
{ 
	HTREEITEM hti = m_data.GetItem(dwTaskID);

	if (hti)
		SelectItem(hti);

	return (hti != NULL);
}

void CToDoCtrl::SelectItem(HTREEITEM hti) 
{ 
	if (m_tree.GetSafeHwnd()) 
	{
		if (Selection().RemoveAll() && !hti)
			m_tree.RedrawGutter();
		
		if (hti)
		{
			Selection().AddItem(hti);
			Selection().SetAnchor(hti);
			
			m_tree.SelectItem(hti); // will cause a sel change to update the controls 
			m_tree.TCH().EnsureVisibleEx(hti, TRUE);
		}
		
		UpdateControls(); // load newly selected item
		UpdateSelectedTaskPath();
		
		m_treeDragDrop.EnableDragDrop(!IsReadOnly());
	}
}

void CToDoCtrl::DeselectAll() 
{ 
	if (Selection().RemoveAll())
	{
		m_tree.RedrawGutter();
		
		UpdateControls(); // load newly selected item
		UpdateSelectedTaskPath();
	}
	
	// re-enable dragdrop
	m_treeDragDrop.EnableDragDrop(FALSE);
}

void CToDoCtrl::SelectAll() 
{ 
	if (Selection().AddAll())
	{
		m_tree.RedrawGutter();
		
		UpdateControls(); // load newly selected item
		UpdateSelectedTaskPath();
	}
	
	// re-enable dragdrop
	m_treeDragDrop.EnableDragDrop(FALSE);
}

LRESULT CToDoCtrl::OnGutterIsItemSelected(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGITEMSELECTION* pNCGIS = (NCGITEMSELECTION*)lParam;
	pNCGIS->bSelected = IsItemSelected((HTREEITEM)pNCGIS->dwItem);
	
	return TRUE;
}

LRESULT CToDoCtrl::OnGutterGetSelectedCount(WPARAM /*wParam*/, LPARAM lParam)
{
	int* pCount = (int*)lParam;
	*pCount = Selection().GetCount();
	
	return TRUE;
}

LRESULT CToDoCtrl::OnGutterNotifyHeaderClick(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGHDRCLICK* pNGHC = (NCGHDRCLICK*)lParam;
	
	if (pNGHC->nMsgID != WM_NCLBUTTONUP)
	{
		pNGHC->bPressed = FALSE;
		return TRUE;
	}
	
	TDC_SORTBY nPrev = m_nSortBy;
	TDC_SORTBY nSortBy = (TDC_SORTBY)-1;
	
	// convert index to TDC_COLUMN
	TDCCOLUMN* pTDCC = GetColumn(pNGHC->nColID);
	
	if (!pTDCC) // not one of ours so preserve existing sort selection
		return TRUE; // we handled it
	
	if (!IsColumnShowing(pTDCC->nColID))
		return TRUE; // we handled it
	
	switch (pTDCC->nColID)
	{
	case TDCC_CLIENT:		nSortBy = TDC_SORTBYNAME;			break;
	case TDCC_PRIORITY:		nSortBy = TDC_SORTBYPRIORITY;		break;
	case TDCC_ID:			nSortBy = TDC_SORTBYID;				break;
	case TDCC_ALLOCTO:		nSortBy = TDC_SORTBYALLOCTO;		break;
	case TDCC_ALLOCBY:		nSortBy = TDC_SORTBYALLOCBY;		break;
	case TDCC_STATUS:		nSortBy = TDC_SORTBYSTATUS;			break;
	case TDCC_CATEGORY:		nSortBy = TDC_SORTBYCATEGORY;		break;
	case TDCC_TIMEEST:		nSortBy = TDC_SORTBYTIMEEST;		break;
	case TDCC_TIMESPENT:	nSortBy = TDC_SORTBYTIMESPENT;		break;
	case TDCC_PERCENT:		nSortBy = TDC_SORTBYPERCENT;		break;
	case TDCC_STARTDATE:	nSortBy = TDC_SORTBYSTARTDATE;		break;
	case TDCC_DUEDATE:		nSortBy = TDC_SORTBYDUEDATE;		break;
	case TDCC_DONEDATE:		nSortBy = TDC_SORTBYDONEDATE;		break;
	case TDCC_DONE:			nSortBy = TDC_SORTBYDONE;			break;
	case TDCC_FLAG:			nSortBy = TDC_SORTBYFLAG;			break;
	case TDCC_CREATIONDATE:	nSortBy = TDC_SORTBYCREATIONDATE;	break;
	case TDCC_CREATEDBY:	nSortBy = TDC_SORTBYCREATEDBY;		break;
	case TDCC_LASTMOD:		nSortBy = TDC_SORTBYLASTMOD;		break;
	case TDCC_RISK:			nSortBy = TDC_SORTBYRISK;			break;
	case TDCC_EXTERNALID:	nSortBy = TDC_SORTBYEXTERNALID;		break;
	case TDCC_COST:			nSortBy = TDC_SORTBYCOST;			break;
	case TDCC_VERSION:		nSortBy = TDC_SORTBYVERSION;		break;
	case TDCC_RECURRENCE:	nSortBy = TDC_SORTBYRECURRENCE;		break;
		
	default: break;
	}
	
	if (nSortBy != (TDC_SORTBY)-1)
	{
		Sort(nSortBy);
		pNGHC->bPressed = FALSE;
		
		// notify parent
		GetParent()->SendMessage(WM_TDCN_SORT, GetDlgCtrlID(), MAKELPARAM((WORD)nPrev, (WORD)nSortBy));
		
		return TRUE; // we handled it
	}
	
	return 0;
}

LRESULT CToDoCtrl::OnGutterRecalcColWidth(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGRECALCCOLUMN* pNCRC = (NCGRECALCCOLUMN*)lParam;
	
	// convert colpos to TDC_COLUMN
	TDCCOLUMN* pTDCC = GetColumn(pNCRC->nColID);
	
	if (!pTDCC)
		return FALSE;
	
	if (!IsColumnShowing(pTDCC->nColID))
	{
		pNCRC->nWidth = 0;
		return TRUE;
	}
	
	switch (pTDCC->nColID)
	{
	case TDCC_ID:
		pNCRC->nWidth = Misc::GetTextWidth(pNCRC->pDC, "%u", m_dwNextUniqueID - 1);
		break; 
		
	case TDCC_PRIORITY:
		if (HasStyle(TDCS_HIDEPRIORITYNUMBER))
			pNCRC->nWidth = 10;
		else
		{
			pNCRC->nWidth = pNCRC->pDC->GetTextExtent("10").cx;
			pNCRC->nWidth = max(10, pNCRC->nWidth);
		}
		break; 
		
	case TDCC_RISK:
		pNCRC->nWidth = pNCRC->pDC->GetTextExtent("10").cx;
		pNCRC->nWidth = max(10, pNCRC->nWidth);
		break; 
		
	case TDCC_RECURRENCE:
		pNCRC->nWidth = CRecurringTaskEdit::CalcMaxRegularityWidth(pNCRC->pDC, FALSE);
		break; 
		
	case TDCC_EXTERNALID:
		{
			// determine the longest visible string
			CString sLongest = m_data.GetLongestVisibleExternalID();

			// calc required length based on average char width
			float fAve = Misc::GetAverageCharWidth(pNCRC->pDC);
			
			pNCRC->nWidth = (int)(sLongest.GetLength() * fAve);
			pNCRC->nWidth = max(10, pNCRC->nWidth);
		}
		break; 
		
	case TDCC_FILEREF:
		pNCRC->nWidth = HasStyle(TDCS_SHOWNONFILEREFSASTEXT) ? 60 : 16; 
		break; 
		
	case TDCC_DEPENDENCY:
		pNCRC->nWidth = 16; 
		break; 
		
	case TDCC_VERSION:
		pNCRC->nWidth = CalcMaxTextWidth(m_cbVersion, 20, FALSE, pNCRC->pDC);
		break;
		
	case TDCC_ALLOCTO:
		{
			// determine the longest visible string
			CString sLongest = m_data.GetLongestVisibleAllocTo();

			// include whatever's in the category edit field
			CString sEdit;
			m_cbAllocTo.GetWindowText(sEdit);

			if (sEdit.GetLength() > sLongest.GetLength())
				sLongest = sEdit;

			pNCRC->nWidth = pNCRC->pDC->GetTextExtent(sLongest).cx;
			pNCRC->nWidth = max(10, pNCRC->nWidth);
		}
		break;
		
	case TDCC_ALLOCBY:
		pNCRC->nWidth = CalcMaxTextWidth(m_cbAllocBy, 20, FALSE, pNCRC->pDC);
		break;
		
	case TDCC_STATUS:
		pNCRC->nWidth = CalcMaxTextWidth(m_cbStatus, 20, FALSE, pNCRC->pDC);
		break;
		
	case TDCC_CATEGORY:
		{
			// determine the longest visible string
			CString sLongest = m_data.GetLongestVisibleCategory();

			// include whatever's in the category edit field
			CString sEdit;
			m_cbCategory.GetWindowText(sEdit);

			if (sEdit.GetLength() > sLongest.GetLength())
				sLongest = sEdit;

			pNCRC->nWidth = pNCRC->pDC->GetTextExtent(sLongest).cx;
			pNCRC->nWidth = max(10, pNCRC->nWidth);
		}
		break;
		
	case TDCC_CREATEDBY:
		pNCRC->nWidth = 30; // for now
		break;
		
	case TDCC_COST:
		{
			// calc the greatest cumulative cost of the top level items
			int nWidth = 1;
			
			HTREEITEM hti = m_tree.GetChildItem(NULL);
			double dMaxCost = 0;
			
			while (hti)
			{
				TODOITEM* pTDI = GetTask(GetTaskID(hti));
				
				if (pTDI)
				{
					double dCost = m_data.CalcCost(hti, pTDI);
					dMaxCost = max(dMaxCost, dCost);
				}
				
				hti = m_tree.GetNextItem(hti, TVGN_NEXT);
			}
			
			if (dMaxCost > 0 || !HasStyle(TDCS_HIDEZEROTIMECOST))
				nWidth = Misc::GetTextWidth(pNCRC->pDC, "%.2f", dMaxCost);
			
			pNCRC->nWidth = nWidth;
		}
		break;
		
	case TDCC_TIMEEST:
		if (HasStyle(TDCS_DISPLAYHMSTIMEFORMAT))
			pNCRC->nWidth = pNCRC->pDC->GetTextExtent("12m4w").cx;
		else
		{
			int nWidth = 1;
			
			// else calc the greatest cumulative time estimate of the top level items
			HTREEITEM hti = m_tree.GetChildItem(NULL);
			double dMaxTime = 0;
			
			while (hti)
			{
				TODOITEM* pTDI = GetTask(GetTaskID(hti));
				
				if (pTDI)
				{
					double dTime = m_data.CalcTimeEstimate(hti, pTDI, TDITU_MINS);//TDITU_HOURS);
					dMaxTime = max(dMaxTime, dTime);
				}
				
				hti = m_tree.GetNextItem(hti, TVGN_NEXT);
			}
			
			if (dMaxTime > 0 || !HasStyle(TDCS_HIDEZEROTIMECOST))
			{
				int nDecPlaces = HasStyle(TDCS_ROUNDTIMEFRACTIONS) ? 0 : 2;
				
				CString sTime = CTimeHelper::FormatTime(dMaxTime, TDITU_MINS/*THU_HOURS*/, nDecPlaces);
				nWidth = pNCRC->pDC->GetTextExtent(sTime).cx;
			}
			
			pNCRC->nWidth = nWidth;
		}
		break;
		
	case TDCC_TIMESPENT:
		if (HasStyle(TDCS_DISPLAYHMSTIMEFORMAT))
			pNCRC->nWidth = pNCRC->pDC->GetTextExtent("12m4w").cx;
		else
		{
			int nWidth = 1;
			
			// else calc the greatest cumulative time spent of the top level items
			HTREEITEM hti = m_tree.GetChildItem(NULL);
			double dMaxTime = 0;
			
			while (hti)
			{
				TODOITEM* pTDI = GetTask(GetTaskID(hti));
				
				if (pTDI)
				{
					double dTime = m_data.CalcTimeSpent(hti, pTDI, TDITU_HOURS);
					dMaxTime = max(dMaxTime, dTime);
				}
				
				hti = m_tree.GetNextItem(hti, TVGN_NEXT);
			}
			
			if (dMaxTime > 0 || !HasStyle(TDCS_HIDEZEROTIMECOST))
			{
				int nDecPlaces = HasStyle(TDCS_ROUNDTIMEFRACTIONS) ? 0 : 2;
				
				CString sTime = CTimeHelper::FormatTime(dMaxTime, THU_HOURS, nDecPlaces);
				nWidth = pNCRC->pDC->GetTextExtent(sTime).cx;
			}
			
			pNCRC->nWidth = nWidth;
		}
		break;
		
	case TDCC_TRACKTIME:
	case TDCC_FLAG:
		pNCRC->nWidth = 16;
		break;
		
	case TDCC_PERCENT:
		pNCRC->nWidth = pNCRC->pDC->GetTextExtent("100%").cx;
		break;
		
	case TDCC_LASTMOD:
	case TDCC_CREATIONDATE:
	case TDCC_STARTDATE:
	case TDCC_DUEDATE:
	case TDCC_DONEDATE:
		{
			BOOL bISO = HasStyle(TDCS_SHOWDATESINISO);
			
			CString sDate = CDateHelper::FormatDate(COleDateTime(2000, 12, 30,0, 0, 0), bISO);
			
			if (HasStyle(TDCS_SHOWWEEKDAYINDATES))
			{
				int nLongestWD = 0, nLongestWDWidth = 0;

				// figure out the longest day in pixels
				for (int nWD = 1; nWD <= 7; nWD++)
				{
					int nWDWidth = pNCRC->pDC->GetTextExtent(CDateHelper::GetWeekday(nWD, TRUE)).cx;

					if (nWDWidth > nLongestWDWidth)
					{
						nLongestWDWidth = nWDWidth;
						nLongestWD = nWD;
					}
				}

				// build the entire date string
				sDate = CDateHelper::GetWeekday(nLongestWD, TRUE) + ' ' + sDate;
			}
	
			pNCRC->nWidth = pNCRC->pDC->GetTextExtent(sDate).cx;
		}
		break;
		
	case TDCC_DONE:
		// hide this when the done checkbox is displayed in the tree
		if (HasStyle(TDCS_TREECHECKBOXES))
			pNCRC->nWidth = 0;
		else
			pNCRC->nWidth = (18 -  2 * NCG_COLPADDING);
		break;
		
	default:
		ASSERT (0);
		break;
	}
	
	return TRUE;
}

int CToDoCtrl::GetTasklist(CTaskFile& tasklist, DWORD dwFlags) const
{
	int nCount = GetTasks(tasklist, TDCF_ALL, dwFlags);

	// add taskfile related stuff
	tasklist.SetHeader(m_sXmlHeader);
	tasklist.SetLastModified(CDateHelper::FormatCurrentDate(TRUE));
	tasklist.SetProjectName(m_sProjectName);
	tasklist.SetArchive(m_bArchive);
	tasklist.SetFileFormat(FILEFORMAT);
	tasklist.SetFileVersion(m_nFileVersion);

	return nCount;
}

int CToDoCtrl::GetTasks(CTaskFile& tasks, TDC_FILTER nFilter, DWORD dwFlags, LPCTSTR szAllocatedTo) const
{
	TDCFILTER filter;
	InitExportFilter(filter, nFilter, dwFlags, szAllocatedTo);

	AddChildren(NULL, tasks, NULL, filter);

	// and specify the next unique ID
	tasks.SetNextUniqueID(m_dwNextUniqueID);

	return tasks.GetTaskCount();
}

int CToDoCtrl::GetAllTasks(CTaskFile& tasks) const
{
	return GetTasks(tasks, TDCF_ALL);
}

HTREEITEM CToDoCtrl::SetAllTasks(const CTaskFile& tasks)
{
	Selection().RemoveAll(FALSE);
	m_data.DeleteAllTasks();
	
	return AddItem(tasks, NULL);
}

int CToDoCtrl::GetSelectedTasks(CTaskFile& tasks, DWORD dwFlags) const
{
	if (!GetSelectedCount())
		return FALSE;
	
	CHTIList selection;
	
	Selection().CopySelection(selection);
	Selection().RemoveChildDuplicates(selection, m_tree);
	
	TDCFILTER filter(TDCF_ALL, dwFlags);
	POSITION pos = selection.GetHeadPosition();
	
	while (pos)
	{
		HTREEITEM hti = selection.GetNext(pos);
		
		AddItem(hti, tasks, NULL, filter, 0);
	}
	
	return (tasks.GetTaskCount());
}

BOOL CToDoCtrl::SetTasks(const CTaskFile& tasks, BOOL bAppend)
{
	if (IsReadOnly())
		return FALSE;
	
	Flush();
	SaveExpandedState();
	
	// build the current tasklist into a CTaskFile object
	CTaskFile fileDest;

	if (bAppend)
	{
		GetAllTasks(fileDest);
		
		// merge dest tasks into source
		if (!fileDest.Merge(tasks, FALSE, TRUE))
			return TRUE; // just means no items were updated
	}

	CHoldRedraw hr2(&m_tree);
	CHoldRedraw hr1(this);

	HTREEITEM htiFirst = SetAllTasks(bAppend ? fileDest : tasks);
	HTREEITEM htiSel = LoadExpandedState(FALSE);
	
	SelectItem(htiSel ? htiSel : htiFirst);
	m_tree.SetFocus();
	
	m_bModified = TRUE;

	UpdateData(FALSE);
	UpdateComments(FALSE);
	
	return TRUE;
}

int CToDoCtrl::AddChildren(HTREEITEM hti, CTaskFile& file, HTASKITEM hTask, const TDCFILTER& filter) const
{
	if (filter.dwFlags & TDCGT_NOTSUBTASKS)
		return 0;

	HTREEITEM htiChild = m_tree.GetChildItem(hti);
	int nChildren = 0;
	int nPos = 1; // positions are 1-based
	
	while (htiChild)
	{
		if (AddItem(htiChild, file, hTask, filter, nPos))
			nChildren++;
		
		// next
		nPos++;
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return nChildren;
}

BOOL CToDoCtrl::AddItem(HTREEITEM hti, CTaskFile& file, HTASKITEM hParentTask, 
						const TDCFILTER& filter, int nPos) const
{
	// attributes
	DWORD dwID = GetTaskID(hti);
	TODOITEM* pTDI = GetTask(dwID);
	
	if (pTDI)
	{
		// special optimization check for TDC_FULLYDONE and TDC_NOTFULLYDONE
		if (filter.nFilter == TDCF_FULLYDONE || filter.nFilter == TDCF_NOTFULLYDONE)
		{
			BOOL bDone = m_data.IsTaskFullyDone(hti, pTDI, TRUE);
			
			if (filter.nFilter == TDCF_FULLYDONE && !bDone)
				return FALSE;
			
			else if (filter.nFilter == TDCF_NOTFULLYDONE && bDone)
			{
				// check 'flagged' flag
				if (!(filter.dwFlags & TDCGT_KEEPFLAGGED) || !pTDI->bFlagged) 
					return FALSE;
			}
			
			// else continue
		}

		BOOL bDone = pTDI->IsDone();
		BOOL bGoodAsDone = bDone ? TRUE : m_data.IsTaskDone(hti, TDCCHECKALL);
		
		BOOL bISODates = (filter.dwFlags & TDCGT_ISODATES);
		BOOL bVisCols = (filter.dwFlags & TDCGT_VISIBLECOLSONLY);
		BOOL bTitleCommentsOnly = m_tree.ItemHasChildren(hti) &&
								(filter.dwFlags & TDCGT_PARENTTITLECOMMENTSONLY);
		BOOL bTitleOnly = (filter.dwFlags & TDCGT_TITLESONLY);
		BOOL bHtmlComments = (filter.dwFlags & TDCGT_HTMLCOMMENTS);
		BOOL bTransform = (filter.dwFlags & TDCGT_TRANSFORM);

		HTASKITEM hTask = file.NewTask(pTDI->sTitle, hParentTask, dwID);
		
		// attributes
		file.SetTaskTitle(hTask, pTDI->sTitle);
		
		// hide ID if not wanted
		if (!WantExportColumn(TDCC_ID, bVisCols))
			file.HideAttribute(hTask, TDL_TASKID); 
		
		// comments
		if (!bTitleOnly)
		{
			if (bHtmlComments && !pTDI->sCustomComments.IsEmpty())
			{
				CString sHtml;
				m_mgrContent.ConvertContentToHtml(pTDI->sCustomComments, sHtml, m_idCommentsType);
				
				file.SetTaskHtmlComments(hTask, sHtml, bTransform);
			}
			
			if (!pTDI->sComments.IsEmpty())
				file.SetTaskComments(hTask, pTDI->sComments);
		}

		// items position
		if (!bTitleOnly && WantExportColumn(TDCC_POSITION, bVisCols))
			file.SetTaskPosition(hTask, nPos);

		// priority
		int nPriority = pTDI->nPriority;
		int nHighestPriority = m_data.GetHighestPriority(hti, pTDI, FALSE);

		if (!(bTitleOnly || bTitleCommentsOnly))
		{
			// last mod
			if (pTDI->HasLastMod())
			{
				if (WantExportColumn(TDCC_LASTMOD, bVisCols))
					file.SetTaskLastModified(hTask, pTDI->tLastMod, bISODates);
				else
					file.SetTaskLastModified(hTask, pTDI->tLastMod);
			}
			
			if (pTDI->bFlagged && WantExportColumn(TDCC_FLAG, bVisCols))
				file.SetTaskFlag(hTask, pTDI->bFlagged != FALSE);

			if (pTDI->trRecurrence.nRegularity != TDIR_ONCE && WantExportColumn(TDCC_RECURRENCE, bVisCols))
				file.SetTaskRecurrence(hTask, pTDI->trRecurrence, 
										CRecurringTaskEdit::GetRegularity(pTDI->trRecurrence.nRegularity));

// 			if (!pTDI->sAllocTo.IsEmpty() && WantExportColumn(TDCC_ALLOCTO, bVisCols))
// 				file.SetTaskAllocatedTo(hTask, pTDI->sAllocTo);

			if (pTDI->aAllocTo.GetSize() && WantExportColumn(TDCC_ALLOCTO, bVisCols))
				file.SetTaskAllocatedTo(hTask, pTDI->aAllocTo);

			if (!pTDI->sAllocBy.IsEmpty() && WantExportColumn(TDCC_ALLOCBY, bVisCols))
				file.SetTaskAllocatedBy(hTask, pTDI->sAllocBy);

			if (!pTDI->sStatus.IsEmpty() && WantExportColumn(TDCC_STATUS, bVisCols))
				file.SetTaskStatus(hTask, pTDI->sStatus);

			if (!pTDI->sVersion.IsEmpty() && WantExportColumn(TDCC_VERSION, bVisCols))
				file.SetTaskVersion(hTask, pTDI->sVersion);

			if (pTDI->aCategories.GetSize() && WantExportColumn(TDCC_CATEGORY, bVisCols))
				file.SetTaskCategories(hTask, pTDI->aCategories);

			if (!pTDI->sFileRefPath.IsEmpty() && WantExportColumn(TDCC_FILEREF, bVisCols))
				file.SetTaskFileReferencePath(hTask, pTDI->sFileRefPath);
 
			if (!pTDI->sCreatedBy.IsEmpty() && WantExportColumn(TDCC_CREATEDBY, bVisCols))
				file.SetTaskCreatedBy(hTask, pTDI->sCreatedBy);
 
			if (!pTDI->sExternalID.IsEmpty() && WantExportColumn(TDCC_EXTERNALID, bVisCols))
				file.SetTaskExternalID(hTask, pTDI->sExternalID);
 
			if (!pTDI->sDependency.IsEmpty() && WantExportColumn(TDCC_DEPENDENCY, bVisCols))
				file.SetTaskDependency(hTask, pTDI->sDependency);
 
			if (WantExportColumn(TDCC_PRIORITY, bVisCols))
			{
				file.SetTaskPriority(hTask, nPriority);

				if (nHighestPriority > nPriority)
					file.SetTaskHighestPriority(hTask, nHighestPriority);
			}

			if (WantExportColumn(TDCC_RISK, bVisCols))
			{
				file.SetTaskRisk(hTask, pTDI->nRisk);

				int nHighestRisk = m_data.GetHighestRisk(hti, pTDI);

				if (nHighestRisk > pTDI->nRisk)
					file.SetTaskHighestRisk(hTask, nHighestRisk);
			}

			// percent done
			if (WantExportColumn(TDCC_PERCENT, bVisCols))
			{
				file.SetTaskPercentDone(hTask, (unsigned char)(pTDI->IsDone() ? 100 : pTDI->nPercentDone));

				int nPercent = m_data.CalcPercentDone(hti, pTDI);

				if (nPercent > 0)
					file.SetTaskCalcCompletion(hTask, nPercent);
			}
			
			// cost
			if (WantExportColumn(TDCC_COST, bVisCols))
			{
				if (pTDI->dCost > 0)
					file.SetTaskCost(hTask, pTDI->dCost);

				double dCost = m_data.CalcCost(hti, pTDI);

				if (dCost > 0)
					file.SetTaskCalcCost(hTask, dCost);
			}
			
			// time estimate
			if (WantExportColumn(TDCC_TIMEEST, bVisCols))
			{
				if (pTDI->dTimeEstimate > 0)
					file.SetTaskTimeEstimate(hTask, pTDI->dTimeEstimate, m_data.MapTimeUnits(pTDI->nTimeEstUnits)[0]);

				double dTime = m_data.CalcTimeEstimate(hti, pTDI, TDITU_HOURS);

				if (dTime > 0)
					file.SetTaskCalcTimeEstimate(hTask, dTime);
			}
			
			// time spent
			if (WantExportColumn(TDCC_TIMESPENT, bVisCols))
			{
				if (pTDI->dTimeSpent > 0)
					file.SetTaskTimeSpent(hTask, pTDI->dTimeSpent, m_data.MapTimeUnits(pTDI->nTimeSpentUnits)[0]);

				double dTime = m_data.CalcTimeSpent(hti, pTDI, TDITU_HOURS);

				if (dTime > 0)
					file.SetTaskCalcTimeSpent(hTask, dTime);
			}
			
			// done date
			if (bDone)
			{
				file.SetTaskDoneDate(hTask, pTDI->dateDone, bISODates == TRUE);

				// hide it if column not visible
				if (!WantExportColumn(TDCC_DONEDATE, bVisCols))
					file.HideAttribute(hTask, TDL_TASKDONEDATE);
			}
			
			// add due date if we're filtering by due date
			if (pTDI->HasDue() && (filter.dateDueBy > 0 || WantExportColumn(TDCC_DUEDATE, bVisCols)))
				file.SetTaskDueDate(hTask, pTDI->dateDue, bISODates == TRUE);

			if (HasStyle(TDCS_USEEARLIESTDUEDATE) && WantExportColumn(TDCC_DUEDATE, bVisCols))
			{
				double dDate = m_data.GetEarliestDueDate(hti, pTDI, TRUE);

				if (dDate > 0)
					file.SetTaskEarliestDueDate(hTask, dDate);
			}

			// start date
			if (pTDI->HasStart() && WantExportColumn(TDCC_STARTDATE, bVisCols))
				file.SetTaskStartDate(hTask, pTDI->dateStart, bISODates == TRUE);

			// creation date
			if (WantExportColumn(TDCC_CREATIONDATE, bVisCols))
				file.SetTaskCreationDate(hTask, pTDI->dateCreated, bISODates == TRUE);

			// modify date
			if (pTDI->HasLastMod() && WantExportColumn(TDCC_LASTMOD, bVisCols))
				file.SetTaskLastModified(hTask, pTDI->tLastMod, bISODates == TRUE);

			// custom comments
			if (!bVisCols && !pTDI->sCustomComments.IsEmpty())
				file.SetTaskCustomComments(hTask, pTDI->sCustomComments);
		}
		else if (bDone)
		{
			file.SetTaskDoneDate(hTask, pTDI->dateDone, bISODates == TRUE);
			file.HideAttribute(hTask, TDL_TASKDONEDATE);
		}
		
		// task color
		if (pTDI->color)
			file.SetTaskColor(hTask, pTDI->color);

		// actual text color
		COLORREF colorText = pTDI->color;
		
		if (bDone)
			colorText = m_crTaskDone;

		else if (pTDI->IsDue())
			colorText = m_crDue;//GetPriorityColor(DUECOLORINDEX);

		else if (HasStyle(TDCS_COLORTEXTBYCATEGORY))
			GetCategoryColor(pTDI->GetFirstCategory(), colorText);

		else if (HasStyle(TDCS_COLORTEXTBYPRIORITY))
			colorText = GetPriorityColor(nHighestPriority);

		if (colorText)
			file.SetTaskTextColor(hTask, colorText);

		// priority color
		file.SetTaskPriorityColor(hTask, GetPriorityColor(nHighestPriority));
		
		// we return TRUE if we match the filter _or_ we have any matching children
		BOOL bMatch = AddChildren(hti, file, hTask, filter);

		if (!bMatch) //  no children matched
		{
			switch (filter.nFilter)
			{
			case TDCF_ALL:
			case TDCF_FULLYDONE: // we did a check at the start
			case TDCF_NOTFULLYDONE: // we did a check at the start
				bMatch = TRUE; // always
				break;
				
			case TDCF_DUE:
			case TDCF_DUETOMORROW:
			case TDCF_DUETHISWEEK:
			case TDCF_DUENEXTWEEK:
			case TDCF_DUETHISMONTH:
			case TDCF_DUENEXTMONTH:
				bMatch |= (!bGoodAsDone && pTDI->IsDue(filter.dateDueBy));
				break;
				
			case TDCF_DONE:
				bMatch |= (bGoodAsDone || bDone);
				break;
				
			case TDCF_NOTDONE:
				bMatch |= !bGoodAsDone; // 'good as' includes 'done'

				// check 'flagged' flag
				if (!bMatch && (filter.dwFlags & TDCGT_KEEPFLAGGED) && pTDI->bFlagged) 
					bMatch = TRUE;
				break;
				
			default:
				bMatch = FALSE;
			}

			// then check 'allocated to' if set
			if (bMatch && !filter.sAllocTo.IsEmpty())
				bMatch = Misc::Find(pTDI->aAllocTo, filter.sAllocTo);
		}
		
		// if we don't match, we remove the item unless we're flagged
		if (!bMatch)
			file.DeleteTask(hTask);
		
		return bMatch;
	}
	
	return FALSE;
}

void CToDoCtrl::OnGotoFileRef()
{
	if (!m_sFileRefPath.IsEmpty())
		GotoFile(m_sFileRefPath, TRUE);
}

void CToDoCtrl::SetFocusToTree()
{
	if (GetFocus() != &m_tree)
		m_tree.SetFocus();

	// ensure the selected tree item is visible
	if (GetSelectedCount())
		m_tree.TCH().EnsureVisibleEx(m_tree.GetSelectedItem(), FALSE);
	else
		SelectItem(m_tree.GetChildItem(NULL));

	m_tree.RedrawGutter();
}

LRESULT CToDoCtrl::OnDropFileRef(WPARAM wParam, LPARAM lParam)
{
	if (IsReadOnly())
		return 0;
	
	HTREEITEM hti = (HTREEITEM)wParam;
	
	if (hti)
	{
		SelectItem(hti);
		SetSelectedTaskFileRef((LPCTSTR)lParam);
	}
	else // its the file ref edit field
	{
		m_eFileRef.SetWindowText((LPCTSTR)lParam);
	}
	
	return 0;
}

void CToDoCtrl::SaveSortState()
{
	// create a new key using the filepath
	ASSERT (GetSafeHwnd());
	
	if (!m_sLastSavePath.IsEmpty())
	{
		CString sKey, sFilePath(m_sLastSavePath);
		sFilePath.Replace('\\', '_');
		sKey.Format("FileStates\\%s\\SortState", sFilePath);
		
		AfxGetApp()->WriteProfileInt(sKey, "Column", m_nSortBy);
		AfxGetApp()->WriteProfileInt(sKey, "Ascending", m_bSortAscending);
	}
}

void CToDoCtrl::LoadSortState(LPCTSTR szFilePath)
{
	CString sFilePath(szFilePath);
	
	if (sFilePath.IsEmpty())
		sFilePath = m_sLastSavePath;
	
	if (!sFilePath.IsEmpty())
	{
		sFilePath.Replace('\\', '_');
		
		CString sKey;
		sKey.Format("FileStates\\%s\\SortState", sFilePath);
		
		m_nSortBy = (TDC_SORTBY)AfxGetApp()->GetProfileInt(sKey, "Column", TDC_SORTUNDEF);
		m_bSortAscending = AfxGetApp()->GetProfileInt(sKey, "Ascending", TRUE);

		UpdateColumnHeaderClicking();
	}
}

void CToDoCtrl::SaveSplitPos()
{
	ASSERT (GetSafeHwnd());
	
	if (!m_sLastSavePath.IsEmpty())
	{
		CString sKey, sFilePath(m_sLastSavePath);
		sFilePath.Replace('\\', '_');
		sKey.Format("FileStates\\%s", sFilePath);

		AfxGetApp()->WriteProfileInt(sKey, "SplitPos", m_nCommentsSize);
	}

	if (HasStyle(TDCS_SHAREDCOMMENTSHEIGHT))
		AfxGetApp()->WriteProfileInt("FileStates", "SharedSplitPos", s_nCommentsSize);
}

void CToDoCtrl::LoadSplitPos()
{
	s_nCommentsSize = AfxGetApp()->GetProfileInt("FileStates", "SharedSplitPos", DEFCOMMENTSIZE);

	if (!m_sLastSavePath.IsEmpty())
	{
		CString sKey, sFilePath(m_sLastSavePath);
		sFilePath.Replace('\\', '_');
		sKey.Format("FileStates\\%s", sFilePath);

		m_nCommentsSize = AfxGetApp()->GetProfileInt(sKey, "SplitPos", DEFCOMMENTSIZE);
	}
	else
		m_nCommentsSize = s_nCommentsSize;

	if (!s_nCommentsSize)
		s_nCommentsSize = m_nCommentsSize;
}

void CToDoCtrl::SaveExpandedState()
{
	ASSERT (GetSafeHwnd());
	
	// ignore this if we have no tasks
	if (GetTaskCount() == 0)
		return;
	
	// create a new key using the filepath and simply save the ID 
	// of every expanded item
	if (!m_sLastSavePath.IsEmpty())
	{
		CString sKey, sFilePath(m_sLastSavePath);
		sFilePath.Replace('\\', '_');
		sKey.Format("FileStates\\%s\\ExpandedState", sFilePath);
		
		int nCount = m_data.SaveTreeExpandedState(sKey);
		
		// save expanded count
		AfxGetApp()->WriteProfileInt(sKey, "Count", nCount);
		
		// and selected item
		AfxGetApp()->WriteProfileInt(sKey, "SelItem", GetSelectedTaskID());
	}
}

HTREEITEM CToDoCtrl::LoadExpandedState(BOOL bResetSel)
{
	ASSERT (GetSafeHwnd());
	
	CString sKey, sFilePath(m_sLastSavePath);
	sFilePath.Replace('\\', '_');
	sKey.Format("FileStates\\%s\\ExpandedState", sFilePath);
	
	m_data.LoadTreeExpandedState(sKey);

	// restore prev selected item
	UINT uID = AfxGetApp()->GetProfileInt(sKey, "SelItem", 0);
	
	if (uID)
	{
		// find the corresponding tree item
		HTREEITEM hti = NULL;
		
		CHTIMap htiMap;
		m_data.BuildHTIMap(htiMap);
		
		if (htiMap.Lookup(uID, hti) && hti)
		{
			if (bResetSel)
				SelectItem(hti);
			
			return hti;
		}
	}
	
	return NULL;
}

LRESULT CToDoCtrl::OnGutterWidthChange(WPARAM /*wParam*/, LPARAM lParam)
{
	// let parent know if min width has changed
	int nPrevWidth = LOWORD(lParam);
	int nNewWidth = HIWORD(lParam);
	
	if (nNewWidth < nPrevWidth || (!nPrevWidth && nNewWidth))
		GetParent()->SendMessage(WM_TDCN_MINWIDTHCHANGE);
	
	return 0;
}

LRESULT CToDoCtrl::OnGutterGetCursor(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGGETCURSOR* pNCGGC = (NCGGETCURSOR*)lParam;

	BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000); // ctrl key down
	BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000); // shift key down

	HTREEITEM hti = (HTREEITEM)pNCGGC->dwItem;

	switch (pNCGGC->nColID)
	{
	case TDCC_FILEREF:
		// we handle the file ref column if the ctrl key is down
		if (bCtrl && hti)
		{
			DWORD dwUniqueID = GetTaskID(hti);
			TODOITEM* pTDI = GetTask(dwUniqueID);
			
			if (pTDI && !pTDI->sFileRefPath.IsEmpty())
				return (LRESULT)Misc::HandCursor();
		}
		break;
		
	case TDCC_DEPENDENCY:
		// we handle the depends column if the ctrl key is down
		if (bCtrl && hti)
		{
			DWORD dwUniqueID = GetTaskID(hti);
			TODOITEM* pTDI = GetTask(dwUniqueID);
			
			if (pTDI && !pTDI->sDependency.IsEmpty())
				return (LRESULT)Misc::HandCursor();
		}
		break;
		
	case TDCC_TRACKTIME:
		// check tasklist is editable, task is trackable and 
		// neither the ctrl not shift keys are pressed (ie => multiple selection)
		if (!bCtrl && !bShift && !IsReadOnly() && m_data.IsTaskTimeTrackable(hti))
		{
			return (LRESULT)Misc::HandCursor();
		}
		break;

	case TDCC_FLAG:
		if (hti)
			return (LRESULT)Misc::HandCursor();
		break;
	}
	
	return 0L;
}

void CToDoCtrl::Flush() // called to end current editing actions
{
	m_tree.TCH().EndLabelEdit(FALSE);

	m_data.ResetCachedCalculations();
}

BOOL CToDoCtrl::CheckIn()
{
	if (!HasStyle(TDCS_ENABLESOURCECONTROL))
		return FALSE;
	
	if (m_sLastSavePath.IsEmpty())
		return TRUE;
	
	CTaskFile file(m_sPassword);
	
	// backup the file
	CFileBackup backup(m_sLastSavePath);

	// load the file in its encrypted state because we're not interested 
	// in changing that bit of it
	if (file.Open(m_sLastSavePath, XF_READWRITE, FALSE) && file.LoadEx())
	{
		m_bSourceControlled = file.IsSourceControlled();

		if (m_bSourceControlled)
		{
			CString sCheckedOutTo = file.GetCheckOutTo();
			
			if (sCheckedOutTo.IsEmpty() || sCheckedOutTo == m_sMachineName) // its us
			{
				file.SetCheckedOutTo(""); // retain the item
				
				// and rewrite the file but keeping the same timestamp
				FILETIME ftMod;
				::GetFileTime((HANDLE)file.GetFileHandle(), NULL, NULL, &ftMod);
				
				m_bCheckedOut = !file.SaveEx(); // ie if successfully saved then m_bCheckedOut == 0
				
				::SetFileTime((HANDLE)file.GetFileHandle(), NULL, NULL, &ftMod);
				
				return !m_bCheckedOut;
			}
		}
	}
	
	// else someone else or invalid file
	return FALSE;
}

BOOL CToDoCtrl::CheckOut()
{
	CString sTemp;
	return CheckOut(sTemp);
}

BOOL CToDoCtrl::CheckOut(CString& sCheckedOutTo)
{
	if (!HasStyle(TDCS_ENABLESOURCECONTROL))
		return FALSE;
	
	if (m_sLastSavePath.IsEmpty())
		return TRUE;
	
	CTaskFile file(m_sPassword);
	CWaitCursor cursor;
	
	// backup the file
	CFileBackup backup(m_sLastSavePath);

	if (file.Open(m_sLastSavePath, XF_READWRITE, FALSE) && file.LoadEx())
	{
		m_bCheckedOut = file.CheckOut(sCheckedOutTo);
		m_bSourceControlled = file.IsSourceControlled();

		if (m_bCheckedOut)
		{
		    file.Decrypt();

			// restore unsorted order
			file.SortTasksByPos();
			
			Load(file); // reload file
			return TRUE;
		}
	}
	
	return FALSE;
}

int CToDoCtrl::FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const
{
	return m_data.FindTasks(params, aResults);
}

DWORD CToDoCtrl::FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	return m_data.FindFirstTask(params, result);
}

BOOL CToDoCtrl::TaskMatches(DWORD dwID, const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	return m_data.TaskMatches(dwID, params, result);
}

void CToDoCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// is it over the comments splitter
	ASSERT (!m_bSplitting);
	
	if ((!HasStyle(TDCS_MAXIMIZE) || HasStyle(TDCS_SHOWCOMMENTSALWAYS)) &&
		GetSplitterRect().PtInRect(point))
	{
		m_bSplitting = TRUE;
		SetCapture();
	}
	
	CRuntimeDlg::OnLButtonDown(nFlags, point);
}

void CToDoCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bSplitting)
	{
		ReleaseCapture();
		m_bSplitting = FALSE;
	}
	
	CRuntimeDlg::OnLButtonUp(nFlags, point);
}

void CToDoCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSplitting)
	{
		CRect rComments, rClient, rTaskList;
		
		GetClientRect(rClient);
		
		GetDlgItem(IDC_TASKLIST)->GetWindowRect(rTaskList);
		ScreenToClient(rTaskList);

		int nNewSize, nMaxCommentSize;
		
		if (HasStyle(TDCS_VERTCOMMENTS))
		{
			nMaxCommentSize  = rClient.Width() - GetMinNonCommentSize();
			nNewSize = m_nCommentsSize - (point.x - rTaskList.right);
		}
		else
		{
			nMaxCommentSize  = rClient.Height() - GetMinNonCommentSize();
			nNewSize = m_nCommentsSize - (point.y - rTaskList.bottom);
		}

		nNewSize = min(max(nNewSize, MINCOMMENTSIZE), nMaxCommentSize);
		
		if (nNewSize != m_nCommentsSize)
		{
			m_nCommentsSize = nNewSize;
			Resize();
			UpdateWindow();
		}
	}
	
	CRuntimeDlg::OnMouseMove(nFlags, point);
}

CRect CToDoCtrl::GetSplitterRect()
{
	CRect rSplitter, rClient;
	
	GetDlgItem(IDC_TASKLIST)->GetWindowRect(rSplitter);
	ScreenToClient(rSplitter);
	
	if (HasStyle(TDCS_VERTCOMMENTS))
	{
		GetClientRect(rClient);
		rSplitter.left = rSplitter.right;
		rSplitter.right += SPLITSIZE;
		rSplitter.top = 0;
		rSplitter.bottom = rClient.bottom;
	}
	else
	{
		rSplitter.top = rSplitter.bottom;
		rSplitter.bottom += SPLITSIZE;
	}
	
	return rSplitter;
}

BOOL CToDoCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (!HasStyle(TDCS_MAXIMIZE) || HasStyle(TDCS_SHOWCOMMENTSALWAYS))
	{
		CPoint point(::GetMessagePos());
		ScreenToClient(&point);
		
		if (GetSplitterRect().PtInRect(point))
		{
			if (HasStyle(TDCS_VERTCOMMENTS))
				SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR));
			else
				SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR));
	
			return TRUE;
		}
	}
	
	// else
	return CRuntimeDlg::OnSetCursor(pWnd, nHitTest, message);
}

void CToDoCtrl::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_bSplitting)
	{
		// save latest split pos
		s_nCommentsSize = m_nCommentsSize;
		m_bSplitting = FALSE;
	}
	
	CRuntimeDlg::OnCaptureChanged(pWnd);
}

void CToDoCtrl::UpdateColumnHeaderClicking()
{
	BOOL bEnable = HasStyle(TDCS_COLUMNHEADERCLICKING) && (m_nSortBy != TDC_SORTDISABLED);
	
	// extra gutter columns
	for (int nCol = 0; nCol < NUM_COLUMNS; nCol++)
	{
		TDCCOLUMN& tdcc = COLUMNS[nCol];
		
		m_tree.EnableGutterColumnHeaderClicking(tdcc.nColID, tdcc.bClickable && bEnable);
	}
}

void CToDoCtrl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CRuntimeDlg::OnShowWindow(bShow, nStatus);
	
	if (bShow)
	{
		// resize if we have shared splitter pos and its different
		if (HasStyle(TDCS_SHAREDCOMMENTSHEIGHT) && (m_nCommentsSize != s_nCommentsSize))
		{
			m_nCommentsSize = s_nCommentsSize;
			Resize();
		}

		// re-calc the tree nc border if the comments are to the right
		// to 'fix' a UI quirk
		if (HasStyle(TDCS_VERTCOMMENTS))
			m_tree.SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); 
	}
}

LRESULT CToDoCtrl::OnTimeUnitsChange(WPARAM wParam, LPARAM /*lParam*/)
{
	switch (wParam)
	{
	case IDC_TIMEEST:
		UpdateTask(TDCA_TIMEEST); 
		break;
	
	case IDC_TIMESPENT:
		UpdateTask(TDCA_TIMESPENT); 
		break;
	}
	
	return 0;
}

void CToDoCtrl::SpellcheckSelectedTask(BOOL bTitle) 
{
	// one off spell check
	CSpellCheckDlg dialog(NULL, NULL);
	
	SpellcheckItem(GetSelectedItem(), &dialog, bTitle, TRUE);
}

BOOL CToDoCtrl::CanSpellcheckSelectedTaskComments()
{
	if (!CanSpellcheckComments())
		return FALSE;

	return !GetSelectedTaskComments().IsEmpty();
}

BOOL CToDoCtrl::CanSpellcheckComments()
{
	return Misc::GuidIsNull(m_idCommentsType);
}

void CToDoCtrl::Spellcheck()
{
	// top level items
	CSpellCheckDlg dialog(NULL, NULL);
	
	HTREEITEM hti = m_tree.GetChildItem(NULL);
	
	while (hti)
	{
		if (!SpellcheckItem(hti, &dialog))
			return;
		
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}
}

BOOL CToDoCtrl::SpellcheckItem(HTREEITEM hti, CSpellCheckDlg* pSpellChecker, BOOL bTitle, BOOL bNotifyNoErrors)
{
	ASSERT(pSpellChecker);
	
	if (!pSpellChecker)
		return FALSE;

	if (!bTitle && !CanSpellcheckComments())
		return TRUE;
	
	DWORD dwID = GetTaskID(hti);
	TODOITEM* pTDI = m_data.GetTask(dwID);
	
	if (pTDI)
	{
		CString sText = bTitle ? m_data.GetTaskTitle(dwID) : m_data.GetTaskComments(dwID);
		
		if (!sText.IsEmpty())
		{
			SelectItem(hti);
			pSpellChecker->SetText(sText);
			
			int nRet = pSpellChecker->DoModal(TRUE);
			
			if (nRet == IDOK && !IsReadOnly())
			{
				int nChange = SET_NOCHANGE;
				sText = pSpellChecker->GetCorrectedText();
				
				if (bTitle)
				{
					nChange = m_data.SetTaskTitle(dwID, sText);
					
					if (nChange == SET_CHANGE)
						m_tree.SetItemText(hti, sText);
				}
				else
					nChange = m_data.SetTaskComments(dwID, sText);
				
				if (nChange == SET_CHANGE)
					SetModified(TRUE, TDCA_TASKNAME);
			}
			else if (nRet == IDNOERRORS && bNotifyNoErrors)
			{
				MessageBoxEx(this, bTitle ? IDS_TDC_NOTITLESPELLERRORS : IDS_TDC_NOCOMMENTSPELLERRORS, 
							IDS_TDC_SPELLCHECK_TITLE);
			}
			else if (nRet == IDCANCEL)
				return FALSE;
		}
		
		return TRUE;
	}
	
	// else
	ASSERT(0);
	return FALSE;
}

BOOL CToDoCtrl::SpellcheckItem(HTREEITEM hti, CSpellCheckDlg* pSpellChecker)
{
	if (!SpellcheckItem(hti, pSpellChecker, TRUE, FALSE) || 
		!SpellcheckItem(hti, pSpellChecker, FALSE, FALSE))
		return FALSE;
	
	// subtasks
	HTREEITEM htiSub = m_tree.GetNextItem(hti, TVGN_CHILD);
	
	while (htiSub)
	{
		if (!SpellcheckItem(htiSub, pSpellChecker))
			return FALSE;
		
		htiSub = m_tree.GetNextItem(htiSub, TVGN_NEXT);
	}
	
	return TRUE;
}

void CToDoCtrl::SetDefaultTaskAttributes(LPCTSTR szTitle, LPCTSTR szComments, COLORREF color,
										 const double& dTimeEst, int nTimeEstUnits, 
										 const CStringArray& aAllocTo/*LPCTSTR szAllocTo*/,
										 LPCTSTR szAllocBy, LPCTSTR szStatus, const CStringArray& aCats,
										 int nPriority, int nRisk, BOOL bAutoStartDate, 
										 LPCTSTR szCreatedBy, double dCost)
{
	s_tdDefault.sTitle = szTitle;
	s_tdDefault.sComments = szComments;
	s_tdDefault.color = color;
	s_tdDefault.dateStart.m_dt = bAutoStartDate ? -1 : 0;
//	s_tdDefault.sAllocTo = szAllocTo;
	s_tdDefault.sAllocBy = szAllocBy;
	s_tdDefault.sStatus = szStatus;
	s_tdDefault.dTimeEstimate = dTimeEst;
	s_tdDefault.nTimeEstUnits = nTimeEstUnits;
	s_tdDefault.nTimeSpentUnits = nTimeEstUnits; // to match
	s_tdDefault.sCreatedBy = szCreatedBy;
	s_tdDefault.dCost = dCost;

	s_tdDefault.aAllocTo.Copy(aAllocTo);
	s_tdDefault.aCategories.Copy(aCats);

	if (nPriority != FT_NOPRIORITY)
		nPriority = max(0, min(10, nPriority));

	s_tdDefault.nPriority = nPriority;

	if (nRisk != FT_NORISK)
		nRisk = max(0, min(10, nRisk));

	s_tdDefault.nRisk = nRisk;
	
	UpdateControls();
}

LRESULT CToDoCtrl::OnEEBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDC_TIMESPENT:
		if (lParam == ID_TIMEEDITBTN && GetSelectedCount() == 1)
			TimeTrackTask(GetSelectedItem());
		break;

	case IDC_DEPENDS:
		{
			CString sLink = GetSelectedTaskDependency();

			if (!sLink.IsEmpty())
				ShowTaskLink(0, sLink);
		}
		break;
	}
	
	return 0L;
}

void CToDoCtrl::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case TIMER_TRACK:
		IncrementTrackedTime();
		break;

	case TIMER_MIDNIGHT:
		{
			// check if we've just passed midnight, in which case some tasks
			// may have just become due
			static time_t tPrev = time(NULL); // once only
			time_t tNow = time(NULL);

			if (tNow / DAY_IN_SECS > tPrev / DAY_IN_SECS)
			{
				m_data.ResetCachedCalculations();
				Invalidate();
			}

			tPrev = tNow;
		}
		break;
	}

	
	CRuntimeDlg::OnTimer(nIDEvent);
}

void CToDoCtrl::IncrementTrackedTime()
{
	DWORD dwTick = GetTickCount();
	
	// if time tracking is paused we such jump out
	if (IsActivelyTimeTracking())
	{
		int nUnits;
		double dTime = m_data.GetTaskTimeSpent(m_dwTimeTrackTaskID, nUnits);
		
		ASSERT (m_dwTickLast);
		double dInc = (dwTick - m_dwTickLast) * TICKS2HOURS;
		
		dTime = CTimeHelper::GetTime(dTime, nUnits, THU_HOURS);
		dTime += dInc;
		m_dLogTime += dInc;
		dTime = CTimeHelper::GetTime(dTime, THU_HOURS, nUnits);
		
		if (m_dwTimeTrackTaskID == GetSelectedTaskID())
			SetSelectedTaskTimeSpent(dTime, nUnits);
		else
		{
			m_data.SetTaskTimeSpent(m_dwTimeTrackTaskID, dTime, nUnits);
			SetModified(TRUE, TDCA_TIMESPENT);
		}
		
		TRACE ("%d ms added to task %d\n", (dwTick - m_dwTickLast), m_dwTimeTrackTaskID);
	}
	else if (m_dwTimeTrackTaskID)
		TRACE ("no time added to task %d\n", m_dwTimeTrackTaskID);
	
	m_dwTickLast = dwTick;
}

LRESULT CToDoCtrl::OnCustomUrl(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	ASSERT(wParam == (WPARAM)m_ctrlComments.GetDlgCtrlID());
	
	GotoFile((LPCTSTR)lParam, FALSE);
	
	return 0;
}

void CToDoCtrl::SelectNextTasksInHistory() 
{ 
	if (Selection().NextSelection())
	{
		HTREEITEM hti = m_tree.GetSelectedItem();
		
		if (!Selection().HasItem(hti) && Selection().GetCount())
			m_tree.SelectItem(Selection().GetFirstItem());
	}
}

void CToDoCtrl::SelectPrevTasksInHistory() 
{ 
	if (Selection().PrevSelection())
	{
		HTREEITEM hti = m_tree.GetSelectedItem();
		
		if (!Selection().HasItem(hti) && Selection().GetCount())
			m_tree.SelectItem(Selection().GetFirstItem());
	}
}

LRESULT CToDoCtrl::OnFileEditWantIcon(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_FILEPATH)
	{
		const CString& sUrl = (LPCTSTR)lParam;
		
		if (sUrl.Find(TDL_PROTOCOL) != -1)
			return (LRESULT)AfxGetMainWnd()->SendMessage(WM_GETICON, ICON_SMALL, 0);
	}
	
	return 0;
}

LRESULT CToDoCtrl::OnFileEditDisplayFile(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_FILEPATH && GotoFile((LPCTSTR)lParam, FALSE))
		return TRUE;
	
	return 0;
}

BOOL CToDoCtrl::GotoFile(const CString& sUrl, BOOL bShellExecute)
{
	if (sUrl.IsEmpty())
		return FALSE;
	
	// see if its a 'tdl://'
	int nFind = sUrl.Find(TDL_PROTOCOL);
	
	if (nFind >= 0)
	{
		OnTDCDoTaskLink(0, (LPARAM)(LPCTSTR)sUrl);
		return TRUE;
	}
	else if (bShellExecute)
	{
		int nRes = (int)ShellExecute(*this, NULL, sUrl, NULL, NULL, SW_SHOWNORMAL); 
		
		if (nRes < 32)
		{
			CString sMessage;
			
			switch (nRes)
			{
			case SE_ERR_NOASSOC:
				sMessage.Format(FILEEDIT_ASSOCAPPFAILURE, sUrl);
				break;
				
			default:
				sMessage.Format(FILEEDIT_FILEOPENFAILED, sUrl, nRes);
				break;
			}
			
			AfxMessageBox(sMessage, MB_OK);
		}
		
		return TRUE;
	}
	
	return FALSE;
}

void CToDoCtrl::ExpandAllTasks(BOOL bExpand)
{
	CHoldRedraw hr(*this);
	m_tree.TCH().ExpandAll(bExpand);
}

void CToDoCtrl::ExpandSelectedTask(BOOL bExpand)
{
	if (!CanExpandSelectedTask(bExpand))
		return;

	CHoldRedraw hr(*this);
	CHoldRedraw hr2(m_tree);

	if (Selection().GetCount())
	{
		POSITION pos = Selection().GetFirstItemPos();
		
		while (pos)
		{
			HTREEITEM hti = Selection().GetNextItem(pos);
			m_tree.TCH().ExpandItem(hti, bExpand);
		}
	}
	else
		m_tree.TCH().ExpandItem(NULL, bExpand);
}

BOOL CToDoCtrl::CanExpandSelectedTask(BOOL /*bExpand*/) const
{
	return Selection().AnyItemsHaveChildren();
}

LRESULT CToDoCtrl::OnTDCHasClipboard(WPARAM /*wParam*/, LPARAM lParam)
{
	if (!lParam || s_clipboard.hwndToDoCtrl == *this)
		return !IsClipboardEmpty();

	return 0;
}

LRESULT CToDoCtrl::OnTDCGetClipboard(WPARAM /*wParam*/, LPARAM lParam)
{
	if (!lParam || s_clipboard.hwndToDoCtrl == *this)
		return (LRESULT)&s_clipboard.tasks;

	// else
	return NULL;
}

LRESULT CToDoCtrl::OnTDCDoTaskLink(WPARAM wParam, LPARAM lParam)
{
	ShowTaskLink(wParam, (LPCTSTR)lParam);

	return 0;
}

BOOL CToDoCtrl::AdjustTaskDates(DWORD dwTaskID, DWORD dwDependencyID)
{
	TODOITEM* pTDI = GetTask(dwTaskID);
	TODOITEM* pTDIDepends = GetTask(dwDependencyID);
	
	if (!pTDI || !pTDIDepends)
		return FALSE;
	
	// don't bother fixing tasks that are already completed
	if (!pTDI->IsDone())
	{
		COleDateTime dtNewStart;
		
		// make the start dates match the preceding task's end date 
		// or it's due date if it's not yet finished
		if (pTDIDepends->IsDone())
			dtNewStart = pTDIDepends->dateDone.m_dt + 1;
		
// 		else if (pTDIDepends->HasDue())
// 			dtNewStart = pTDIDepends->dateDue.m_dt + 1;

		if (dtNewStart > pTDI->dateStart)
		{
			// bump the due date too if present
			if (pTDI->HasDue() && pTDI->HasStart())
			{
				double dLength = max(0, pTDI->dateDue - pTDI->dateStart);
				pTDI->dateDue.m_dt = dtNewStart.m_dt + dLength;
			}
			
			// always set the start date
			pTDI->dateStart = dtNewStart;

			return TRUE;
		}
	}

	// no change
	return FALSE;
}

void CToDoCtrl::FixupTaskDependentsDates(DWORD dwTaskID)
{
	CDWordArray aDependents;

	// check for circular dependency before continuing
	if (m_data.HasCircularDependencies(dwTaskID))
		return;
	
	if (m_data.GetTaskDependents(dwTaskID, aDependents))
	{
		for (int nLink = 0; nLink < aDependents.GetSize(); nLink++)
		{
			DWORD dwIDDependent = aDependents[nLink];
			
			if (AdjustTaskDates(dwIDDependent, dwTaskID))
			{
				// then process this task's dependents
				FixupTaskDependentsDates(dwIDDependent);
			}
		}
	}
}

BOOL CToDoCtrl::ShowTaskLink(DWORD dwTaskID, LPCTSTR szLink)
{
	CString sFile;

	ParseTaskLink(szLink, dwTaskID, sFile);

	// if there's a file attached then pass to parent because
	// we can't handle it.
	if (!sFile.IsEmpty())
		return GetParent()->SendMessage(WM_TDCM_TASKLINK, dwTaskID, (LPARAM)(LPCTSTR)sFile);

	else if (dwTaskID)
	{
		HTREEITEM hti;
		CHTIMap mapHTI;
		
		m_data.BuildHTIMap(mapHTI);
		
		if (mapHTI.Lookup(dwTaskID, hti))
		{
			SelectItem(hti);
			m_tree.SelectItem(hti);
			m_tree.SetFocus();

			return TRUE;
		}
		else
			MessageBoxEx(this, CEnString(IDS_TDC_TASKIDNOTFOUND, dwTaskID), IDS_TDC_TASKIDNOTFOUND_TITLE);
	}
	else
		MessageBoxEx(this, IDS_TDC_ZEROINVALIDTASKID, IDS_TDC_ZEROINVALIDTASKID_TITLE);

	return FALSE;
}

void CToDoCtrl::ParseTaskLink(LPCTSTR szLink, DWORD& dwTaskID, CString& sFile)
{
	CString sLink(szLink);

	// strip off protocol (if not done)
	int nProtocol = sLink.Find(TDL_PROTOCOL);

	if (nProtocol != -1)
		sLink = sLink.Mid(nProtocol + strlen(TDL_PROTOCOL));

	// cleanup
	sLink.Replace("%20", " ");
	sLink.Replace("/", "\\");

	// parse the url
	int nDiv = sLink.Find('?');

	if (nDiv >= 0)
	{
		sFile = sLink.Left(nDiv);
		sFile.TrimLeft();
		sFile.TrimRight();
		
		CString sTaskID = sLink.Mid(nDiv + 1);
		dwTaskID = atoi(sTaskID);
	}
	else if (!sLink.IsEmpty())
	{
		if (isdigit(sLink[0])) // number
		{
			dwTaskID = atoi(sLink);
			sFile.Empty();
		}
		else
		{
			dwTaskID = 0;
			sFile = sLink;
		}
	}
}

BOOL CToDoCtrl::SelectionHasIncompleteDependencies() const
{
	const CTreeSelectionHelper& selection = Selection();
	POSITION pos = selection.GetFirstItemPos();

	// traverse the selected items looking for the first one
	// who has an incomplete dependent.
	while (pos)
	{
		HTREEITEM hti = selection.GetNextItem(pos);
		CString sDepends = m_data.GetTaskDependency(GetTaskID(hti));

		if (!sDepends.IsEmpty())
		{
			CString sFile;
			DWORD dwTaskID;

			ParseTaskLink(sDepends, dwTaskID, sFile);

			// see if dependent is one of 'our' tasks
			if (dwTaskID && sFile.IsEmpty())
			{
				if (!IsTaskDone(dwTaskID))
					return TRUE;
			}
			// else pass to parent if we can't handle
			else if (!sFile.IsEmpty())
			{
				BOOL bDependentIsDone = GetParent()->SendMessage(WM_TDCM_TASKISDONE, 
													dwTaskID, (LPARAM)(LPCTSTR)sFile);

				if (!bDependentIsDone)
					return TRUE;
			}
		}
	}

	return FALSE;
}

void CToDoCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CRuntimeDlg::OnSettingChange(uFlags, lpszSection);

    // spin button visibility goes badly wrong after a settings change
    // this is my best current solution other than subclassing the
    // spin button. Simply calling ShowWindow(SW_HIDE) from here
    // does not work.
    PostMessage(WM_TDC_REFRESHPERCENTSPINVISIBILITY);
}

LRESULT CToDoCtrl::OnRefreshPercentSpinVisibility(WPARAM /*wp*/, LPARAM /*lp*/)
{
    if (!m_ePercentDone.IsWindowVisible())
        m_spinPercent.ShowWindow(SW_HIDE);

    return 0L;
}

