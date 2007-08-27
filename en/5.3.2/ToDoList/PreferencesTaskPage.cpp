// PreferencesTaskPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesTaskPage.h"

#include "..\shared\Misc.h"

//#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage property page

IMPLEMENT_DYNCREATE(CPreferencesTaskPage, CPreferencesPageBase)

CPreferencesTaskPage::CPreferencesTaskPage() : 
   CPreferencesPageBase(CPreferencesTaskPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesTaskPage)
	//}}AFX_DATA_INIT

}

CPreferencesTaskPage::~CPreferencesTaskPage()
{
}

void CPreferencesTaskPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskPage)
	DDX_Check(pDX, IDC_TREATSUBCOMPLETEDASDONE, m_bTreatSubCompletedAsDone);
	DDX_Check(pDX, IDC_USEHIGHESTPRIORITY, m_bUseHighestPriority);
	DDX_Check(pDX, IDC_AUTOCALCTIMEEST, m_bAutoCalcTimeEst);
	DDX_Check(pDX, IDC_INCLUDEDONEINPRIORITYCALC, m_bIncludeDoneInPriorityCalc);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYTIMEEST, m_bWeightPercentCompletionByTimeEst);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYPRIORITY, m_bWeightPercentCompletionByPriority);
	DDX_Check(pDX, IDC_AUTOCALCPERCENTDONE, m_bAutoCalcPercentDone);
	DDX_Check(pDX, IDC_TRACKNONSELECTEDTASKS, m_bTrackNonSelectedTasks);
	DDX_Check(pDX, IDC_TRACKSCREENSAVER, m_bTrackOnScreenSaver);
	DDX_Check(pDX, IDC_TRACKNONACTIVETASKLISTS, m_bTrackNonActiveTasklists);
	DDX_Check(pDX, IDC_TRACKHIBERNATED, m_bTrackHibernated);
	DDX_CBString(pDX, IDC_DAYSINONEWEEK, m_sDaysInWeek);
	DDX_CBString(pDX, IDC_HOURSINONEDAY, m_sHoursInDay);
	DDX_Check(pDX, IDC_LOGTIME, m_bLogTime);
	DDX_Check(pDX, IDC_LOGTASKSEPARATELY, m_bLogTasksSeparately);
	DDX_Check(pDX, IDC_EXCLUSIVETRACKING, m_bExclusiveTimeTracking);
	DDX_Check(pDX, IDC_ALLOWPARENTRACKING, m_bAllowParentTimeTracking);
	DDX_Check(pDX, IDC_AUTOUPDATEDEPENDEES, m_bAutoAdjustDependents);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYNUMSUB, m_bWeightPercentCompletionByNumSubtasks);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_USEEARLIESTDUEDATE, m_bUseEarliestDueDate);
	DDX_Check(pDX, IDC_USEPERCENTDONEINTIMEEST, m_bUsePercentDoneInTimeEst);
	DDX_Check(pDX, IDC_AVERAGEPERCENTSUBCOMPLETION, m_bAveragePercentSubCompletion);
	DDX_Check(pDX, IDC_INCLUDEDONEINAVERAGECALC, m_bIncludeDoneInAverageCalc);
}


BEGIN_MESSAGE_MAP(CPreferencesTaskPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesTaskPage)
	ON_BN_CLICKED(IDC_USEHIGHESTPRIORITY, OnUsehighestpriority)
	ON_BN_CLICKED(IDC_LOGTIME, OnLogtime)
	ON_BN_CLICKED(IDC_AUTOCALCPERCENTDONE, OnAutocalcpercentdone)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_AVERAGEPERCENTSUBCOMPLETION, OnAveragepercentChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage message handlers

BOOL CPreferencesTaskPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_TRACKGROUP, *this); 
	m_mgrGroupLines.AddGroupLine(IDC_TIMEGROUP, *this); 

	GetDlgItem(IDC_INCLUDEDONEINAVERAGECALC)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYTIMEEST)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYPRIORITY)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYNUMSUB)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_INCLUDEDONEINPRIORITYCALC)->EnableWindow(m_bUseHighestPriority);
	GetDlgItem(IDC_LOGTASKSEPARATELY)->EnableWindow(m_bLogTime);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTaskPage::OnAveragepercentChange() 
{
	UpdateData();

	GetDlgItem(IDC_INCLUDEDONEINAVERAGECALC)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYTIMEEST)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYPRIORITY)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYNUMSUB)->EnableWindow(m_bAveragePercentSubCompletion);

	// uncheck IDC_AUTOCALCPERCENTDONE if m_bAveragePercentSubCompletion is TRUE
	if (m_bAveragePercentSubCompletion && m_bAutoCalcPercentDone)
	{
		m_bAutoCalcPercentDone = FALSE;
		UpdateData(FALSE);
	}
}


