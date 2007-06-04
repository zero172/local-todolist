// ToDoListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ToDoList.h"
#include "ToDoListWnd.h"
#include "ToolsCmdlineParser.h"
#include "ToolsUserInputDlg.h"
#include "Toolshelper.h"
#include "exportDlg.h"
#include "tasklisthtmlexporter.h"
#include "tasklisttxtexporter.h"
#include "tdcmsg.h"
#include "tdlprintdialog.h"
#include "tdltransformdialog.h"
#include "tdstringres.h"
#include "filterdlg.h"
#include "OffsetDatesDlg.h"
#include "KeyboardShortcutDisplayDlg.h"

#include "..\shared\aboutdlg.h"
#include "..\shared\holdredraw.h"
#include "..\shared\autoflag.h"
#include "..\shared\enbitmap.h"
#include "..\shared\spellcheckdlg.h"
#include "..\shared\encolordialog.h"
#include "..\shared\winclasses.h"
#include "..\shared\wclassdefines.h"
#include "..\shared\datehelper.h"
#include "..\shared\osversion.h"
#include "..\shared\filedialogex.h"
#include "..\shared\misc.h"
#include "..\shared\browserdlg.h"
#include "..\shared\themed.h"
#include "..\shared\enstring.h"
#include "..\shared\fileregister.h"
#include "..\shared\mousewheelMgr.h"
#include "..\shared\dlgunits.h"
#include "..\shared\passworddialog.h"
#include "..\3rdparty\gui.h"
#include "..\shared\sysimagelist.h"

#include <shlwapi.h>
#include <windowsx.h>
#include <direct.h>

#ifdef _AFXDLL
#  define COMPILE_MULTIMON_STUBS
#endif

#pragma warning(disable:4706)
#include <multimon.h>
#pragma warning(default:4706)

#include <afxpriv.h>        // for WM_KICKIDLE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDoListWnd dialog

// popup menus
enum { TRAYICON, TASKCONTEXT, TABCTRLCONTEXT, HEADERCONTEXT };
enum { TB_TOOLBARHIDDEN, TB_DUMMY, TB_TOOLBARANDMENU };
enum { FILEALL, NEWTASK, EDITTASK, VIEW, MOVE, SORT, DELETETASK, TOOLS, HELP };

const int TD_VERSION = 31000;
const int SB_HEIGHT = 10; // dlu
const int BEVEL = 2; // pixels
const int BORDER = 3; // pixels

enum
{
	WM_POSTONCREATE = (WM_USER+1),
	WM_WEBUPDATEWIZARD,
	WM_ADDTOOLBARTOOLS,
};

enum 
{
	TIMER_READONLYSTATUS = 1,
		TIMER_TIMESTAMPCHANGE,
		TIMER_AUTOSAVE,
		TIMER_CHECKOUTSTATUS,
		TIMER_DUEITEMS,
		TIMER_TIMETRACKING,
};

enum 
{
	INTERVAL_READONLYSTATUS = 1000,
		INTERVAL_TIMESTAMPCHANGE = 10000,
		//	INTERVAL_AUTOSAVE, // dynamically determined
		INTERVAL_CHECKOUTSTATUS = 5000,
		INTERVAL_DUEITEMS = 60000,
		INTERVAL_TIMETRACKING = 5000,
};

/////////////////////////////////////////////////////////////////////////////

CToDoListWnd::CToDoListWnd() : CFrameWnd(), 
		m_bVisible(-1), 
		m_mruList(0, "MRU", "TaskList%d", 16, AFX_ABBREV_FILENAME_LEN, CEnString(IDS_RECENTFILES)),
		m_nLastSelItem(-1),
		m_bMaxTasklists(FALSE),
		m_bShowFilterBar(TRUE),
		m_bShowStatusBar(TRUE),
		m_statusBar(TRUE),
		m_bInNewTask(FALSE),
		m_bSaving(FALSE),
		m_mgrShortcuts(FALSE),
		m_pPrefs(NULL),
		m_bClosing(FALSE),
		m_mgrToDoCtrls(m_tabCtrl),
		m_bFindShowing(FALSE),
		m_bShowProjectName(TRUE),
		m_bQueryOpenAllow(FALSE),
		m_bPasswordPrompting(TRUE),
		m_bShowToolbar(TRUE),
		m_bReloading(FALSE),
		m_hIcon(NULL),
		m_hwndLastFocus(NULL),
		m_bStartHidden(FALSE)
{
	// init content manager BEFORE prefs
	VERIFY (m_mgrContent.Initialize());
	
	// init preferences
	ResetPrefs();

	// set up UI strings for helper classes
	CTimeHelper::SetUnits(THU_MINS, CEnString(IDS_MINS_ABBREV));
	CTimeHelper::SetUnits(THU_HOURS, CEnString(IDS_HOURS_ABBREV));
	CTimeHelper::SetUnits(THU_DAYS, CEnString(IDS_DAYS_ABBREV));
	CTimeHelper::SetUnits(THU_WEEKS, CEnString(IDS_WEEKS_ABBREV));
	CTimeHelper::SetUnits(THU_MONTHS, CEnString(IDS_MONTHS_ABBREV));
	CTimeHelper::SetUnits(THU_YEARS, CEnString(IDS_YEARS_ABBREV));

	CTimeEdit::SetRecalcTimePrompt(CEnString(IDS_TE_RECALCTITLE), CEnString(IDS_TE_RECALCPROMPT));

	CXmlFileEx::SetUIStrings(IDS_ENCRYPTEDFILE, IDS_DECRYPTFAILED);

	CPasswordDialog::SetUIStrings(IDS_PASSWORDTITLE, IDS_PASSWORDPROMPT,
		IDS_CONFIRMPROMPT, IDS_CONFIRMFAILED, IDS_OK, IDS_CANCEL);

	m_bAutoMenuEnable = FALSE;
}

CToDoListWnd::~CToDoListWnd()
{
	CToDoCtrl::ReleaseClipboard();
	
	delete m_pPrefs;

	if (m_hIcon)
		DestroyIcon(m_hIcon);

	// cleanup temp files
	::DeleteFile(FileMisc::GetTempFileName("ToDoList.print", "html"));
	::DeleteFile(FileMisc::GetTempFileName("ToDoList.due", "html"));
	::DeleteFile(FileMisc::GetTempFileName("ToDoList.due", "txt"));
}

BEGIN_MESSAGE_MAP(CToDoListWnd, CFrameWnd)
//{{AFX_MSG_MAP(CToDoListWnd)
	ON_COMMAND(ID_VIEW_EXPANDTASK, OnViewExpandtask)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPANDTASK, OnUpdateViewExpandtask)
	ON_COMMAND(ID_VIEW_COLLAPSETASK, OnViewCollapsetask)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLLAPSETASK, OnUpdateViewCollapsetask)
	ON_COMMAND(ID_VIEW_COLLAPSEALL, OnViewCollapseall)
	ON_COMMAND(ID_VIEW_EXPANDALL, OnViewExpandall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPANDALL, OnUpdateViewExpandall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLLAPSEALL, OnUpdateViewCollapseall)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_USEREXPORT1, OnUpdateUserExport1)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_USERIMPORT1, OnUpdateUserImport1)
	ON_UPDATE_COMMAND_UI(ID_WINDOW1, OnUpdateWindow)
	ON_WM_ACTIVATE()
	ON_WM_ENABLE()
	ON_COMMAND(ID_NEWTASK, OnNewtask)
	ON_COMMAND(ID_NEWSUBTASK, OnNewsubtask)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBTASK, OnUpdateNewsubtask)
	ON_COMMAND(ID_TOOLS_TRANSFORM, OnToolsTransformactivetasklist)
	ON_COMMAND(ID_VIEW_SORTTASKLISTTABS, OnViewSorttasklisttabs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTTASKLISTTABS, OnUpdateViewSorttasklisttabs)
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_EDIT_INCTASKPERCENTDONE, OnEditInctaskpercentdone)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INCTASKPERCENTDONE, OnUpdateEditInctaskpercentdone)
	ON_COMMAND(ID_EDIT_DECTASKPERCENTDONE, OnEditDectaskpercentdone)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DECTASKPERCENTDONE, OnUpdateEditDectaskpercentdone)
	ON_COMMAND(ID_EDIT_DECTASKPRIORITY, OnEditDectaskpriority)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DECTASKPRIORITY, OnUpdateEditDectaskpriority)
	ON_COMMAND(ID_EDIT_INCTASKPRIORITY, OnEditInctaskpriority)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INCTASKPRIORITY, OnUpdateEditInctaskpriority)
	ON_COMMAND(ID_EDIT_FLAGTASK, OnEditFlagtask)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FLAGTASK, OnUpdateEditFlagtask)
	ON_COMMAND(ID_FILE_OPENARCHIVE, OnFileOpenarchive)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENARCHIVE, OnUpdateFileOpenarchive)
	ON_WM_WINDOWPOSCHANGED()
	ON_COMMAND(ID_VIEW_REFRESHFILTER, OnViewRefreshfilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESHFILTER, OnUpdateViewRefreshfilter)
	ON_COMMAND(ID_VIEW_SHOWFILTERBAR, OnViewShowfilterbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWFILTERBAR, OnUpdateViewShowfilterbar)
	ON_COMMAND(ID_VIEW_CLEARFILTER, OnViewClearfilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLEARFILTER, OnUpdateViewClearfilter)
	ON_COMMAND(ID_VIEW_FILTER, OnViewFilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTER, OnUpdateViewFilter)
	ON_COMMAND(ID_TABCTRL_PREFERENCES, OnTabctrlPreferences)
	ON_COMMAND(ID_TASKLIST_SELECTCOLUMNS, OnTasklistSelectColumns)
	ON_COMMAND(ID_VIEW_PROJECTNAME, OnViewProjectname)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECTNAME, OnUpdateViewProjectname)
	ON_COMMAND(ID_EDIT_OFFSETDATES, OnEditOffsetdates)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OFFSETDATES, OnUpdateEditOffsetdates)
	ON_COMMAND(ID_PRINTPREVIEW, OnPrintpreview)
	ON_COMMAND(ID_SHOWTIMELOGFILE, OnShowTimelogfile)
	ON_UPDATE_COMMAND_UI(ID_SHOWTIMELOGFILE, OnUpdateShowTimelogfile)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_WM_QUERYOPEN()
	ON_COMMAND(ID_TASKCONTEXT_COPYAS_TASKREF, OnCopyTaskasRef)
	ON_UPDATE_COMMAND_UI(ID_TASKCONTEXT_COPYAS_TASKREF, OnUpdateCopyTaskasRef)
	ON_COMMAND(ID_TASKCONTEXT_COPYAS_TASKDEPENDENCY, OnCopyTaskasDependency)
	ON_UPDATE_COMMAND_UI(ID_TASKCONTEXT_COPYAS_TASKDEPENDENCY, OnUpdateCopyTaskasDependency)
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_TOOLS_CHECKFORUPDATES, OnToolsCheckforupdates)
	ON_COMMAND(ID_HELP_KEYBOARDSHORTCUTS, OnShowKeyboardshortcuts)
	ON_COMMAND(ID_EDIT_INSERTDATETIME, OnEditInsertdatetime)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTDATETIME, OnUpdateEditInsertdatetime)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SYSCOMMAND()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_WINDOW1, ID_WINDOW16, OnWindow)
	ON_COMMAND_RANGE(ID_TOOLS_USERIMPORT1, ID_TOOLS_USERIMPORT8, OnUserImport)
	ON_COMMAND_RANGE(ID_TOOLS_USEREXPORT1, ID_TOOLS_USEREXPORT8, OnUserExport)
	ON_COMMAND(ID_VIEW_NEXT, OnViewNext)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT, OnUpdateViewNext)
	ON_COMMAND(ID_VIEW_PREV, OnViewPrev)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREV, OnUpdateViewPrev)
	ON_WM_SYSCOMMAND()
	ON_UPDATE_COMMAND_UI(ID_IMPORT_TASKLIST, OnUpdateImport)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK, OnUpdateNewtask)
	ON_COMMAND(ID_TOOLS_CHECKOUT, OnToolsCheckout)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHECKOUT, OnUpdateToolsCheckout)
	ON_COMMAND(ID_TOOLS_CHECKIN, OnToolsCheckin)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHECKIN, OnUpdateToolsCheckin)
	ON_COMMAND(ID_TOOLS_TOGGLECHECKIN, OnToolsToggleCheckin)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TOGGLECHECKIN, OnUpdateToolsToggleCheckin)
	ON_COMMAND(ID_TOOLS_EXPORT, OnExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXPORT, OnUpdateExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TRANSFORM, OnUpdateExport) // use same text as export
	ON_COMMAND(ID_NEXTTOPLEVELTASK, OnNexttopleveltask)
	ON_COMMAND(ID_PREVTOPLEVELTASK, OnPrevtopleveltask)
	ON_UPDATE_COMMAND_UI(ID_NEXTTOPLEVELTASK, OnUpdateNexttopleveltask)
	ON_UPDATE_COMMAND_UI(ID_PREVTOPLEVELTASK, OnUpdatePrevtopleveltask)
	ON_COMMAND(ID_EDIT_FINDTASKS, OnFindTasks)
	ON_COMMAND(ID_VIEW_MOVETASKLISTRIGHT, OnViewMovetasklistright)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOVETASKLISTRIGHT, OnUpdateViewMovetasklistright)
	ON_COMMAND(ID_VIEW_MOVETASKLISTLEFT, OnViewMovetasklistleft)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOVETASKLISTLEFT, OnUpdateViewMovetasklistleft)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_SORT, OnSort)
	ON_UPDATE_COMMAND_UI(ID_SORT, OnUpdateSort)
	ON_COMMAND(ID_NEWTASK_ATTOP, OnNewtaskAttop)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATTOP, OnUpdateNewtaskAttop)
	ON_COMMAND(ID_NEWTASK_ATBOTTOM, OnNewtaskAtbottom)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATBOTTOM, OnUpdateNewtaskAtbottom)
	ON_COMMAND(ID_EDIT_SPELLCHECKCOMMENTS, OnSpellcheckcomments)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPELLCHECKCOMMENTS, OnUpdateSpellcheckcomments)
	ON_COMMAND(ID_EDIT_SPELLCHECKTITLE, OnSpellchecktitle)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPELLCHECKTITLE, OnUpdateSpellchecktitle)
	ON_COMMAND(ID_FILE_ENCRYPT, OnFileEncrypt)
	ON_UPDATE_COMMAND_UI(ID_FILE_ENCRYPT, OnUpdateFileEncrypt)
	ON_COMMAND(ID_FILE_RESETVERSION, OnFileResetversion)
	ON_UPDATE_COMMAND_UI(ID_FILE_RESETVERSION, OnUpdateFileResetversion)
	ON_COMMAND(ID_TOOLS_SPELLCHECKTASKLIST, OnSpellcheckTasklist)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SPELLCHECKTASKLIST, OnUpdateSpellcheckTasklist)
	ON_COMMAND(ID_EDIT_PASTEAFTER, OnEditPasteAfter)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEAFTER, OnUpdateEditPasteAfter)
	ON_COMMAND(ID_EDIT_CLOCK_TASK, OnEditTimeTrackTask)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLOCK_TASK, OnUpdateEditTimeTrackTask)
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_VIEW_NEXT_SEL, OnViewNextSel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_SEL, OnUpdateViewNextSel)
	ON_COMMAND(ID_VIEW_PREV_SEL, OnViewPrevSel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREV_SEL, OnUpdateViewPrevSel)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND_RANGE(ID_NEWTASK_SPLITTASKINTO_TWO, ID_NEWTASK_SPLITTASKINTO_FIVE, OnSplitTaskIntoPieces)
	ON_UPDATE_COMMAND_UI_RANGE(ID_NEWTASK_SPLITTASKINTO_TWO, ID_NEWTASK_SPLITTASKINTO_FIVE, OnUpdateSplitTaskIntoPieces)
	ON_COMMAND_RANGE(ID_TOOLS_SHOWTASKS_DUETODAY, ID_TOOLS_SHOWTASKS_DUEENDNEXTMONTH, OnToolsShowtasksDue)
	ON_COMMAND(ID_DELETETASK, OnDeleteTask)
	ON_COMMAND(ID_DELETEALLTASKS, OnDeleteAllTasks)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_SAVE_NORMAL, OnSave)
	ON_COMMAND(ID_LOAD_NORMAL, OnLoad)
	ON_COMMAND(ID_NEW, OnNew)
	ON_UPDATE_COMMAND_UI(ID_DELETETASK, OnUpdateDeletetask)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TASKCOLOR, OnUpdateTaskcolor)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TASKDONE, OnUpdateTaskdone)
	ON_UPDATE_COMMAND_UI(ID_DELETEALLTASKS, OnUpdateDeletealltasks) 
	ON_UPDATE_COMMAND_UI(ID_SAVE_NORMAL, OnUpdateSave)
	ON_UPDATE_COMMAND_UI(ID_NEW, OnUpdateNew)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_NEWTASK_ATTOPSELECTED, OnNewtaskAttopSelected)
	ON_COMMAND(ID_NEWTASK_ATBOTTOMSELECTED, OnNewtaskAtbottomSelected)
	ON_COMMAND(ID_NEWTASK_AFTERSELECTEDTASK, OnNewtaskAfterselectedtask)
	ON_COMMAND(ID_NEWTASK_BEFORESELECTEDTASK, OnNewtaskBeforeselectedtask)
	ON_COMMAND(ID_NEWSUBTASK_ATBOTTOM, OnNewsubtaskAtbottom)
	ON_COMMAND(ID_NEWSUBTASK_ATTOP, OnNewsubtaskAttop)
	ON_COMMAND(ID_EDIT_TASKCOLOR, OnEditTaskcolor)
	ON_COMMAND(ID_EDIT_TASKDONE, OnEditTaskdone)
	ON_COMMAND(ID_EDIT_TASKTEXT, OnEditTasktext)
	ON_COMMAND(ID_MOVETASKDOWN, OnMovetaskdown)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKDOWN, OnUpdateMovetaskdown)
	ON_COMMAND(ID_MOVETASKUP, OnMovetaskup)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKUP, OnUpdateMovetaskup)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TRAYICON_CLOSE, OnTrayiconClose)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBTASK_ATBOTTOM, OnUpdateNewsubtaskAtBottom)
	ON_COMMAND(ID_SAVEAS, OnSaveas)
	ON_UPDATE_COMMAND_UI(ID_SAVEAS, OnUpdateSaveas)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TRAYICON_SHOW, OnTrayiconShow)
	ON_COMMAND_RANGE(ID_TRAYICON_SHOWDUETASKS1, ID_TRAYICON_SHOWDUETASKS20, OnTrayiconShowDueTasks)
	ON_WM_QUERYENDSESSION()
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateRecentFileMenu)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_PREFERENCES, OnPreferences)
	ON_WM_COPYDATA()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTESUB, OnEditPasteSub)
	ON_COMMAND(ID_EDIT_COPYASTEXT, OnEditCopyastext)
	ON_COMMAND(ID_EDIT_COPYASHTML, OnEditCopyashtml)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTESUB, OnUpdateEditPasteSub)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATTOPSELECTED, OnUpdateNewtaskAttopSelected)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATBOTTOMSELECTED, OnUpdateNewtaskAtbottomSelected)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_AFTERSELECTEDTASK, OnUpdateNewtaskAfterselectedtask)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_BEFORESELECTEDTASK, OnUpdateNewtaskBeforeselectedtask)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBTASK_ATTOP, OnUpdateNewsubtaskAttop)
	ON_COMMAND(ID_SIMPLEMODE, OnSimplemode)
	ON_UPDATE_COMMAND_UI(ID_SIMPLEMODE, OnUpdateSimplemode)
	ON_COMMAND(ID_OPEN_RELOAD, OnReload)
	ON_UPDATE_COMMAND_UI(ID_OPEN_RELOAD, OnUpdateReload)
	ON_COMMAND(ID_ARCHIVE_COMPLETEDTASKS, OnArchiveCompletedtasks)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_COMPLETEDTASKS, OnUpdateArchiveCompletedtasks)
	ON_COMMAND(ID_PRINT, OnPrint)
	ON_UPDATE_COMMAND_UI(ID_PRINT, OnUpdatePrint)
	ON_COMMAND(ID_MOVETASKRIGHT, OnMovetaskright)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKRIGHT, OnUpdateMovetaskright)
	ON_COMMAND(ID_MOVETASKLEFT, OnMovetaskleft)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKLEFT, OnUpdateMovetaskleft)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCONTROL, OnSelchangeTabcontrol)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TABCONTROL, OnSelchangingTabcontrol)
	ON_NOTIFY(NM_MCLICK, IDC_TABCONTROL, OnMBtnClickTabcontrol)
	ON_COMMAND(ID_CLOSE, OnCloseTasklist)
	ON_COMMAND(ID_SAVEALL, OnSaveall)
	ON_UPDATE_COMMAND_UI(ID_SAVEALL, OnUpdateSaveall)
	ON_COMMAND(ID_CLOSEALL, OnCloseall)
	ON_UPDATE_COMMAND_UI(ID_CLOSEALL, OnUpdateCloseall)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_UPDATE_COMMAND_UI(ID_EXIT, OnUpdateExit)
	ON_UPDATE_COMMAND_UI(ID_MOVETASK, OnUpdateMovetask)
	ON_WM_TIMER()
	ON_COMMAND(ID_IMPORT_TASKLIST, OnImportTasklist)
	ON_COMMAND_RANGE(ID_SORT_BYCOST, ID_SORT_NONE, OnSortBy)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORT_BYCOST, ID_SORT_NONE, OnUpdateSortBy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TASKTEXT, OnUpdateEditTasktext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_TRAYICON_CREATETASK, OnTrayiconCreatetask)
	ON_COMMAND_RANGE(ID_EDIT_SETPRIORITY0, ID_EDIT_SETPRIORITY10, OnSetPriority)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_SETPRIORITY0, ID_EDIT_SETPRIORITY10, OnUpdateSetPriority)
	ON_COMMAND(ID_EDIT_SETFILEREF, OnEditSetfileref)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETFILEREF, OnUpdateEditSetfileref)
	ON_COMMAND(ID_EDIT_OPENFILEREF, OnEditOpenfileref)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OPENFILEREF, OnUpdateEditOpenfileref)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYASHTML, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYASTEXT, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOLS_USERTOOL1, ID_TOOLS_USERTOOL16, OnUpdateUserTool)
	ON_COMMAND_RANGE(ID_TOOLS_USERTOOL1, ID_TOOLS_USERTOOL16, OnUserTool)
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(NM_CLICK, IDC_TRAYICON, OnTrayIconClick)
	ON_NOTIFY(NM_DBLCLK, IDC_TRAYICON, OnTrayIconDblClk)
	ON_NOTIFY(NM_RCLICK, IDC_TRAYICON, OnTrayIconRClick)
	ON_NOTIFY(TTN_NEEDTEXT, 0, OnNeedUserTooltipText)
	ON_REGISTERED_MESSAGE(WM_FBN_FILTERCHNG, OnSelchangeFilter)
	ON_REGISTERED_MESSAGE(WM_TDCN_LISTCHANGE, OnToDoCtrlNotifyListChange)
	ON_REGISTERED_MESSAGE(WM_TDCN_SORT, OnToDoCtrlNotifySort)
	ON_REGISTERED_MESSAGE(WM_TDCN_MODIFY, OnToDoCtrlNotifyMod)
	ON_REGISTERED_MESSAGE(WM_TDCN_MINWIDTHCHANGE, OnToDoCtrlNotifyMinWidthChange)
	ON_REGISTERED_MESSAGE(WM_TDCN_TIMETRACK, OnToDoCtrlNotifyTimeTrack)
	ON_REGISTERED_MESSAGE(WM_TDCM_TASKLINK, OnToDoCtrlDoTaskLink)
	ON_REGISTERED_MESSAGE(WM_TDCM_TASKISDONE, OnToDoCtrlTaskIsDone)
	ON_REGISTERED_MESSAGE(WM_TDL_SHOWWINDOW , OnToDoListShowWindow)
	ON_REGISTERED_MESSAGE(WM_TDL_GETVERSION , OnToDoListGetVersion)
	ON_REGISTERED_MESSAGE(WM_TDL_ISCLOSING , OnToDoListIsClosing)
	ON_REGISTERED_MESSAGE(WM_TDL_REFRESHPREFS , OnToDoListRefreshPrefs)
	ON_REGISTERED_MESSAGE(WM_TDL_RESTORE , OnToDoListRestore)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_REGISTERED_MESSAGE(WM_FTD_FIND, OnFindDlgFind)
	ON_REGISTERED_MESSAGE(WM_FTD_SELECTRESULT, OnFindSelectResult)
	ON_REGISTERED_MESSAGE(WM_FTD_SELECTALL, OnFindSelectAll)
	ON_REGISTERED_MESSAGE(WM_FTD_CLOSE, OnFindDlgClose)
	ON_REGISTERED_MESSAGE(WM_TLDT_DROPFILE, OnDropFile)
	ON_REGISTERED_MESSAGE(WM_PTP_TESTTOOL, OnPreferencesTestTool)
	ON_REGISTERED_MESSAGE(WM_PGP_CLEARMRU, OnPreferencesClearMRU)
	ON_MESSAGE(WM_GETICON, OnGetIcon)
	ON_MESSAGE(WM_POSTONCREATE, OnPostOnCreate)
	ON_MESSAGE(WM_WEBUPDATEWIZARD, OnWebUpdateWizard)
	ON_MESSAGE(WM_ADDTOOLBARTOOLS, OnAddToolbarTools)
	ON_MESSAGE(WM_HOTKEY, OnHotkey)
	ON_MESSAGE(WM_POWERBROADCAST, OnPowerBroadcast)
	ON_MESSAGE(WM_GETFONT, OnGetFont)

#ifdef _DEBUG
	ON_COMMAND(ID_DEBUGQUERYENDSESSION, OnDebugQueryEndSession)
#endif

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoListWnd message handlers

BOOL CToDoListWnd::Create(DWORD dwFlags, LPCTSTR szTDListPath)
{
	m_startupOptions.sFilePath = szTDListPath;
	m_bVisible = (dwFlags & TLD_FORCEVISIBLE) ? 1 : -1;
	m_bPasswordPrompting = (dwFlags & TLD_PASSWORDPROMPTING);
	
	if (LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, NULL, NULL) &&
		LoadMenubar())
	{
		return TRUE;
	}

	return FALSE;
}

int CToDoListWnd::GetVersion()
{
	return TD_VERSION;
}

int CToDoListWnd::MessageBox(UINT nIDText, UINT nIDCaption, UINT nType)
{
	return CFrameWnd::MessageBox(CEnString(nIDText), CEnString(nIDCaption), nType);
}

int CToDoListWnd::MessageBox(LPCTSTR szText, UINT nIDCaption, UINT nType)
{
	return CFrameWnd::MessageBox(szText, CEnString(nIDCaption), nType);
}

int CToDoListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!CMouseWheelMgr::Initialize())
		return -1;
	
	// import export manager
	m_mgrImportExport.Initialize();
	
	// drop target
	if (!m_dropTarget.Register(this, this))
		return -1;

	// shortcut manager
	InitShortcutManager();
	
	// trayicon
	// we always create the trayicon (for simplicity) but we only
	// show it if required
	BOOL bUseSysTray = Prefs().GetUseSysTray();
	UINT nIDTrayIcon = (COSVersion() >= OSV_XP) ? IDI_TRAYICONXP : IDI_TRAYICON;
	
	m_ti.Create(WS_CHILD | (bUseSysTray ? WS_VISIBLE : 0), this, IDC_TRAYICON, nIDTrayIcon, CEnString(IDS_COPYRIGHT));
	
	// toolbar
	if (!InitToolbar())
		return -1;

	// menu icons
	InitMenuIconManager();
	
	// filterbar
	if (!m_filterBar.Create(this, (UINT)IDC_STATIC))
		return -1;

	m_filterBar.EnableMultiCategorySelection(Prefs().GetMultiSelCategoryFilter());
	
	// tabctrl
	if (!m_tabCtrl.Create(WS_CHILD | WS_VISIBLE | TCS_HOTTRACK | TCS_TABS | TCS_SINGLELINE | TCS_RIGHTJUSTIFY, 
							CRect(0, 0, 10, 10), this, IDC_TABCONTROL))
		return -1;

	BOOL bStackTabbar = Prefs().GetStackTabbarItems();
	
	m_tabCtrl.ModifyStyle(bStackTabbar ? 0 : TCS_MULTILINE, bStackTabbar ? TCS_MULTILINE : 0);
	UpdateTabSwitchTooltip();
	
	if (m_ilTabCtrl.Create(IDB_SOURCECONTROL, 16, 1, RGB(0, 255, 0)))
		m_tabCtrl.SetImageList(&m_ilTabCtrl);
	else
		return -1;
	
	// statusbar
	if (!m_statusBar.Create(NULL, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN | SS_CENTERIMAGE, 
							CRect(0, 0, 10, 10), this, IDC_FILENAME))
		return -1;

	// UI Font
	InitUIFont();

	// add a barebones tasklist while we're still hidden
	if (!CreateNewTaskList(FALSE))
		return -1;

	LoadSettings();

	// timers
	SetTimer(TIMER_DUEITEMS, TRUE);
	SetTimer(TIMER_TIMETRACKING, Prefs().GetUseSysTray());

	// notify users about dodgy content plugins
	if (m_mgrContent.SomePluginsHaveBadversions())
	{
		if (MessageBox(IDS_BADPLUGINVERSIONS, IDS_BADPLUGINTITLE, MB_OKCANCEL) == IDCANCEL)
			return -1;
	}
	
	// late initialization
	PostMessage(WM_POSTONCREATE);
	
	return 0; // success
}

void CToDoListWnd::InitUIFont()
{
	m_fontMain.DeleteObject();

/*
	// Get the UI font
	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm,sizeof(ncm));
	ncm.cbSize = sizeof(ncm);
	
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID)&ncm, FALSE);
	
	HFONT hFontUI = ::CreateFontIndirect(&ncm.lfMessageFont);
		
	if (!hFontUI)
*/
	HFONT hFontUI = Misc::CreateFont("Tahoma", 8);

	if (m_fontMain.Attach(hFontUI))
		CDialogHelper::SetFont(this, m_fontMain); // will update all child controls
}

