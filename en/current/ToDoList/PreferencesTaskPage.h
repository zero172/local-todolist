#if !defined(AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
#define AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTaskPage.h : header file
//

#include "..\shared\groupline.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\preferencesbase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage dialog

class CPreferencesTaskPage : public CPreferencesPageBase, public CDialogHelper
{
	DECLARE_DYNCREATE(CPreferencesTaskPage)

// Construction
public:
	CPreferencesTaskPage();
	~CPreferencesTaskPage();

	BOOL GetAveragePercentSubCompletion() const { return m_bAveragePercentSubCompletion; }
	BOOL GetIncludeDoneInAverageCalc() const { return m_bIncludeDoneInAverageCalc; }
	BOOL GetUseEarliestDueDate() const { return m_bUseEarliestDueDate; }
	BOOL GetUsePercentDoneInTimeEst() const { return m_bUsePercentDoneInTimeEst; }
	BOOL GetTreatSubCompletedAsDone() const { return m_bTreatSubCompletedAsDone; }
	BOOL GetUseHighestPriority() const { return m_bUseHighestPriority; }
	BOOL GetUseHighestRisk() const { return m_bUseHighestPriority; } // Note: this uses same flag as priority
	BOOL GetAutoCalcTimeEstimates() const { return m_bAutoCalcTimeEst; }
	BOOL GetIncludeDoneInPriorityCalc() const { return m_bIncludeDoneInPriorityCalc; }
	BOOL GetWeightPercentCompletionByTimeEst() const { return m_bWeightPercentCompletionByTimeEst; }
	BOOL GetWeightPercentCompletionByPriority() const { return m_bWeightPercentCompletionByPriority; }
	BOOL GetWeightPercentCompletionByNumSubtasks() const { return m_bWeightPercentCompletionByNumSubtasks; }
	BOOL GetAutoCalcPercentDone() const { return m_bAutoCalcPercentDone; }
	BOOL GetTrackNonActiveTasklists() const { return m_bTrackNonActiveTasklists; }
	BOOL GetTrackOnScreenSaver() const { return m_bTrackOnScreenSaver; }
	BOOL GetTrackNonSelectedTasks() const { return m_bTrackNonSelectedTasks; }
	BOOL GetTrackHibernated() const { return m_bTrackHibernated; }
	double GetHoursInOneDay() const;
	double GetDaysInOneWeek() const;
	BOOL GetLogTimeTracking() const { return m_bLogTime; }
	BOOL GetLogTaskTimeSeparately() const { return m_bLogTime && m_bLogTasksSeparately; }
	BOOL GetExclusiveTimeTracking() const { return m_bExclusiveTimeTracking; }
	BOOL GetAllowParentTimeTracking() const { return m_bAllowParentTimeTracking; }
	BOOL GetAutoAdjustDependents() const { return m_bAutoAdjustDependents; }

//	BOOL Get() const { return m_b; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesTaskPage)
	enum { IDD = IDD_PREFTASK_PAGE };
	BOOL	m_bTreatSubCompletedAsDone;
	BOOL	m_bUseHighestPriority;
	BOOL	m_bAutoCalcTimeEst;
	BOOL	m_bIncludeDoneInPriorityCalc;
	BOOL	m_bWeightPercentCompletionByTimeEst;
	BOOL	m_bWeightPercentCompletionByPriority;
	BOOL	m_bAutoCalcPercentDone;
	CString	m_sDaysInWeek;
	CString	m_sHoursInDay;
	BOOL	m_bLogTime;
	BOOL	m_bLogTasksSeparately;
	BOOL	m_bExclusiveTimeTracking;
	BOOL	m_bAllowParentTimeTracking;
	BOOL	m_bAutoAdjustDependents;
	BOOL	m_bWeightPercentCompletionByNumSubtasks;
	//}}AFX_DATA
	BOOL	m_bUseEarliestDueDate;
	BOOL	m_bUsePercentDoneInTimeEst;
	BOOL	m_bAveragePercentSubCompletion;
	BOOL	m_bIncludeDoneInAverageCalc;
	BOOL	m_bTrackNonSelectedTasks;
	BOOL	m_bTrackOnScreenSaver;
	BOOL	m_bTrackNonActiveTasklists;
	BOOL	m_bTrackHibernated;
	CGroupLineManager m_mgrGroupLines;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTaskPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesTaskPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsehighestpriority();
	afx_msg void OnLogtime();
	afx_msg void OnAutocalcpercentdone();
	//}}AFX_MSG
	afx_msg void OnAveragepercentChange();
	DECLARE_MESSAGE_MAP()

   virtual void LoadPreferences(const CPreferencesStorage& prefs);
   virtual void SavePreferences(CPreferencesStorage& prefs);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
