// RecurrenceEdit.cpp: implementation of the CRecurringTaskEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecurringTaskEdit.h"
#include "resource.h"

#include "..\shared\datehelper.h"
#include "..\shared\dialoghelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecurringTaskEdit::CRecurringTaskEdit()
{
	AddButton(1, "...", "Options");
}

CRecurringTaskEdit::~CRecurringTaskEdit()
{

}

BEGIN_MESSAGE_MAP(CRecurringTaskEdit, CEnEdit)
	//{{AFX_MSG_MAP(CRecurringTaskEdit)
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(EM_SETREADONLY, OnSetReadOnly)
	ON_WM_STYLECHANGING()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////

void CRecurringTaskEdit::PreSubclassWindow()
{
	CEnEdit::PreSubclassWindow();

	SetWindowText(GetRegularity()); // for display purposes
}

void CRecurringTaskEdit::OnBtnClick(UINT nID)
{
	switch (nID)
	{
	case 1:
		if (!m_bReadOnly)
		{
			CRecurringTaskOptionDlg dialog(m_tr, m_dtDefault);

			if (dialog.DoModal() == IDOK)
			{
				TDIRECURRENCE tr;
				dialog.GetRecurrenceOptions(tr);

				if (m_tr != tr)
				{
					m_tr = tr;
					SetWindowText(GetRegularity()); // for display purposes

					// notify parent
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(EN_CHANGE, GetDlgCtrlID()), (LPARAM)GetSafeHwnd());
				}
			}
		}
		break;
	}
}

BOOL CRecurringTaskEdit::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	
	return TRUE;//CEnEdit::OnSetCursor(pWnd, nHitTest, message);
}

HBRUSH CRecurringTaskEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	HBRUSH hbr = NULL;
	pDC->SetBkMode(TRANSPARENT);

	if (!IsWindowEnabled() || m_bReadOnly)
		hbr = GetSysColorBrush(COLOR_3DFACE);
	else
		hbr = GetSysColorBrush(COLOR_WINDOW);

	return hbr;
}

void CRecurringTaskEdit::GetRecurrenceOptions(TDIRECURRENCE& tr) const
{
	tr = m_tr;
}

void CRecurringTaskEdit::SetRecurrenceOptions(const TDIRECURRENCE& tr)
{
	m_tr = tr;

	if (GetSafeHwnd())
		SetWindowText(GetRegularity()); // for display purposes
}

BOOL CRecurringTaskEdit::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT /*nFlags*/)
{
	if ((dwRemove & ES_READONLY) != (dwAdd & ES_READONLY))
	{
		m_bReadOnly = (dwAdd & ES_READONLY);
		
		EnableButton(1, !m_bReadOnly);
		Invalidate();
	}

	// make sure we stay readonly
	return CEnEdit::ModifyStyle(dwRemove & ~ES_READONLY, dwAdd | ES_READONLY);
}

LRESULT CRecurringTaskEdit::OnSetReadOnly(WPARAM wp, LPARAM /*lp*/)
{
	m_bReadOnly = wp;
	EnableButton(1, !m_bReadOnly);

	// always set to readonly
	return DefWindowProc(EM_SETREADONLY, TRUE, 0);
}

void CRecurringTaskEdit::OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	CEnEdit::OnStyleChanging(nStyleType, lpStyleStruct);

	if (nStyleType == GWL_STYLE)
	{
		// check for change in readonly style
		if ((lpStyleStruct->styleOld & ES_READONLY) != (lpStyleStruct->styleNew & ES_READONLY))
		{
			m_bReadOnly = (lpStyleStruct->styleNew & ES_READONLY);
			lpStyleStruct->styleNew |= ES_READONLY; // make sure we stay readonly

			EnableButton(1, !m_bReadOnly);
			Invalidate();
		}
	}
}

CString CRecurringTaskEdit::GetRegularity() const
{
	return GetRegularity(m_tr.nRegularity);
}

CString CRecurringTaskEdit::GetRegularity(TDI_REGULARITY nRegularity)
{
	CString sRegularity;

	switch (nRegularity)
	{
	case TDIR_ONCE:    sRegularity.LoadString(IDS_ONCEONLY); break;
	case TDIR_DAILY:   sRegularity.LoadString(IDS_DAILY);    break;
	case TDIR_WEEKLY:  sRegularity.LoadString(IDS_WEEKLY);   break;
	case TDIR_MONTHLY: sRegularity.LoadString(IDS_MONTHLY);  break;
	case TDIR_YEARLY:  sRegularity.LoadString(IDS_YEARLY);   break;
	}

	return sRegularity;
}