void CToDoListWnd::InitShortcutManager()
{
	// setup defaults first
	m_mgrShortcuts.AddShortcut(ID_LOAD_NORMAL, 'O', HOTKEYF_CONTROL); 
	m_mgrShortcuts.AddShortcut(ID_OPEN_RELOAD, 'O', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_SAVE_NORMAL, 'S', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_NEWTASK_BEFORESELECTEDTASK, 'N', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_NEWSUBTASK_ATTOP, 'N', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_NEWTASK_AFTERSELECTEDTASK, 'N', HOTKEYF_CONTROL | HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_SIMPLEMODE, 'M', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_PRINT, 'P', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_NEXT, VK_TAB, HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_PREV, VK_TAB, HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_EDIT_CUT, 'X', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_COPY, 'C', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_PASTESUB, 'V', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_INSERTDATETIME, 'D', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_OPENFILEREF, 'G', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EXIT, VK_F4, HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_CLOSE, 'C', HOTKEYF_CONTROL | HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKDOWN, VK_DOWN, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKUP, VK_UP, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKLEFT, VK_LEFT, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKRIGHT, VK_RIGHT, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_DELETETASK, VK_DELETE, HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_EDIT_TASKTEXT, VK_F2, 0);
	m_mgrShortcuts.AddShortcut(ID_EDIT_TASKDONE, VK_SPACE, HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_TOOLS_EXPORT, 'E', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_HELP, VK_F1, 0);
	m_mgrShortcuts.AddShortcut(ID_VIEW_TOOLBARANDMENU, 'M', HOTKEYF_CONTROL | HOTKEYF_ALT | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW1, '1', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW2, '2', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW3, '3', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW4, '4', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW5, '5', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW6, '6', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW7, '7', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW8, '8', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW9, '9', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_TOOLS_TRANSFORM, 'T', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_FINDTASKS, 'F', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_REFRESHFILTER, VK_F5, HOTKEYF_CONTROL);
	
	// now init shortcut mgr which will override the defaults
	// with the users actual settings
	if (m_mgrShortcuts.Initialize(this))
	{
		// fix for previously adding escape key as a shortcut for IDCLOSE 
		// (big mistake)
		if (m_mgrShortcuts.GetShortcut(IDCLOSE) == VK_ESCAPE)
			m_mgrShortcuts.DeleteShortcut(IDCLOSE);

		// fix for paste being wrongly set up
		if (m_mgrShortcuts.GetShortcut(ID_EDIT_PASTE))
		{
			// delete existing
			m_mgrShortcuts.DeleteShortcut(ID_EDIT_PASTE);

			// if nothing already assigned use Ctrl+V
			if (!m_mgrShortcuts.GetShortcut(ID_EDIT_PASTESUB))
				m_mgrShortcuts.AddShortcut(ID_EDIT_PASTESUB, 'V', HOTKEYF_CONTROL);
		}
	}
}

void CToDoListWnd::InitMenuIconManager()
{
	if (!m_mgrMenuIcons.Initialize(this))
		return;
	
	m_mgrMenuIcons.ClearImages();
	
	CUIntArray aCmdIDs;
	
	aCmdIDs.Add(ID_LOAD_NORMAL);
	aCmdIDs.Add(ID_SAVE_NORMAL);
	aCmdIDs.Add(ID_SAVEALL);
	
	// new tasks
	aCmdIDs.Add(GetNewTaskCmdID());
	aCmdIDs.Add(GetNewSubtaskCmdID());
	
	aCmdIDs.Add(ID_EDIT_TASKTEXT);
	aCmdIDs.Add(ID_SIMPLEMODE);
	aCmdIDs.Add(ID_EDIT_FINDTASKS);
	
	// sorting
	if (GetTDCCount())
	{
		TDC_SORTBY nSort = GetToDoCtrl().GetSortBy();
		UINT nSortID = GetSortID(nSort);
		aCmdIDs.Add(nSortID);
	}
	else
		aCmdIDs.Add(ID_SORT);
	
	aCmdIDs.Add(ID_DELETETASK);
	
	// checkin
	if (GetTDCCount() && m_mgrToDoCtrls.PathSupportsSourceControl(GetSelToDoCtrl()))
	{
		CToDoCtrl& tdc = GetToDoCtrl();

		if (tdc.IsCheckedOut())
			aCmdIDs.Add(ID_TOOLS_CHECKIN);
		else
			aCmdIDs.Add(ID_TOOLS_CHECKOUT);
	}
	else
		aCmdIDs.Add(ID_TOOLS_TOGGLECHECKIN);
		
	aCmdIDs.Add(ID_PREFERENCES);
	aCmdIDs.Add(ID_HELP);
	
	m_mgrMenuIcons.AddImages(aCmdIDs, IDB_TOOLBAR16, 16, RGB(255, 0, 255));//RGB(192, 192, 192));
}

void CToDoListWnd::OnShowKeyboardshortcuts() 
{
	CStringArray aMapping;

	if (m_mgrShortcuts.BuildMapping(IDR_MAINFRAME, aMapping, '|'))
	{
		// add a few misc items that don't appear in the menus
		CString sMisc;

		sMisc.Format("%s|%s", m_mgrShortcuts.GetShortcutText(MAKELONG('B', HOTKEYF_ALT)), CEnString(IDS_SETFOCUS2TASLIST));
		aMapping.Add(sMisc);
	
		CKeyboardShortcutDisplayDlg dialog(aMapping, '|');

		dialog.DoModal();
	}
}

LRESULT CToDoListWnd::OnGetIcon(WPARAM bLargeIcon, LPARAM /*not used*/)
{
	if (!bLargeIcon)
	{
		// cache small icon for reuse
		if (!m_hIcon)
			m_hIcon = CSysImageList(FALSE).ExtractAppIcon();
		
		return (LRESULT)m_hIcon;
	}
	else
		return Default();
}

BOOL CToDoListWnd::InitToolbar()
{
	if (!m_toolbar.GetSafeHwnd())
	{
		UINT nIDToolbarImage = Prefs().GetLargeToolbarIcons() ? IDB_TOOLBAR24 : IDB_TOOLBAR16;
		
		if (m_toolbar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_WRAPABLE, 
								WS_CHILD | WS_VISIBLE | CBRS_TOP |  
								CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
		{
			if (m_toolbar.LoadToolBar(IDR_TOOLBAR))
			{
				m_toolbar.SetImage(nIDToolbarImage, RGB(255, 0, 255));
				m_toolbar.GetToolBarCtrl().HideButton(ID_TOOLS_TOGGLECHECKIN, !Prefs().GetEnableSourceControl());
			}
		}
	}
	
	return (NULL != m_toolbar.GetSafeHwnd());
}

BOOL CToDoListWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CLOSE)
	{
		OnQueryEndSession();
		return TRUE;
	}

	if (ProcessDialogControlShortcut(pMsg))
		return TRUE;
	
	UINT nCmdID = m_mgrShortcuts.ProcessMessage(pMsg);
	
	if (nCmdID)
	{
		BOOL bSendMessage = TRUE; // default
		
		switch (nCmdID)
		{
		case ID_EDIT_COPY:
			// tree must have the focus
			if (!GetToDoCtrl().TreeHasFocus())
			{
				bSendMessage = FALSE;
				GetToDoCtrl().ClearCopiedItem();
			}
			break;
			
		case ID_EDIT_PASTE: // tree must have the focus
			bSendMessage = GetToDoCtrl().TreeHasFocus();
			break;
		}
		
		if (bSendMessage)
		{
			SendMessage(WM_COMMAND, nCmdID);
			return TRUE;
		}
	}
	
	// try active task list
	if (GetTDCCount() && GetToDoCtrl().PreTranslateMessage(pMsg))
		return TRUE;
	
	// we need to check for <escape>
	if (Prefs().GetEscapeMinimizes())
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			{
				switch (pMsg->wParam)
				{
				case VK_ESCAPE:
					{
						// if the window with the target is either a combobox or
						// the child edit of a combobox and the combo is
						// dropped down then let it thru else if the target is
						// a child of ours then treat as a cancel
						BOOL bHandle = TRUE;
						
						if (CWinClasses::IsClass(pMsg->hwnd, WC_COMBOBOX))
							bHandle = !ComboBox_GetDroppedState(pMsg->hwnd);
						
						else if (CWinClasses::IsClass(::GetParent(pMsg->hwnd), WC_COMBOBOX))
							bHandle = !ComboBox_GetDroppedState(::GetParent(pMsg->hwnd));
						
						if (bHandle && ::IsChild(*this, pMsg->hwnd))
						{
							OnCancel();
							return TRUE;
						}
					}
				}
			}
			break;
		}

	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CToDoListWnd::OnCancel()
{
	ASSERT (Prefs().GetEscapeMinimizes());

	// if the close button has been configured to Minimize to tray
	// then do that here else normal minimize 
	int nOption = Prefs().GetSysTrayOption();
	
	if (nOption == STO_ONMINCLOSE || nOption == STO_ONCLOSE)
		MinimizeToTray();
	else
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CToDoListWnd::OnDeleteTask() 
{
	if (GetToDoCtrl().GetSelectedItem())
		GetToDoCtrl().DeleteSelectedTask();
}

void CToDoListWnd::OnDeleteAllTasks() 
{
	if (GetToDoCtrl().DeleteAllTasks())
	{
		m_mgrToDoCtrls.ClearFilePath(GetSelToDoCtrl()); // this will ensure that the user must explicitly overwrite the original file
		UpdateStatusbar();
	}
}

void CToDoListWnd::OnSave() 
{
	if (!SaveTaskList(GetSelToDoCtrl()))
	{
		// check readonly status and notify user
		// TODO
	}
	else
		UpdateCaption();
}

BOOL CToDoListWnd::SaveTaskList(int nIndex, LPCTSTR szFilePath)
{
	CAutoFlag af(m_bSaving, TRUE);
	CString sFilePath = szFilePath ? szFilePath : m_mgrToDoCtrls.GetFilePath(nIndex);
	
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	
	tdc.Flush();
	
	// build dialog title, incorporating tasklist name
	CString sName = m_mgrToDoCtrls.GetFriendlyProjectName(nIndex);
	CEnString sTitle(IDS_SAVETASKLIST_TITLE, sName);
	
	// conditions for saving
	// 1. Save As... ie szFilePath != NULL and not empty
	// 2. tasklist has been modified
	if ((szFilePath && !sFilePath.IsEmpty()) || tdc.IsModified())
	{
		CPreferencesDlg& prefs = Prefs();
		
		// do this in a loop in case the save fails for _any_ reason
		while (TRUE)
		{
			if (sFilePath.IsEmpty()) // means first time save
			{
				// activate tasklist
				if (!SelectToDoCtrl(nIndex, (nIndex != GetSelToDoCtrl())))
					return FALSE;
				
				// use tab text as hint to user
				sFilePath = m_mgrToDoCtrls.GetFilePath(nIndex, FALSE);

				CFileDialogEx dialog(FALSE, GetDefaultFileExt(), sFilePath, 
					OFN_OVERWRITEPROMPT, GetFileFilter(), this);
				
				dialog.m_ofn.lpstrTitle = sTitle;
				
				if (dialog.DoModal() != IDOK)
					return FALSE; // user elected not to proceed
				
				// else make sure the file is not readonly
				sFilePath = dialog.GetPathName();
				
				if (CDriveInfo::IsReadonlyPath(sFilePath) > 0)
				{
					CEnString sMessage(IDS_SAVEREADONLY, sFilePath);
					
					if (MessageBox(sMessage, sTitle, MB_OKCANCEL) == IDCANCEL)
						return FALSE; // user elected not to proceed
					else
					{
						sFilePath.Empty(); // try again
						continue;
					}
				}
			}
			
			// update source control status
			BOOL bSrcControl = m_mgrToDoCtrls.PathSupportsSourceControl(sFilePath);
			
			tdc.SetStyle(TDCS_ENABLESOURCECONTROL, bSrcControl);
			tdc.SetStyle(TDCS_CHECKOUTONLOAD, bSrcControl ? prefs.GetAutoCheckOut() : FALSE);
			
			CWaitCursor cursor;
			CTaskFile tasks;
			
			TDC_FILE nResult = tdc.Save(tasks, sFilePath);
			
			if (nResult == TDCO_SUCCESS)
			{
				m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
				m_mgrToDoCtrls.RefreshLastModified(nIndex);
				m_mgrToDoCtrls.RefreshReadOnlyStatus(nIndex);
				m_mgrToDoCtrls.RefreshPathType(nIndex);
				
				if (prefs.GetAddFilesToMRU())
					m_mruList.Add(sFilePath);
				
				UpdateCaption();
				UpdateStatusbar();
				
				// export to html?
				if (prefs.GetAutoHtmlExport())
				{
					CString sExportPath = prefs.GetAutoExportFolderPath();
					char szDrive[_MAX_DRIVE], szFolder[_MAX_DIR], szFName[_MAX_FNAME];
					
					_splitpath(sFilePath, szDrive, szFolder, szFName, NULL);
					
					if (!sExportPath.IsEmpty() && FileMisc::CreateFolder(sExportPath))
						_makepath(sFilePath.GetBuffer(MAX_PATH), NULL, sExportPath, szFName, ".html");
					else
						_makepath(sFilePath.GetBuffer(MAX_PATH), szDrive, szFolder, szFName, ".html");
					
					sFilePath.ReleaseBuffer();
					
					// if the users wants all attributes exported then
					// we can simply export the tasks retrieved from CToDoCtrl::Save
					if (prefs.GetExportVisibleColsOnly())
						Export2Html(tdc, sFilePath, prefs.GetSaveExportStylesheet());
					else
						Export2Html(tasks, sFilePath, prefs.GetSaveExportStylesheet());
				}
				
				// we're done
				break;
			}
			else // error handling
			{
				switch (nResult)
				{
				case TDCO_NOTALLOWED:
					{
						CEnString sMessage(IDS_SAVEACCESSDENIED, sFilePath);
						
						if (IDYES == MessageBox(sMessage, sTitle, MB_YESNO | MB_ICONEXCLAMATION))
						{
							sFilePath.Empty(); // try again
							continue;
						}
						else // clear modified status
						{
							tdc.SetModified(FALSE);
							m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
						}
					}
					break;
					
				case TDCO_CANCELLED:
					break;
					
				default:
					{
						int nError = nResult - (int)TDCO_OTHER;
						
						CEnString sMessage(IDS_UNKNOWNOPENERROR, sFilePath, nError);
						MessageBox(sMessage, sTitle, MB_OK);
					}
					break;
				}
				
				return FALSE;
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CToDoListWnd::GetFileFilter()
{
	static CEnString TDLFILEFILTER(IDS_TDLFILEFILTER);
	static CEnString XMLFILEFILTER(IDS_XMLFILEFILTER);
	
	if (Prefs().GetEnableTDLExtension())
		return TDLFILEFILTER;
	else
		return XMLFILEFILTER;
}

LPCTSTR CToDoListWnd::GetDefaultFileExt()
{
	static LPCTSTR TDLEXT = "tdl";
	static LPCTSTR XMLEXT = "xml";
	
	if (Prefs().GetEnableTDLExtension())
		return TDLEXT;
	else
		return XMLEXT;
}

void CToDoListWnd::UpdateStatusbar()
{
	if (m_bShowStatusBar)
	{
		int nSel = GetSelToDoCtrl();
		const CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);
		CString sFilePath = tdc.GetFilePath(), sStatus;
		
		UINT nVisibleTasks;
		UINT nTotalTasks = tdc.GetTaskCount(&nVisibleTasks);
		
		if (!sFilePath.IsEmpty())
			sStatus.Format(IDS_STATUSBAR, sFilePath, nVisibleTasks, nTotalTasks, tdc.GetFileVersion());
		else
			sStatus.Format(IDS_STATUSBARSHORT, nVisibleTasks, nTotalTasks);
		
		m_statusBar.SetWindowText(sStatus);
	}
}

void CToDoListWnd::OnLoad() 
{
	CFileDialogEx dialog(TRUE, GetDefaultFileExt(), NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
		OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST, 
		GetFileFilter(), this);
	
	const UINT BUFSIZE = 1024 * 5;
	static char FILEBUF[BUFSIZE] = { 0 };
	
	CEnString sTitle(IDS_OPENTASKLIST_TITLE);
	
	dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
	dialog.m_ofn.lpstrFile = FILEBUF;
	dialog.m_ofn.nMaxFile = BUFSIZE;
	
	if (dialog.DoModal() == IDOK)
	{
		UpdateWindow();
		CWaitCursor cursor;
		
		POSITION pos = dialog.GetStartPosition();
		
		while (pos)
		{
			CString sTaskList = dialog.GetNextPathName(pos);
			TDC_FILE nResult = OpenTaskList(sTaskList);
			
			if (nResult == TDCO_SUCCESS)
			{
				ResizeDlg();
				UpdateWindow();
			}
			else
				HandleLoadTasklistError(nResult, sTaskList);
		}
		
		RefreshTabOrder();
	}
}

void CToDoListWnd::HandleLoadTasklistError(TDC_FILE nErr, LPCTSTR szTaskList)
{
	CEnString sMessage, sTitle(IDS_OPENTASKLIST_TITLE);
	
	switch (nErr)
	{
	case TDCO_SUCCESS:
		break; // not an error!
		
	case TDCO_CANCELLED:
		break; 
		
	case TDCO_NOTEXIST:
		sMessage.Format(IDS_TASKLISTNOTFOUND, szTaskList);
		break;
		
	case TDCO_NOTTASKLIST:
		sMessage.Format(IDS_INVALIDTASKLIST, szTaskList);
		break;
		
	case TDCO_NOTALLOWED:
		sMessage.Format(IDS_OPENACCESSDENIED, szTaskList);
		break;
		
	case TDCO_BADMSXML:
		sMessage.Format(IDS_BADXML, szTaskList);
		break;
		
	case TDCO_NOENCRYPTIONDLL:
		sMessage.Format(IDS_NOENCRYPTIONDLL, szTaskList);
		break;
		
	case TDCO_UNKNOWNENCRYPTION:
		sMessage.Format(IDS_UNKNOWNENCRYPTION, szTaskList);
		break;
		
	default: // all the other errors
		sMessage.Format(IDS_UNKNOWNOPENERROR, szTaskList, nErr - (int)TDCO_OTHER);
		break;
	}
	
	if (!sMessage.IsEmpty())
		MessageBox(sMessage, sTitle, MB_OK);
}

void CToDoListWnd::SaveSettings()
{
	// pos
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	
	AfxGetApp()->WriteProfileInt("Pos", "TopLeft", MAKELPARAM(wp.rcNormalPosition.left, wp.rcNormalPosition.top));
	AfxGetApp()->WriteProfileInt("Pos", "BottomRight", MAKELPARAM(wp.rcNormalPosition.right, wp.rcNormalPosition.bottom));
	AfxGetApp()->WriteProfileInt("Pos", "Hidden", !m_bVisible);
	AfxGetApp()->WriteProfileInt("Pos", "Maximized", IsZoomed());
	
	// version
	AfxGetApp()->WriteProfileInt("Version", "Version", GetVersion());
	
	// last open files
	int nTDC = GetTDCCount();
	int nSel = GetSelToDoCtrl(); // and last active file
	
	if (nTDC) // but don't overwrite files saved in OnQueryEndSession() or OnClose()
	{
		AfxGetApp()->WriteProfileInt("Settings", "NumLastFiles", nTDC);
		
		while (nTDC--)
		{
			CEnString sKey("LastFile%d", nTDC);
			AfxGetApp()->WriteProfileString("Settings", sKey, m_mgrToDoCtrls.GetFilePath(nTDC));
			
			if (nSel == nTDC)
				AfxGetApp()->WriteProfileString("Settings", "LastActiveFile", m_mgrToDoCtrls.GetFilePath(nTDC));
		}
	}
	
	// other settings
	AfxGetApp()->WriteProfileInt("Settings", "SimpleMode", m_bMaxTasklists);
	AfxGetApp()->WriteProfileInt("Settings", "ShowFilterBar", m_bShowFilterBar);
	AfxGetApp()->WriteProfileInt("Settings", "ToolbarOption", m_bShowToolbar ? TB_TOOLBARANDMENU : TB_TOOLBARHIDDEN);
	AfxGetApp()->WriteProfileInt("Settings", "ShowProjectName", m_bShowProjectName);
	AfxGetApp()->WriteProfileInt("Settings", "ShowStatusBar", m_bShowStatusBar);

	if (m_findDlg.GetSafeHwnd())
		AfxGetApp()->WriteProfileInt("Settings", "FindTasksVisible", m_bFindShowing && m_findDlg.IsWindowVisible());
	
	if (Prefs().GetAddFilesToMRU())
		m_mruList.WriteList();
}

LRESULT CToDoListWnd::OnWebUpdateWizard(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT (Prefs().GetAutoCheckForUpdates());

	CheckForUpdates(FALSE);
	return 0L;
}

LRESULT CToDoListWnd::OnAddToolbarTools(WPARAM /*wp*/, LPARAM /*lp*/)
{
	Misc::ProcessMsgLoop();
	AppendTools2Toolbar();
	return 0L;
}

LRESULT CToDoListWnd::OnPostOnCreate(WPARAM /*wp*/, LPARAM /*lp*/)
{
	CAutoFlag af(m_bReloading, TRUE);
	CPreferencesDlg& prefs = Prefs();

	RestoreVisibility();

	LockWindowUpdate();
	
	// open cmdline tasklist
	int nTDCCount = AfxGetApp()->GetProfileInt("Settings", "NumLastFiles", 0);

	// if we have a file on the commandline or any previous tasklists
	// set the prompt of the initial tasklist to something appropriate
	if (!m_startupOptions.sFilePath.IsEmpty() || nTDCCount)
	{
		//	TODO
	}
	
	BOOL bOpen = FALSE;

	if (!m_startupOptions.sFilePath.IsEmpty())
	{
		bOpen = (TDCO_SUCCESS == OpenTaskList(m_startupOptions.sFilePath));

		// other startup options
		if (bOpen)
		{
			CFilteredToDoCtrl& tdc = GetToDoCtrl();

			if (m_startupOptions.dwIDSel)
				tdc.SelectTask(m_startupOptions.dwIDSel);

			else if (!m_startupOptions.sNewTask.IsEmpty())
			{
				tdc.NewTask(m_startupOptions.sNewTask, TDC_INSERTATTOP, TRUE, FALSE);

				if (!m_startupOptions.sComments.IsEmpty())
					tdc.SetSelectedTaskComments(m_startupOptions.sComments, m_startupOptions.sComments);
			}
		}
	}
	
	m_startupOptions.Clear(); // always
	
	// load last files
	if (!bOpen)
	{
		CString sLastActiveFile = AfxGetApp()->GetProfileString("Settings", "LastActiveFile");
		
		for (int nTDC = 0; nTDC < nTDCCount; nTDC++)
		{
			CEnString sKey("LastFile%d", nTDC);
			CString sLastFile = AfxGetApp()->GetProfileString("Settings", sKey);
			
			if (!sLastFile.IsEmpty())
				OpenTaskList(sLastFile, FALSE);

			// process all pending messages
			Misc::ProcessMsgLoop();
		}
		
		// select 
		if (GetTDCCount())
		{
			if (!SelectToDoCtrl(sLastActiveFile, FALSE))
				SelectToDoCtrl(0, FALSE); // the first one
		}
	}

	// if there's only one tasklist open and it's pristine then 
	// it's the original one so add a sample task
	if (m_mgrToDoCtrls.IsPristine(0))
	{
		ASSERT (GetTDCCount() == 1);

		CToDoCtrl& tdc = GetToDoCtrl();
		ASSERT (tdc.GetTaskCount() == 0);

		tdc.NewTask(CEnString(IDS_SAMPLETASK), TDC_INSERTATTOP, TRUE, FALSE);
		tdc.SetModified(FALSE);
		m_mgrToDoCtrls.SetModifiedStatus(0, FALSE);

		UpdateCaption();
	}
	else // due task notifications
	{
		int nDueBy = prefs.GetNotifyDueByOnLoad();
		
		if (nDueBy != PFP_INVALID)
		{
			UpdateWindow();
			m_tabCtrl.UpdateWindow();
			
			int nCtrls = GetTDCCount();
			
			for (int nCtrl = 0; nCtrl < nCtrls; nCtrl++)
				DoDueTaskNotification(&GetToDoCtrl(nCtrl), nDueBy);
		}
	}

	UnlockWindowUpdate();
	
	// find tasks dialog
	if (AfxGetApp()->GetProfileInt("Settings", "FindTasksVisible", 0))
	{
		OnFindTasks();
		
		if (prefs.GetRefreshFindOnLoad())
			m_findDlg.RefreshSearch();
	}

	// refresh toolbar 'tools' buttons unless minimized because
	// we must handle it when we're first shown
	if (m_bShowToolbar && AfxGetApp()->m_nCmdShow != SW_SHOWMINIMIZED)
		AppendTools2Toolbar();

	InitMenuIconManager();
	
	// web update
	if (Prefs().GetAutoCheckForUpdates())
		PostMessage(WM_WEBUPDATEWIZARD);

	return 0L;
}

void CToDoListWnd::CheckForUpdates(BOOL bManual)
{
	// only check once a day
	int nLastUpdate = AfxGetApp()->GetProfileInt("Updates", "LastUpdate", 0);

	if (!bManual && nLastUpdate >= (int)COleDateTime::GetCurrentTime().m_dt)
		return;

	AfxGetApp()->WriteProfileInt("Updates", "LastUpdate", (int)COleDateTime::GetCurrentTime().m_dt);

	// get the app wuw path
	char szFolder[MAX_PATH], szDrive[_MAX_DRIVE];
	char szWuwPath[MAX_PATH + 1];

	GetModuleFileName(NULL, szWuwPath, MAX_PATH);
	_splitpath(szWuwPath, szDrive, szFolder, NULL, NULL);
	_makepath(szWuwPath, szDrive, szFolder, "WebUpdateSvc.exe", NULL);

	// check for existence if manual
	if (bManual && GetFileAttributes(szWuwPath) == 0xffffffff)
	{
		const LPCTSTR DOWNLOAD_WUW_PATH = "http://www.abstractspoon.com/todolist_wuw.zip";

		if (AfxMessageBox(IDS_NOWUW, MB_YESNO) == IDYES)
			ShellExecute(NULL, "open", DOWNLOAD_WUW_PATH, NULL, NULL, SW_HIDE);
		else
			return;
	}

	const LPCTSTR UPDATE_SCRIPT_PATH = "http://www.abstractspoon.com/todolist_update.txt";
	const LPCTSTR UPDATE_SCRIPT_PATH_MANUAL = "http://www.abstractspoon.com/todolist_update_manual.txt";

	if (bManual)
		ShellExecute(NULL, "open", szWuwPath, UPDATE_SCRIPT_PATH_MANUAL, NULL, SW_HIDE);
	else
		ShellExecute(NULL, "open", szWuwPath, UPDATE_SCRIPT_PATH, NULL, SW_HIDE);
} 
	
void CToDoListWnd::LoadSettings()
{
	CPreferencesDlg& prefs = Prefs();
	
	// MRU
	if (prefs.GetAddFilesToMRU())
		m_mruList.ReadList();
	
	// settings
	m_bMaxTasklists = AfxGetApp()->GetProfileInt("Settings", "SimpleMode", m_bMaxTasklists);
	m_bShowFilterBar = AfxGetApp()->GetProfileInt("Settings", "ShowFilterBar", m_bShowFilterBar);
	m_bShowProjectName = AfxGetApp()->GetProfileInt("Settings", "ShowProjectName", m_bShowProjectName);

	m_bShowStatusBar = AfxGetApp()->GetProfileInt("Settings", "ShowStatusBar", m_bShowStatusBar);
	m_statusBar.ShowWindow(m_bShowStatusBar ? SW_SHOW : SW_HIDE);
	
	// toolbar
	m_bShowToolbar = (AfxGetApp()->GetProfileInt("Settings", "ToolbarOption", TB_TOOLBARANDMENU) != TB_TOOLBARHIDDEN);
	m_toolbar.ShowWindow(m_bShowToolbar ? SW_SHOW : SW_HIDE);
	m_toolbar.EnableWindow(m_bShowToolbar);

	RefreshFilterLabelAlignment();
	
	CXmlFile::EnableFormattedOutput(prefs.GetFormatXmlOutput());
	
	// pos
	RestorePosition();

	// note: we do not restore visibility until OnPostOnCreate
	
	// hotkey
	UpdateGlobalHotkey();
	
	// time periods
	CTimeHelper::SetHoursInOneDay(prefs.GetHoursInOneDay());
	CTimeHelper::SetDaysInOneWeek(prefs.GetDaysInOneWeek());

	// support for .tdl
	CFileRegister filereg("tdl", "tdl_Tasklist");
	
	if (prefs.GetEnableTDLExtension())
		filereg.RegisterFileType("Tasklist", 0);
	else
		filereg.UnRegisterFileType();
}

void CToDoListWnd::RestoreVisibility()
{
	CPreferencesDlg& prefs = Prefs();

	int nDefShowState = AfxGetApp()->m_nCmdShow;
	BOOL bShowOnStartup = prefs.GetShowOnStartup();

	BOOL bMaximized = AfxGetApp()->GetProfileInt("Pos", "Maximized", FALSE) || (nDefShowState == SW_SHOWMAXIMIZED);
	BOOL bMinimized = !bShowOnStartup && (nDefShowState == SW_SHOWMINIMIZED || nDefShowState == SW_SHOWMINNOACTIVE);
	
	if (bMinimized)
	{
		bMaximized = FALSE; // can't be max-ed and min-ed
		m_bStartHidden = TRUE;
	}
	
	if (m_bVisible == -1) // not yet set
	{
		m_bVisible = TRUE;
		
		// the only reason it can be hidden is if it uses the systray
		// and the user has elected to not have it show at startup
		// and it was hidden the last time it closed or its set to run
		// minimized and that is the trigger to hide it
		if (!bShowOnStartup && prefs.GetUseSysTray())
		{
			if (AfxGetApp()->GetProfileInt("Pos", "Hidden", FALSE))
				m_bVisible = FALSE;
			
			// also if wp.showCmd == minimized and we would hide to sys
			// tray when minimized then hide here too
			else if (nDefShowState == SW_SHOWMINIMIZED || nDefShowState == SW_SHOWMINNOACTIVE)
			{
				int nSysTrayOption = Prefs().GetSysTrayOption();
				
				if (nSysTrayOption == STO_ONMINIMIZE || nSysTrayOption == STO_ONMINCLOSE)
					m_bVisible = FALSE;
			}
		}
	}
	
	if (m_bVisible)
	{
		int nShowCmd = (bMaximized ? SW_SHOWMAXIMIZED : 
						(bMinimized ? SW_SHOWMINIMIZED : SW_SHOW));
		
		ShowWindow(nShowCmd);

		Invalidate();
		UpdateWindow();
	}
	else
		m_bStartHidden = TRUE;

	// don't set topmost if maximized
	if (prefs.GetAlwaysOnTop() && !bMaximized)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CToDoListWnd::RestorePosition()
{
	DWORD dwTopLeft = (DWORD)AfxGetApp()->GetProfileInt("Pos", "TopLeft", -1);
	DWORD dwBottomRight = (DWORD)AfxGetApp()->GetProfileInt("Pos", "BottomRight", -1);
	
	CRect rect(0, 0, 0, 0);
	
	if (dwTopLeft != -1 && dwBottomRight != -1)
	{
		// get min size before we resize
		MINMAXINFO mmi;
		SendMessage(WM_GETMINMAXINFO, 0, (LPARAM)&mmi);
		
		CRect rTemp(GET_X_LPARAM(dwTopLeft), GET_Y_LPARAM(dwTopLeft), 
			GET_X_LPARAM(dwBottomRight), GET_Y_LPARAM(dwBottomRight));
		
		rTemp.right = max(rTemp.right, rTemp.left + mmi.ptMinTrackSize.x);
		rTemp.bottom = max(rTemp.bottom, rTemp.top + mmi.ptMinTrackSize.y);
		
		// ensure this intersects with the desktop
		if (NULL != MonitorFromRect(rTemp, MONITOR_DEFAULTTONULL))
		{
			rect = rTemp;

			// because the position was save using the results of 
			// GetWindowPlacement we must use SetWindowPlacement
			// to restore the window
			WINDOWPLACEMENT wp;
			ZeroMemory(&wp, sizeof(wp));

			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			// MoveWindow(rect);
		}
	}
	
	// first time or monitors changed?
	if (rect.IsRectEmpty())
	{
		MoveWindow(0, 0, 800, 600);
		CenterWindow();
	}
}

void CToDoListWnd::OnNew() 
{
	CreateNewTaskList(TRUE);
	RefreshTabOrder();
}

BOOL CToDoListWnd::CreateNewTaskList(BOOL bAddDefTask)
{
	CFilteredToDoCtrl* pNew = NewToDoCtrl();
	
	if (pNew)
	{
		int nNew = AddToDoCtrl(pNew);
		
		// insert a default task
		if (bAddDefTask && pNew->GetTaskCount() == 0)
			VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP, TRUE, FALSE));
		
		// clear modified flag
		pNew->SetModified(FALSE);
		m_mgrToDoCtrls.SetModifiedStatus(nNew, FALSE);
		
		// update filter controls
		RefreshFilterControls();
		
		UpdateCaption();
		UpdateStatusbar();
	}

	return (pNew != NULL);
}

void CToDoListWnd::OnUpdateDeletetask(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection());	
}

void CToDoListWnd::OnUpdateEditTasktext(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(nSelCount == 1 && !tdc.IsReadOnly());	
}

void CToDoListWnd::OnUpdateTaskcolor(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection() && 
		Prefs().GetTextColorOption() == COLOROPT_DEFAULT);	
}

void CToDoListWnd::OnUpdateTaskdone(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	if (nSelCount == 1)
		pCmdUI->SetCheck(tdc.IsSelectedTaskDone() ? 1 : 0);
	
	pCmdUI->Enable(!tdc.IsReadOnly() && GetToDoCtrl().GetSelectedItem());	
}

void CToDoListWnd::OnUpdateDeletealltasks(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && GetToDoCtrl().GetTaskCount());	
}