void CPreferencesTaskPage::OnUsehighestpriority() 
{
	UpdateData();

	GetDlgItem(IDC_INCLUDEDONEINPRIORITYCALC)->EnableWindow(m_bUseHighestPriority);
}

double CPreferencesTaskPage::GetHoursInOneDay() const
{
	double dHours = Misc::Atof(m_sHoursInDay);

	if (dHours <= 0 || dHours > 24)
		dHours = 8;

	return dHours;
}

double CPreferencesTaskPage::GetDaysInOneWeek() const
{
	double dDays = Misc::Atof(m_sDaysInWeek);

	if (dDays <= 0 || dDays > 7)
		dDays = 5;

	return dDays;
}

void CPreferencesTaskPage::OnLogtime() 
{
	UpdateData();
	
	GetDlgItem(IDC_LOGTASKSEPARATELY)->EnableWindow(m_bLogTime);
}

void CPreferencesTaskPage::OnAutocalcpercentdone() 
{
	UpdateData();

	// uncheck IDC_AUTOCALCPERCENTDONE if m_bAveragePercentSubCompletion is TRUE
	if (m_bAutoCalcPercentDone && m_bAveragePercentSubCompletion)
	{
		m_bAveragePercentSubCompletion = FALSE;
		UpdateData(FALSE);

		OnAveragepercentChange();
	}
}

void CPreferencesTaskPage::LoadPreferences(const CPreferencesStorage& prefs)
{
	// load settings
	m_bTreatSubCompletedAsDone = prefs.GetProfileInt("Preferences", "TreatSubCompletedAsDone", TRUE);
	m_bAveragePercentSubCompletion = prefs.GetProfileInt("Preferences", "AveragePercentSubCompletion", TRUE);
	m_bIncludeDoneInAverageCalc = prefs.GetProfileInt("Preferences", "IncludeDoneInAverageCalc", TRUE);
	m_bUseEarliestDueDate = prefs.GetProfileInt("Preferences", "UseEarliestDueDate", FALSE);
	m_bUsePercentDoneInTimeEst = prefs.GetProfileInt("Preferences", "UsePercentDoneInTimeEst", TRUE);
	m_bUseHighestPriority = prefs.GetProfileInt("Preferences", "UseHighestPriority", FALSE);
	m_bAutoCalcTimeEst = prefs.GetProfileInt("Preferences", "AutoCalcTimeEst", FALSE);
	m_bIncludeDoneInPriorityCalc = prefs.GetProfileInt("Preferences", "IncludeDoneInPriorityCalc", FALSE);
	m_bWeightPercentCompletionByTimeEst = prefs.GetProfileInt("Preferences", "WeightPercentCompletionByTimeEst", FALSE);
	m_bWeightPercentCompletionByPriority = prefs.GetProfileInt("Preferences", "WeightPercentCompletionByPriority", FALSE);
	m_bWeightPercentCompletionByNumSubtasks = prefs.GetProfileInt("Preferences", "WeightPercentCompletionByNumSubtasks", TRUE);
	m_bAutoCalcPercentDone = prefs.GetProfileInt("Preferences", "AutoCalcPercentDone", FALSE);
	m_sDaysInWeek = prefs.GetProfileString("Preferences", "DaysInWeek", "5");
	m_sHoursInDay = prefs.GetProfileString("Preferences", "HoursInDay", "8");
	m_bLogTime = prefs.GetProfileInt("Preferences", "LogTime", FALSE);
	m_bLogTasksSeparately = prefs.GetProfileInt("Preferences", "LogTasksSeparately", FALSE);
	m_bExclusiveTimeTracking = prefs.GetProfileInt("Preferences", "ExclusiveTimeTracking", FALSE);
	m_bAllowParentTimeTracking = prefs.GetProfileInt("Preferences", "AllowParentTimeTracking", TRUE);
	m_bAutoAdjustDependents = prefs.GetProfileInt("Preferences", "AutoAdjustDependents", FALSE);

	// messy but I decided to change the logic to clarify the user interface
	m_bTrackNonActiveTasklists = prefs.GetProfileInt("Preferences", "TrackNonActiveTasklists", -1);

	if (m_bTrackNonActiveTasklists == -1) // first time
		m_bTrackNonActiveTasklists = !(prefs.GetProfileInt("Preferences", "TrackActiveTasklistOnly", TRUE));

	m_bTrackNonSelectedTasks = prefs.GetProfileInt("Preferences", "TrackNonSelectedTasks", -1);

	if (m_bTrackNonSelectedTasks == -1) // first time
		m_bTrackNonSelectedTasks = !(prefs.GetProfileInt("Preferences", "TrackSelectedTaskOnly", TRUE));

	m_bTrackOnScreenSaver = prefs.GetProfileInt("Preferences", "TrackOnScreenSaver", -1);

	if (m_bTrackOnScreenSaver == -1) // first time
		m_bTrackOnScreenSaver = !(prefs.GetProfileInt("Preferences", "PauseTimeTrackingOnScrnSaver", TRUE));

	m_bTrackHibernated = prefs.GetProfileInt("Preferences", "AllowTrackingWhenHibernated", FALSE);

	// fix up m_bAveragePercentSubCompletion because it's overridden by m_bAutoCalcPercentDone
	if (m_bAutoCalcPercentDone)
		m_bAveragePercentSubCompletion = FALSE;

//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);
}

