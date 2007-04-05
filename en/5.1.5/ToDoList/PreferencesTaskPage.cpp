// PreferencesTaskPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesTaskPage.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage property page

IMPLEMENT_DYNCREATE(CPreferencesTaskPage, CPropertyPage)

CPreferencesTaskPage::CPreferencesTaskPage() : CPropertyPage(CPreferencesTaskPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesTaskPage)
	m_bAllowParentTimeTracking = FALSE;
	m_bAutoAdjustDependents = FALSE;
	m_bWeightPercentCompletionByNumSubtasks = FALSE;
	//}}AFX_DATA_INIT

	// load settings
	m_bTreatSubCompletedAsDone = AfxGetApp()->GetProfileInt("Preferences", "TreatSubCompletedAsDone", TRUE);
	m_bAveragePercentSubCompletion = AfxGetApp()->GetProfileInt("Preferences", "AveragePercentSubCompletion", TRUE);
	m_bIncludeDoneInAverageCalc = AfxGetApp()->GetProfileInt("Preferences", "IncludeDoneInAverageCalc", TRUE);
	m_bUseEarliestDueDate = AfxGetApp()->GetProfileInt("Preferences", "UseEarliestDueDate", FALSE);
	m_bUsePercentDoneInTimeEst = AfxGetApp()->GetProfileInt("Preferences", "UsePercentDoneInTimeEst", TRUE);
	m_bUseHighestPriority = AfxGetApp()->GetProfileInt("Preferences", "UseHighestPriority", FALSE);
	m_bAutoCalcTimeEst = AfxGetApp()->GetProfileInt("Preferences", "AutoCalcTimeEst", FALSE);
	m_bIncludeDoneInPriorityCalc = AfxGetApp()->GetProfileInt("Preferences", "IncludeDoneInPriorityCalc", FALSE);
	m_bWeightPercentCompletionByTimeEst = AfxGetApp()->GetProfileInt("Preferences", "WeightPercentCompletionByTimeEst", FALSE);
	m_bWeightPercentCompletionByPriority = AfxGetApp()->GetProfileInt("Preferences", "WeightPercentCompletionByPriority", FALSE);
	m_bWeightPercentCompletionByNumSubtasks = AfxGetApp()->GetProfileInt("Preferences", "WeightPercentCompletionByNumSubtasks", TRUE);
	m_bAutoCalcPercentDone = AfxGetApp()->GetProfileInt("Preferences", "AutoCalcPercentDone", FALSE);
	m_bTrackActiveTasklistOnly = AfxGetApp()->GetProfileInt("Preferences", "TrackActiveTasklistOnly", TRUE);
	m_bTrackSelectedTaskOnly = AfxGetApp()->GetProfileInt("Preferences", "TrackSelectedTaskOnly", TRUE);
	m_bNoTrackOnScreenSaver = AfxGetApp()->GetProfileInt("Preferences", "PauseTimeTrackingOnScrnSaver", TRUE);
	m_sDaysInWeek = AfxGetApp()->GetProfileString("Preferences", "DaysInWeek", "5");
	m_sHoursInDay = AfxGetApp()->GetProfileString("Preferences", "HoursInDay", "8");
	m_bLogTime = AfxGetApp()->GetProfileInt("Preferences", "LogTime", FALSE);
	m_bLogTasksSeparately = AfxGetApp()->GetProfileInt("Preferences", "LogTasksSeparately", FALSE);
	m_bExclusiveTimeTracking = AfxGetApp()->GetProfileInt("Preferences", "ExclusiveTimeTracking", FALSE);
	m_bAllowParentTimeTracking = AfxGetApp()->GetProfileInt("Preferences", "AllowParentTimeTracking", TRUE);
	m_bAutoAdjustDependents = AfxGetApp()->GetProfileInt("Preferences", "AutoAdjustDependents", FALSE);

	// fix up m_bAveragePercentSubCompletion because it's overridden by m_bAutoCalcPercentDone
	if (m_bAutoCalcPercentDone)
		m_bAveragePercentSubCompletion = FALSE;

//	m_b = AfxGetApp()->GetProfileInt("Preferences", "", FALSE);
}

CPreferencesTaskPage::~CPreferencesTaskPage()
{
}

void CPreferencesTaskPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskPage)
	DDX_Check(pDX, IDC_TREATSUBCOMPLETEDASDONE, m_bTreatSubCompletedAsDone);
	DDX_Check(pDX, IDC_USEHIGHESTPRIORITY, m_bUseHighestPriority);
	DDX_Check(pDX, IDC_AUTOCALCTIMEEST, m_bAutoCalcTimeEst);
	DDX_Check(pDX, IDC_INCLUDEDONEINPRIORITYCALC, m_bIncludeDoneInPriorityCalc);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYTIMEEST, m_bWeightPercentCompletionByTimeEst);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYPRIORITY, m_bWeightPercentCompletionByPriority);
	DDX_Check(pDX, IDC_AUTOCALCPERCENTDONE, m_bAutoCalcPercentDone);
	DDX_Check(pDX, IDC_TRACKSELECTEDTASKONLY, m_bTrackSelectedTaskOnly);
	DDX_Check(pDX, IDC_TRACKNOSCRNSAVER, m_bNoTrackOnScreenSaver);
	DDX_Check(pDX, IDC_TRACKACTIVETASKLISTONLY, m_bTrackActiveTasklistOnly);
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


BEGIN_MESSAGE_MAP(CPreferencesTaskPage, CPropertyPage)
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
	CPropertyPage::OnInitDialog();

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

void CPreferencesTaskPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save settings
	AfxGetApp()->WriteProfileInt("Preferences", "TreatSubCompletedAsDone", m_bTreatSubCompletedAsDone);
	AfxGetApp()->WriteProfileInt("Preferences", "AveragePercentSubCompletion", m_bAveragePercentSubCompletion);
	AfxGetApp()->WriteProfileInt("Preferences", "IncludeDoneInAverageCalc", m_bIncludeDoneInAverageCalc);
	AfxGetApp()->WriteProfileInt("Preferences", "UseEarliestDueDate", m_bUseEarliestDueDate);
	AfxGetApp()->WriteProfileInt("Preferences", "UsePercentDoneInTimeEst", m_bUsePercentDoneInTimeEst);
	AfxGetApp()->WriteProfileInt("Preferences", "UseHighestPriority", m_bUseHighestPriority);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoCalcTimeEst", m_bAutoCalcTimeEst);
	AfxGetApp()->WriteProfileInt("Preferences", "IncludeDoneInPriorityCalc", m_bIncludeDoneInPriorityCalc);
	AfxGetApp()->WriteProfileInt("Preferences", "WeightPercentCompletionByTimeEst", m_bWeightPercentCompletionByTimeEst);
	AfxGetApp()->WriteProfileInt("Preferences", "WeightPercentCompletionByPriority", m_bWeightPercentCompletionByPriority);
	AfxGetApp()->WriteProfileInt("Preferences", "WeightPercentCompletionByNumSubtasks", m_bWeightPercentCompletionByNumSubtasks);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoCalcPercentDone", m_bAutoCalcPercentDone);
	AfxGetApp()->WriteProfileInt("Preferences", "TrackSelectedTaskOnly", m_bTrackSelectedTaskOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "TrackActiveTasklistOnly", m_bTrackActiveTasklistOnly);
	AfxGetApp()->WriteProfileInt("Preferences", "PauseTimeTrackingOnScrnSaver", m_bNoTrackOnScreenSaver);
	AfxGetApp()->WriteProfileInt("Preferences", "LogTime", m_bLogTime);
	AfxGetApp()->WriteProfileInt("Preferences", "LogTasksSeparately", m_bLogTasksSeparately);
	AfxGetApp()->WriteProfileInt("Preferences", "ExclusiveTimeTracking", m_bExclusiveTimeTracking);
	AfxGetApp()->WriteProfileInt("Preferences", "AllowParentTimeTracking", m_bAllowParentTimeTracking);
	AfxGetApp()->WriteProfileInt("Preferences", "AutoAdjustDependents", m_bAutoAdjustDependents);

	// validate time periods before writing
	m_sHoursInDay.Format("%.2f", GetHoursInOneDay());
	m_sDaysInWeek.Format("%.2f", GetDaysInOneWeek());
	AfxGetApp()->WriteProfileString("Preferences", "DaysInWeek", m_sDaysInWeek);
	AfxGetApp()->WriteProfileString("Preferences", "HoursInDay", m_sHoursInDay);

//	AfxGetApp()->WriteProfileInt("Preferences", "", m_b);
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
	// handle locale specific decimal separator
	setlocale(LC_NUMERIC, "");

	double dHours = atof(m_sHoursInDay);

	if (dHours <= 0 || dHours > 24)
		dHours = 8;

	// restore decimal separator to '.'
	setlocale(LC_NUMERIC, "English");

	return dHours;
}

double CPreferencesTaskPage::GetDaysInOneWeek() const
{
	// handle locale specific decimal separator
	setlocale(LC_NUMERIC, "");

	double dDays = atof(m_sDaysInWeek);

	if (dDays <= 0 || dDays > 7)
		dDays = 5;

	// restore decimal separator to '.'
	setlocale(LC_NUMERIC, "English");

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