void CToDoListWnd::OnUpdateSave(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(GetTDCCount() && tdc.IsModified() && !tdc.IsReadOnly());	
}

void CToDoListWnd::OnUpdateNew(CCmdUI* pCmdUI)  
{
	pCmdUI->Enable(TRUE);	
}

BOOL CToDoListWnd::OnEraseBkgnd(CDC* pDC) 
{
	CRect rClient;

	GetClientRect(rClient);

	// we must draw out own bevel below the toolbar (or menu if
	// the toolbar is not visible)
	int nVPos = 0;

	if (m_bShowToolbar)
		nVPos += m_toolbar.CalcDynamicLayout(rClient.Width(), LM_HORZ | LM_STRETCH).cy;

	pDC->FillSolidRect(0, nVPos, rClient.Width(), 1, GetSysColor(COLOR_3DSHADOW));
	pDC->FillSolidRect(0, nVPos + 1, rClient.Width(), 1, GetSysColor(COLOR_3DHIGHLIGHT));

	rClient.top = nVPos + BEVEL;
	pDC->FillSolidRect(rClient, GetSysColor(COLOR_3DFACE));

	// this is definitely amongst the worst hacks I've ever had to implement.
	// It occurs because the CSysImageList class seems to not initialize 
	// properly unless the main window is visible. so in the case of starting hidden
	// or starting minimized we must wait until we become visible before
	// adding the tools to the toolbar.
	if (m_bStartHidden)
	{
		m_bStartHidden = FALSE;
		PostMessage(WM_ADDTOOLBARTOOLS);
	}

	return TRUE;
}

void CToDoListWnd::OnSortBy(UINT nCmdID) 
{
	TDC_SORTBY nSortBy = GetSortBy(nCmdID);
	
	if (nSortBy != (TDC_SORTBY)-1)
	{
		// update menu icon mgr
		TDC_SORTBY nPrev = GetToDoCtrl().GetSortBy();
		m_mgrMenuIcons.ChangeImageID(GetSortID(nPrev), nCmdID);

		// update todoctrl
		GetToDoCtrl().Sort(nSortBy);
	}
}

void CToDoListWnd::OnUpdateSortBy(CCmdUI* pCmdUI)
{
	TDC_SORTBY nSortBy = GetToDoCtrl().GetSortBy();
	UINT nCmdID = GetSortID(nSortBy);

	// let menu icon manager handle everything except 'sorting disabled'
	if (nSortBy == TDC_SORTDISABLED && nCmdID == pCmdUI->m_nID)
		pCmdUI->SetCheck(1);
}

TDC_SORTBY CToDoListWnd::GetSortBy(UINT nSortID) 
{
	switch (nSortID)
	{
	case ID_SORT_BYNAME:			return TDC_SORTBYNAME;
	case ID_SORT_BYID:				return TDC_SORTBYID;
	case ID_SORT_BYALLOCTO:			return TDC_SORTBYALLOCTO;
	case ID_SORT_BYALLOCBY:			return TDC_SORTBYALLOCBY;
	case ID_SORT_BYSTATUS:			return TDC_SORTBYSTATUS;
	case ID_SORT_BYCATEGORY:		return TDC_SORTBYCATEGORY;
	case ID_SORT_BYPERCENT:			return TDC_SORTBYPERCENT;
	case ID_SORT_BYCOLOR:			return TDC_SORTBYCOLOR;
	case ID_SORT_BYTIMEEST:			return TDC_SORTBYTIMEEST;
	case ID_SORT_BYTIMESPENT:		return TDC_SORTBYTIMESPENT;
	case ID_SORT_BYSTARTDATE:		return TDC_SORTBYSTARTDATE;
	case ID_SORT_BYDUEDATE:			return TDC_SORTBYDUEDATE;
	case ID_SORT_BYDONEDATE:		return TDC_SORTBYDONEDATE;
	case ID_SORT_BYDONE:			return TDC_SORTBYDONE;
	case ID_SORT_BYPRIORITY:		return TDC_SORTBYPRIORITY;
	case ID_SORT_BYCREATEDBY:		return TDC_SORTBYCREATEDBY;
	case ID_SORT_BYCREATIONDATE:	return TDC_SORTBYCREATIONDATE;
	case ID_SORT_BYMODIFYDATE:		return TDC_SORTBYLASTMOD;
	case ID_SORT_BYRISK:			return TDC_SORTBYRISK;
	case ID_SORT_BYEXTERNALID:		return TDC_SORTBYEXTERNALID;
	case ID_SORT_BYCOST:			return TDC_SORTBYCOST;
	case ID_SORT_BYVERSION:			return TDC_SORTBYVERSION;
	case ID_SORT_BYRECURRENCE:		return TDC_SORTBYRECURRENCE;
	case ID_SORT_NONE:				return TDC_SORTDISABLED;
	}
	
	ASSERT (0);
	return (TDC_SORTBY)-1;
}

UINT CToDoListWnd::GetSortID(TDC_SORTBY nSortBy) 
{
	switch (nSortBy)
	{
	case TDC_SORTBYNAME:			return ID_SORT_BYNAME;
	case TDC_SORTBYID:				return ID_SORT_BYID;
	case TDC_SORTBYALLOCTO:			return ID_SORT_BYALLOCTO;
	case TDC_SORTBYALLOCBY:			return ID_SORT_BYALLOCBY;
	case TDC_SORTBYSTATUS:			return ID_SORT_BYSTATUS;
	case TDC_SORTBYCATEGORY:		return ID_SORT_BYCATEGORY;
	case TDC_SORTBYPERCENT:			return ID_SORT_BYPERCENT;
	case TDC_SORTBYCOLOR:			return ID_SORT_BYCOLOR;
	case TDC_SORTBYTIMEEST:			return ID_SORT_BYTIMEEST;
	case TDC_SORTBYTIMESPENT:		return ID_SORT_BYTIMESPENT;
	case TDC_SORTBYSTARTDATE:		return ID_SORT_BYSTARTDATE;
	case TDC_SORTBYDUEDATE:			return ID_SORT_BYDUEDATE;
	case TDC_SORTBYDONEDATE:		return ID_SORT_BYDONEDATE;
	case TDC_SORTBYDONE:			return ID_SORT_BYDONE;
	case TDC_SORTBYPRIORITY:		return ID_SORT_BYPRIORITY;
	case TDC_SORTBYFLAG:			return ID_SORT_BYFLAG;
	case TDC_SORTBYCREATEDBY:		return ID_SORT_BYCREATEDBY;
	case TDC_SORTBYCREATIONDATE:	return ID_SORT_BYCREATIONDATE;
	case TDC_SORTBYLASTMOD:			return ID_SORT_BYMODIFYDATE;
	case TDC_SORTBYRISK:			return ID_SORT_BYRISK;
	case TDC_SORTBYEXTERNALID:		return ID_SORT_BYEXTERNALID;
	case TDC_SORTBYCOST:			return ID_SORT_BYCOST;
	case TDC_SORTBYVERSION:			return ID_SORT_BYVERSION;
	case TDC_SORTBYRECURRENCE:		return ID_SORT_BYRECURRENCE;
	case TDC_SORTDISABLED:			return ID_SORT_NONE;
	}
	
	ASSERT (0);
	return 0;
}

void CToDoListWnd::OnNewtaskAttopSelected() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATTOPOFSELTASKPARENT));
}

void CToDoListWnd::OnNewtaskAtbottomSelected() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATBOTTOMOFSELTASKPARENT));
}

void CToDoListWnd::OnNewtaskAfterselectedtask() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTAFTERSELTASK));
}

void CToDoListWnd::OnNewtaskBeforeselectedtask() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTBEFORESELTASK));
}

void CToDoListWnd::OnNewsubtaskAtbottom() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATBOTTOMOFSELTASK));
}

void CToDoListWnd::OnNewsubtaskAttop() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATTOPOFSELTASK));
}

BOOL CToDoListWnd::NewTask(LPCTSTR szTitle, TDC_INSERTWHERE nInsertWhere, BOOL bSelect, BOOL bEdit)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (tdc.IsReadOnly())
		return FALSE;
	
	// handle special case when tasklist is empty
	if (!tdc.GetTaskCount())
		nInsertWhere = TDC_INSERTATBOTTOM;
	
	if (!tdc.NewTask(szTitle, nInsertWhere, bSelect, FALSE))
		return FALSE;
	
	// else init attributes other than defaults
	CAutoFlag af(m_bInNewTask, TRUE);
	CPreferencesDlg& prefs = Prefs();
	
	if (prefs.GetUseParentAttrib(PTPA_ALLOCTO))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_ALLOCTO);
	
	if (prefs.GetUseParentAttrib(PTPA_ALLOCBY))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_ALLOCBY);
	
	if (prefs.GetUseParentAttrib(PTPA_STATUS))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_STATUS);
	
	if (prefs.GetUseParentAttrib(PTPA_CATEGORY))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_CATEGORY);
	
	if (prefs.GetUseParentAttrib(PTPA_TIMEEST))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_TIMEEST);
	
	if (prefs.GetUseParentAttrib(PTPA_PRIORITY))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_PRIORITY);
	
	if (prefs.GetUseParentAttrib(PTPA_COLOR))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_COLOR);
	
	if (prefs.GetUseParentAttrib(PTPA_DUEDATE))
		tdc.SetSelectedTaskAttributeAsParent(TDCA_DUEDATE);

	// finally do the edit
	if (bEdit)
		tdc.EditSelectedTask(TRUE);
	
	return TRUE;
}

void CToDoListWnd::OnUpdateSort(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	// note: we're still allowed to sort when readonly but the result is not saved
	// but can't resort if 'unsorted'. however if we're in 'toolbar only' mode
	// we can't disable the button else the drop arrow gets disabled too.
	BOOL bSortDisabled = (tdc.GetSortBy() == TDC_SORTDISABLED);
	
	pCmdUI->Enable(!bSortDisabled && tdc.GetTaskCount());	
}

void CToDoListWnd::OnEditTaskcolor() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (!tdc.IsReadOnly() && tdc.HasSelection())
	{
		CEnColorDialog dialog(tdc.GetSelectedTaskColor(), CC_FULLOPEN | CC_RGBINIT);
		
		if (dialog.DoModal() == IDOK)
			tdc.SetSelectedTaskColor(dialog.GetColor());
	}
}

void CToDoListWnd::OnEditTaskdone() 
{
	GetToDoCtrl().SetSelectedTaskDone(!GetToDoCtrl().IsSelectedTaskDone());
}

void CToDoListWnd::OnEditTasktext() 
{
	GetToDoCtrl().EditSelectedTask();
}

void CToDoListWnd::OnTrayIconClick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	Show(Prefs().GetToggleTrayVisibility());
	*pResult = 0;
}

LRESULT CToDoListWnd::OnToDoListShowWindow(WPARAM /*wp*/, LPARAM /*lp*/)
{
	Show(FALSE);
	return 0;
}

LRESULT CToDoListWnd::OnToDoListGetVersion(WPARAM /*wp*/, LPARAM /*lp*/)
{
	return GetVersion();
}

LRESULT CToDoListWnd::OnToDoListRefreshPrefs(WPARAM /*wp*/, LPARAM /*lp*/)
{
	// sent by the app object if registry settings have changed
	ResetPrefs();

	// mark all tasklists as needing update
	m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);
	
	// then update active tasklist
	UpdateToDoCtrlPreferences();

	return 0;
}

void CToDoListWnd::OnTrayIconDblClk(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	Show(FALSE);
	
	*pResult = 0;
}

void CToDoListWnd::OnTrayiconCreatetask() 
{
	Show(FALSE);
	
	// create a task at the top of the tree
	GetToDoCtrl().NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP);
}

void CToDoListWnd::OnTrayIconRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetForegroundWindow();
	
	// show context menu
	CMenu menu;
	
	if (menu.LoadMenu(IDR_MISC))
	{
		CMenu* pSubMenu = menu.GetSubMenu(TRAYICON);
		pSubMenu->SetDefaultItem(ID_TRAYICON_SHOW);
		
		if (pSubMenu)
		{
			m_mgrToDoCtrls.PreparePopupMenu(*pSubMenu, ID_TRAYICON_SHOWDUETASKS1);
			
			NM_TRAYICON* pNMTI = (NM_TRAYICON*)pNMHDR;

			// in order to ensure that multiple password dialogs cannot 
			// appear we must make sure that all the command handling is
			// done before we return from here
			UINT nCmdID = ::TrackPopupMenu(*pSubMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON, 
											pNMTI->ptAction.x, pNMTI->ptAction.y, 0, *this, NULL);

			PostMessage(WM_NULL);

			if (nCmdID != (UINT)-1)
				SendMessage(WM_COMMAND, nCmdID);
		}
	}
	
	*pResult = 0;
}

void CToDoListWnd::OnClose() 
{
	int nSysTrayOption = Prefs().GetSysTrayOption();
	
	if (nSysTrayOption == STO_ONCLOSE || nSysTrayOption == STO_ONMINCLOSE)
		MinimizeToTray();
	
	else // shutdown but user can cancel
		DoExit();
}

void CToDoListWnd::MinimizeToTray()
{
	// save prev state so we can restore properly
	AfxGetApp()->WriteProfileInt("Pos", "Maximized", IsZoomed());
	
	if (Prefs().GetAutoSaveOnSysTray())
	{
		// save all
		SaveAll(Prefs().GetAutoSaveUnsavedOnMinimize(), FALSE, FALSE, TRUE);
	}
	
	// hide main window and release resources
	MinToTray(*this); // courtesy of floyd
	SetProcessWorkingSetSize(GetCurrentProcess(), (DWORD)-1, (DWORD)-1); 
	m_bVisible = FALSE;
	
	// hide find dialog
	if (m_findDlg.GetSafeHwnd())
	{
		m_bFindShowing = m_findDlg.IsWindowVisible();
		m_findDlg.Show(FALSE);
	}
	else
		m_bFindShowing = FALSE;
}

void CToDoListWnd::OnTrayiconClose() 
{
	DoExit();
}

LRESULT CToDoListWnd::OnToDoCtrlNotifySort(WPARAM /*wp*/, LPARAM lp)
{
	// update menuicon mgr
	TDC_SORTBY nPrev = (TDC_SORTBY)LOWORD(lp);
	TDC_SORTBY nSortBy = (TDC_SORTBY)HIWORD(lp);

	m_mgrMenuIcons.ChangeImageID(GetSortID(nPrev), GetSortID(nSortBy));

	return 0;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyMinWidthChange(WPARAM /*wp*/, LPARAM /*lp*/)
{
	CheckMinWidth();
	
	return 0;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyListChange(WPARAM /*wp*/, LPARAM lp)
{
	// decide whether the filter controls need updating
	switch (lp)
	{
	case TDCA_ALLOCTO:
	case TDCA_ALLOCBY:
	case TDCA_STATUS:
	case TDCA_CATEGORY:
		RefreshFilterControls();
		break;
	}
	
	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyTimeTrack(WPARAM /*wp*/, LPARAM lp)
{
	BOOL bTrack = lp;

	if (bTrack && Prefs().GetExclusiveTimeTracking())
	{
		// end time tracking on every other tasklist
		int nSel = GetSelToDoCtrl();
		ASSERT (nSel != -1);

		for (int nCtrl = 0; nCtrl < GetTDCCount(); nCtrl++)
		{
			if (nCtrl != nSel)
				GetToDoCtrl(nCtrl).EndTimeTracking();
		}
	}

	return 0L;
}
	
LRESULT CToDoListWnd::OnToDoCtrlNotifyMod(WPARAM /*wp*/, LPARAM lp)
{
	int nSel = GetSelToDoCtrl();
	
	if (nSel == -1)
		return 0L;
	
	BOOL bWasMod = m_mgrToDoCtrls.GetModifiedStatus(nSel);
	m_mgrToDoCtrls.SetModifiedStatus(nSel, TRUE);
	
	// update the caption only if the control is not currently modified
	// or the project name changed
	if (!bWasMod || lp == TDCA_PROJNAME)
		UpdateCaption();
	
	// update due items
	if (lp == TDCA_DONEDATE || lp == TDCA_DUEDATE)
		OnTimerDueItems(nSel);
	
	// update tab order
	if (lp == TDCA_PROJNAME && Prefs().GetKeepTabsOrdered())
		RefreshTabOrder();
	
	UpdateStatusbar();
	
	return 0L;
}

void CToDoListWnd::UpdateCaption()
{
	int nSel = GetSelToDoCtrl();
	
	CString sProjectName = m_mgrToDoCtrls.UpdateTabItemText(nSel);
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	UINT nIDStatus = 0;
	
	if (m_mgrToDoCtrls.GetReadOnlyStatus(nSel) > 0)
		nIDStatus = IDS_STATUSREADONLY;
	
	else if (tdc.CompareFileFormat() == TDCFF_NEWER)
		nIDStatus = IDS_STATUSNEWERFORMAT;
	
	else if (m_mgrToDoCtrls.GetReadOnlyStatus(nSel) == 0 && 
		m_mgrToDoCtrls.PathSupportsSourceControl(nSel))
	{
		if (tdc.IsCheckedOut())
			nIDStatus = IDS_STATUSCHECKEDOUT;
		else
			nIDStatus = IDS_STATUSCHECKEDIN;
	}
	else if (!Prefs().GetEnableSourceControl() && m_mgrToDoCtrls.IsSourceControlled(nSel))
	{
		nIDStatus = IDS_STATUSSOURCECONTROLLED;
	}
	
	CString sCaption;
	CEnString sCopyright(IDS_COPYRIGHT), sStatus(nIDStatus);
	
	if (nIDStatus)
		sCaption.Format("%s [%s] - %s", sProjectName, (LPCTSTR)sStatus, (LPCTSTR)sCopyright);
	else
		sCaption.Format("%s - %s", sProjectName, (LPCTSTR)sCopyright);
	
	SetWindowText(sCaption);

	// set tray tip too
	m_ti.SetTip(sCaption);
}

void CToDoListWnd::OnUpdateNewsubtaskAtBottom(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount == 1);
}

BOOL CToDoListWnd::Export2Html(CFilteredToDoCtrl& tdc, LPCTSTR szFilePath, LPCTSTR szStylesheet) const
{
	CWaitCursor cursor;
	
	tdc.Flush();
	
	// options
	DWORD dwFlags = 0;

	if (Prefs().GetExportVisibleColsOnly())
		dwFlags |= TDCGT_VISIBLECOLSONLY;

	// stylesheets don't seem to like the HTML comments at present
//	if (GetFileAttributes(szStylesheet) == 0xffffffff)
		dwFlags |= TDCGT_HTMLCOMMENTS;
	
	CTaskFile tasks;
	tdc.GetTasklist(tasks, dwFlags);
	
	return Export2Html(tasks, szFilePath, szStylesheet);
}

BOOL CToDoListWnd::Export2Html(const CTaskFile& tasks, LPCTSTR szFilePath, LPCTSTR szStylesheet) const
{
	CWaitCursor cursor;
	
	if (GetFileAttributes(szStylesheet) != 0xffffffff)
	{
		CTaskFile* pTasks = const_cast<CTaskFile*>(&tasks);
		pTasks->Save("printPreview.xml");

		return tasks.TransformToFile(szStylesheet, szFilePath, Prefs().GetHtmlCharSet());
	}
	
	// else default export
	return m_mgrImportExport.ExportTaskListToHtml(&tasks, szFilePath);
}

void CToDoListWnd::OnSaveas() 
{
	int nSel = GetSelToDoCtrl();

	// use tab text as hint to user
	CString sFilePath = m_mgrToDoCtrls.GetFilePath(nSel, FALSE);
	
	// display the dialog
	CFileDialogEx dialog(FALSE, GetDefaultFileExt(), sFilePath, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, GetFileFilter(), this);
	
	CEnString sTitle(IDS_SAVETASKLISTAS_TITLE);
	
	dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
	
	int nRes = dialog.DoModal();
	
	if (nRes == IDOK)
		SaveTaskList(nSel, dialog.GetPathName());
}

void CToDoListWnd::OnUpdateSaveas(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(tdc.GetTaskCount() || tdc.IsModified());
}

void CToDoListWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	UINT nMenuID = 0;
	
	if (pWnd == &m_tabCtrl)
	{
		// if point.x,y are both -1 then we just use the cursor pos
		// which is what windows appears to do mostly/sometimes
		if (point.x == -1 && point.y == -1)
		{
			CRect rTab;
			
			if (m_tabCtrl.GetItemRect(m_tabCtrl.GetCurSel(), rTab))
			{
				point = rTab.CenterPoint();
				m_tabCtrl.ClientToScreen(&point);
				
				// load popup menu
				nMenuID = TABCTRLCONTEXT;
			}
		}
		else
		{
			// activate clicked tab
			TCHITTESTINFO tcht = { { point.x, point.y }, TCHT_NOWHERE  };
			m_tabCtrl.ScreenToClient(&tcht.pt);
			
			int nTab = m_tabCtrl.HitTest(&tcht);
			
			if (nTab != -1 && !(tcht.flags & TCHT_NOWHERE))
			{
				if (nTab != m_tabCtrl.GetCurSel())
				{
					if (!SelectToDoCtrl(nTab, TRUE))
						return; // user cancelled
				}
				
				m_tabCtrl.SetFocus(); // give user feedback
				
				// load popup menu
				nMenuID = TABCTRLCONTEXT;
			}
		}
	}
	else if (pWnd == &GetToDoCtrl()) // try active todoctrl
	{
		if (GetToDoCtrl().PtInHeader(point))
			nMenuID = HEADERCONTEXT;
		else
			nMenuID = TASKCONTEXT;
	}
	
	// show the menu
	if (nMenuID)
	{
		CMenu menu;
		
		if (menu.LoadMenu(IDR_MISC))
		{
			CMenu* pPopup = menu.GetSubMenu(nMenuID);
			
			if (pPopup)
			{
				if (nMenuID == TASKCONTEXT)
					PrepareEditMenu(pPopup);
				
				CToolbarHelper::PrepareMenuItems(pPopup, this);
				
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
			}
		}
	}
	else
		CFrameWnd::OnContextMenu(pWnd, point);
}

void CToDoListWnd::OnTrayiconShow() 
{
	Show(FALSE);
}

void CToDoListWnd::OnTrayiconShowDueTasks(UINT nCmdID) 
{
	int nTDC = nCmdID - ID_TRAYICON_SHOWDUETASKS1;
	int nSelTDC = GetSelToDoCtrl();

	// verify password if encrypted tasklist is active
	// unless app is already visible
	if (!m_bVisible || IsIconic() || (nTDC != nSelTDC))
	{
		if (!VerifyToDoCtrlPassword(nTDC))
			return;
	}

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);

	if (!DoDueTaskNotification(&tdc, PFP_DUETODAY))
	{
		CEnString sMessage(IDS_NODUETODAY, tdc.GetFriendlyProjectName());
		MessageBox(sMessage, IDS_DUETASKS_TITLE);
	}
}

LRESULT CToDoListWnd::OnHotkey(WPARAM /*wp*/, LPARAM /*lp*/)
{
	Show(TRUE);
	return 0L;
}

BOOL CToDoListWnd::VerifyToDoCtrlPassword() const
{
	return VerifyToDoCtrlPassword(GetSelToDoCtrl());
}

BOOL CToDoListWnd::VerifyToDoCtrlPassword(int nIndex) const
{
	if (m_bPasswordPrompting)
	{
		CEnString sExplanation(IDS_SELECTENCRYPTED, 
								m_mgrToDoCtrls.GetFriendlyProjectName(nIndex));

		return GetToDoCtrl(nIndex).VerifyPassword(sExplanation);
	}
	
	// else
	return TRUE;
}

void CToDoListWnd::Show(BOOL bAllowToggle)
{
	if (!m_bVisible || !IsWindowVisible()) // restore from the tray
	{
		SetForegroundWindow();
		
		if (!VerifyToDoCtrlPassword())
			return;

		m_bVisible = TRUE;
		SetProcessWorkingSetSize(GetCurrentProcess(), 0, 0); 
		RestoreFromTray(*this, AfxGetApp()->GetProfileInt("Pos", "Maximized", FALSE));
		
		// restore find dialog
		if (m_bFindShowing)
			m_findDlg.Show();
		
		m_statusBar.Invalidate();
	}
	else if (IsIconic())
	{
		SetForegroundWindow();
		ShowWindow(SW_RESTORE); // this will force a password check
	}
	// if we're already visible then either bring to the foreground 
	// or hide if we're right at the top of the z-order
	else if (!bAllowToggle || IsObscured(*this))
		SetForegroundWindow();
	else
	{
		m_bVisible = FALSE;
		MinToTray(*this);
		SetProcessWorkingSetSize(GetCurrentProcess(), (DWORD)-1, (DWORD)-1); 
	}
	
	// refresh all tasklists if we are visible
	if (m_bVisible && !IsIconic())
	{
		const CPreferencesDlg& prefs = Prefs();
		
		if (prefs.GetReadonlyReloadOption() != RO_NO)
			OnTimerReadOnlyStatus();
		
		if (prefs.GetTimestampReloadOption() != RO_NO)
			OnTimerTimestampChange();
		
		if (prefs.GetEnableSourceControl())
			OnTimerCheckoutStatus();
	}	
}

BOOL CToDoListWnd::OnQueryEndSession() 
{
	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;
	
	SaveAll(TRUE, TRUE, TRUE, TRUE);
	SaveSettings();
	
	// simple cleanup so that encrypted tasklists do not become visible
	// inadvertently
	int nCtrl = GetTDCCount();
	
	while (nCtrl--)
	{
		CToDoCtrl* pTDC = &GetToDoCtrl(nCtrl);
		pTDC->DestroyWindow();
		delete pTDC;
	}

	DestroyWindow(); // because otherwise we would get left in an undefined state
	
	return TRUE;
}