int CRecurringTaskEdit::CalcMaxRegularityWidth(CDC* pDC, BOOL bIncOnce)
{
	int nReg = (int)TDIR_YEARLY + 1;
	int nMax = 0;

	while (nReg-- && (bIncOnce || nReg > TDIR_ONCE))
	{
		int nItem = pDC->GetTextExtent(GetRegularity((TDI_REGULARITY)nReg)).cx;
		nMax = max(nItem, nMax);
	}

	return nMax;
}

/////////////////////////////////////////////////////////////////////////////
// CRecurringTaskOptionDlg dialog

#define WM_VALUECHANGE (WM_APP+1)

CRecurringTaskOptionDlg::CRecurringTaskOptionDlg(const TDIRECURRENCE& tr, const COleDateTime& dtDefault, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_RECURRING_TASK_DIALOG, pParent)
{
	//{{AFX_DATA_INIT(CRecurringTaskOptionDlg)
	m_nRegularity = tr.nRegularity;
	m_bRecalcFromDue = tr.bRecalcFromDue;
	//}}AFX_DATA_INIT

	m_host.AddPage(&m_pageDaily);
	m_host.AddPage(&m_pageWeekly);
	m_host.AddPage(&m_pageMonthly);
	m_host.AddPage(&m_pageYearly);

	// init pages

	// first set to default values
	if (dtDefault.m_dt > 0.0)
	{
		SYSTEMTIME stDefault;
		dtDefault.GetAsSystemTime(stDefault);

		m_pageWeekly.m_dwWeekdays = 2 << (stDefault.wDayOfWeek - 1);
		m_pageMonthly.m_nDay = stDefault.wDay;
		m_pageYearly.m_nMonth = stDefault.wMonth - 1;
		m_pageYearly.m_nDay = stDefault.wDay;
	}
	
	// then overwrite specific values
	switch (tr.nRegularity)
	{
	case TDIR_DAILY:
		m_pageDaily.m_nNumDays = tr.dwSpecific1;
		break;

	case TDIR_WEEKLY:
		m_pageWeekly.m_nNumWeeks = tr.dwSpecific1;
		m_pageWeekly.m_dwWeekdays = tr.dwSpecific2;
		break;

	case TDIR_MONTHLY:
		m_pageMonthly.m_nNumMonths = tr.dwSpecific1;
		m_pageMonthly.m_nDay = tr.dwSpecific2;
		break;

	case TDIR_YEARLY:
		m_pageYearly.m_nMonth = tr.dwSpecific1 - 1;
		m_pageYearly.m_nDay = tr.dwSpecific2;
		break;
	}
}


void CRecurringTaskOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecurringTaskOptionDlg)
	DDX_CBIndex(pDX, IDC_REGULARITY, m_nRegularity);
	DDX_CBIndex(pDX, IDC_RECALCFROM, m_bRecalcFromDue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRecurringTaskOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CRecurringTaskOptionDlg)
	ON_CBN_SELCHANGE(IDC_REGULARITY, OnSelchangeRegularity)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VALUECHANGE, OnValueChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecurringTaskOptionDlg message handlers
/////////////////////////////////////////////////////////////////////////////

void CRecurringTaskOptionDlg::OnOK()
{
	CDialog::OnOK();

	m_host.OnOK();
}

void CRecurringTaskOptionDlg::GetRecurrenceOptions(TDIRECURRENCE& tr) const
{
	tr.nRegularity = (TDI_REGULARITY)m_nRegularity;
	tr.bRecalcFromDue = m_bRecalcFromDue;

	switch (tr.nRegularity)
	{
	case TDIR_DAILY:
		tr.dwSpecific1 = m_pageDaily.m_nNumDays;
		break;

	case TDIR_WEEKLY:
		tr.dwSpecific1 = m_pageWeekly.m_nNumWeeks;
		tr.dwSpecific2 = m_pageWeekly.m_dwWeekdays;
		break;

	case TDIR_MONTHLY:
		tr.dwSpecific1 = m_pageMonthly.m_nNumMonths;
		tr.dwSpecific2 = m_pageMonthly.m_nDay;
		break;

	case TDIR_YEARLY:
		tr.dwSpecific1 = m_pageYearly.m_nMonth + 1;
		tr.dwSpecific2 = m_pageYearly.m_nDay;
		break;
	}
}

void CRecurringTaskOptionDlg::OnSelchangeRegularity() 
{
	UpdateData();

	BOOL bOnce = (m_nRegularity == TDIR_ONCE);
	UINT nShow = bOnce ? SW_HIDE : SW_SHOW;

	if (!bOnce)
		m_host.SetActivePage(m_nRegularity - 1);

	m_host.ShowWindow(nShow);
	GetDlgItem(IDC_RECALCFROMLABEL)->ShowWindow(nShow);
	GetDlgItem(IDC_RECALCFROM)->ShowWindow(nShow);
}