void CPreferencesTaskPage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "TreatSubCompletedAsDone", m_bTreatSubCompletedAsDone);
	prefs.WriteProfileInt("Preferences", "AveragePercentSubCompletion", m_bAveragePercentSubCompletion);
	prefs.WriteProfileInt("Preferences", "IncludeDoneInAverageCalc", m_bIncludeDoneInAverageCalc);
	prefs.WriteProfileInt("Preferences", "UseEarliestDueDate", m_bUseEarliestDueDate);
	prefs.WriteProfileInt("Preferences", "UsePercentDoneInTimeEst", m_bUsePercentDoneInTimeEst);
	prefs.WriteProfileInt("Preferences", "UseHighestPriority", m_bUseHighestPriority);
	prefs.WriteProfileInt("Preferences", "AutoCalcTimeEst", m_bAutoCalcTimeEst);
	prefs.WriteProfileInt("Preferences", "IncludeDoneInPriorityCalc", m_bIncludeDoneInPriorityCalc);
	prefs.WriteProfileInt("Preferences", "WeightPercentCompletionByTimeEst", m_bWeightPercentCompletionByTimeEst);
	prefs.WriteProfileInt("Preferences", "WeightPercentCompletionByPriority", m_bWeightPercentCompletionByPriority);
	prefs.WriteProfileInt("Preferences", "WeightPercentCompletionByNumSubtasks", m_bWeightPercentCompletionByNumSubtasks);
	prefs.WriteProfileInt("Preferences", "AutoCalcPercentDone", m_bAutoCalcPercentDone);
	prefs.WriteProfileInt("Preferences", "TrackNonSelectedTasks", m_bTrackNonSelectedTasks);
	prefs.WriteProfileInt("Preferences", "TrackNonActiveTasklists", m_bTrackNonActiveTasklists);
	prefs.WriteProfileInt("Preferences", "TrackOnScreenSaver", m_bTrackOnScreenSaver);
	prefs.WriteProfileInt("Preferences", "AllowTrackingWhenHibernated", m_bTrackHibernated);
	prefs.WriteProfileInt("Preferences", "LogTime", m_bLogTime);
	prefs.WriteProfileInt("Preferences", "LogTasksSeparately", m_bLogTasksSeparately);
	prefs.WriteProfileInt("Preferences", "ExclusiveTimeTracking", m_bExclusiveTimeTracking);
	prefs.WriteProfileInt("Preferences", "AllowParentTimeTracking", m_bAllowParentTimeTracking);
	prefs.WriteProfileInt("Preferences", "AutoAdjustDependents", m_bAutoAdjustDependents);

	// validate time periods before writing
	m_sHoursInDay.Format("%.2f", GetHoursInOneDay());
	m_sDaysInWeek.Format("%.2f", GetDaysInOneWeek());
	prefs.WriteProfileString("Preferences", "DaysInWeek", m_sDaysInWeek);
	prefs.WriteProfileString("Preferences", "HoursInDay", m_sHoursInDay);

//	prefs.WriteProfileInt("Preferences", "", m_b);
}