void CToDoListWnd::OnUpdateRecentFileMenu(CCmdUI* pCmdUI) 
{
	// check that this is not occurring because our CFrameWnd
	// base class is routing this to the first item in a submenu
	if (pCmdUI->m_pMenu && 
		pCmdUI->m_pMenu->GetMenuItemID(pCmdUI->m_nIndex) == (UINT)-1)
		return;

	m_mruList.UpdateMenu(pCmdUI);	
}

BOOL CToDoListWnd::OnOpenRecentFile(UINT nID)
{
	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_mruList.GetSize());
	
	int nIndex = nID - ID_FILE_MRU_FILE1;
	CString sTaskList = m_mruList[nIndex];
	
	TDC_FILE nResult = OpenTaskList(sTaskList);
	
	if (nResult == TDCO_SUCCESS)
	{
		ResizeDlg();
		UpdateWindow();
	}
	else
	{
		HandleLoadTasklistError(nResult, sTaskList);
		
		if (nResult != TDCO_CANCELLED)
			m_mruList.Remove(nIndex);
		}
	
	// always return TRUE to say we handled it
	return TRUE;
}

void CToDoListWnd::RefreshTabOrder()
{
	if (Prefs().GetKeepTabsOrdered())
	{
		int nSelOrg = GetSelToDoCtrl();
		int nSel = m_mgrToDoCtrls.SortToDoCtrlsByName();
		
		if (nSel != nSelOrg)
			SelectToDoCtrl(nSel, FALSE);
	}
}

TDC_FILE CToDoListWnd::OpenTaskList(LPCTSTR szFilePath, BOOL bNotifyDueTasks)
{
	// see if the tasklist is already open
	if (SelectToDoCtrl(szFilePath, TRUE))
		return TDCO_SUCCESS;
	
	// init archive params?
	CString sArchivePath;
	TDC_ARCHIVE nRemove = TDC_REMOVENONE;
	CPreferencesDlg& prefs = Prefs();
	
	if (prefs.GetAutoArchive())
	{
		if (prefs.GetRemoveArchivedTasks())
		{
			if (prefs.GetRemoveOnlyOnAbsoluteCompletion())
				nRemove = TDC_REMOVEIFSIBLINGSANDSUBTASKSCOMPLETE;
			else
				nRemove = TDC_REMOVEALL;
		}
		
		sArchivePath = m_mgrToDoCtrls.GetArchivePath(szFilePath);
	}
	
	CFilteredToDoCtrl* pCtrl = NewToDoCtrl();
	
	TDC_FILE nResult = OpenTaskList(pCtrl, szFilePath, sArchivePath, nRemove);
	
	if (nResult == TDCO_SUCCESS)
	{
		int nIndex = AddToDoCtrl(pCtrl);
		
		// notify user of due tasks if req
		if (bNotifyDueTasks)
			DoDueTaskNotification(pCtrl, prefs.GetNotifyDueByOnLoad());
		
		// save checkout status
		if (prefs.GetAutoCheckOut())
			m_mgrToDoCtrls.SetLastCheckoutStatus(nIndex, pCtrl->IsCheckedOut());
		
		UpdateCaption();
		UpdateStatusbar();
		OnTimerDueItems(nIndex);
		
		// notify user if file is readonly for any reason except checked in
		if (prefs.GetNotifyReadOnly())
		{
			CString sMessage;
			
			if (CDriveInfo::IsReadonlyPath(szFilePath))
				sMessage.Format(IDS_OPENREADONLY, szFilePath);
			
			else if (!prefs.GetEnableSourceControl() && m_mgrToDoCtrls.IsSourceControlled(nIndex))
				sMessage.Format(IDS_OPENSOURCECONTROLLED, szFilePath);
			
            else if (pCtrl->CompareFileFormat() == TDCFF_NEWER)
                sMessage.Format(IDS_OPENNEWER, szFilePath);
			
			if (!sMessage.IsEmpty())
				MessageBox(sMessage, IDS_OPENTASKLIST_TITLE);
		}
		
		// update search
		if (prefs.GetRefreshFindOnLoad() && m_findDlg.GetSafeHwnd())
			m_findDlg.RefreshSearch();
	}
	else if (GetTDCCount() >= 1) // only delete if there's another ctrl existing
	{
		pCtrl->DestroyWindow();
		delete pCtrl;
	}
	else // re-add
	{
		AddToDoCtrl(pCtrl);
	}
	
	return nResult;
}

TDC_FILE CToDoListWnd::OpenTaskList(CFilteredToDoCtrl* pCtrl, LPCTSTR szFilePath, LPCTSTR szArchivePath, TDC_ARCHIVE nRemove)
{
	CPreferencesDlg& prefs = Prefs();
	CWaitCursor cursor;
	CString sFilePath(szFilePath);
	
	if (sFilePath.IsEmpty())
		sFilePath = (LPCTSTR)pCtrl->GetFilePath(); // ie. reload
	
	else // initialize source control prior to loading
	{
		BOOL bSrcControl = m_mgrToDoCtrls.PathSupportsSourceControl(szFilePath);
		
		pCtrl->SetStyle(TDCS_ENABLESOURCECONTROL, bSrcControl);
		pCtrl->SetStyle(TDCS_CHECKOUTONLOAD, bSrcControl ? prefs.GetAutoCheckOut() : FALSE);
	}
	
	TDC_FILE nResult = pCtrl->Load(sFilePath, szArchivePath, nRemove);
	
	if (nResult == TDCO_SUCCESS)
	{
		if (prefs.GetAddFilesToMRU())
			m_mruList.Add(sFilePath);

		if (prefs.GetExpandTasksOnLoad())
			pCtrl->ExpandAllTasks();
		
		// update readonly status
		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(*pCtrl);
	}
	else
		pCtrl->SetModified(FALSE);
	
	return nResult;
}

BOOL CToDoListWnd::DoDueTaskNotification(int nDueBy)
{
	return DoDueTaskNotification(&GetToDoCtrl(), nDueBy);
}

BOOL CToDoListWnd::DoDueTaskNotification(const CFilteredToDoCtrl* pCtrl, int nDueBy)
{
	// check prefs
	if (nDueBy == -1)
		return TRUE; // nothing to do
	
	ASSERT (pCtrl);
	
	if (!pCtrl)
		return FALSE;
	
	CPreferencesDlg& prefs = Prefs();
	
	// preferences
	BOOL bVisColsOnly = prefs.GetExportVisibleColsOnly();
	BOOL bParentTitleCommentsOnly = prefs.GetExportParentTitleCommentsOnly();
	BOOL bDueTaskTitlesOnly = prefs.GetDueTaskTitlesOnly();
	CString sStylesheet = prefs.GetDueTaskStylesheet();
	BOOL bTransform = (GetFileAttributes(sStylesheet) != 0xffffffff);
	BOOL bHtmlNotify = prefs.GetDisplayDueTasksInHtml();
	
	DWORD dwFlags = TDCGT_ISODATES;
	
	if (bVisColsOnly && !bTransform)
		dwFlags |= TDCGT_VISIBLECOLSONLY;

	if (bParentTitleCommentsOnly)
		dwFlags |= TDCGT_PARENTTITLECOMMENTSONLY;

	if (bHtmlNotify)
		dwFlags |= TDCGT_HTMLCOMMENTS;

	if (bDueTaskTitlesOnly)
		dwFlags |= TDCGT_TITLESONLY;

	TDC_FILTER nFilter = TDCF_DUE;
	UINT nIDDueBy = IDS_DUETODAY;
	
	switch (nDueBy)
	{
	case PFP_DUETODAY:
		break; // done
		
	case PFP_DUETOMORROW:
		nIDDueBy = IDS_DUETOMORROW;
		nFilter = TDCF_DUETOMORROW;
		break;
		
	case PFP_DUETHISWEEK:
		nIDDueBy = IDS_DUETHISWEEK;
		nFilter = TDCF_DUETHISWEEK;
		break;
		
	case PFP_DUENEXTWEEK:
		nIDDueBy = IDS_DUENEXTWEEK;
		nFilter = TDCF_DUENEXTWEEK;
		break;
		
	case PFP_DUETHISMONTH:
		nIDDueBy = IDS_DUETHISMONTH;
		nFilter = TDCF_DUETHISMONTH;
		break;
		
	case PFP_DUENEXTMONTH:
		nIDDueBy = IDS_DUENEXTMONTH;
		nFilter = TDCF_DUENEXTMONTH;
		break;
		
	default:
		ASSERT (0);
		return FALSE;
	}
	
	CTaskFile tasks;
	
	if (!pCtrl->GetTasks(tasks, nFilter, dwFlags, prefs.GetDueTaskPerson()))
		return FALSE;
	
	// set an appropriate title
	CEnString sTitle(nIDDueBy, pCtrl->GetFriendlyProjectName());

	tasks.SetReportAttributes(sTitle);
	tasks.SetCharSet(prefs.GetHtmlCharSet());

	// nasty hack to prevent exporters adding space for notes
	BOOL bSpaceForNotes = prefs.GetExportSpaceForNotes();

	if (bSpaceForNotes)
		AfxGetApp()->WriteProfileInt("Preferences", "ExportSpaceForNotes", FALSE);
	
	// always use the same file
	CString sTempFile = FileMisc::GetTempFileName("ToDoList.due", bHtmlNotify ? "html" : "txt");
			
	BOOL bRes = FALSE;
	
	if (bHtmlNotify) // display in browser?
	{
		if (bTransform)
			bRes = tasks.TransformToFile(sStylesheet, sTempFile, prefs.GetHtmlCharSet());
		else
			bRes = m_mgrImportExport.ExportTaskListToHtml(&tasks, sTempFile);
	}
	else
		bRes = m_mgrImportExport.ExportTaskListToText(&tasks, sTempFile);

	if (bRes)
	{
		CBrowserDlg dialog(bHtmlNotify);
		dialog.DoModal(CEnString(IDS_DUETASKS_TITLE), sTempFile, this);
	}

	// undo hack
	if (bSpaceForNotes)
		AfxGetApp()->WriteProfileInt("Preferences", "ExportSpaceForNotes", TRUE);
	
	return TRUE;
}

void CToDoListWnd::EnsureVisible() 
{
	// make sure app window is visible
	if (!IsWindowVisible())
	{
		m_bVisible = TRUE;
		ShowWindow(SW_SHOWNORMAL);
	}
}

void CToDoListWnd::OnAbout() 
{
	CAboutDlg dialog(IDR_MAINFRAME, ABS_EDITCOPYRIGHT, "<b>ToDoList 5.1.5</b>",
		CEnString(IDS_ABOUTHEADING), CEnString(IDS_ABOUTCOPYRIGHT), 1, 2, 8);
	
	dialog.DoModal();
}

void CToDoListWnd::OnPreferences() 
{
	DoPreferences();
}

void CToDoListWnd::DoPreferences(int nInitPage) 
{
	// take a copy of current prefs to check changes against
	CPreferencesDlg curPrefs; 
	
	// kill timers
	SetTimer(TIMER_READONLYSTATUS, FALSE);
	SetTimer(TIMER_TIMESTAMPCHANGE, FALSE);
	SetTimer(TIMER_CHECKOUTSTATUS, FALSE);
	SetTimer(TIMER_AUTOSAVE, FALSE);
	SetTimer(TIMER_TIMETRACKING, FALSE);
	
	UINT nRet = Prefs().DoModal(nInitPage);
	
	// updates prefs
	ResetPrefs();
	
	if (nRet == IDOK)
	{
		const CPreferencesDlg& prefs = Prefs(); // new prefs
		
		// mark all todoctrls as needing refreshing
		m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE); 
		
		// delete fonts if they appear to have changed
		// and recreate in UpdateToDoCtrlPrefs
		CString sFaceName;
		int nFontSize;
		
		if (!prefs.GetTreeFont(sFaceName, nFontSize) || !Misc::SameFont(m_fontTree, sFaceName, nFontSize))
			m_fontTree.DeleteObject();
		
		if (!prefs.GetCommentsFont(sFaceName, nFontSize) || !Misc::SameFont(m_fontComments, sFaceName, nFontSize))
			m_fontComments.DeleteObject();
		
		BOOL bResizeDlg = FALSE;
		
		// topmost
		BOOL bTopMost = (prefs.GetAlwaysOnTop() && !IsZoomed());
		
		SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		
		// tray icon
		m_ti.ShowTrayIcon(prefs.GetUseSysTray());
		
		// support for .tdl
		if (curPrefs.GetEnableTDLExtension() != prefs.GetEnableTDLExtension())
		{
			CFileRegister filereg("tdl", "tdl_Tasklist");
			
			if (prefs.GetEnableTDLExtension())
				filereg.RegisterFileType("Tasklist", 0);
			else
				filereg.UnRegisterFileType();
		}
		
		// source control
		BOOL bSourceControl = prefs.GetEnableSourceControl();
		
		if (curPrefs.GetEnableSourceControl() != bSourceControl ||
			curPrefs.GetSourceControlLanOnly() != prefs.GetSourceControlLanOnly())
		{
			// update all open files to ensure they're in the right state
			int nCtrl = GetTDCCount();
			
			while (nCtrl--)
			{
				CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
				
				// check files in if we're disabling sc and this file is
				// checked out. however although we 
				// are checking in, the file cannot be edited by the user
				// until they remove the file from under source control
				if (!bSourceControl && tdc.IsCheckedOut())
				{
					if (tdc.IsModified())
						tdc.Save(NULL, FALSE);
					
					tdc.CheckIn();
				}
				// else checkout if we're enabling and auto-checkout
				// is also enabled
				else if (bSourceControl)
				{
					// there can be two reasons for wanting to check out a file
					// either the autocheckout preference is set or its a local
					// file which is not checked out but has been modified and source
					// control now covers all files in which case we save it first
					BOOL bPathSupports = m_mgrToDoCtrls.PathSupportsSourceControl(nCtrl);
					BOOL bNeedsSave = bPathSupports && !tdc.IsCheckedOut() && tdc.IsModified();
					BOOL bWantCheckOut = bNeedsSave || (bPathSupports && prefs.GetAutoCheckOut());
					
					if (bNeedsSave)
						tdc.Save(NULL, FALSE); // save silently
					
					tdc.SetStyle(TDCS_ENABLESOURCECONTROL, bPathSupports);
					tdc.SetStyle(TDCS_CHECKOUTONLOAD, bPathSupports && prefs.GetAutoCheckOut());
					
					if (bWantCheckOut && !tdc.IsCheckedOut())
						tdc.CheckOut();
				}
				
				// re-sync
				m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);				
				m_mgrToDoCtrls.RefreshModifiedStatus(nCtrl);
				m_mgrToDoCtrls.RefreshLastModified(nCtrl);
				m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
			}
		}
		
		m_toolbar.GetToolBarCtrl().HideButton(ID_TOOLS_TOGGLECHECKIN, !bSourceControl);
		
		// toolbar image
		if (prefs.GetLargeToolbarIcons() != curPrefs.GetLargeToolbarIcons())
		{
			UINT nIDToolbarImage = prefs.GetLargeToolbarIcons() ? IDB_TOOLBAR24 : IDB_TOOLBAR16;
			
			m_toolbar.LoadToolBar(IDR_TOOLBAR, nIDToolbarImage);
			bResizeDlg = TRUE;
		}

		
		// priority colors
		prefs.GetPriorityColors(m_aPriorityColors);
		m_findDlg.SetPriorityColors(m_aPriorityColors);

		// reload menu 
		LoadMenubar();
		
		// tab bar
		bResizeDlg |= (curPrefs.GetAutoHideTabbar() != prefs.GetAutoHideTabbar());
		
		if (curPrefs.GetStackTabbarItems() != prefs.GetStackTabbarItems())
		{
			BOOL bStackTabbar = prefs.GetStackTabbarItems();
			
			bResizeDlg = TRUE;
			m_tabCtrl.ModifyStyle(bStackTabbar ? 0 : TCS_MULTILINE, bStackTabbar ? TCS_MULTILINE : 0);
		}
		else
			m_tabCtrl.Invalidate(); // handle priority colour changes
		
		// filtering
		RefreshFilterLabelAlignment();
		
		// and visible filter controls
		if (m_bShowFilterBar)
			bResizeDlg = TRUE;

		BOOL bMultiSelCat = prefs.GetMultiSelCategoryFilter();

		if (curPrefs.GetMultiSelCategoryFilter() != bMultiSelCat)
		{
			m_filterBar.EnableMultiCategorySelection(bMultiSelCat);

			// if it was was previously multisel (but not now) then
			// refresh the filter because we may have gone from
			// multiple categories down to only one
			if (!bMultiSelCat)
				OnViewRefreshfilter();
		}
		
		// hotkey
		UpdateGlobalHotkey();
		
		// time periods
		CTimeHelper::SetHoursInOneDay(prefs.GetHoursInOneDay());
		CTimeHelper::SetDaysInOneWeek(prefs.GetDaysInOneWeek());
		
		RefreshTabOrder();
		
		// time tracking
		if (curPrefs.GetTrackActiveTasklistOnly() != prefs.GetTrackActiveTasklistOnly())
			PauseTimeTracking(FALSE);
		
		UpdateCaption();
		UpdateTabSwitchTooltip();
		
		// active tasklist prefs
		UpdateToDoCtrlPreferences();
		
		if (bResizeDlg)
			ResizeDlg();
		
		// don't ask me for the full details on this but it seems as
		// though the CSysImageList class is waiting to process a 
		// message before we can switch image sizes so we put it
		// right at the end after everything is done.
		Misc::ProcessMsgLoop();
		AppendTools2Toolbar(TRUE);
	}
	
	// finally set or terminate the various status check timers
	SetTimer(TIMER_READONLYSTATUS, Prefs().GetReadonlyReloadOption() != RO_NO);
	SetTimer(TIMER_TIMESTAMPCHANGE, Prefs().GetTimestampReloadOption() != RO_NO);
	SetTimer(TIMER_AUTOSAVE, Prefs().GetAutoSaveFrequency());
	SetTimer(TIMER_CHECKOUTSTATUS, Prefs().GetCheckoutOnCheckin() || Prefs().GetCheckinOnNoEditTime());
	SetTimer(TIMER_TIMETRACKING, Prefs().GetUseSysTray());
}

BOOL CToDoListWnd::LoadMenubar()
{
	m_menubar.DestroyMenu();
	
	if (!m_menubar.LoadMenu(IDR_MAINFRAME))
		return FALSE;

	SetMenu(&m_menubar);
	m_hMenuDefault = m_menubar;
	
#ifdef _DEBUG
	// add menu option to simulate WM_QUERYENDSESSION
	m_menubar.InsertMenu((UINT)-1, MFT_STRING, ID_DEBUGQUERYENDSESSION, "QueryEndSession");
#endif

	if (Prefs().GetShowTasklistCloseButton()) 
		m_menubar.AddMDIButton(MEB_CLOSE, ID_CLOSE, (UINT)-1, TRUE);
	
	DrawMenuBar();

	return TRUE;
}

void CToDoListWnd::UpdateGlobalHotkey()
{
	static DWORD dwPrevHotkey = 0;
	DWORD dwHotkey = Prefs().GetGlobalHotkey();
	
	if (dwPrevHotkey == dwHotkey)
		return;
	
	// map modifiers to those wanted by RegisterHotKey
	DWORD dwPrefMods = HIWORD(dwHotkey);
	DWORD dwVKey = LOWORD(dwHotkey);
	
	DWORD dwMods = (dwPrefMods & HOTKEYF_ALT) ? MOD_ALT : 0;
	dwMods |= (dwPrefMods & HOTKEYF_CONTROL) ? MOD_CONTROL : 0;
	dwMods |= (dwPrefMods & HOTKEYF_SHIFT) ? MOD_SHIFT : 0;
	
	RegisterHotKey(*this, 1, dwMods, dwVKey);
}

void CToDoListWnd::PauseTimeTracking(BOOL bPause)
{
	// time tracking
	int nCtrl = GetTDCCount();
	int nSel = GetSelToDoCtrl();
	BOOL bTrackActiveOnly = Prefs().GetTrackActiveTasklistOnly();
	
	while (nCtrl--)
	{
		BOOL bSel = (nCtrl == nSel);
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		
		tdc.PauseTimeTracking(bPause ? TRUE : (bSel ? FALSE : bTrackActiveOnly));
	}
}

void CToDoListWnd::CheckMinWidth()
{
	if (GetTDCCount())
	{
		CRect rTDC;
		GetToDoCtrl().GetClientRect(rTDC);
		int nMinWidth = GetToDoCtrl().GetMinWidth();
		
		if (rTDC.Width() < nMinWidth)
		{
			CRect rWindow;
			GetWindowRect(rWindow);
			
			SetWindowPos(NULL, 0, 0, rWindow.Width() - 1, rWindow.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}

BOOL CToDoListWnd::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	switch (pCopyDataStruct->dwData)
	{
	case OPENTASKLIST:
		{
			LPCTSTR szFilePath = (LPCTSTR)pCopyDataStruct->lpData;
			OpenTaskList(szFilePath);
		}
		break;
		
	case ADDNEWTASK:
		{
			LPCTSTR szTitle = (LPCTSTR)pCopyDataStruct->lpData;
			
			if (szTitle && *szTitle)
			{
				// do we need to cache this ?
				if (!m_startupOptions.sFilePath.IsEmpty())
					m_startupOptions.sNewTask = szTitle;
				else
					NewTask(szTitle, TDC_INSERTATTOP, TRUE, FALSE);
			}
			else
			{
				NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP, TRUE, FALSE);
				
				// must post this else edit gets cancelled by
				// changes in activation
				PostMessage(WM_COMMAND, ID_EDIT_TASKTEXT);
			}
		}
		break;
		
	case SETCOMMENTS:
		{
			LPCTSTR szComments = (LPCTSTR)pCopyDataStruct->lpData;
			
			if (szComments && *szComments)
			{
				// do we need to cache this ?
				if (!m_startupOptions.sFilePath.IsEmpty())
					m_startupOptions.sComments = szComments;
				else
				{
					CToDoCtrl& tdc = GetToDoCtrl();
					
					// comments can only be applied to singly selected tasks
					if (tdc.GetSelectedCount() == 1)
						tdc.SetSelectedTaskComments(szComments, szComments);
				}
			}
		}
		break;

	case SELECTTASK:
		if (pCopyDataStruct->lpData)
		{
			DWORD dwID = *((DWORD*)pCopyDataStruct->lpData);

			// do we need to cache this ?
			if (!m_startupOptions.sFilePath.IsEmpty())
				m_startupOptions.dwIDSel = dwID;
			else
				GetToDoCtrl().SelectTask(dwID);
		}
		break;
	}
	
	return CFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

void CToDoListWnd::OnEditCopy() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	tdc.Flush();
	tdc.CopySelectedTask();
}

void CToDoListWnd::OnEditCut() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	tdc.Flush();
	tdc.CutSelectedTask();
}

void CToDoListWnd::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount);	
}

void CToDoListWnd::OnEditPasteSub() 
{
	GetToDoCtrl().PasteOnSelectedTask();
}

void CToDoListWnd::OnUpdateEditPasteSub(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	// modify the text appropriately if the tasklist is empty
	if (nSelCount == 0)
		pCmdUI->SetText(CEnString(IDS_PASTETOPLEVELTASK));
	
	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.CanPaste());	
}

void CToDoListWnd::OnEditPasteAfter() 
{
	GetToDoCtrl().PasteAfterSelectedTask();
}

void CToDoListWnd::OnUpdateEditPasteAfter(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.CanPaste() && nSelCount == 1);	
}

void CToDoListWnd::OnEditCopyastext() 
{
	CopySelectedTasksToClipboard(CT_ASTEXT);
}

void CToDoListWnd::OnEditCopyashtml() 
{
	CopySelectedTasksToClipboard(CT_ASHTML);
}

void CToDoListWnd::CopySelectedTasksToClipboard(int nAsFormat)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.Flush();
	
	CTaskFile tasks;
	BOOL bVisColsOnly = Prefs().GetExportVisibleColsOnly();
	BOOL bParentTitleCommentsOnly = Prefs().GetExportParentTitleCommentsOnly();
	DWORD dwFlags = (bVisColsOnly ? TDCGT_VISIBLECOLSONLY : 0) | TDCGT_ISODATES |
		(bParentTitleCommentsOnly ? TDCGT_PARENTTITLECOMMENTSONLY : 0);
	
	tdc.GetSelectedTasks(tasks, dwFlags);
	CString sTasks;
	
	switch (nAsFormat)
	{	
	case CT_ASHTML:
		sTasks = m_mgrImportExport.ExportTaskListToHtml(&tasks);
		break;
		
	case CT_ASTEXT:
		sTasks = m_mgrImportExport.ExportTaskListToText(&tasks);
		break;
		
	case CT_ASREF:
		sTasks.Format("tdl://%ld", tdc.GetSelectedTaskID());
		break;
		
	case CT_ASDEPENDS:
		sTasks.Format("%ld", tdc.GetSelectedTaskID());
		break;
	}

	Misc::CopyTexttoClipboard(sTasks, *this);
}

void CToDoListWnd::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() > 0);
}