BOOL CRecurringTaskOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_host.Create(IDC_REC_OPTIONS_FRAME, this);

	BOOL bOnce = (m_nRegularity == TDIR_ONCE);
	UINT nShow = bOnce ? SW_HIDE : SW_SHOW;

	if (!bOnce)
		m_host.SetActivePage(m_nRegularity - 1);

	m_host.ShowWindow(nShow);
	GetDlgItem(IDC_RECALCFROMLABEL)->ShowWindow(nShow);
	GetDlgItem(IDC_RECALCFROM)->ShowWindow(nShow);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CRecurringTaskOptionDlg::OnValueChange(WPARAM /*wp*/, LPARAM /*lp*/)
{
	BOOL bValid = FALSE;

	switch (m_nRegularity)
	{
	case TDIR_ONCE:		bValid = TRUE;							break;
	case TDIR_DAILY:	bValid = m_pageDaily.HasValidData();	break;
	case TDIR_WEEKLY:	bValid = m_pageWeekly.HasValidData();	break;
	case TDIR_MONTHLY:	bValid = m_pageMonthly.HasValidData();	break;
	case TDIR_YEARLY:	bValid = m_pageYearly.HasValidData();	break;
	}

	GetDlgItem(IDOK)->EnableWindow(bValid);
	return 0L;
}

///////////////////////////////////////////////////////////////////////////////
// CRecTaskDailyPage property page

IMPLEMENT_DYNCREATE(CRecTaskDailyPage, CPropertyPage)

CRecTaskDailyPage::CRecTaskDailyPage() : CPropertyPage(IDD_REC_TASK_DAILY_PAGE)
{
	//{{AFX_DATA_INIT(CRecTaskDailyPage)
	m_nNumDays = 1;
	//}}AFX_DATA_INIT
}

CRecTaskDailyPage::~CRecTaskDailyPage()
{
}

void CRecTaskDailyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecTaskDailyPage)
	//}}AFX_DATA_MAP
	CDialogHelper::DDX_Text(pDX, IDC_DAYS, m_nNumDays);
}