void CToDoListWnd::OnUpdateNewtaskAttopSelected(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	// enable this when the tasklist is empty even if no 
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewtaskAtbottomSelected(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	// enable this when the tasklist is empty even if no 
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewtaskAfterselectedtask(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	// enable this when the tasklist is empty even if no 
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewtaskBeforeselectedtask(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	// enable this when the tasklist is empty even if no 
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewsubtaskAttop(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnSimplemode() 
{
	m_bMaxTasklists = !m_bMaxTasklists;
	
	// mark all tasklists as needing update
	m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);
	
	// then update active tasklist
	GetToDoCtrl().SetStyle(TDCS_MAXIMIZE, m_bMaxTasklists);
	m_mgrToDoCtrls.SetNeedsPreferenceUpdate(GetSelToDoCtrl(), FALSE);
}

void CToDoListWnd::OnUpdateSimplemode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bMaxTasklists ? 1 : 0);
}

void CToDoListWnd::OnReload() 
{
	int nSel = GetSelToDoCtrl();
	
	if (m_mgrToDoCtrls.GetModifiedStatus(nSel))
	{ 
		if (IDNO == MessageBox(IDS_CONFIRMRELOAD, IDS_CONFIRMRELOAD_TITLE, MB_YESNO | MB_DEFBUTTON2))
		{
			return;
		}
	}
	
	// else reload
	ReloadTaskList(nSel, TRUE);
	RefreshTabOrder();
}

void CToDoListWnd::ReloadTaskList(int nIndex, BOOL bNotifyDueTasks)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	
	TDC_FILE nRes = OpenTaskList(&tdc);
	
	if (nRes == TDCO_SUCCESS)
	{
		CPreferencesDlg& prefs = Prefs();
		
		// update file status
		if (prefs.GetAutoCheckOut())
			m_mgrToDoCtrls.SetLastCheckoutStatus(nIndex, tdc.IsCheckedOut());
		
		m_mgrToDoCtrls.RefreshLastModified(nIndex);
		m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
		m_mgrToDoCtrls.UpdateTabItemText(nIndex);
		
		// notify user of due tasks if req
		if (bNotifyDueTasks)
			DoDueTaskNotification(&tdc, prefs.GetNotifyDueByOnLoad());
		
		UpdateCaption();
		UpdateStatusbar();
	}
	else
		HandleLoadTasklistError(nRes, tdc.GetFilePath());
}

void CToDoListWnd::OnUpdateReload(CCmdUI* pCmdUI) 
{
	int nSel = GetSelToDoCtrl();
	
	pCmdUI->Enable(!m_mgrToDoCtrls.GetFilePath(nSel).IsEmpty());
}

void CToDoListWnd::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	BOOL bMaximized = (nType == SIZE_MAXIMIZED);
	
	if (nType != SIZE_MINIMIZED)
		ResizeDlg(cx, cy, bMaximized);
	
	m_statusBar.EnableGripper(!bMaximized);
	
	// if not maximized then set topmost if that's the preference
	BOOL bTopMost = (Prefs().GetAlwaysOnTop() && !bMaximized) ? 1 : 0;
	
	// do nothing if no change
	BOOL bIsTopMost = (GetExStyle() & WS_EX_TOPMOST) ? 1 : 0;
	
	if (bTopMost != bIsTopMost)
		SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

BOOL CToDoListWnd::CalcToDoCtrlRect(CRect& rect, int cx, int cy, BOOL bMaximized)
{
	if (!cx && !cy)
	{
		CRect rClient;
		GetClientRect(rClient);
		
		cx = rClient.right;
		cy = rClient.bottom;
		bMaximized = IsZoomed();
		
		// check again 
		if (!cx && !cy)
			return FALSE;
	}
	
	CRect rTaskList(0, BEVEL, cx - BEVEL, cy);
	
	// toolbar

	if (m_bShowToolbar) // showing toolbar
	{
		int nTBHeight = m_toolbar.CalcDynamicLayout(cx, LM_HORZ | LM_STRETCH).cy;
		rTaskList.top += nTBHeight;
	}
	
	// resize tabctrl
	CDeferWndMove dwm(0); // dummy
	
	CPoint ptOrg(0, rTaskList.top);
	int nTabHeight = ReposTabBar(dwm, ptOrg, cx, TRUE);
	
	if (nTabHeight)
		rTaskList.top += nTabHeight + 1; // hide the bottom of the tab ctrl
	
	// filter controls
	int nFilterHeight = m_bShowFilterBar ? m_filterBar.CalcHeight(cx) : 0;
	
	if (nFilterHeight)
		rTaskList.top += nFilterHeight + 2;
	
	// statusbar
	CRect rStatus = OffsetCtrl(IDC_FILENAME);
	rStatus.OffsetRect(0, cy - rStatus.bottom);
	rStatus.right = cx;
	
	// finally the active todoctrl
	CDlgUnits dlu(this);
	
	if (m_bShowStatusBar)
		rTaskList.bottom = cy - dlu.ToPixelsY(SB_HEIGHT) - 1;
	else
		rTaskList.bottom = cy;
	
	// shrink slightly so that edit controls do not merge with window border
	int nInset = (CThemed().IsNonClientThemed() ? BORDER : BEVEL);
	rTaskList.left += nInset;
	
	rect = rTaskList;
	
	return TRUE;
}

void CToDoListWnd::ResizeDlg(int cx, int cy, BOOL bMaximized)
{
	static int nLastCx = 0, nLastCy = 0;

	if (!cx && !cy)
	{
		CRect rClient;
		GetClientRect(rClient);
		
		cx = rClient.right;
		cy = rClient.bottom;
		bMaximized = IsZoomed();
		
		// check again 
		if (!cx && !cy)
			return;
	}

	if (cx == nLastCx && cy == nLastCy && !GetTDCCount())
		return;

	nLastCx = cx;
	nLastCy = cy;
	
	// resize in one go
	CDlgUnits dlu(*this);
	CDeferWndMove dwm(6);
	CRect rTaskList(0, BEVEL, cx - BEVEL, cy);
	
	// toolbar
	if (m_bShowToolbar) // showing toolbar
	{
		int nTBHeight = m_toolbar.CalcDynamicLayout(cx, LM_HORZ | LM_STRETCH).cy;
		rTaskList.top += nTBHeight;
		m_toolbar.MoveWindow(0, 0, cx, nTBHeight);
	}
	
	// resize tabctrl
	CPoint ptOrg(0, rTaskList.top);
	int nTabHeight = ReposTabBar(dwm, ptOrg, cx);
	
	if (nTabHeight)
		rTaskList.top += nTabHeight + 1; // hide the bottom of the tab ctrl
	
	// filter controls
	int nFilterHeight = m_bShowFilterBar ? m_filterBar.CalcHeight(cx) : 0;
	int nInset = (CThemed().IsNonClientThemed() ? BORDER : BEVEL);
	
	dwm.MoveWindow(&m_filterBar, nInset, rTaskList.top + 4, cx - 2 * nInset, nFilterHeight);
	
	if (nFilterHeight)
		rTaskList.top += nFilterHeight + 2;
	
	// resize filepath static
	int nSBHeight = dlu.ToPixelsY(SB_HEIGHT);
	CRect rStatus(1, cy - nSBHeight, cx - 1, cy);
	
	dwm.MoveWindow(&m_statusBar, rStatus);
	
	// finally the active todoctrl
	if (GetTDCCount())
	{
		if (m_bShowStatusBar)
			rTaskList.bottom = rStatus.top - 1;
		else
			rTaskList.bottom = rStatus.bottom;
		
		// shrink slightly so that edit controls do not merge with window border
		rTaskList.left = nInset;

		CRect rCalc;
		CalcToDoCtrlRect(rCalc, cx, cy, IsZoomed());

		ASSERT (rCalc == rTaskList);
		
		dwm.MoveWindow(&GetToDoCtrl(), rTaskList);


	}
}

int CToDoListWnd::ReposTabBar(CDeferWndMove& dwm, const CPoint& ptOrg, int nWidth, BOOL bCalcOnly)
{
	CRect rTabs(0, 0, nWidth, 0);
	m_tabCtrl.AdjustRect(TRUE, rTabs);
	int nTabHeight = rTabs.Height() - 5;
	
	rTabs = dwm.OffsetCtrl(this, IDC_TABCONTROL); // not actually a move
	rTabs.right = nWidth + 1;
	rTabs.bottom = rTabs.top + nTabHeight;
	rTabs.OffsetRect(0, ptOrg.y - rTabs.top);
	
	BOOL bNeedTabCtrl = (GetTDCCount() > 1 || !Prefs().GetAutoHideTabbar());
	
	if (!bCalcOnly)
	{
		dwm.MoveWindow(&m_tabCtrl, rTabs);
		
		// hide and disable tabctrl if not needed
		m_tabCtrl.ShowWindow(bNeedTabCtrl ? SW_SHOW : SW_HIDE);
		m_tabCtrl.EnableWindow(bNeedTabCtrl);
	}
	
	return bNeedTabCtrl ? rTabs.Height() : 0;
}

CRect CToDoListWnd::OffsetCtrl(UINT uCtrlID, int cx, int cy)
{
	CWnd* pCtrl = GetDlgItem(uCtrlID);
	
	if (pCtrl)
	{
		CRect rChild;
		pCtrl->GetWindowRect(rChild);
		ScreenToClient(rChild);
		
		if (cx || cy)
		{
			rChild.OffsetRect(cx, cy);
			pCtrl->MoveWindow(rChild);
		}
		return rChild;
	}
	
	return CRect(0, 0, 0, 0);
}

void CToDoListWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
	
	// don't handle if maximized or minimized
	if (!IsZoomed() && !IsIconic())
	{
		int nMinWidth = 352;
		
		if (GetTDCCount()) // ie there is at least CToDoCtrl
		{
			CRect rClient, rWindow;
			
			GetClientRect(rClient);
			GetWindowRect(rWindow);
			
			// odd 'bug'. when the window is being restored the client
			// rect is NULL and this generates an invalid min size
			if (!rClient.IsRectNull())
				nMinWidth = GetToDoCtrl().GetMinWidth() + (rWindow.Width() - rClient.Width());
		}
		
		lpMMI->ptMinTrackSize.x = nMinWidth; // so caption and toolbar is fully visible
		lpMMI->ptMinTrackSize.y = 460; // arbitrary
	}
}

void CToDoListWnd::OnPrint() 
{
	DoPrint();
}

void CToDoListWnd::DoPrint(BOOL bPreview)
{
	int nSelTDC = GetSelToDoCtrl();

	// pass the project name as the title field
	CString sDate, sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nSelTDC);

	// export to html and then print in IE
	CTDLPrintDialog dialog(sTitle, bPreview);
	
	if (dialog.DoModal() != IDOK)
		return;
	
	// always use the same file
	CString sTempFile = FileMisc::GetTempFileName("ToDoList.print", "html");
	
	BOOL bIncDone = dialog.GetWantCompletedTasks();
	BOOL bIncNotDone = dialog.GetWantInCompleteTasks();
	BOOL bSelTaskOnly = dialog.GetWantSelectedTasks();
	BOOL bSelSubtasks = dialog.GetWantSelectedSubtasks();
	BOOL bDate = dialog.GetWantDate();

	// stylesheets don't seem to like the way we do html comments
	CString sStyleSheet = dialog.GetStylesheet();
	BOOL bTransform = (GetFileAttributes(sStyleSheet) != 0xffffffff);

	sTitle = dialog.GetTitle();
	
	// export
	CWaitCursor cursor;
	CTaskFile tasks;
	
	GetTasks(nSelTDC, bIncDone, bIncNotDone, bSelTaskOnly, 
			bSelSubtasks, TRUE, bTransform, tasks);

	// add title and date 
	if (bDate)
		sDate = COleDateTime::GetCurrentTime().Format(VAR_DATEVALUEONLY);

	tasks.SetReportAttributes(sTitle, sDate);
	
	if (!Export2Html(tasks, sTempFile, dialog.GetStylesheet()))
		return;
	
	// print from browser
	CRect rHidden(-20, -20, -10, -10); // create IE off screen
	
	if (m_IE.GetSafeHwnd() || m_IE.Create(NULL, WS_CHILD | WS_VISIBLE, rHidden, this, (UINT)IDC_STATIC))
	{
		if (bPreview)
			m_IE.PrintPreview(sTempFile);
		else
			m_IE.Print(sTempFile);
	}
	else // try sending to browser
	{
		int nRes = (int)ShellExecute(*this, bPreview ? "print" : NULL, sTempFile, NULL, NULL, SW_HIDE);
								
		if (nRes < 32)
			MessageBox(IDS_PRINTFAILED, IDS_PRINTFAILED_TITLE, MB_OK);
	}
}

void CToDoListWnd::OnUpdatePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}

int CToDoListWnd::AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bResizeDlg)
{
	// add tdc first to ensure tab controls has some
	// items before we query it for its size
	int nSel = m_mgrToDoCtrls.AddToDoCtrl(pCtrl);
	
	// make sure size is right
	CRect rTDC;
	
	if (CalcToDoCtrlRect(rTDC))
		pCtrl->MoveWindow(rTDC);
	
	SelectToDoCtrl(nSel, FALSE);
	pCtrl->SetFocusToTree();
	
	// make sure the tab control is correctly sized
	if (bResizeDlg)
		ResizeDlg();
	
	// if this is the only control then set or terminate the various status 
	// check timers
	if (GetTDCCount() == 1)
	{
		CPreferencesDlg& prefs = Prefs();
		
		SetTimer(TIMER_READONLYSTATUS, prefs.GetReadonlyReloadOption() != RO_NO);
		SetTimer(TIMER_TIMESTAMPCHANGE, prefs.GetTimestampReloadOption() != RO_NO);
		SetTimer(TIMER_AUTOSAVE, prefs.GetAutoSaveFrequency());
		SetTimer(TIMER_CHECKOUTSTATUS, prefs.GetCheckoutOnCheckin() ||
			prefs.GetCheckinOnNoEditTime());
	}
	
	// update filter controls
	RefreshFilterControls();

	// make sure everything looks okay
	Invalidate();
	UpdateWindow();
	
	return nSel;
}

void CToDoListWnd::SetTimer(UINT nTimerID, BOOL bOn)
{
	if (bOn)
	{
		UINT nPeriod = 0;
		
		switch (nTimerID)
		{
		case TIMER_READONLYSTATUS:
			nPeriod = INTERVAL_READONLYSTATUS;
			break;
			
		case TIMER_TIMESTAMPCHANGE:
			nPeriod = INTERVAL_TIMESTAMPCHANGE;
			break;
			
		case TIMER_AUTOSAVE:
			nPeriod = Prefs().GetAutoSaveFrequency() * 60000;
			break;
			
		case TIMER_CHECKOUTSTATUS:
			nPeriod = INTERVAL_CHECKOUTSTATUS;
			break;
			
		case TIMER_DUEITEMS:
			nPeriod = INTERVAL_DUEITEMS;
			break;
			
		case TIMER_TIMETRACKING:
			nPeriod = INTERVAL_TIMETRACKING;
			break;
		}
		
		if (nPeriod)
		{
			UINT nID = CFrameWnd::SetTimer(nTimerID, nPeriod, NULL);
			ASSERT (nID);
		}
	}
	else
		KillTimer(nTimerID);
}

void CToDoListWnd::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);
	
	// if we are disabled (== modal dialog visible) then do not respond
	if (!IsWindowEnabled())
		return;
	
	// don't check whilst in the middle of saving or closing
	if (m_bSaving || m_bClosing)
		return;
	
	// if no controls are active kill the timers
	if (!GetTDCCount())
	{
		SetTimer(TIMER_READONLYSTATUS, FALSE);
		SetTimer(TIMER_TIMESTAMPCHANGE, FALSE);
		SetTimer(TIMER_AUTOSAVE, FALSE);
		SetTimer(TIMER_CHECKOUTSTATUS, FALSE);
		return;
	}
	
	switch (nIDEvent)
	{
	case TIMER_READONLYSTATUS:
		OnTimerReadOnlyStatus();
		break;
		
	case TIMER_TIMESTAMPCHANGE:
		OnTimerTimestampChange();
		break;
		
	case TIMER_AUTOSAVE:
		OnTimerAutoSave();
		break;
		
	case TIMER_CHECKOUTSTATUS:
		OnTimerCheckoutStatus();
		break;
		
	case TIMER_DUEITEMS:
		OnTimerDueItems();
		break;
		
	case TIMER_TIMETRACKING:
		OnTimerTimeTracking();
		break;
	}
}

void CToDoListWnd::OnTimerTimeTracking()
{
	AF_NOREENTRANT // macro helper
		
	static BOOL bWasTimeTracking = FALSE;
	BOOL bNowTimeTracking = FALSE;
	
	// cycle thru tasklists until we find a time tracker
	int nCtrls = GetTDCCount();
	
	for (int nCtrl = 0; nCtrl < nCtrls && !bNowTimeTracking; nCtrl++)
	{
		const CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		
		if (tdc.IsActivelyTimeTracking())
			bNowTimeTracking = TRUE;
	}
	
	if (bWasTimeTracking != bNowTimeTracking)
	{
		UINT nIDTrayIcon = 0;
		
		if (bNowTimeTracking)
			nIDTrayIcon = (COSVersion() >= OSV_XP) ? IDI_TRAYICONXPTRACK : IDI_TRAYICONTRACK;
		else
			nIDTrayIcon = (COSVersion() >= OSV_XP) ? IDI_TRAYICONXP : IDI_TRAYICON;
		
		m_ti.SetIcon(nIDTrayIcon);
	}
	
	bWasTimeTracking = bNowTimeTracking;
}

void CToDoListWnd::OnTimerDueItems(int nCtrl)
{
	AF_NOREENTRANT // macro helper
		
	SEARCHPARAMS sp;
	SEARCHRESULT result;
	
	sp.dwFlags = 0;
	sp.nFindWhat = FIND_DUEDATE;
	sp.dateTo = COleDateTime::GetCurrentTime();
	
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;
	BOOL bRepaint = FALSE;
	
	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		
		BOOL bDueItems = tdc.FindFirstTask(sp, result);
		
		if (bDueItems != m_mgrToDoCtrls.GetDueItemStatus(nCtrl))
		{
			m_mgrToDoCtrls.SetDueItemStatus(nCtrl, bDueItems);
			bRepaint = TRUE;
		}
	}

	if (bRepaint)
		m_tabCtrl.Invalidate(FALSE);
}

void CToDoListWnd::OnTimerReadOnlyStatus(int nCtrl)
{
	AF_NOREENTRANT // macro helper
		
		CPreferencesDlg& prefs = Prefs();
	
	// work out whether we should check remote files or not
	BOOL bCheckRemoteFiles = (nCtrl != -1);
	
	if (!bCheckRemoteFiles)
	{
		static int nElapsed = 0;
		UINT nRemoteFileCheckInterval = prefs.GetRemoteFileCheckFrequency() * 1000; // in ms
		
		nElapsed %= nRemoteFileCheckInterval;
		bCheckRemoteFiles = !nElapsed;
		
		nElapsed += INTERVAL_READONLYSTATUS;
	}
	
	int nReloadOption = prefs.GetReadonlyReloadOption();
	
	ASSERT (nReloadOption != RO_NO);
	
	// process files
	CString sFileList;
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;
	
	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		
		// don't check removeable drives
		int nType = m_mgrToDoCtrls.GetFilePathType(nCtrl);
		
        if (nType == TDCM_UNDEF || nType == TDCM_REMOVABLE)
			continue;
		
		// check remote files?
		if (!bCheckRemoteFiles && nType == TDCM_REMOTE)
			continue;
		
		CString sFilePath = tdc.GetFilePath();
		
		if (m_mgrToDoCtrls.RefreshReadOnlyStatus(nCtrl))
		{
			BOOL bReadOnly = m_mgrToDoCtrls.GetReadOnlyStatus(nCtrl);
			BOOL bReload = FALSE;
			
			if (nReloadOption == RO_ASK)
			{
				CEnString sMessage(bReadOnly ? IDS_WRITABLETOREADONLY : IDS_READONLYTOWRITABLE, sFilePath);
				
				if (!bReadOnly) // might been modified
					sMessage += CEnString(IDS_WANTRELOAD);
				
				bReload = (MessageBox(sMessage, IDS_STATUSCHANGE_TITLE, !m_mgrToDoCtrls.GetReadOnlyStatus(nCtrl) ? MB_YESNO : MB_OK) == IDYES);
			}
			else
				bReload = !bReadOnly; // now writable
			
			if (bReload)
			{
				ReloadTaskList(nCtrl, FALSE);
				
				// notify the user if we haven't already and the trayicon is visible
				if (nReloadOption != RO_ASK && prefs.GetSysTrayOption() != STO_NONE)
				{
					sFileList += tdc.GetFriendlyProjectName();
					sFilePath += "\n";
				}
			}
			else // update the UI
			{
				if (nCtrl == m_tabCtrl.GetCurSel())
					UpdateCaption();
				
				m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
				m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
			}
		}
	}
	
	// do we need to notify the user?
	// don't if they are busy typing
	if (!sFileList.IsEmpty() && !CWinClasses::IsEditControl(::GetFocus()))
	{
		CEnString sMessage(IDS_TASKLISTSRELOADED, sFileList);
		//		m_ti.ShowBalloon(sMessage, "Readonly Status Change", NIIF_INFO);
	}
}

void CToDoListWnd::OnTimerTimestampChange(int nCtrl)
{
	AF_NOREENTRANT // macro helper
		
		CPreferencesDlg& prefs = Prefs();
	int nReloadOption = prefs.GetTimestampReloadOption();
	
	ASSERT (nReloadOption != RO_NO);
	
	// work out whether we should check remote files or not
	BOOL bCheckRemoteFiles = (nCtrl != -1);
	
	if (!bCheckRemoteFiles)
	{
		static int nElapsed = 0;
		UINT nRemoteFileCheckInterval = prefs.GetRemoteFileCheckFrequency() * 1000; // in ms
		
		nElapsed %= nRemoteFileCheckInterval;
		bCheckRemoteFiles = !nElapsed;
		
		nElapsed += INTERVAL_TIMESTAMPCHANGE;
	}
	
	// process files
	CString sFileList;
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;
	
	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		CString sFilePath = tdc.GetFilePath();
		
		// don't check removeable drives
		int nType = m_mgrToDoCtrls.GetFilePathType(nCtrl);
		
        if (nType == TDCM_UNDEF || nType == TDCM_REMOVABLE)
			continue;
		
		// check remote files?
		if (!bCheckRemoteFiles && nType == TDCM_REMOTE)
			continue;
		
		if (m_mgrToDoCtrls.RefreshLastModified(nCtrl))
		{
			BOOL bReload = TRUE; // default
			
			if (nReloadOption == RO_ASK)
			{
				CEnString sMessage(IDS_MODIFIEDELSEWHERE, sFilePath);
				sMessage += CEnString(IDS_WANTRELOAD);
				
				bReload = (MessageBox(sMessage, IDS_TIMESTAMPCHANGE_TITLE, MB_YESNO) == IDYES);
			}
			
			if (bReload)
			{
				ReloadTaskList(nCtrl, FALSE);
				
				// notify the user if we haven't already and the trayicon is visible
				if (nReloadOption != RO_ASK && prefs.GetSysTrayOption() != STO_NONE)
				{
					sFileList += tdc.GetFriendlyProjectName();
					sFilePath += "\n";
				}
				
				// update UI
				if (nCtrl == m_tabCtrl.GetCurSel())
					UpdateCaption();
				
				m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
				m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
			}
		}
	}
	
	// do we need to notify the user?
	// but not if they are busy typing
	if (!sFileList.IsEmpty() && !CWinClasses::IsEditControl(::GetFocus()))
	{
		CEnString sMessage(IDS_TASKLISTSRELOADED, sFileList);
		//		m_ti.ShowBalloon(sMessage, "File Timestamp Change", NIIF_INFO);
	}
}

void CToDoListWnd::OnTimerAutoSave()
{
	AF_NOREENTRANT // macro helper
		
	// don't save if the user is editing a task label
	if (!GetToDoCtrl().IsTreeLabelEditing())
		SaveAll(Prefs().GetAutoSaveUnsaved(), FALSE, FALSE, FALSE);
}

void CToDoListWnd::OnTimerCheckoutStatus(int nCtrl)
{
	AF_NOREENTRANT // macro helper
		
		CPreferencesDlg& prefs = Prefs();
	
	// work out whether we should check remote files or not
	BOOL bCheckRemoteFiles = (nCtrl != -1);
	
	if (!bCheckRemoteFiles)
	{
		static int nElapsed = 0;
		UINT nRemoteFileCheckInterval = prefs.GetRemoteFileCheckFrequency() * 1000; // in ms
		
		nElapsed %= nRemoteFileCheckInterval;
		bCheckRemoteFiles = !nElapsed;
		
		nElapsed += INTERVAL_CHECKOUTSTATUS;
	}
	
	// process files
	CString sFileList;
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;
	
	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		
		if (!m_mgrToDoCtrls.PathSupportsSourceControl(nCtrl))
            continue;
		
		// try to check out only if the previous attempt failed
		if (!tdc.IsCheckedOut() && prefs.GetCheckoutOnCheckin())
		{
			// we only try to check if the previous checkout failed
			if (!m_mgrToDoCtrls.GetLastCheckoutStatus(nCtrl))
			{
				if (tdc.CheckOut())
				{
					// update timestamp 
					m_mgrToDoCtrls.RefreshLastModified(nCtrl);
					m_mgrToDoCtrls.SetLastCheckoutStatus(nCtrl, TRUE);
					
					if (nCtrl == m_tabCtrl.GetCurSel())
						UpdateCaption();
					
					m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
					m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
					
					// notify the user if the trayicon is visible
					if (prefs.GetSysTrayOption() != STO_NONE)
					{
						sFileList += tdc.GetFriendlyProjectName();
						sFileList += "\n";
					}
				}
				else // make sure we try again later
					m_mgrToDoCtrls.SetLastCheckoutStatus(nCtrl, FALSE);
			}
		}
		// only checkin if sufficient time has elapsed since last mod
		// and there are no mods outstanding
		else if (tdc.IsCheckedOut() && prefs.GetCheckinOnNoEditTime())
		{
			if (!tdc.IsModified())
			{
				double dElapsed = COleDateTime::GetCurrentTime() - tdc.GetLastTaskModified();
				dElapsed *= 24 * 60; // convert to minutes
				
				if (dElapsed > (double)prefs.GetCheckinOnNoEditTime())
				{
					if (tdc.CheckIn())	
					{
						m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
						m_mgrToDoCtrls.RefreshLastModified(nCtrl);
						m_mgrToDoCtrls.SetLastCheckoutStatus(nCtrl, TRUE);
						m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
						
						UpdateCaption();
					}
				}
			}
		}
	}
	// do we need to notify the user?
	// but not if they are busy typing
	if (!sFileList.IsEmpty() && !CWinClasses::IsEditControl(::GetFocus()))
	{
		CEnString sMessage(IDS_TASKLISTSCHECKEDOUT, sFileList);
		//		m_ti.ShowBalloon(sMessage, "Source Control Change", NIIF_INFO);
	}
}

void CToDoListWnd::OnNeedUserTooltipText(NMHDR* pNMHDR, LRESULT* pResult)
{
	static CString sTipText;
	sTipText.Empty();

	switch (pNMHDR->idFrom)
	{
	case ID_SORT:
		{
			TDC_SORTBY nSortBy = GetToDoCtrl().GetSortBy();
			UINT nCmdID = GetSortID(nSortBy);

			sTipText.LoadString(nCmdID);
		}
		break;

	default:
		{
			// should only otherwise be the tools which do not have tooltips
			int nTool = pNMHDR->idFrom - ID_TOOLS_USERTOOL1;
			ASSERT (nTool >= 0 && nTool < 16);

			USERTOOL ut;
			
			if (Prefs().GetUserTool(nTool, ut))
				sTipText = ut.sToolName;
		}
		break;
	}
	
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	pTTT->lpszText = (LPTSTR)(LPCTSTR)sTipText;

	*pResult = 0;
}

void CToDoListWnd::OnUpdateUserTool(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pMenu && pCmdUI->m_nID == ID_TOOLS_USERTOOL1) // only handle first item
	{
		CUserToolArray aTools;
		Prefs().GetUserTools(aTools);
		
		CToolsHelper th(ID_TOOLS_USERTOOL1);
		th.UpdateMenu(pCmdUI, aTools);
	}
	else if (m_bShowToolbar) 
	{
		int nTool = pCmdUI->m_nID - ID_TOOLS_USERTOOL1;
		ASSERT (nTool >= 0 && nTool < 16);

		USERTOOL ut;
		
		if (Prefs().GetUserTool(nTool, ut))
			pCmdUI->Enable(TRUE);
	}
}

void CToDoListWnd::OnUserTool(UINT nCmdID) 
{
	int nTool = nCmdID - ID_TOOLS_USERTOOL1;
	USERTOOL ut;
	
	ASSERT (nTool >= 0 && nTool < 16);
	
	if (Prefs().GetUserTool(nTool, ut))
	{
		CToolsHelper th(ID_TOOLS_USERTOOL1);
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		CString sFullPath = tdc.GetFilePath();
		
		// Save all tasklists before executing the user tool
		SaveAll(FALSE, FALSE, FALSE, TRUE);
		
		USERTOOLARGS args;
		args.sTasklist = tdc.GetFilePath();
		args.dwTaskID = tdc.GetSelectedTaskID();
		args.sTaskTitle = tdc.GetSelectedTaskTitle();
		args.sTaskExtID = tdc.GetSelectedTaskExtID();
		args.sTaskComments = tdc.GetSelectedTaskComments();
		
		th.RunTool(ut, args, this);
	}
}

void CToDoListWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if (!bSysMenu)
	{
		// remove relevant commands from the edit menu
		if (nIndex == EDITTASK)
			PrepareEditMenu(pPopupMenu);
	}
}

void CToDoListWnd::OnViewToolbar() 
{
	m_bShowToolbar = !m_bShowToolbar;
	m_toolbar.ShowWindow(m_bShowToolbar ? SW_SHOW : SW_HIDE);
	m_toolbar.EnableWindow(m_bShowToolbar);

	ResizeDlg();
}

void CToDoListWnd::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowToolbar);
}

void CToDoListWnd::AppendTools2Toolbar(BOOL bAppend)
{
	CToolsHelper th(ID_TOOLS_USERTOOL1);
	
	if (bAppend)
	{
		// then re-add
		CUserToolArray aTools;
		Prefs().GetUserTools(aTools);
		
		th.AppendToolsToToolbar(aTools, m_toolbar, ID_PREFERENCES);
	}
	else // remove
		th.RemoveToolsFromToolbar(m_toolbar, ID_PREFERENCES);
}

void CToDoListWnd::OnSort() 
{
	// we should get this only when both the toolbar and menu are visible
	ASSERT (m_bShowToolbar);
	
	GetToDoCtrl().Resort();
}

void CToDoListWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CFrameWnd::OnWindowPosChanged(lpwndpos);
}

void CToDoListWnd::RefreshFilterLabelAlignment()
{
	m_filterBar.SetFilterLabelAlignment(!Prefs().GetRightAlignLabels());
}


void CToDoListWnd::OnArchiveCompletedtasks() 
{
	CWaitCursor cursor;
	
	if (m_mgrToDoCtrls.ArchiveDoneTasks(GetSelToDoCtrl()) > 0)
		UpdateCaption();
}

void CToDoListWnd::OnUpdateArchiveCompletedtasks(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && !m_mgrToDoCtrls.GetArchivePath(tdc.GetFilePath()).IsEmpty());
}

void CToDoListWnd::OnMovetaskdown() 
{
	GetToDoCtrl().MoveSelectedTask(MOVE_DOWN);	
}

void CToDoListWnd::OnUpdateMovetaskdown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(MOVE_DOWN));	
}

void CToDoListWnd::OnMovetaskup() 
{
	GetToDoCtrl().MoveSelectedTask(MOVE_UP);	
}

void CToDoListWnd::OnUpdateMovetaskup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(MOVE_UP));	
}

void CToDoListWnd::OnMovetaskright() 
{
	GetToDoCtrl().MoveSelectedTask(MOVE_RIGHT);	
}

void CToDoListWnd::OnUpdateMovetaskright(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(MOVE_RIGHT));	
}

void CToDoListWnd::OnMovetaskleft() 
{
	GetToDoCtrl().MoveSelectedTask(MOVE_LEFT);	
}

void CToDoListWnd::OnUpdateMovetaskleft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(MOVE_LEFT));	
}

CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl()
{
	return GetToDoCtrl(GetSelToDoCtrl());
}

CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl(int nIndex)
{
	return m_mgrToDoCtrls.GetToDoCtrl(nIndex);
}

const CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl() const
{
	return GetToDoCtrl(GetSelToDoCtrl());
}

const CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl(int nIndex) const
{
	return m_mgrToDoCtrls.GetToDoCtrl(nIndex);
}

CFilteredToDoCtrl* CToDoListWnd::NewToDoCtrl()
{
	CFilteredToDoCtrl* pCtrl = NULL;
	
	// if the active tasklist is untitled and unmodified then reuse it
	if (GetTDCCount())
	{
		int nSel = GetSelToDoCtrl();
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		
		// make sure that we don't accidently reuse a just edited tasklist
		tdc.Flush(); 
		
		if (m_mgrToDoCtrls.IsPristine(nSel))
		{
			pCtrl = &tdc;
			m_mgrToDoCtrls.RemoveToDoCtrl(nSel, FALSE);
			
			// make sure we reset the selection to a valid index
			if (GetTDCCount())
			{
				// try leaving the selection as-is
				if (nSel >= GetTDCCount())
					nSel--; // try the preceding item
				
				SelectToDoCtrl(nSel, FALSE);
			}
			
			return pCtrl;
		}
	}
	
	// else
	pCtrl = new CFilteredToDoCtrl(m_mgrContent, Prefs().GetDefaultCommentsFormat());
	
	if (pCtrl && CreateToDoCtrl(pCtrl))
		return pCtrl;
	
	// else
	delete pCtrl;
	return NULL;
}

BOOL CToDoListWnd::CreateToDoCtrl(CFilteredToDoCtrl* pCtrl)
{
	// note: size not important since tdc will get resized
	// in AddToDoCtrl()
	if (pCtrl->Create(CRect(0, 0, 10, 10), this, IDC_TODOLIST, FALSE))
	{
		// set font to our font
		CDialogHelper::SetFont(pCtrl, m_fontMain, FALSE);
		
		if (!m_ilToDoCtrl.GetSafeHandle())
			InitCheckboxImageList();
		
		UpdateToDoCtrlPreferences(pCtrl);
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CToDoListWnd::InitCheckboxImageList()
{
	if (m_ilToDoCtrl.GetSafeHandle())
		return TRUE;
	
	const int nStates[] = { -1, CBS_UNCHECKEDNORMAL, CBS_CHECKEDNORMAL };//, CBS_MIXEDNORMAL };
	const int nNumStates = sizeof(nStates) / sizeof(int);
	
	CThemed th;
	
	if (th.Open(this, "BUTTON") && th.AreControlsThemed())
	{
		th.BuildImageList(m_ilToDoCtrl, BP_CHECKBOX, nStates, nNumStates);
	}
	
	// unthemed + fallback
	if (!m_ilToDoCtrl.GetSafeHandle())
	{
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TODO);
		
		BITMAP BM;
		bitmap.GetBitmap(&BM);
		
		if (m_ilToDoCtrl.Create(BM.bmWidth / nNumStates, BM.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 1))
			m_ilToDoCtrl.Add(&bitmap, 255);
	}
	
	return (NULL != m_ilToDoCtrl.GetSafeHandle());
}

void CToDoListWnd::OnMBtnClickTabcontrol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (Prefs().GetEnableCtrlMBtnClose())
	{
		NMTCMBTNCLK* pTCHDR = (NMTCMBTNCLK*)pNMHDR;
		
		// check valid tab
		if (pTCHDR->iTab >= 0)
		{
			CFilteredToDoCtrl& tdc = GetToDoCtrl(pTCHDR->iTab);
			tdc.Flush();

			CloseToDoCtrl(pTCHDR->iTab);
				
			if (!GetTDCCount())
				CreateNewTaskList(FALSE);
		}
	}
	*pResult = 0;
}

void CToDoListWnd::OnSelchangeTabcontrol(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	// show the incoming selection
	int nCurSel = GetSelToDoCtrl();

	// check password if necessary
	if (m_nLastSelItem != -1 && !VerifyToDoCtrlPassword())
	{
		m_tabCtrl.SetCurSel(m_nLastSelItem);
		return;
	}
	
	if (nCurSel != -1)
	{
		UpdateToDoCtrlPreferences();
		
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCurSel);
		
		// make sure size is right
		CRect rTDC;
		
		if (CalcToDoCtrlRect(rTDC))
			tdc.MoveWindow(rTDC);
		
		tdc.EnableWindow(TRUE);
		tdc.SetFocusToTree();
		tdc.ShowWindow(SW_SHOW);
		tdc.PauseTimeTracking(FALSE); // always
		
		// update the filter selection
		RefreshFilterControls();
		
		// update status bar
		UpdateStatusbar();
		UpdateCaption();
	}
	
	// hide the outgoing selection
	if (m_nLastSelItem != -1)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(m_nLastSelItem);
		
		tdc.ShowWindow(SW_HIDE);
		tdc.EnableWindow(FALSE);
		tdc.PauseTimeTracking(Prefs().GetTrackActiveTasklistOnly());
		
		m_nLastSelItem = -1; // reset
	}
	
	if (nCurSel != -1)
	{
		// notify user of due tasks if req
		DoDueTaskNotification(Prefs().GetNotifyDueByOnSwitch());
	}

	InitMenuIconManager();
	
	*pResult = 0;
}

void CToDoListWnd::RefreshFilterControls()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	m_filterBar.RefreshFilterControls(tdc);
}

void CToDoListWnd::OnSelchangingTabcontrol(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	// cache the outgoing selection
	m_nLastSelItem = GetSelToDoCtrl();
	
	// and flush
	if (m_nLastSelItem != -1)
		GetToDoCtrl(m_nLastSelItem).Flush();
	
	*pResult = 0;
}

BOOL CToDoListWnd::ConfirmSaveTaskList(int nIndex, BOOL bClosingTaskList, BOOL bClosingWindows)
{
	bClosingTaskList |= bClosingWindows; // sanity check
	
	// save changes
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	
	if (tdc.IsModified())
	{
		BOOL bFirstTimeSave = tdc.GetFilePath().IsEmpty();

		if (bClosingTaskList && (bFirstTimeSave || Prefs().GetConfirmSaveOnExit()))
		{
			// save tasklist
			CString sName = m_mgrToDoCtrls.GetFriendlyProjectName(nIndex);
			CEnString sMessage(IDS_SAVEBEFORECLOSE, sName);
			
			// don't allow user to cancel if closing down
			int nRet = MessageBox(sMessage, IDS_CONFIRMSAVE_TITLE, bClosingWindows ? MB_YESNO : MB_YESNOCANCEL);
			
			if (nRet == IDYES)
			{
				// user can still cancel save dialog even if closing down
				if (!SaveTaskList(nIndex))
					nRet = bClosingWindows ? IDNO : IDCANCEL;
			}
			
			ASSERT (!(bClosingWindows && nRet == IDCANCEL)); // sanity check
			
			if (nRet == IDCANCEL)
				return FALSE;
			else
			{
				tdc.SetModified(FALSE); // so we don't get prompted again
				m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
			}
		}
		else if (!SaveTaskList(nIndex))
			return FALSE;
	}
	
	return TRUE; // user did not cancel
}

BOOL CToDoListWnd::CloseToDoCtrl(int nIndex)
{
	ASSERT (nIndex >= 0);
	ASSERT (nIndex < GetTDCCount());

	CFilteredToDoCtrl& tdcSel = GetToDoCtrl();
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	tdc.Flush();
	
	if (!ConfirmSaveTaskList(nIndex, TRUE, FALSE))
		return FALSE;
	
	CWaitCursor cursor;
	
	int nNewSel = m_mgrToDoCtrls.RemoveToDoCtrl(nIndex, TRUE);
	
	if (nNewSel != -1)
	{
		// if we're closing TDL then the main window will not
		// be visible at this point so we don't have to worry about
		// encrypted tasklists becoming visible. however if as a result
		// of this closure an encrypted tasklist would become visible
		// then we need to prompt for a password and if this fails
		// we must create another tasklist to hide the encrypted one.
		// unless the tasklist being closed was not active and the 
		// new selection hasn't actually changed
		BOOL bCheckPassword = !m_bClosing && (&GetToDoCtrl(nNewSel) != &tdcSel);

		if (!SelectToDoCtrl(nNewSel, bCheckPassword))
		{
			CreateNewTaskList(FALSE);
			RefreshTabOrder();
		}

		if (!m_bClosing)
			ResizeDlg();
	}
	
	return TRUE;
}

void CToDoListWnd::OnCloseTasklist() 
{
	int nSel = GetSelToDoCtrl();
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);

	// make sure there are no edits pending
	tdc.Flush(); 
	
	// check if its a pristine tasklist and the last tasklist and 
	// if so only close it if the default comments type has changed
	if (m_mgrToDoCtrls.IsPristine(nSel) && GetTDCCount() == 1)
	{
		GUID idDef, idCtrl;
		
		Misc::NullGuid(idDef);
		Misc::NullGuid(idCtrl);
		
		tdc.GetCustomCommentsType(idCtrl);
		m_mgrContent.GetContentTypeID(Prefs().GetDefaultCommentsFormat(), idDef);
		
		if (Misc::SameGuids(idDef, idCtrl))
			return;
	}
	
	CloseToDoCtrl(nSel);
	
	// if empty then create a new dummy item		
	if (!GetTDCCount())
		CreateNewTaskList(FALSE);
	else
		ResizeDlg();
}

BOOL CToDoListWnd::SelectToDoCtrl(LPCTSTR szFilePath, BOOL bCheckPassword)
{
	int nCtrl = m_mgrToDoCtrls.FindToDoCtrl(szFilePath);
	
	if (nCtrl != -1)
	{
		SelectToDoCtrl(nCtrl, bCheckPassword);
		return TRUE;
	}
	
	return FALSE;
}

int CToDoListWnd::GetSelToDoCtrl() const 
{ 
	if (m_tabCtrl.GetSafeHwnd()) 
		return m_tabCtrl.GetCurSel(); 
	else
		return -1;
}

BOOL CToDoListWnd::SelectToDoCtrl(int nIndex, BOOL bCheckPassword)
{
	ASSERT (nIndex >= 0);
	ASSERT (nIndex < GetTDCCount());
	
	// validate password first if necessary
	if (bCheckPassword && !VerifyToDoCtrlPassword(nIndex))
		return FALSE;
	
	int nCurSel = GetSelToDoCtrl(); // cache this

	// resize tdc first
	CRect rTDC;
	
	if (CalcToDoCtrlRect(rTDC))
		GetToDoCtrl(nIndex).MoveWindow(rTDC);
	
	m_tabCtrl.SetCurSel(nIndex); // this changes the selected CToDoCtrl
	m_tabCtrl.UpdateWindow();
	
	if (!m_bClosing)
		UpdateToDoCtrlPreferences();
	
	// show the chosen item
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	tdc.EnableWindow(TRUE);
	tdc.SetFocusToTree();
	tdc.ShowWindow(SW_SHOW);
	tdc.PauseTimeTracking(FALSE); // always
	
	// before hiding the previous selection
	if (nCurSel != -1 && nCurSel != nIndex)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCurSel);
		
		tdc.ShowWindow(SW_HIDE);
		tdc.EnableWindow(FALSE);
		tdc.PauseTimeTracking(Prefs().GetTrackActiveTasklistOnly());
	}
	
	if (!m_bClosing)
	{
		const CPreferencesDlg& prefs = Prefs();
		
		if (prefs.GetReadonlyReloadOption() != RO_NO)
			OnTimerReadOnlyStatus(nIndex);
		
		if (prefs.GetTimestampReloadOption() != RO_NO)
			OnTimerTimestampChange(nIndex);
		
		if (prefs.GetEnableSourceControl())
			OnTimerCheckoutStatus(nIndex);
	}
	
	UpdateCaption();
	UpdateStatusbar();
	
	// update the filter selection
	RefreshFilterControls();

	// and the menu icon manager
	InitMenuIconManager();

	// and current directory
	UpdateCwd();

	return TRUE;
}

void CToDoListWnd::UpdateToDoCtrlPreferences()
{
	// check if this has already been done since the last prefs change
	int nSel = GetSelToDoCtrl();
	
	if (m_mgrToDoCtrls.GetNeedsPreferenceUpdate(nSel))
	{
		UpdateToDoCtrlPreferences(&GetToDoCtrl(nSel));
		
		// reset flag
		m_mgrToDoCtrls.SetNeedsPreferenceUpdate(nSel, FALSE);
	}
}

void CToDoListWnd::UpdateToDoCtrlPreferences(CFilteredToDoCtrl* pCtrl)
{
	CPreferencesDlg& prefs = Prefs();
	CFilteredToDoCtrl& tdc = *pCtrl;
	
	CTDCStyles styles;
	
	styles[TDCS_COLORTEXTBYPRIORITY] = (prefs.GetTextColorOption() == COLOROPT_PRIORITY);
	styles[TDCS_COLORTEXTBYCATEGORY] = (prefs.GetTextColorOption() == COLOROPT_CATEGORY);
	styles[TDCS_SHOWINFOTIPS] = prefs.GetShowInfoTips();
	styles[TDCS_SHOWCOMMENTSINLIST] = prefs.GetShowComments();
	styles[TDCS_TREATSUBCOMPLETEDASDONE] = prefs.GetTreatSubCompletedAsDone();
	styles[TDCS_HIDEPERCENTFORDONETASKS] = prefs.GetHidePercentForDoneTasks();
	styles[TDCS_HIDESTARTDUEFORDONETASKS] = prefs.GetHideStartDueForDoneTasks();
	styles[TDCS_HIDEZEROTIMECOST] = prefs.GetHideZeroTimeCost();
	styles[TDCS_CONFIRMDELETE] = prefs.GetConfirmDelete();
	styles[TDCS_AVERAGEPERCENTSUBCOMPLETION] = prefs.GetAveragePercentSubCompletion();
	styles[TDCS_INCLUDEDONEINAVERAGECALC] = prefs.GetIncludeDoneInAverageCalc();
	styles[TDCS_SHOWBUTTONSINTREE] = prefs.GetShowButtonsInTree();
	styles[TDCS_USEEARLIESTDUEDATE] = prefs.GetUseEarliestDueDate();
	styles[TDCS_USEPERCENTDONEINTIMEEST] = prefs.GetUsePercentDoneInTimeEst();
	styles[TDCS_SHOWCTRLSASCOLUMNS] = prefs.GetShowCtrlsAsColumns();
	styles[TDCS_SHOWCOMMENTSALWAYS] = prefs.GetShowCommentsAlways();
	styles[TDCS_AUTOREPOSCTRLS] = prefs.GetAutoReposCtrls();
	styles[TDCS_SHOWPERCENTASPROGRESSBAR] = prefs.GetShowPercentAsProgressbar();
	styles[TDCS_STRIKETHOUGHDONETASKS] = prefs.GetStrikethroughDone();
	styles[TDCS_SHOWPATHINHEADER] = prefs.GetShowPathInHeader();
	styles[TDCS_FULLROWSELECTION] = prefs.GetFullRowSelection();
	styles[TDCS_TREECHECKBOXES] = prefs.GetTreeCheckboxes();
	styles[TDCS_COLUMNHEADERCLICKING] = prefs.GetEnableHeaderSorting();
	styles[TDCS_SORTVISIBLETASKSONLY] = prefs.GetSortVisibleOnly();
	styles[TDCS_SHAREDCOMMENTSHEIGHT] = prefs.GetSharedCommentsHeight();
	styles[TDCS_TASKCOLORISBACKGROUND] = prefs.GetColorTaskBackground();
	styles[TDCS_COMMENTSUSETREEFONT] = prefs.GetCommentsUseTreeFont();
	styles[TDCS_SHOWDATESINISO] = prefs.GetDisplayDatesInISO();
	styles[TDCS_USEHIGHESTPRIORITY] = prefs.GetUseHighestPriority();
	styles[TDCS_AUTOCALCTIMEESTIMATES] = prefs.GetAutoCalcTimeEstimates();
	styles[TDCS_SHOWWEEKDAYINDATES] = prefs.GetShowWeekdayInDates();
	styles[TDCS_ROUNDTIMEFRACTIONS] = prefs.GetRoundTimeFractions();
	styles[TDCS_SHOWNONFILEREFSASTEXT] = prefs.GetShowNonFilesAsText();
	styles[TDCS_INCLUDEDONEINPRIORITYCALC] = prefs.GetIncludeDoneInPriorityCalc();
	styles[TDCS_WEIGHTPERCENTCALCBYTIMEEST] = prefs.GetWeightPercentCompletionByTimeEst();
	styles[TDCS_WEIGHTPERCENTCALCBYPRIORITY] = prefs.GetWeightPercentCompletionByPriority();
	styles[TDCS_WEIGHTPERCENTCALCBYNUMSUB] = prefs.GetWeightPercentCompletionByNumSubtasks();
	styles[TDCS_RIGHTALIGNLABELS] = prefs.GetRightAlignLabels();
	styles[TDCS_SHOWPARENTSASFOLDERS] = prefs.GetShowParentsAsFolders();
	styles[TDCS_FOCUSTREEONENTER] = prefs.GetFocusTreeOnEnter();
	styles[TDCS_AUTOCALCPERCENTDONE] = prefs.GetAutoCalcPercentDone();
	styles[TDCS_TRACKSELECTEDTASKONLY] = prefs.GetTrackSelectedTaskOnly();
	styles[TDCS_HIDEPRIORITYNUMBER] = prefs.GetHidePriorityNumber();
	styles[TDCS_MAXIMIZE] = m_bMaxTasklists;
	styles[TDCS_SHOWPROJECTNAME] = m_bShowProjectName;
	styles[TDCS_PAUSETIMETRACKINGONSCRNSAVER] = prefs.GetNoTrackOnScreenSaver();
	styles[TDCS_SHOWFIRSTCOMMENTLINEINLIST] = prefs.GetDisplayFirstCommentLine();
	styles[TDCS_DISPLAYHMSTIMEFORMAT] = prefs.GetUseHMSTimeFormat();
	styles[TDCS_SORTDONETASKSATBOTTOM] = prefs.GetSortDoneTasksAtBottom();
//	styles[TDCS_RTLCOMMENTS] = prefs.GetRTLComments();
	styles[TDCS_LOGTIMETRACKING] = prefs.GetLogTimeTracking();
	styles[TDCS_LOGTASKTIMESEPARATELY] = prefs.GetLogTaskTimeSeparately();
	styles[TDCS_WARNADDDELETEARCHIVE] = prefs.GetWarnAddDeleteArchive();
	styles[TDCS_SHOWSUBTASKCOMPLETION] = prefs.GetShowSubtaskCompletion();
	styles[TDCS_RESORTONMODIFY] = prefs.GetAutoReSort();
	styles[TDCS_ALLOWPARENTTIMETRACKING] = prefs.GetAllowParentTimeTracking();
	styles[TDCS_REFILTERONMODIFY] = prefs.GetAutoReFilter();
	styles[TDCS_AUTOADJUSTDEPENDENTS] = prefs.GetAutoAdjustDependents();

	// comments pos
	int nPos = prefs.GetCommentsPos();

	styles[TDCS_VERTCOMMENTS] = (nPos == PUIP_RIGHTCOMMENTS);
//	styles[TDCS_RIGHTCOMMENTS] = (nPos == PUIP_RIGHTCOMMENTS);
//	styles[TDCS_BOTTOMCOMMENTS] = (nPos == PUIP_BOTTOMCOMMENTS);
//	styles[TDCS_BOTTOMRIGHTCOMMENTS] = (nPos == PUIP_BOTTOMRIGHTCOMMENTS);
	
	// source control
	BOOL bSrcControl = m_mgrToDoCtrls.PathSupportsSourceControl(tdc.GetFilePath());
	
	styles[TDCS_ENABLESOURCECONTROL] = bSrcControl;
	styles[TDCS_CHECKOUTONLOAD] = bSrcControl && prefs.GetAutoCheckOut();
	
	tdc.SetStyles(styles);

	// columns
	// note: all but last column has bUpdate = FALSE
	ShowColumn(TDCC_POSITION, tdc, prefs.GetShowColumn(TDLBC_POSITION), FALSE);
	ShowColumn(TDCC_ID, tdc, prefs.GetShowColumn(TDLBC_ID), FALSE);
	ShowColumn(TDCC_PERCENT, tdc, prefs.GetShowColumn(TDLBC_PERCENT), FALSE);
	ShowColumn(TDCC_PRIORITY, tdc, prefs.GetShowColumn(TDLBC_PRIORITY), FALSE);
	ShowColumn(TDCC_TIMEEST, tdc, prefs.GetShowColumn(TDLBC_TIMEEST), FALSE);
	ShowColumn(TDCC_TIMESPENT, tdc, prefs.GetShowColumn(TDLBC_TIMESPENT), FALSE);
	ShowColumn(TDCC_STARTDATE, tdc, prefs.GetShowColumn(TDLBC_STARTDATE), FALSE);
	ShowColumn(TDCC_DUEDATE, tdc, prefs.GetShowColumn(TDLBC_DUEDATE), FALSE);
	ShowColumn(TDCC_DONEDATE, tdc, prefs.GetShowColumn(TDLBC_DONEDATE), FALSE);
	ShowColumn(TDCC_ALLOCTO, tdc, prefs.GetShowColumn(TDLBC_ALLOCTO), FALSE);
	ShowColumn(TDCC_ALLOCBY, tdc, prefs.GetShowColumn(TDLBC_ALLOCBY), FALSE);
	ShowColumn(TDCC_STATUS, tdc, prefs.GetShowColumn(TDLBC_STATUS), FALSE);
	ShowColumn(TDCC_CATEGORY, tdc, prefs.GetShowColumn(TDLBC_CATEGORY), FALSE);
	ShowColumn(TDCC_FILEREF, tdc, prefs.GetShowColumn(TDLBC_FILEREF), FALSE);
	ShowColumn(TDCC_DONE, tdc, prefs.GetShowColumn(TDLBC_DONE), FALSE);
	ShowColumn(TDCC_TRACKTIME, tdc, prefs.GetShowColumn(TDLBC_TRACKTIME), FALSE);
	ShowColumn(TDCC_FLAG, tdc, prefs.GetShowColumn(TDLBC_FLAG), FALSE);
	ShowColumn(TDCC_CREATIONDATE, tdc, prefs.GetShowColumn(TDLBC_CREATIONDATE), FALSE);
	ShowColumn(TDCC_CREATEDBY, tdc, prefs.GetShowColumn(TDLBC_CREATEDBY), FALSE);
	ShowColumn(TDCC_LASTMOD, tdc, prefs.GetShowColumn(TDLBC_MODIFYDATE), FALSE);
	ShowColumn(TDCC_RISK, tdc, prefs.GetShowColumn(TDLBC_RISK), FALSE);
	ShowColumn(TDCC_EXTERNALID, tdc, prefs.GetShowColumn(TDLBC_EXTERNALID), FALSE);
	ShowColumn(TDCC_COST, tdc, prefs.GetShowColumn(TDLBC_COST), FALSE);
	ShowColumn(TDCC_RECURRENCE, tdc, prefs.GetShowColumn(TDLBC_RECURRENCE), FALSE);
	ShowColumn(TDCC_VERSION, tdc, prefs.GetShowColumn(TDLBC_VERSION), FALSE);
	ShowColumn(TDCC_DEPENDENCY, tdc, prefs.GetShowColumn(TDLBC_DEPENDS), TRUE);
	
	// info tips
	tdc.SetMaxInfotipCommentsLength(prefs.GetMaxInfoTipCommentsLength());
	
	// update default task preferences
	int nUnits;
	double dTimeEst = prefs.GetDefaultTimeEst(nUnits);

	CStringArray aCats;
	prefs.GetDefaultCategories(aCats);
	
	tdc.SetDefaultTaskAttributes(CEnString(IDS_TASK), NULL, prefs.GetDefaultColor(),
								dTimeEst, nUnits, prefs.GetDefaultAllocTo(),
								prefs.GetDefaultAllocBy(), prefs.GetDefaultStatus(),
								aCats, prefs.GetDefaultPriority(),
								prefs.GetDefaultRisk(),	prefs.GetAutoDefaultStartDate(), 
								prefs.GetDefaultCreatedBy(), prefs.GetDefaultCost());
	
	// default string lists
	CStringArray aItems;
	
	if (prefs.GetCategoryList(aItems))
		tdc.SetDefaultCategoryNames(aItems);
	
	if (prefs.GetStatusList(aItems))
		tdc.SetDefaultStatusNames(aItems);
	
	if (prefs.GetAllocByList(aItems))
		tdc.SetDefaultAllocByNames(aItems);
	
	if (prefs.GetAllocToList(aItems))
		tdc.SetDefaultAllocToNames(aItems);
	
	CheckMinWidth();
	
	// fonts
	if (!m_fontTree.GetSafeHandle() || !m_fontComments.GetSafeHandle())
	{
		CString sFaceName;
		int nFontSize;
		
		if (!m_fontTree.GetSafeHandle() && prefs.GetTreeFont(sFaceName, nFontSize))
			m_fontTree.Attach(Misc::CreateFont(sFaceName, nFontSize));
		
		if (!m_fontComments.GetSafeHandle() && prefs.GetCommentsFont(sFaceName, nFontSize))
			m_fontComments.Attach(Misc::CreateFont(sFaceName, nFontSize));
	}
	
	tdc.SetTreeFont(m_fontTree);
	tdc.SetCommentsFont(m_fontComments);
	
	tdc.SetGridlineColor(prefs.GetGridlineColor());
	tdc.SetTaskCompletedColor(prefs.GetTaskDoneColor());
	tdc.SetAlternateLineColor(prefs.GetAlternateLineColor());
	
	// checkbox images
	tdc.SetCheckImageList(m_ilToDoCtrl);
	
	// colors
	tdc.SetPriorityColors(m_aPriorityColors);
	
	CTDCColorMap mapColors;
	CCatColorArray aCatColors;
	int nColor = prefs.GetCategoryColors(aCatColors);
	
	while (nColor--)
	{
		CATCOLOR& cc = aCatColors[nColor];
		mapColors[cc.sCategory] = cc.color;
	}
	tdc.SetCategoryColors(mapColors);
	
	tdc.Flush(); // clear any outstanding issues
}

void CToDoListWnd::ShowColumn(TDC_COLUMN nColumn, CFilteredToDoCtrl& tdc, BOOL bShow, BOOL bUpdate)
{
	tdc.ShowColumn(nColumn, bShow, bUpdate);
	m_filterBar.ShowFilter(nColumn, bShow, bUpdate);
}

void CToDoListWnd::UpdateTabSwitchTooltip()
{
	CToolTipCtrl* pTT = m_tabCtrl.GetToolTips();
	
	if (pTT)
	{
		// get the string for tab switching
		CString sShortcut = m_mgrShortcuts.GetShortcutTextByCmd(ID_VIEW_NEXT);
		
		if (sShortcut.IsEmpty())
			sShortcut = m_mgrShortcuts.GetShortcutTextByCmd(ID_VIEW_PREV);
		
		pTT->DelTool(&m_tabCtrl); // always
		
		if (!sShortcut.IsEmpty())
		{
			CEnString sTip(IDS_TABSWITCHTOOLTIP, sShortcut);
			pTT->AddTool(&m_tabCtrl, sTip);
		}
	}
}

void CToDoListWnd::OnSaveall() 
{
	SaveAll(TRUE, FALSE, FALSE, TRUE);
}

void CToDoListWnd::OnUpdateSaveall(CCmdUI* pCmdUI) 
{
	int nCtrl = GetTDCCount();
	
	while (nCtrl--)
	{
		if (GetToDoCtrl(nCtrl).IsModified())
		{
			pCmdUI->Enable();
			return;
		}
	}
	
	// else nothing modified
	pCmdUI->Enable(FALSE);
}

void CToDoListWnd::OnCloseall() 
{
	// save first
	if (!SaveAll(TRUE, TRUE, FALSE, TRUE))
		return;

	// delete tasklists
	int nCtrl = GetTDCCount();
	
	while (nCtrl--)
		m_mgrToDoCtrls.RemoveToDoCtrl(nCtrl, TRUE);

	// if empty then create a new dummy item		
	if (!GetTDCCount())
		CreateNewTaskList(FALSE);
	else
		ResizeDlg();
}

void CToDoListWnd::OnUpdateCloseall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTDCCount());
}

void CToDoListWnd::OnExit()
{
    OnClose();
}

void CToDoListWnd::OnUpdateExit(CCmdUI* pCmdUI)
{
	UINT nIDText = IDS_EXITTODOLIST;
	
    if (Prefs().GetSysTrayOption() == STO_ONCLOSE || Prefs().GetSysTrayOption() == STO_ONMINCLOSE)
		nIDText = IDS_MINIMIZETOTRAY;
	
    pCmdUI->SetText(CEnString(nIDText));
}

void CToDoListWnd::DoExit() 
{
    // save all first to ensure new tasklists get reloaded on startup
    if (!SaveAll(TRUE, TRUE, FALSE, TRUE))
        return; // user cancelled
	
	SaveSettings(); // before we close the tasklists

	m_bClosing = TRUE;
	
	// hide thw window as soon as possible so users do not
	// see the machinations of closing down
	ShowWindow(SW_HIDE);
	
	// delete tasklists
	{
		int nCtrl = GetTDCCount();
		
		while (nCtrl--)
			VERIFY(CloseToDoCtrl(nCtrl)); // shouldn't fail now
	}
	
	// if there are any still open then the user must have cancelled else destroy the window
	ASSERT (!GetTDCCount());
	
	if (!GetTDCCount())
		DestroyWindow();	
	else
		m_bClosing = FALSE;
}

void CToDoListWnd::OnUpdateMovetask(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount == 1);	
}

void CToDoListWnd::OnImportTasklist() 
{
	CFileDialogEx dialog(TRUE, GetDefaultFileExt(), NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, GetFileFilter(), this);
	
	CEnString sTitle(IDS_IMPORTTASKLIST_TITLE);
	dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
	
	if (dialog.DoModal() == IDOK)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		
		if (tdc.Import(dialog.GetPathName(), TRUE))
		{
			m_mgrToDoCtrls.RefreshModifiedStatus(GetSelToDoCtrl());
			UpdateCaption();
		}
		else
		{
			CEnString sMessage(IDS_INVALIDTASKLIST, dialog.GetPathName());
			MessageBox(sMessage, sTitle, MB_OK);
		}
	}
}

void CToDoListWnd::OnSetPriority(UINT nCmdID) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (!tdc.IsReadOnly() && tdc.HasSelection())
		tdc.SetSelectedTaskPriority(nCmdID - ID_EDIT_SETPRIORITY0);
}

void CToDoListWnd::OnUpdateSetPriority(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount);
	
	if (nSelCount == 1)
		pCmdUI->SetCheck((int)(pCmdUI->m_nID - ID_EDIT_SETPRIORITY0) == tdc.GetSelectedTaskPriority());
	else
		pCmdUI->SetCheck(0);
}

void CToDoListWnd::OnEditSetfileref() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (!tdc.IsReadOnly() && tdc.HasSelection())
	{
		CString sFileRef = tdc.GetSelectedTaskFileRef();
		
		CFileDialogEx dialog(TRUE, NULL, sFileRef, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CEnString(IDS_ALLFILEFILTER));
		CEnString sTitle(IDS_SETFILEREF_TITLE);
		
		dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
		
		if (dialog.DoModal() == IDOK)
			tdc.SetSelectedTaskFileRef(dialog.GetPathName());
	}
}

void CToDoListWnd::OnUpdateEditSetfileref(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection());
}

void CToDoListWnd::OnEditOpenfileref() 
{
	GetToDoCtrl().GotoSelectedTaskFileRef();
}

void CToDoListWnd::OnUpdateEditOpenfileref(CCmdUI* pCmdUI) 
{
	CString sFileRef = GetToDoCtrl().GetSelectedTaskFileRef();
	
	if (sFileRef.IsEmpty())
		pCmdUI->Enable(FALSE);
	else
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(sFileRef);
	}
}

LRESULT CToDoListWnd::OnPreferencesTestTool(WPARAM /*wp*/, LPARAM lp)
{
	USERTOOL* pTool = (USERTOOL*)lp;
	
	if (pTool)
	{
		CToolsHelper th(ID_TOOLS_USERTOOL1);
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		
		USERTOOLARGS args;
		args.sTasklist = tdc.GetFilePath();
		args.dwTaskID = tdc.GetSelectedTaskID();
		args.sTaskTitle = tdc.GetSelectedTaskTitle();
		args.sTaskExtID = tdc.GetSelectedTaskExtID();
		args.sTaskComments = tdc.GetSelectedTaskComments();
		
		th.TestTool(*pTool, args, &Prefs());
	}
	
	return 0;
}