BEGIN_MESSAGE_MAP(CRecTaskDailyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRecTaskDailyPage)
	ON_EN_CHANGE(IDC_DAYS, OnChangeValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CRecTaskDailyPage::HasValidData()
{
	if (GetSafeHwnd())
		UpdateData();

	return (m_nNumDays > 0);
}

void CRecTaskDailyPage::OnChangeValues() 
{
	UpdateData();
	GetParent()->GetParent()->SendMessage(WM_VALUECHANGE);
}

///////////////////////////////////////////////////////////////////////////////
// CRecTaskWeeklyPage property page

IMPLEMENT_DYNCREATE(CRecTaskWeeklyPage, CPropertyPage)

CRecTaskWeeklyPage::CRecTaskWeeklyPage() : CPropertyPage(IDD_REC_TASK_WEEKLY_PAGE)
{
	//{{AFX_DATA_INIT(CRecTaskWeeklyPage)
	m_nNumWeeks = 1;
	m_dwWeekdays = 0;
	//}}AFX_DATA_INIT
}

CRecTaskWeeklyPage::~CRecTaskWeeklyPage()
{
}

void CRecTaskWeeklyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecTaskWeeklyPage)
	DDX_Control(pDX, IDC_WEEKDAYS, m_lbWeekdays);
	//}}AFX_DATA_MAP
	CDialogHelper::DDX_Text(pDX, IDC_WEEKS, m_nNumWeeks);

	if (pDX->m_bSaveAndValidate)
	{
		m_dwWeekdays = 0;

		if (m_lbWeekdays.GetCheck(0))
			m_dwWeekdays |= TDIW_SUNDAY;

		if (m_lbWeekdays.GetCheck(1))
			m_dwWeekdays |= TDIW_MONDAY;

		if (m_lbWeekdays.GetCheck(2))
			m_dwWeekdays |= TDIW_TUESDAY;

		if (m_lbWeekdays.GetCheck(3))
			m_dwWeekdays |= TDIW_WEDNESDAY;

		if (m_lbWeekdays.GetCheck(4))
			m_dwWeekdays |= TDIW_THURSDAY;

		if (m_lbWeekdays.GetCheck(5))
			m_dwWeekdays |= TDIW_FRIDAY;

		if (m_lbWeekdays.GetCheck(6))
			m_dwWeekdays |= TDIW_SATURDAY;
	}
	else
	{
		m_lbWeekdays.SetCheck(0, (m_dwWeekdays & TDIW_SUNDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(1, (m_dwWeekdays & TDIW_MONDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(2, (m_dwWeekdays & TDIW_TUESDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(3, (m_dwWeekdays & TDIW_WEDNESDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(4, (m_dwWeekdays & TDIW_THURSDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(5, (m_dwWeekdays & TDIW_FRIDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(6, (m_dwWeekdays & TDIW_SATURDAY) ? 1 : 0);
	}
}


BEGIN_MESSAGE_MAP(CRecTaskWeeklyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRecTaskWeeklyPage)
	ON_EN_CHANGE(IDC_WEEKS, OnChangeValues)
	ON_CLBN_CHKCHANGE(IDC_WEEKDAYS, OnChangeValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CRecTaskWeeklyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// init weekdays
	for (int nDay = 1; nDay <= 7; nDay++)
		m_lbWeekdays.AddString(CDateHelper::GetWeekday(nDay, FALSE));

	// restore check states
	UpdateData(FALSE);

	// set the listbox column width to be half the width
	CRect rListbox;
	m_lbWeekdays.GetClientRect(rListbox);
	m_lbWeekdays.SetColumnWidth(rListbox.Width() / 2);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRecTaskWeeklyPage::HasValidData()
{
	if (GetSafeHwnd())
		UpdateData();

	return (m_nNumWeeks > 0/* && m_dwWeekdays != 0*/);
}

void CRecTaskWeeklyPage::OnChangeValues() 
{
	UpdateData();
	GetParent()->GetParent()->SendMessage(WM_VALUECHANGE);
}

///////////////////////////////////////////////////////////////////////////////
// CRecTaskMonthlyPage property page

IMPLEMENT_DYNCREATE(CRecTaskMonthlyPage, CPropertyPage)

CRecTaskMonthlyPage::CRecTaskMonthlyPage() : CPropertyPage(IDD_REC_TASK_MONTHLY_PAGE)
{
	//{{AFX_DATA_INIT(CRecTaskMonthlyPage)
	m_nNumMonths = 1;
	m_nDay = 1;
	//}}AFX_DATA_INIT
}

CRecTaskMonthlyPage::~CRecTaskMonthlyPage()
{
}

void CRecTaskMonthlyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecTaskMonthlyPage)
	//}}AFX_DATA_MAP
	CDialogHelper::DDX_Text(pDX, IDC_MONTHS, m_nNumMonths);
	CDialogHelper::DDX_Text(pDX, IDC_MONTHDAY, m_nDay);
}


BEGIN_MESSAGE_MAP(CRecTaskMonthlyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRecTaskMonthlyPage)
	ON_EN_CHANGE(IDC_MONTHDAY, OnChangeValues)
	ON_EN_CHANGE(IDC_MONTHS, OnChangeValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CRecTaskMonthlyPage::HasValidData()
{
	if (GetSafeHwnd())
		UpdateData();

	return (m_nNumMonths && (m_nDay > 0 && m_nDay <= 31));
}

void CRecTaskMonthlyPage::OnChangeValues() 
{
	UpdateData();
	GetParent()->GetParent()->SendMessage(WM_VALUECHANGE);
}

///////////////////////////////////////////////////////////////////////////////
// CRecTaskYearlyPage property page

IMPLEMENT_DYNCREATE(CRecTaskYearlyPage, CPropertyPage)

CRecTaskYearlyPage::CRecTaskYearlyPage() : CPropertyPage(IDD_REC_TASK_YEARLY_PAGE)
{
	//{{AFX_DATA_INIT(CRecTaskYearlyPage)
	m_nMonth = 0;
	m_nDay = 1;
	//}}AFX_DATA_INIT
}

CRecTaskYearlyPage::~CRecTaskYearlyPage()
{
}

void CRecTaskYearlyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecTaskYearlyPage)
	DDX_Control(pDX, IDC_MONTHS, m_cbMonths);
	DDX_CBIndex(pDX, IDC_MONTHS, m_nMonth);
	DDX_Text(pDX, IDC_MONTHDAY, m_nDay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecTaskYearlyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRecTaskYearlyPage)
	ON_CBN_SELCHANGE(IDC_MONTHS, OnChangeValues)
	ON_EN_CHANGE(IDC_MONTHDAY, OnChangeValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CRecTaskYearlyPage::HasValidData()
{
	if (GetSafeHwnd())
		UpdateData();

	return (m_nMonth >= 0 && m_nMonth < 12) && (m_nDay > 0 && m_nDay <= 31);
}

void CRecTaskYearlyPage::OnChangeValues() 
{
	UpdateData();
	GetParent()->GetParent()->SendMessage(WM_VALUECHANGE);
}

/////////////////////////////////////////////////////////////////////////////