LRESULT CToDoListWnd::OnPreferencesClearMRU(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_mruList.RemoveAll();
	
	return 0;
}

void CToDoListWnd::PrepareEditMenu(CMenu* pMenu)
{
	const CPreferencesDlg& prefs = Prefs();
	
	if (!prefs.GetShowEditMenuAsColumns())
		return;
	
	int nItem = pMenu->GetMenuItemCount();
	
	while (nItem--)
	{
		BOOL bDeleteItem = FALSE;
		BOOL bDeletePrevSep = FALSE;
		
		switch (pMenu->GetMenuItemID(nItem))
		{
		case -1: // its a popup so recursively handle it
			{
				CMenu* pPopup = pMenu->GetSubMenu(nItem);
				PrepareEditMenu(pPopup);
				
				// if the popup is now empty remove it too
				bDeleteItem = !pPopup->GetMenuItemCount();
			}
			break;
			
        case ID_EDIT_TASKDONE:
			bDeleteItem = !prefs.GetShowColumn(TDLBC_DONE) && !prefs.GetTreeCheckboxes();
			bDeletePrevSep = bDeleteItem && !pMenu->GetMenuItemID(nItem + 1);
			break;
			
        case ID_EDIT_DECTASKPERCENTDONE:
        case ID_EDIT_INCTASKPERCENTDONE:
			bDeleteItem = !prefs.GetShowColumn(TDLBC_PERCENT);
			break;
			
        case ID_EDIT_DECTASKPRIORITY:
			bDeletePrevSep = !prefs.GetShowColumn(TDLBC_PRIORITY);
			// fall thru
        case ID_EDIT_INCTASKPRIORITY:
        case ID_EDIT_SETPRIORITY0:
        case ID_EDIT_SETPRIORITY1:
        case ID_EDIT_SETPRIORITY2:
        case ID_EDIT_SETPRIORITY3:
        case ID_EDIT_SETPRIORITY4:
        case ID_EDIT_SETPRIORITY5:
        case ID_EDIT_SETPRIORITY6:
        case ID_EDIT_SETPRIORITY7:
        case ID_EDIT_SETPRIORITY8:
        case ID_EDIT_SETPRIORITY9:
        case ID_EDIT_SETPRIORITY10:
			bDeleteItem = !prefs.GetShowColumn(TDLBC_PRIORITY);
			break;
			
		case ID_EDIT_OFFSETDATES:
			bDeleteItem = !prefs.GetShowColumn(TDLBC_STARTDATE) &&
				!prefs.GetShowColumn(TDLBC_DUEDATE) && 
				!prefs.GetShowColumn(TDLBC_DONEDATE);
			bDeletePrevSep = bDeleteItem;
			break;
			
        case ID_EDIT_CLOCK_TASK:
			bDeletePrevSep = !(prefs.GetShowColumn(TDLBC_TRACKTIME) ||
								prefs.GetShowColumn(TDLBC_TIMESPENT));
			// fall thru
        case ID_SHOWTIMELOGFILE:
			bDeleteItem = !(prefs.GetShowColumn(TDLBC_TRACKTIME) ||
							prefs.GetShowColumn(TDLBC_TIMESPENT));
			break;
			
        case ID_EDIT_OPENFILEREF:
			bDeletePrevSep = !prefs.GetShowColumn(TDLBC_FILEREF);
			// fall thru
        case ID_EDIT_SETFILEREF:
			bDeleteItem = !prefs.GetShowColumn(TDLBC_FILEREF);
			break;
			
        case ID_EDIT_FLAGTASK:
			bDeletePrevSep = bDeleteItem = !prefs.GetShowColumn(TDLBC_FLAG);
			break;
		}
		
		// handle separator first
		if (bDeletePrevSep)
			pMenu->DeleteMenu(nItem + 1, MF_BYPOSITION);
		
		if (bDeleteItem)
			pMenu->DeleteMenu(nItem, MF_BYPOSITION);
	}
}

void CToDoListWnd::OnViewNext() 
{
	if (GetTDCCount() < 2)
		return;
	
	int nNext = GetSelToDoCtrl() + 1;
	
	if (nNext >= GetTDCCount())
		nNext = 0;
	
	if (SelectToDoCtrl(nNext, TRUE))
		DoDueTaskNotification(Prefs().GetNotifyDueByOnSwitch());
}

void CToDoListWnd::OnUpdateViewNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTDCCount() > 1);
}

void CToDoListWnd::OnViewPrev() 
{
	if (GetTDCCount() < 2)
		return;
	
	int nPrev = GetSelToDoCtrl() - 1;
	
	if (nPrev < 0)
		nPrev = GetTDCCount() - 1;
	
	if (SelectToDoCtrl(nPrev, TRUE))
		DoDueTaskNotification(Prefs().GetNotifyDueByOnSwitch());
}

void CToDoListWnd::OnUpdateViewPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTDCCount() > 1);
}

void CToDoListWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// we don't minimize if we're going to be hiding to then system tray
	if (nID == SC_MINIMIZE)
	{
		int nSysTrayOption = Prefs().GetSysTrayOption();
		
		if (nSysTrayOption == STO_ONMINIMIZE || nSysTrayOption == STO_ONMINCLOSE)
		{
			MinimizeToTray();
			return; // eat it
		}
	}
	else if (nID == SC_HOTKEY)
		Show(FALSE);
	
	// else
	CFrameWnd::OnSysCommand(nID, lParam);
}

void CToDoListWnd::OnUpdateImport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!GetToDoCtrl().IsReadOnly());
}

UINT CToDoListWnd::MapNewTaskPos(int nPos, BOOL bSubtask)
{
	if (!bSubtask) // task
	{
		switch (nPos)
		{
		case PUIP_TOP:		return ID_NEWTASK_ATTOP;
		case PUIP_BOTTOM:	return ID_NEWTASK_ATBOTTOM;
		case PUIP_BELOW:	return ID_NEWTASK_AFTERSELECTEDTASK;
			
		case PUIP_ABOVE: 
		default:			return ID_NEWTASK_BEFORESELECTEDTASK;
		}
	}
	else // subtask
	{
		if (nPos == PUIP_BOTTOM)
			return ID_NEWSUBTASK_ATBOTTOM;
		else
			return ID_NEWSUBTASK_ATTOP;
	}
}

UINT CToDoListWnd::GetNewTaskCmdID()
{
	return MapNewTaskPos(Prefs().GetNewTaskPos(), FALSE);
}

UINT CToDoListWnd::GetNewSubtaskCmdID()
{
	return MapNewTaskPos(Prefs().GetNewSubtaskPos(), TRUE);
}

void CToDoListWnd::OnNewtask() 
{
	// convert to users choice
	SendMessage(WM_COMMAND, GetNewTaskCmdID());
}

void CToDoListWnd::OnUpdateNewtask(CCmdUI* pCmdUI) 
{
	switch (GetNewTaskCmdID())
	{
	case ID_NEWTASK_ATTOPSELECTED:
		OnUpdateNewtaskAttopSelected(pCmdUI);
		break;

	case ID_NEWTASK_ATBOTTOMSELECTED:	
		OnUpdateNewtaskAtbottomSelected(pCmdUI);
		break;
	
	case ID_NEWTASK_AFTERSELECTEDTASK:
		OnUpdateNewtaskAfterselectedtask(pCmdUI);
		break;

	case ID_NEWTASK_BEFORESELECTEDTASK:
		OnUpdateNewtaskBeforeselectedtask(pCmdUI);
		break;

	case ID_NEWTASK_ATTOP:
		OnUpdateNewtaskAttop(pCmdUI);
		break;

	case ID_NEWTASK_ATBOTTOM:
		OnUpdateNewtaskAtbottom(pCmdUI);
		break;
	}
}

void CToDoListWnd::OnNewsubtask() 
{
	// convert to users choice
	SendMessage(WM_COMMAND, GetNewSubtaskCmdID());
}

void CToDoListWnd::OnUpdateNewsubtask(CCmdUI* pCmdUI) 
{
	switch (GetNewSubtaskCmdID())
	{
	case ID_NEWSUBTASK_ATTOP:
		OnUpdateNewsubtaskAttop(pCmdUI);
		break;

	case ID_NEWSUBTASK_ATBOTTOM:
		OnUpdateNewsubtaskAtBottom(pCmdUI);
		break;
	}
}

void CToDoListWnd::OnToolsCheckout() 
{
	int nSel = GetSelToDoCtrl();

	// sanity check
	if (!m_mgrToDoCtrls.CanCheckOut(nSel))
		return;
	
	CAutoFlag af(m_bSaving, TRUE);

	CString sCheckedOutTo;
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);
	
	if (tdc.CheckOut(sCheckedOutTo))
	{
		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nSel);
		m_mgrToDoCtrls.SetLastCheckoutStatus(nSel, TRUE);
		m_mgrToDoCtrls.RefreshLastModified(nSel);
		
		UpdateCaption();

		// update menu icon mgr
		m_mgrMenuIcons.ChangeImageID(ID_TOOLS_CHECKOUT, ID_TOOLS_CHECKIN);
	}
	else // failed
	{
		m_mgrToDoCtrls.SetLastCheckoutStatus(nSel, FALSE);
		
		// notify user
		CString sMessage;
		
		if (!sCheckedOutTo.IsEmpty())
			sMessage.Format(IDS_CHECKEDOUTBYOTHER, tdc.GetFilePath(), sCheckedOutTo);
		else
			// if sCheckedOutTo is empty then the error is most likely because
			// the file has been deleted or cannot be opened for editing
			sMessage.Format(IDS_CHECKOUTFAILED, tdc.GetFilePath());
		
		MessageBox(sMessage, IDS_CHECKOUT_TITLE, MB_OK | MB_ICONEXCLAMATION);
	}
}

void CToDoListWnd::OnUpdateToolsCheckout(CCmdUI* pCmdUI) 
{
	int nSel = GetSelToDoCtrl();
	
	pCmdUI->Enable(m_mgrToDoCtrls.CanCheckOut(nSel));
}

void CToDoListWnd::OnToolsToggleCheckin() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (tdc.IsCheckedOut())
		OnToolsCheckin();
	else
		OnToolsCheckout();
}

void CToDoListWnd::OnUpdateToolsToggleCheckin(CCmdUI* pCmdUI) 
{
	int nSel = GetSelToDoCtrl();
	BOOL bEnable = m_mgrToDoCtrls.CanCheckInOut(nSel);
	
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && GetToDoCtrl().IsCheckedOut() ? 1 : 0);
}

void CToDoListWnd::OnToolsCheckin() 
{
	int nSel = GetSelToDoCtrl();

	// sanity check
	if (!m_mgrToDoCtrls.CanCheckIn(nSel))
		return;
	
	CAutoFlag af(m_bSaving, TRUE);
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);

	ASSERT (!tdc.GetFilePath().IsEmpty());
	
	tdc.Flush();
	
	// save modifications
	if (tdc.IsModified())
	{
		if (Prefs().GetConfirmSaveOnExit())
		{
			CString sName = m_mgrToDoCtrls.GetFriendlyProjectName(nSel);
			CEnString sMessage(IDS_SAVEBEFORECLOSE, sName);
			
			int nRet = MessageBox(sMessage, IDS_CHECKIN_TITLE, MB_YESNOCANCEL);
			
			switch (nRet)
			{
			case IDYES:
				SaveTaskList(nSel);
				break;
				
			case IDNO:
				ReloadTaskList(nSel, FALSE);
				break;
				
			case IDCANCEL:
				return;
			}
		}
		else
			SaveTaskList(nSel); 
	}
	
	if (tdc.CheckIn())	
	{
		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nSel);
		m_mgrToDoCtrls.RefreshLastModified(nSel);
		UpdateCaption();

		// update menu icon mgr
		m_mgrMenuIcons.ChangeImageID(ID_TOOLS_CHECKIN, ID_TOOLS_CHECKOUT);
	}

	m_mgrToDoCtrls.SetLastCheckoutStatus(nSel, TRUE); // so we won't try to immediately check it out again
}

void CToDoListWnd::OnUpdateToolsCheckin(CCmdUI* pCmdUI) 
{
	int nSel = GetSelToDoCtrl();
	
	pCmdUI->Enable(m_mgrToDoCtrls.CanCheckIn(nSel));
}

void CToDoListWnd::OnExport() 
{
	const CPreferencesDlg& prefs = Prefs();
	
	int nTDCCount = GetTDCCount(), nSelTDC = GetSelToDoCtrl();
	ASSERT (nTDCCount >= 1);
	
	CExportDlg dialog(m_mgrImportExport, nTDCCount == 1, 
						m_mgrToDoCtrls.GetFilePath(nSelTDC, FALSE), 
						prefs.GetAutoExportFolderPath());
	
	if (dialog.DoModal() != IDOK)
		return;
	
	int nFormat = dialog.GetExportFormat();
	BOOL bHtmlComments = (nFormat == EXP2HTML);

	CString sExportPath = dialog.GetExportPath();
	
	BOOL bIncDone = dialog.GetWantCompletedTasks();
	BOOL bIncNotDone = dialog.GetWantInCompleteTasks();
	BOOL bSelTaskOnly = dialog.GetWantSelectedTasks();
	BOOL bSelSubtasks = dialog.GetWantSelectedSubtasks();
	
	// export
	CWaitCursor cursor;
	CTaskFile tasks;
	
	if (nTDCCount == 1 || !dialog.GetExportAllTasklists())
	{
		// Note: don't need to verify password if encrypted tasklist is active
		GetTasks(nSelTDC, bIncDone, bIncNotDone, bSelTaskOnly, 
				 bSelSubtasks, bHtmlComments, FALSE, tasks);

		// add project name as report title
		CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nSelTDC);
		tasks.SetReportAttributes(sTitle);
		
		if (m_mgrImportExport.ExportTaskList(&tasks, sExportPath, nFormat))
		{
			// and preview
			if (prefs.GetPreviewExport())
				ShellExecute(*this, NULL, sExportPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	else // multiple tasklists
	{
		CString sExt = m_mgrImportExport.GetExporterFileExtension(nFormat);
		
		for (int nCtrl = 0; nCtrl < nTDCCount; nCtrl++)
		{
			// verify password
			if (nCtrl != nSelTDC && !VerifyToDoCtrlPassword(nCtrl))
				continue;

			tasks.Reset();

			GetTasks(nCtrl, bIncDone, bIncNotDone, bSelTaskOnly, 
					bSelSubtasks, bHtmlComments, FALSE, tasks);

			// add project name as report title
			CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nCtrl);
			tasks.SetReportAttributes(sTitle);
			
			// build filepath if required (if exporter has an extension)
			char szFilePath[MAX_PATH] = "";

			if (!sExt.IsEmpty())
			{
				CString sPath = m_mgrToDoCtrls.GetFilePath(nCtrl);
				char szFName[_MAX_FNAME];

				// if the file has not been saved before we use the tab text
				// and prompt the user to confirm
				if (sPath.IsEmpty())
				{
					sPath = m_mgrToDoCtrls.GetFilePath(nCtrl, FALSE);
					_makepath(szFilePath, NULL, sExportPath, sPath, sExt);

					CFileDialogEx dlgFile(FALSE, sExt, szFilePath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
											m_mgrImportExport.GetExporterFileFilter(nFormat));

					if (dlgFile.DoModal() == IDOK)
						strcpy(szFilePath, dlgFile.GetPathName());
					else
						continue; // next tasklist
				}
				else
				{
					_splitpath(sPath, NULL, NULL, szFName, NULL);
					_makepath(szFilePath, NULL, sExportPath, szFName, sExt);
				}
			}
			
			m_mgrImportExport.ExportTaskList(&tasks, szFilePath, nFormat);
		}
	}
}

int CToDoListWnd::GetTasks(int nTDC, BOOL bWantCompleted, BOOL bWantIncomplete,
                           BOOL bWantSelected, BOOL bWantSelSubtasks, BOOL bHtmlComments,
						   BOOL bTransform, CTaskFile& tasks)
{
	// preferences
	const CPreferencesDlg& prefs = Prefs();
	
	BOOL bVisColsOnly = prefs.GetExportVisibleColsOnly();
	BOOL bParentTitleCommentsOnly = prefs.GetExportParentTitleCommentsOnly();
	
	DWORD dwFlags = TDCGT_ISODATES;
	
	if (bVisColsOnly && !bTransform)
		dwFlags |= TDCGT_VISIBLECOLSONLY;

	if (bParentTitleCommentsOnly)
		dwFlags |= TDCGT_PARENTTITLECOMMENTSONLY;

	if (bWantSelected && !bWantSelSubtasks) 
		dwFlags |= TDCGT_NOTSUBTASKS;

	if (bHtmlComments)
		dwFlags |= TDCGT_HTMLCOMMENTS;

	if (bTransform)
		dwFlags |= TDCGT_TRANSFORM;
	
	TDC_FILTER nFilter = TDCF_ALL;
	
	// build filter
	if (!bWantSelected)
	{
		if (bWantCompleted && !bWantIncomplete)
			nFilter = TDCF_DONE;
		
		else if (!bWantCompleted && bWantIncomplete)
			nFilter = TDCF_NOTDONE;
		
		// else nFilter = TDCF_ALL;
	}   
	
	ASSERT (nFilter != TDCF_NONE);
	
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);
	tdc.Flush();
	
	if (bWantSelected)
		tdc.GetSelectedTasks(tasks, dwFlags);
	else
		tdc.GetTasks(tasks, nFilter, dwFlags);
	
	// project name
	tasks.SetProjectName(tdc.GetFriendlyProjectName());
	tasks.SetCharSet(prefs.GetHtmlCharSet());
	
	return tasks.GetTaskCount();
}

void CToDoListWnd::OnUpdateExport(CCmdUI* pCmdUI) 
{
	// make sure at least one control has items
	int nCtrl = GetTDCCount();
	
	while (nCtrl--)
	{
		if (GetToDoCtrl().GetTaskCount())
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	
	// else
	pCmdUI->Enable(FALSE);
}

void CToDoListWnd::OnToolsTransformactivetasklist() 
{
	// pass the project name as the title field
	CString sDate, sTitle = GetToDoCtrl().GetProjectName();

	CTDLTransformDialog dialog(sTitle);
	
	if (dialog.DoModal() != IDOK)
		return;
	
	BOOL bIncDone = dialog.GetWantCompletedTasks();
	BOOL bIncNotDone = dialog.GetWantInCompleteTasks();
	BOOL bSelTaskOnly = dialog.GetWantSelectedTasks();
	BOOL bSelSubtasks = dialog.GetWantSelectedSubtasks();
	CString sStylesheet = dialog.GetStylesheet();
	BOOL bDate = dialog.GetWantDate();
	sTitle = dialog.GetTitle();
	
	// output path
	CString sOutputPath(GetToDoCtrl().GetFilePath()); 
	{
		if (!sOutputPath.IsEmpty())
			FileMisc::ReplaceExtension(sOutputPath.GetBuffer(MAX_PATH), "html");

		CEnString sTitle(IDS_SAVETRANSFORM_TITLE), sFilter(IDS_TRANSFORMFILEFILTER);
		CFileDialogEx dialog(FALSE, "html", sOutputPath, OFN_OVERWRITEPROMPT, sFilter, this);
		
		dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
		
		if (dialog.DoModal() != IDOK)
			return; // user elected not to proceed
		
		sOutputPath = dialog.GetPathName();
	}
	
	// export
	CWaitCursor cursor;
	CTaskFile tasks;
	CString sOutput;
	
	GetTasks(GetSelToDoCtrl(), bIncDone, bIncNotDone, bSelTaskOnly, 
			 bSelSubtasks, TRUE, TRUE, tasks);
	
	// add title and date 
	if (bDate)
		sDate = COleDateTime::GetCurrentTime().Format(VAR_DATEVALUEONLY);

	tasks.SetReportAttributes(sTitle, sDate);
	
	if (tasks.Transform(sStylesheet, sOutput, Prefs().GetHtmlCharSet()))
	{
		CFile output;
		
		if (output.Open(sOutputPath, CFile::modeCreate | CFile::modeWrite))
		{
			output.Write((void*)(LPCTSTR)sOutput, sOutput.GetLength());
			output.Close();
			
			// and preview
			if (Prefs().GetPreviewExport())
				ShellExecute(*this, NULL, sOutputPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CToDoListWnd::OnNexttopleveltask() 
{
	GetToDoCtrl().GotoTopLevelTask(TDCG_NEXT);
}

void CToDoListWnd::OnPrevtopleveltask() 
{
	GetToDoCtrl().GotoTopLevelTask(TDCG_PREV);
}

void CToDoListWnd::OnUpdateNexttopleveltask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanGotoTopLevelTask(TDCG_NEXT));
}

void CToDoListWnd::OnUpdatePrevtopleveltask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanGotoTopLevelTask(TDCG_PREV));
}

//------------------------------------------------------------------------

void CToDoListWnd::OnFindTasks() 
{
	if (!m_findDlg.GetSafeHwnd())
		VERIFY(m_findDlg.Initialize(this));
	
	if (IsWindowVisible())
	{
		// remove results from encrypted tasklists but not from the 
		// active tasklist
		if (!m_findDlg.IsWindowVisible())
		{
			int nSelTDC = GetSelToDoCtrl();
			int nTDC = GetTDCCount();

			while (nTDC--)
			{
				if (nTDC != nSelTDC && GetToDoCtrl(nTDC).IsEncrypted())
					m_findDlg.DeleteResults(nTDC);
			}
		}
		m_findDlg.Show();
	}
	
	m_bFindShowing = TRUE;
}

int CToDoListWnd::MapFindWhat(FIND_WHAT fw)
{
	switch (fw)
	{
	case FW_TITLECOMMENTS: return FIND_TITLECOMMENTS;
	case FW_PRIORITY: return FIND_PRIORITY;
	case FW_PERCENTDONE: return FIND_PERCENTDONE;
	case FW_ALLOCTO: return FIND_ALLOCTO;
	case FW_ALLOCBY: return FIND_ALLOCBY;
	case FW_STATUS: return FIND_STATUS;
	case FW_CATEGORY: return FIND_CATEGORY;
	case FW_TIMEEST: return FIND_TIMEEST;
	case FW_TIMESPENT: return FIND_TIMESPENT;
	case FW_STARTDATE: return FIND_STARTDATE;
	case FW_DUEDATE: return FIND_DUEDATE;
	case FW_DONEDATE: return FIND_DONEDATE;
	case FW_TASKID: return FIND_TASKID;
	case FW_FLAG: return FIND_FLAG;
	case FW_CREATIONDATE: return FIND_CREATIONDATE;
	case FW_CREATEDBY: return FIND_CREATEDBY;
	case FW_LASTMOD: return FIND_LASTMOD;
	case FW_RISK: return FIND_RISK;
	case FW_EXTERNALID: return FIND_EXTERNALID;
	case FW_COST: return FIND_COST;
	case FW_VERSION: return FIND_VERSION;
	}
	
	ASSERT(0);
	return -1;
}

LRESULT CToDoListWnd::OnFindDlgClose(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_bFindShowing = FALSE;
	return 0L;
}

LRESULT CToDoListWnd::OnFindDlgFind(WPARAM /*wp*/, LPARAM /*lp*/)
{
	// bit of a hack but we keep track of the previous
	// results using a static array
	static CFTDResultsArray aPrevResults;
	
	// set up the search
	BOOL bSearchAll = m_findDlg.GetSearchAllTasklists();
	int nSelTaskList = GetSelToDoCtrl();
	
	int nFrom = bSearchAll ? 0 : nSelTaskList;
	int nTo = bSearchAll ? GetTDCCount() - 1 : nSelTaskList;
	
	// search params
	SEARCHPARAMS sp;
	
	sp.dwFlags = m_findDlg.GetIncludeDone() ? FIND_INCLUDEDONE : 0;
	sp.nFindWhat = MapFindWhat(m_findDlg.GetFindWhat());
	
	// find specific params
	switch (sp.nFindWhat)
	{
	case FIND_TITLECOMMENTS:
	case FIND_ALLOCTO:
	case FIND_ALLOCBY:
	case FIND_STATUS:
	case FIND_EXTERNALID:
	case FIND_CREATEDBY:
	case FIND_VERSION:
		sp.dwFlags |= m_findDlg.GetMatchCase() ? FIND_MATCHCASE : 0;
		sp.dwFlags |= m_findDlg.GetMatchWholeWord() ? FIND_MATCHWHOLEWORD : 0;
		sp.sText = m_findDlg.GetText();
		break;

	case FIND_CATEGORY:
		sp.dwFlags |= m_findDlg.GetMatchAllCategories() ? FIND_MATCHALLCATS : 0;
		m_findDlg.GetCategories(sp.aCategories);
		break;
		
	case FIND_RISK:
	case FIND_PRIORITY:
	case FIND_PERCENTDONE:
	case FIND_TASKID:
		m_findDlg.GetRange(sp.nFrom, sp.nTo);
		break;
		
	case FIND_TIMEEST:
	case FIND_TIMESPENT:
	case FIND_COST:
		m_findDlg.GetRange(sp.dFrom, sp.dTo);
		break;
		
	case FIND_CREATIONDATE:
	case FIND_STARTDATE:
	case FIND_DUEDATE:
	case FIND_DONEDATE:
	case FIND_LASTMOD:
		m_findDlg.GetRange(sp.dateFrom, sp.dateTo);
		break;
		
	case FIND_FLAG:
		break; // no additional info req
		
	default:
		ASSERT(0);
		return 0;
	}
	
	// do the search and populate the results
	{
		
		if (!m_findDlg.GetSearchResults())
		{
			int nSel = GetSelToDoCtrl();
			
			for (int nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
			{
				// verify password unless tasklist is already active
				if (nCtrl != nSel && !VerifyToDoCtrlPassword(nCtrl))
					continue;
				
				CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
				CResultArray aResults;
				
				CHoldRedraw hr(m_findDlg);
				
				if (tdc.FindTasks(sp, aResults))
				{
					// use tasklist title from tabctrl
					CString sTitle;
					
					if (bSearchAll)
						sTitle = m_mgrToDoCtrls.GetTabItemText(nCtrl);
					
					for (int nResult = 0; nResult < aResults.GetSize(); nResult++)
						AddFindResult(aResults[nResult], sp, nCtrl, sTitle);
				}
			}
		}
		else // search on previous results
		{
			int nNumRes = aPrevResults.GetSize();
			
			for (int nRes = 0; nRes < nNumRes; nRes++)
			{
				const FTDRESULT& resPrev = aPrevResults[nRes];
				int nCtrl = resPrev.nTaskList;
				
				SEARCHRESULT sr;
				
				if (GetToDoCtrl(nCtrl).TaskMatches(resPrev.dwItemData, sp, sr))
				{
					CString sTitle;
					
					if (bSearchAll)
						sTitle = m_mgrToDoCtrls.GetTabItemText(nCtrl);
					
					AddFindResult(sr, sp, nCtrl, sTitle);
				}
			}
		}
		
		// re-snapshot the results
		m_findDlg.GetAllResults(aPrevResults);
	}	
	
	// auto-select single results
	if (m_findDlg.GetResultCount() == 1 && m_findDlg.GetAutoSelectSingles())
	{
		POSITION pos = m_findDlg.GetFirstResultPos();
		ASSERT (pos);
		
		if (pos)
		{
			FTDRESULT result;
			VERIFY(m_findDlg.GetNextResult(pos, result));
			
			if (OnFindSelectResult(0, (LPARAM)&result))
				m_findDlg.Show(FALSE);	
		}
	}
	
	return 0;
}

void CToDoListWnd::AddFindResult(const SEARCHRESULT& result, const SEARCHPARAMS& params, 
								 int nTaskList, LPCTSTR szTaskList)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nTaskList);
	HTREEITEM htiRes = result.hti;
	CString sPath;
				
	if (szTaskList && *szTaskList)
		sPath.Format("%s / %s", szTaskList, tdc.GetItemPath(htiRes));
	else
		sPath = tdc.GetItemPath(htiRes);
	
	// format the match string
	CString sMatch;
	
	switch (params.nFindWhat)
	{
	case FIND_ALLOCTO:
	case FIND_ALLOCBY:
	case FIND_STATUS:
	case FIND_EXTERNALID:
	case FIND_CATEGORY:
	case FIND_CREATEDBY:
	case FIND_VERSION:
		sMatch = result.sMatch;
		break;
		
	case FIND_PRIORITY:
	case FIND_RISK:
	case FIND_PERCENTDONE:
	case FIND_TASKID:
		sMatch.Format("%d", result.nMatch);
		break;
		
	case FIND_TIMEEST:
	case FIND_TIMESPENT:
	case FIND_COST:
		sMatch.Format("%0.2f", result.dMatch);
		break;
		
	case FIND_CREATIONDATE:
	case FIND_STARTDATE:
	case FIND_DUEDATE:
	case FIND_DONEDATE:
		sMatch = CDateHelper::FormatDate(result.dateMatch, 
			Prefs().GetDisplayDatesInISO(),	
			Prefs().GetShowWeekdayInDates());
		break;
	}
				
	m_findDlg.AddResult(tdc.GetItemText(htiRes), sMatch, sPath, result.bDone, result.dwID, nTaskList);
}

LRESULT CToDoListWnd::OnFindSelectResult(WPARAM /*wp*/, LPARAM lp)
{
	// extract HTREEITEM
	FTDRESULT* pResult = (FTDRESULT*)lp;
	ASSERT (pResult->dwItemData); // HTREEITEM
	
	if (m_tabCtrl.GetCurSel() != pResult->nTaskList)
	{
		if (!SelectToDoCtrl(pResult->nTaskList, TRUE))
			return 0L;
	}
	
	CFilteredToDoCtrl& tdc = GetToDoCtrl(pResult->nTaskList);
	tdc.SetFocusToTree();
	
	if (tdc.GetSelectedTaskID() != pResult->dwItemData)
		tdc.SelectTask(pResult->dwItemData);
	
	return 1L;
}

LRESULT CToDoListWnd::OnFindSelectAll(WPARAM /*wp*/, LPARAM /*lp*/)
{
	if (!m_findDlg.GetResultCount())
		return 0;
	
	CWaitCursor cursor;
	
	for (int nTDC = 0; nTDC < GetTDCCount(); nTDC++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);
		tdc.DeselectAll();
		
		// collate the taskIDs
		FTDRESULT result;
		POSITION pos = m_findDlg.GetFirstResultPos();
		CDWordArray aTasks;
		
		while (pos)
		{
			if (m_findDlg.GetNextResult(pos, result) && (nTDC == result.nTaskList))
				aTasks.Add(result.dwItemData);
		}
		
		// select them in one hit
		if (aTasks.GetSize())
			tdc.MultiSelectItems(aTasks, 1, FALSE);
	}
	
	// redraw the active tasklist
	if (m_findDlg.GetResultCount(GetSelToDoCtrl()))
		CRedrawAll(GetToDoCtrl(), TRUE); 
	
	return 0;
}

//------------------------------------------------------------------------

LRESULT CToDoListWnd::OnDropFile(WPARAM /*wParam*/, LPARAM lParam)
{
	LPCTSTR szFile = (LPCTSTR)lParam;
	TDC_FILE nRes = OpenTaskList(szFile);

	HandleLoadTasklistError(nRes, szFile);
	
	return 0;
}

void CToDoListWnd::OnViewMovetasklistright() 
{
	m_mgrToDoCtrls.MoveToDoCtrl(GetSelToDoCtrl(), 1);
}

void CToDoListWnd::OnUpdateViewMovetasklistright(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!Prefs().GetKeepTabsOrdered() &&
		m_mgrToDoCtrls.CanMoveToDoCtrl(GetSelToDoCtrl(), 1));
}

void CToDoListWnd::OnViewMovetasklistleft() 
{
	m_mgrToDoCtrls.MoveToDoCtrl(GetSelToDoCtrl(), -1);
}

void CToDoListWnd::OnUpdateViewMovetasklistleft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!Prefs().GetKeepTabsOrdered() &&
		m_mgrToDoCtrls.CanMoveToDoCtrl(GetSelToDoCtrl(), -1));
}

void CToDoListWnd::OnToolsShowtasksDue(UINT nCmdID) 
{
	int nDueBy = PFP_DUETODAY;
	UINT nIDDueBy = IDS_NODUETODAY;
	
	switch (nCmdID)
	{
	case ID_TOOLS_SHOWTASKS_DUETODAY:
		break; // done
		
	case ID_TOOLS_SHOWTASKS_DUETOMORROW:
		nIDDueBy = IDS_NODUETOMORROW;
		nDueBy = PFP_DUETOMORROW;
		break;
		
	case ID_TOOLS_SHOWTASKS_DUEENDTHISWEEK:
		nIDDueBy = IDS_NODUETHISWEEK;
		nDueBy = PFP_DUETHISWEEK;
		break;
		
	case ID_TOOLS_SHOWTASKS_DUEENDNEXTWEEK:
		nIDDueBy = IDS_NODUENEXTWEEK;
		nDueBy = PFP_DUENEXTWEEK;
		break;
		
	case ID_TOOLS_SHOWTASKS_DUEENDTHISMONTH:
		nIDDueBy = IDS_NODUETHISMONTH;
		nDueBy = PFP_DUETHISMONTH;
		break;
		
	case ID_TOOLS_SHOWTASKS_DUEENDNEXTMONTH:
		nIDDueBy = IDS_NODUENEXTMONTH;
		nDueBy = PFP_DUENEXTMONTH;
		break;
		
	default:
		ASSERT(0);
		return;
	}
	
	if (!DoDueTaskNotification(nDueBy))
	{
		CEnString sMessage(nIDDueBy, m_mgrToDoCtrls.GetFriendlyProjectName(GetSelToDoCtrl()));

		MessageBox(sMessage, IDS_DUETASKS_TITLE);
	}
}

void CToDoListWnd::ResetPrefs()
{
	delete m_pPrefs;
	m_pPrefs = new CPreferencesDlg(&m_mgrShortcuts, IDR_MAINFRAME, &m_mgrContent);
	
	// update
	m_mgrToDoCtrls.SetPrefs(m_pPrefs); 
	
	// grab current colors
	Prefs().GetPriorityColors(m_aPriorityColors);
	
/*
	if (m_cbPriorityFilter.GetSafeHwnd())
	{
		m_cbPriorityFilter.ResetContent();
		RefreshFilterControls();
	}
*/

	m_filterBar.SetPriorityColors(m_aPriorityColors);
	m_findDlg.SetPriorityColors(m_aPriorityColors);
}

CPreferencesDlg& CToDoListWnd::Prefs() const
{
	ASSERT (m_pPrefs);
	return *m_pPrefs;
}

void CToDoListWnd::OnNewtaskAttop() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP));
}

void CToDoListWnd::OnUpdateNewtaskAttop(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.IsReadOnly());
}

void CToDoListWnd::OnNewtaskAtbottom() 
{
	VERIFY (NewTask(CEnString(IDS_TASK), TDC_INSERTATBOTTOM));
}

void CToDoListWnd::OnUpdateNewtaskAtbottom(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.IsReadOnly());
}

void CToDoListWnd::OnSpellcheckcomments() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SpellcheckSelectedTask(FALSE);
}

void CToDoListWnd::OnUpdateSpellcheckcomments(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(tdc.CanSpellcheckSelectedTaskComments());
}

void CToDoListWnd::OnSpellchecktitle() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SpellcheckSelectedTask(TRUE);
}

void CToDoListWnd::OnUpdateSpellchecktitle(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.GetSelectedTaskTitle().IsEmpty());
}

void CToDoListWnd::OnFileEncrypt() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (!tdc.IsReadOnly())
	{
		// if the tasklist is already encrypted then verify password
		// before allowing change
		if (!tdc.IsEncrypted() || VerifyToDoCtrlPassword())
			tdc.EnableEncryption(!tdc.IsEncrypted());
	}
}

void CToDoListWnd::OnUpdateFileEncrypt(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(tdc.CanEncrypt() && !tdc.IsReadOnly());
	pCmdUI->SetCheck(tdc.IsEncrypted() ? 1 : 0);
}

void CToDoListWnd::OnFileResetversion() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	if (!tdc.IsReadOnly())
	{
		tdc.ResetFileVersion();
		tdc.SetModified();
		
		UpdateStatusbar();
		UpdateCaption();
	}
}

void CToDoListWnd::OnUpdateFileResetversion(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.IsReadOnly());
}

void CToDoListWnd::OnSpellcheckTasklist() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.Spellcheck();
}

void CToDoListWnd::OnUpdateSpellcheckTasklist(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(tdc.GetTaskCount());
}

BOOL CToDoListWnd::SaveAll(BOOL bIncUnsaved, BOOL bClosingTaskLists, BOOL bClosingWindows, BOOL bFlush)
{
	int nCtrl = GetTDCCount();
	
	while (nCtrl--)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

		// bypass unsaved tasklists
		if (!bIncUnsaved && tdc.GetFilePath().IsEmpty())
			continue;
		
		if (bFlush)
			tdc.Flush();
		
		if (!ConfirmSaveTaskList(nCtrl, bClosingTaskLists, bClosingWindows))
			return FALSE; // user cancelled
		else
			m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
	}
	
	UpdateCaption();
    return TRUE;
}

void CToDoListWnd::OnEditTimeTrackTask() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.TimeTrackSelectedTask();
}

void CToDoListWnd::OnUpdateEditTimeTrackTask(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(tdc.CanTimeTrackSelectedTask());
	pCmdUI->SetCheck(tdc.IsSelectedTaskBeingTimeTracked() ? 1 : 0);
}

void CToDoListWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_TABCONTROL)
	{
		if (m_mgrToDoCtrls.GetDueItemStatus(lpDrawItemStruct->itemID))
		{
			CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

			// draw a little tag in the top left corner in the colour
			// of the highest priority
			const int DUECOLORINDEX = 11;
			COLORREF crTag = m_aPriorityColors[DUECOLORINDEX];

			for (int nHPos = 0; nHPos < 6; nHPos++)
			{
				for (int nVPos = 0; nVPos < 6 - nHPos; nVPos++)
				{
					pDC->SetPixelV(lpDrawItemStruct->rcItem.left + nHPos,
									lpDrawItemStruct->rcItem.top + nVPos, crTag);
				}
			}
		}
		return;
	}
	else if (nIDCtl == 0 && lpDrawItemStruct->itemID == ID_CLOSE)
	{
		if (CMenuEx::DrawMDIButton(IDB_XPCLOSE, lpDrawItemStruct, TRUE, RGB(255, 0, 255))) 
			return;
	}

	CFrameWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
} 

void CToDoListWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (nIDCtl == 0 && lpMeasureItemStruct->itemID == ID_CLOSE)
	{
		if (CMenuEx::MeasureMDIButton(IDB_XPCLOSE, lpMeasureItemStruct))
			return;
	}
	
	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CToDoListWnd::OnViewNextSel() 
{
	GetToDoCtrl().SelectNextTasksInHistory();
}

void CToDoListWnd::OnUpdateViewNextSel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanSelectNextTasksInHistory());
}

void CToDoListWnd::OnViewPrevSel() 
{
	GetToDoCtrl().SelectPrevTasksInHistory();
}

void CToDoListWnd::OnUpdateViewPrevSel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanSelectPrevTasksInHistory());
}

void CToDoListWnd::OnSplitTaskIntoPieces(UINT nCmdID) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nNumPieces = 2 + (nCmdID - ID_NEWTASK_SPLITTASKINTO_TWO);
	
	tdc.SplitSelectedTask(nNumPieces);
}

void CToDoListWnd::OnUpdateSplitTaskIntoPieces(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(tdc.CanSplitSelectedTask());
}

void CToDoListWnd::OnViewExpandtask() 
{
	GetToDoCtrl().ExpandSelectedTask(TRUE);
}

void CToDoListWnd::OnUpdateViewExpandtask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanExpandSelectedTask(TRUE));
}

void CToDoListWnd::OnViewCollapsetask() 
{
	GetToDoCtrl().ExpandSelectedTask(FALSE);
}

void CToDoListWnd::OnUpdateViewCollapsetask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanExpandSelectedTask(FALSE));
}

void CToDoListWnd::OnViewCollapseall() 
{
	GetToDoCtrl().ExpandAllTasks(FALSE);
}

void CToDoListWnd::OnViewExpandall() 
{
	GetToDoCtrl().ExpandAllTasks(TRUE);
}

void CToDoListWnd::OnUpdateViewExpandall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}

void CToDoListWnd::OnUpdateViewCollapseall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}

void CToDoListWnd::OnUpdateUserExport1(CCmdUI* pCmdUI) 
{
	// check that this is not occurring because our CFrameWnd
	// base class is routing this to the first item in a submenu
	if (pCmdUI->m_pMenu && 
		pCmdUI->m_pMenu->GetMenuItemID(pCmdUI->m_nIndex) == (UINT)-1)
		return;

	m_mgrImportExport.UpdateExportMenu(pCmdUI, 8, GetToDoCtrl().GetTaskCount());
}

void CToDoListWnd::OnUserExport(UINT nCmdID) 
{
	int nExp = nCmdID - ID_TOOLS_USEREXPORT1;

	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	// get user to pick a file path. base default on current tasklist
	CString sOutputFile = tdc.GetFilePath();

	if (!sOutputFile.IsEmpty())
	{
		FileMisc::ReplaceExtension(sOutputFile.GetBuffer(MAX_PATH), 
							m_mgrImportExport.GetExporterFileExtension(nExp));
		sOutputFile.ReleaseBuffer();
	}

	CFileDialogEx dialog(FALSE, 
		m_mgrImportExport.GetExporterFileExtension(nExp), 
		sOutputFile, 
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
		m_mgrImportExport.GetExporterFileFilter(nExp), 
		this);
	
	CEnString sTitle(IDS_EXPORTTASKLIST_TITLE);
	dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
	
	if (dialog.DoModal() == IDOK)
	{
		sOutputFile = dialog.GetPathName();

		CWaitCursor cursor;
		CTaskFile tasks;

		int nSelTDC = GetSelToDoCtrl();
		GetTasks(nSelTDC, TRUE, TRUE, FALSE, FALSE, (nExp == EXP2HTML), FALSE, tasks);
		
		// add project name as report title
		CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nSelTDC);
		tasks.SetReportAttributes(sTitle);
		
		if (m_mgrImportExport.ExportTaskList(&tasks, sOutputFile, nExp))
		{
			UpdateWindow();
			
			// and preview
			if (Prefs().GetPreviewExport())
				ShellExecute(*this, NULL, sOutputFile, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CToDoListWnd::OnUpdateUserImport1(CCmdUI* pCmdUI) 
{
	// check that this is not occurring because our CFrameWnd
	// base class is routing this to the first item in a submenu
	if (pCmdUI->m_pMenu && 
		pCmdUI->m_pMenu->GetMenuItemID(pCmdUI->m_nIndex) == (UINT)-1)
		return;

	m_mgrImportExport.UpdateImportMenu(pCmdUI, 8);
}

void CToDoListWnd::OnUserImport(UINT nCmdID) 
{
	int nImp = nCmdID - ID_TOOLS_USERIMPORT1;
	
	// get user to pick a file path
	CFileDialogEx dialog(TRUE, 
		m_mgrImportExport.GetImporterFileExtension(nImp), 
		NULL, 
		OFN_HIDEREADONLY, 
		m_mgrImportExport.GetImporterFileFilter(nImp), 
		this);
	
	CEnString sTitle(IDS_IMPORTTASKLIST_TITLE);
	dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;
	
	if (dialog.DoModal() == IDOK)
	{
		CWaitCursor cursor;
		CTaskFile tasks;
		
		m_mgrImportExport.ImportTaskList(dialog.GetPathName(), &tasks, nImp);
		
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		
		if (tdc.SetTasks(tasks, TRUE))
			UpdateCaption();
	}
}

void CToDoListWnd::OnWindow(UINT nCmdID) 
{
	int nTDC = (int)(nCmdID - ID_WINDOW1);
	
	if (nTDC < GetTDCCount())
	{
		if (SelectToDoCtrl(nTDC, (nTDC != GetSelToDoCtrl())))
			DoDueTaskNotification(Prefs().GetNotifyDueByOnSwitch());
	}
}

void CToDoListWnd::OnUpdateWindow(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pMenu)
	{
		ASSERT (ID_WINDOW1 == pCmdUI->m_nID);
		const UINT MAXWINDOWS = 16;
		
		// delete existing tool entries first
		for (int nWnd = 0; nWnd < MAXWINDOWS; nWnd++)
			pCmdUI->m_pMenu->DeleteMenu(ID_WINDOW1 + nWnd, MF_BYCOMMAND);
		
		int nSel = GetSelToDoCtrl();
		int nPos = 0, nTDCCount = GetTDCCount();
		ASSERT (nTDCCount);
		
		for (nWnd = 0; nWnd < nTDCCount; nWnd++)
		{
			CFilteredToDoCtrl& tdc = GetToDoCtrl(nWnd);
			
			CString sMenuItem;
			
			if (nPos < 9)
				sMenuItem.Format("&%d %s", nPos + 1, tdc.GetFriendlyProjectName());
			else
				sMenuItem = tdc.GetFriendlyProjectName();
			
			UINT nFlags = MF_BYPOSITION | MF_STRING | (nSel == nWnd ? MF_CHECKED : MF_UNCHECKED);
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++, nFlags, ID_WINDOW1 + nWnd, sMenuItem);
			
			nPos++;
		}
		
		// update end menu count
		pCmdUI->m_nIndex--; // point to last menu added
		pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
		
		pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
	}
}

void CToDoListWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	// don't activate when in the middle of loading
	if (m_bReloading && nState != WA_ACTIVE)
		return;

	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if (nState == WA_INACTIVE)
		m_hwndLastFocus = ::GetFocus();
	else
	{
		if (GetTDCCount() && (!m_hwndLastFocus || Prefs().GetAutoFocusTasklist()))
			GetToDoCtrl().SetFocusToTree();
		else
			::SetFocus(m_hwndLastFocus);

		UpdateCwd();
	}
}

void CToDoListWnd::UpdateCwd()
{
	// set cwd to active tasklist
	if (GetTDCCount())
	{
		CString sFolder;

		if (FileMisc::GetFolderFromFilePath(m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl()), sFolder) &&
			GetFileAttributes(sFolder) != 0xffffffff)
		{
			_chdir(sFolder);
		}
	}
	
}

BOOL CToDoListWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UpdateWindow();

/*
	UINT nID = LOWORD(wParam);

	// flush the active todoctrl for some important commands

	if (nID != ID_EDIT_INSERTDATETIME)
		GetToDoCtrl().Flush();
*/
	
	return CFrameWnd::OnCommand(wParam, lParam);
}

void CToDoListWnd::OnEnable(BOOL bEnable) 
{
	CFrameWnd::OnEnable(bEnable);
	
	// pause time tracking if disabling else enable as usual
	PauseTimeTracking(!bEnable);
}

void CToDoListWnd::OnViewSorttasklisttabs() 
{
	int nSel = m_mgrToDoCtrls.SortToDoCtrlsByName();
	SelectToDoCtrl(nSel, FALSE);
}

void CToDoListWnd::OnUpdateViewSorttasklisttabs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTDCCount() > 1 && !Prefs().GetKeepTabsOrdered());
}


void CToDoListWnd::OnEditInctaskpercentdone() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPercentDone(5);
}

void CToDoListWnd::OnUpdateEditInctaskpercentdone(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());	
}

void CToDoListWnd::OnEditDectaskpercentdone() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPercentDone(-5);
}

void CToDoListWnd::OnUpdateEditDectaskpercentdone(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());	
}

void CToDoListWnd::OnEditDectaskpriority() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPriority(-1);
}

void CToDoListWnd::OnUpdateEditDectaskpriority(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());	
}

void CToDoListWnd::OnEditInctaskpriority() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPriority(1);
}

void CToDoListWnd::OnUpdateEditInctaskpriority(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());	
}

void CToDoListWnd::OnEditFlagtask() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SetSelectedTaskFlag(!tdc.IsSelectedTaskFlagged());
}

void CToDoListWnd::OnUpdateEditFlagtask(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();
	
	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());	
	pCmdUI->SetCheck(tdc.IsSelectedTaskFlagged() ? 1 : 0);
}

void CToDoListWnd::OnFileOpenarchive() 
{
	CString sArchivePath = m_mgrToDoCtrls.GetArchivePath(GetSelToDoCtrl());
	BOOL bArchiveExists = (GetFileAttributes(sArchivePath) != 0xffffffff);
	
	if (bArchiveExists)
		OpenTaskList(sArchivePath, FALSE);
}

void CToDoListWnd::OnUpdateFileOpenarchive(CCmdUI* pCmdUI) 
{
	CString sArchivePath = m_mgrToDoCtrls.GetArchivePath(GetSelToDoCtrl());
	BOOL bArchiveExists = (GetFileAttributes(sArchivePath) != 0xffffffff);
	
	pCmdUI->Enable(bArchiveExists);
}

LRESULT CToDoListWnd::OnSelchangeFilter(WPARAM /*wp*/, LPARAM lp) 
{
	FTDCFILTER* pFilter = (FTDCFILTER*)lp;
	ASSERT(pFilter);
	
	GetToDoCtrl().SetFilter(*pFilter);
	UpdateStatusbar();

	return 0L;
}

void CToDoListWnd::OnViewRefreshfilter() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	FTDCFILTER filterTDC, filter;

	tdc.GetFilter(filterTDC);
	m_filterBar.GetFilter(filter);
	
	// if the filter has changed then set the new one else
	// refresh the current one
	if (filterTDC == filter)
		tdc.RefreshFilter();	
	else
		tdc.SetFilter(filter);
	
	UpdateStatusbar();
}

void CToDoListWnd::OnUpdateViewRefreshfilter(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	FTDCFILTER filterTDC, filter;

	tdc.GetFilter(filterTDC);
	m_filterBar.GetFilter(filter);
	
	pCmdUI->Enable(tdc.HasFilter() || (filter != filterTDC));
}

void CToDoListWnd::OnViewShowfilterbar() 
{
	m_bShowFilterBar = !m_bShowFilterBar;

	m_filterBar.ShowWindow(m_bShowFilterBar ? SW_SHOW : SW_HIDE);
	ResizeDlg();
}

void CToDoListWnd::OnUpdateViewShowfilterbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowFilterBar ? 1 : 0);
}

void CToDoListWnd::OnViewClearfilter() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	tdc.ClearFilter();
	RefreshFilterControls();
	
	UpdateStatusbar();
}

void CToDoListWnd::OnUpdateViewClearfilter(CCmdUI* pCmdUI) 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	
	pCmdUI->Enable(tdc.HasFilter());
}

void CToDoListWnd::OnViewFilter() 
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	CFilterDlg dialog(Prefs().GetMultiSelCategoryFilter());
	
	if (dialog.DoModal(tdc, m_aPriorityColors) == IDOK)
	{
		FTDCFILTER filter;
		dialog.GetFilter(filter);
		
		// then set the filter
		m_filterBar.SetFilter(filter);
		tdc.SetFilter(filter);
	}
}

void CToDoListWnd::OnUpdateViewFilter(CCmdUI* pCmdUI) 
{
	UNREFERENCED_PARAMETER(pCmdUI);
	//	pCmdUI->Enable(!m_bShowFilterBar);
}


void CToDoListWnd::OnTabctrlPreferences() 
{
	DoPreferences(PREFPAGE_UI);
}

void CToDoListWnd::OnTasklistSelectColumns() 
{
	DoPreferences(PREFPAGE_UITASK);
}

void CToDoListWnd::OnViewProjectname() 
{
	m_bShowProjectName = !m_bShowProjectName;
	
	// mark all tasklists as needing update
	m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);
	
	// then update active tasklist
	GetToDoCtrl().SetStyle(TDCS_SHOWPROJECTNAME, m_bShowProjectName);
	m_mgrToDoCtrls.SetNeedsPreferenceUpdate(GetSelToDoCtrl(), FALSE);
}

void CToDoListWnd::OnUpdateViewProjectname(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowProjectName ? 1 : 0);
}

void CToDoListWnd::OnEditOffsetdates() 
{
	COffsetDatesDlg dialog;
	
	if (dialog.DoModal() == IDOK)
	{
		ODD_UNITS nUnits;
		int nAmount = dialog.GetOffsetAmount(nUnits);
		
		if (!nAmount)
			return;
		
		DWORD dwWhat = dialog.GetOffsetWhat();
		BOOL bSubtasks = dialog.GetOffsetSubtasks();
		
		// translate units
		int nTDCUnits = (nUnits == ODDU_DAYS) ? TDITU_DAYS :
		((nUnits == ODDU_WEEKS) ? TDITU_WEEKS : TDITU_MONTHS);
		
		// do the offsets
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		
		if (dwWhat & ODD_STARTDATE)
			tdc.OffsetSelectedTaskDate(TDCD_START, nAmount, nTDCUnits, bSubtasks);
		
		if (dwWhat & ODD_DUEDATE)
			tdc.OffsetSelectedTaskDate(TDCD_DUE, nAmount, nTDCUnits, bSubtasks);
		
		if (dwWhat & ODD_DONEDATE)
			tdc.OffsetSelectedTaskDate(TDCD_DONE, nAmount, nTDCUnits, bSubtasks);
	}
}

void CToDoListWnd::OnUpdateEditOffsetdates(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount());
}

void CToDoListWnd::OnPrintpreview() 
{
	DoPrint(TRUE);
}

void CToDoListWnd::OnShowTimelogfile() 
{
	CString sLogPath = GetToDoCtrl().GetSelectedTaskTimeLogPath();
	
	if (!sLogPath.IsEmpty())
		ShellExecute(*this, NULL, sLogPath, NULL, NULL, SW_HIDE);
}

void CToDoListWnd::OnUpdateShowTimelogfile(CCmdUI* pCmdUI) 
{
	const CPreferencesDlg& prefs = Prefs();
	int nTasks = GetToDoCtrl().GetSelectedCount();
	
	pCmdUI->Enable(Prefs().GetLogTimeTracking() && 
		(nTasks == 1 || !prefs.GetLogTaskTimeSeparately()));	
}

LRESULT CToDoListWnd::OnToDoCtrlDoTaskLink(WPARAM wParam, LPARAM lParam)
{
	ASSERT (lParam);
	CString sFile((LPCTSTR)lParam);
	
	if (!sFile.IsEmpty())
	{
		// build the full path to the file
		if (::PathIsRelative(sFile))
		{
			// append it to the folder containing the active tasklist
			CString sPathName = m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl());
			
			char szDrive[_MAX_DRIVE], szFolder[_MAX_DIR];
			_splitpath(sPathName, szDrive, szFolder, NULL, NULL);
			
			char szFullPath[MAX_PATH];
			_makepath(szFullPath, szDrive, szFolder, sFile, NULL);
			
			sFile = szFullPath;
		}
		// else its a full path already
		
		if (OpenTaskList(sFile) == TDCO_SUCCESS && wParam)
			GetToDoCtrl().SelectTask(wParam);
	}
	
	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlTaskIsDone(WPARAM wParam, LPARAM lParam)
{
	ASSERT (lParam);
	CString sFile((LPCTSTR)lParam);
	
	if (!sFile.IsEmpty())
	{
		// build the full path to the file
		if (::PathIsRelative(sFile))
		{
			// append it to the folder containing the active tasklist
			CString sPathName = m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl());
			
			char szDrive[_MAX_DRIVE], szFolder[_MAX_DIR];
			_splitpath(sPathName, szDrive, szFolder, NULL, NULL);
			
			char szFullPath[MAX_PATH];
			_makepath(szFullPath, szDrive, szFolder, sFile, NULL);
			
			sFile = szFullPath;
		}
		// else its a full path already
		
		int nTDC = m_mgrToDoCtrls.FindToDoCtrl(sFile);

		if (nTDC != -1) // already loaded
			return GetToDoCtrl(nTDC).IsTaskDone(wParam);
		else
		{
			// we must load the tasklist ourselves
			CTaskFile tasks;

			if (tasks.Load(sFile))
			{
				HTASKITEM ht = tasks.FindTask(wParam);
				return ht ? tasks.IsTaskDone(ht) : FALSE;
			}
		}
	}
	
	return 0L;
}

LRESULT CToDoListWnd::OnPowerBroadcast(WPARAM wp, LPARAM /*lp*/)
{
	if (wp == PBT_APMQUERYSUSPEND)
	{
		// save all tasklists which have been saved previously
		// note CToDoCtrl::Save() will fail silently if the tasklist
		// has not yet been saved, which is what we want.
		int nCtrl = GetTDCCount();
	
		while (nCtrl--)
		{
			if (TDCO_SUCCESS == GetToDoCtrl(nCtrl).Save())
				m_mgrToDoCtrls.RefreshLastModified(nCtrl);
		}
	}

	return TRUE; // allow 
}

LRESULT CToDoListWnd::OnGetFont(WPARAM /*wp*/, LPARAM /*lp*/)
{
	return (LRESULT)m_fontMain.GetSafeHandle();
}

void CToDoListWnd::OnViewStatusBar() 
{
	m_bShowStatusBar = !m_bShowStatusBar;
	m_statusBar.ShowWindow(m_bShowStatusBar ? SW_SHOW : SW_HIDE);
	
	ResizeDlg();

	if (m_bShowStatusBar)
		UpdateStatusbar();
}

void CToDoListWnd::OnUpdateViewStatusBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowStatusBar ? 1 : 0) ;
}

BOOL CToDoListWnd::OnQueryOpen() 
{
	if (CFrameWnd::OnQueryOpen())
	{
		// fail if the active tasklist is encrypted because we have to verify the password
		if (!m_bQueryOpenAllow && GetToDoCtrl().IsEncrypted())
		{
			PostMessage(WM_TDL_RESTORE); 
			return FALSE;
		}
		
		// all others
		return TRUE;
	}
	
	return FALSE;
}

LRESULT CToDoListWnd::OnToDoListRestore(WPARAM /*wp*/, LPARAM /*lp*/)
{
    ASSERT (IsIconic() && GetToDoCtrl().IsEncrypted()); // sanity check
	
    if (IsIconic())
    {
        if (VerifyToDoCtrlPassword())
		{
			CAutoFlag af(m_bQueryOpenAllow, TRUE);
            ShowWindow(SW_RESTORE);
		}
    }

	return 0L;
}

void CToDoListWnd::OnCopyTaskasRef() 
{
	CopySelectedTasksToClipboard(CT_ASREF);
}

void CToDoListWnd::OnUpdateCopyTaskasRef(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

void CToDoListWnd::OnCopyTaskasDependency() 
{
	CopySelectedTasksToClipboard(CT_ASDEPENDS);
}

void CToDoListWnd::OnUpdateCopyTaskasDependency(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

BOOL CToDoListWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (CFrameWnd::PreCreateWindow(cs))
	{
		cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
		return TRUE;
	}
	
	// else
	return FALSE;
}

void CToDoListWnd::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CFrameWnd::OnWindowPosChanging(lpwndpos); 
}

void CToDoListWnd::OnToolsCheckforupdates() 
{
	CheckForUpdates(TRUE);
}

void CToDoListWnd::OnEditInsertdatetime() 
{
	COleDateTime date = COleDateTime::GetCurrentTime();
	const CPreferencesDlg& prefs = Prefs();

	CString sDate = CDateHelper::FormatDate(date, prefs.GetDisplayDatesInISO(),
											prefs.GetShowWeekdayInDates());

	sDate += ' ';
	sDate += date.Format(VAR_TIMEVALUEONLY);

	GetToDoCtrl().PasteText(sDate);
}

void CToDoListWnd::OnUpdateEditInsertdatetime(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetToDoCtrl().CanPasteText());
}

void CToDoListWnd::OnSysColorChange() 
{
	CFrameWnd::OnSysColorChange();
	
	InitMenuIconManager();
}

