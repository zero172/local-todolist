// FilterBar.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FilterBar.h"
#include "tdcmsg.h"
#include "filteredtodoctrl.h"

#include "..\shared\deferwndmove.h"
#include "..\shared\dlgunits.h"
#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

struct FILTERCTRL
{
	UINT nIDLabel;
	UINT nIDCtrl;
	int nLenLabelDLU;
	int nLenCtrlDLU;
	TDC_COLUMN nType;
};

static FILTERCTRL FILTERCTRLS[] = 
{
	{ IDC_FILTERLABEL, IDC_FILTERCOMBO, 22, 150, (TDC_COLUMN)-1 },
	{ IDC_PRIORITYFILTERLABEL, IDC_PRIORITYFILTERCOMBO, 45, 75, TDCC_PRIORITY },
	{ IDC_RISKFILTERLABEL, IDC_RISKFILTERCOMBO, 45, 75, TDCC_RISK },
	{ IDC_ALLOCTOFILTERLABEL, IDC_ALLOCTOFILTERCOMBO, 45, 75, TDCC_ALLOCTO },
	{ IDC_ALLOCBYFILTERLABEL, IDC_ALLOCBYFILTERCOMBO, 45, 75, TDCC_ALLOCBY },
	{ IDC_STATUSFILTERLABEL, IDC_STATUSFILTERCOMBO, 45, 75, TDCC_STATUS },
	{ IDC_CATEGORYFILTERLABEL, IDC_CATEGORYFILTERCOMBO, 45, 75, TDCC_CATEGORY }
};

const int NUMFILTERCTRLS = sizeof(FILTERCTRLS) / sizeof(FILTERCTRL);
const int FILTERCTRLXSPACING = 6; // dlu
const int FILTERCTRLYSPACING = 2; // dlu

#define WM_WANTCOMBOPROMPT (WM_APP+1)

/////////////////////////////////////////////////////////////////////////////
// CFilterBar dialog

CFilterBar::CFilterBar(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FILTER_BAR, pParent), 
	  m_cbCategoryFilter(TRUE, IDS_NOCATEGORY, IDS_TDC_ANY),
	  m_cbAllocToFilter(TRUE, IDS_NOALLOCTO, IDS_TDC_ANYONE),
	  m_cbAllocByFilter(FALSE, 0, IDS_TDC_ANYONE),
	  m_cbStatusFilter(FALSE, 0, IDS_TDC_ANY)
{
	//{{AFX_DATA_INIT(CFilterBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_aVisibility.SetSize(TDCC_LAST);
}


void CFilterBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_FILTERCOMBO, m_cbTaskFilter);
	DDX_Control(pDX, IDC_ALLOCTOFILTERCOMBO, m_cbAllocToFilter);
	DDX_Control(pDX, IDC_ALLOCBYFILTERCOMBO, m_cbAllocByFilter);
	DDX_Control(pDX, IDC_CATEGORYFILTERCOMBO, m_cbCategoryFilter);
	DDX_Control(pDX, IDC_STATUSFILTERCOMBO, m_cbStatusFilter);
	DDX_Control(pDX, IDC_PRIORITYFILTERCOMBO, m_cbPriorityFilter);
	DDX_Control(pDX, IDC_RISKFILTERCOMBO, m_cbRiskFilter);
	DDX_CBIndex(pDX, IDC_FILTERCOMBO, (int&)m_filter.nFilter);
//	DDX_CBString(pDX, IDC_ALLOCTOFILTERCOMBO, m_filter.sAllocTo);
	DDX_CBString(pDX, IDC_ALLOCBYFILTERCOMBO, m_filter.sAllocBy);
	DDX_CBString(pDX, IDC_STATUSFILTERCOMBO, m_filter.sStatus);
	
	// special handling
	if (pDX->m_bSaveAndValidate)
	{
		int nIndex;

		// priority
		DDX_CBIndex(pDX, IDC_PRIORITYFILTERCOMBO, nIndex);

		if (nIndex == 0) // any
			m_filter.nPriority = FT_ANYPRIORITY;

		else if (nIndex == 1) // none
			m_filter.nPriority = FT_NOPRIORITY;
		else
			m_filter.nPriority = nIndex - 2;

		// risk
		DDX_CBIndex(pDX, IDC_RISKFILTERCOMBO, nIndex);

		if (nIndex == 0) // any
			m_filter.nRisk = FT_ANYRISK;

		else if (nIndex == 1) // none
			m_filter.nRisk = FT_NORISK;
		else
			m_filter.nRisk = nIndex - 2;

		// cats
		m_cbCategoryFilter.GetChecked(m_filter.aCategories);

		// allocto
		m_cbAllocToFilter.GetChecked(m_filter.aAllocTo);
	}
	else
	{
		int nIndex;
		
		// priority
		if (m_filter.nPriority == FT_ANYPRIORITY)
			nIndex = 0;

		else if (m_filter.nPriority == FT_NOPRIORITY)
			nIndex = 1;
		else
			nIndex = m_filter.nPriority + 2;

		DDX_CBIndex(pDX, IDC_PRIORITYFILTERCOMBO, nIndex);

		// risk
		if (m_filter.nRisk == FT_ANYRISK)
			nIndex = 0;

		else if (m_filter.nRisk == FT_NORISK)
			nIndex = 1;
		else
			nIndex = m_filter.nRisk + 2;

		DDX_CBIndex(pDX, IDC_RISKFILTERCOMBO, nIndex);

		// cats
		m_cbCategoryFilter.SetChecked(m_filter.aCategories);

		// allocto
		m_cbAllocToFilter.SetChecked(m_filter.aAllocTo);
	}
}


BEGIN_MESSAGE_MAP(CFilterBar, CDialog)
	//{{AFX_MSG_MAP(CFilterBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_FILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_ALLOCTOFILTERCOMBO, OnSelchangeFilter)
//	ON_CBN_CLOSEUP(IDC_ALLOCTOFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_ALLOCBYFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_STATUSFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_PRIORITYFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_CATEGORYFILTERCOMBO, OnSelchangeFilter)
//	ON_CBN_CLOSEUP(IDC_CATEGORYFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_RISKFILTERCOMBO, OnSelchangeFilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterBar message handlers

BOOL CFilterBar::Create(CWnd* pParentWnd, UINT nID)
{
	if (CDialog::Create(IDD_FILTER_BAR, pParentWnd))
	{
		SetDlgCtrlID(nID);
		return TRUE;
	}

	return FALSE;
}

void CFilterBar::OnSelchangeFilter() 
{
	UpdateData();

	GetParent()->SendMessage(WM_FBN_FILTERCHNG, 0, (LPARAM)&m_filter);
}

void CFilterBar::RefreshFilterControls(const CFilteredToDoCtrl& tdc)
{
	// get filter
	tdc.GetFilter(m_filter);
	
	// alloc to filter
	CStringArray aItems;
	tdc.GetAllocToNames(aItems);
	
	// add blank item to top
	aItems.Add("");
	
	SetComboBoxItems(m_cbAllocToFilter, aItems);
	
	// alloc by filter
	tdc.GetAllocByNames(aItems);
	
	// add blank item to top
	aItems.Add("");
	
	SetComboBoxItems(m_cbAllocByFilter, aItems);
	
	// category filter
	tdc.GetCategoryNames(aItems);
	
	// add blank item to top
	aItems.Add("");

	SetComboBoxItems(m_cbCategoryFilter, aItems);
	
	// status filter
	tdc.GetStatusNames(aItems);
	
	// add blank item to top
	aItems.Add("");
	
	SetComboBoxItems(m_cbStatusFilter, aItems);
	
	// priority
	m_cbPriorityFilter.SetColors(m_aPriorityColors);
	m_cbPriorityFilter.InsertColor(0, (COLORREF)-1, CEnString(IDS_TDC_ANY)); // add a blank item
	
	// risk never needs changing
	
	// update UI
	UpdateData(FALSE);
}

void CFilterBar::SetFilterLabelAlignment(BOOL bLeft)
{
	UINT nAlign = bLeft ? SS_LEFT : SS_RIGHT;
	
	int nLabel = NUMFILTERCTRLS;
	
	while (nLabel--)
	{
		CWnd* pLabel = GetDlgItem(FILTERCTRLS[nLabel].nIDLabel);
		pLabel->ModifyStyle(SS_TYPEMASK, nAlign);
		pLabel->Invalidate();
	}
}

void CFilterBar::SetPriorityColors(const CDWordArray& aColors)
{
	m_aPriorityColors.Copy(aColors);

	if (m_cbPriorityFilter.GetSafeHwnd())
	{
		// save and restore current selection
		int nSel = m_cbPriorityFilter.GetCurSel();

		m_cbPriorityFilter.SetColors(aColors);
		m_cbPriorityFilter.InsertColor(0, (COLORREF)-1, CEnString(IDS_TDC_ANY)); // add a blank item

		m_cbPriorityFilter.SetCurSel(nSel);
	}
}

void CFilterBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// check we're not in the middle of creation
	if (m_cbStatusFilter.GetSafeHwnd())
		ReposControls(cx, FALSE);
}

int CFilterBar::CalcHeight(int nWidth)
{
	return ReposControls(nWidth, TRUE);
}

/*
void CFilterBar::ShowFilter(TDC_COLUMN nType, BOOL bShow, BOOL bUpdate)
{
	if ((int)nType < 0)
		return;

	m_aVisibility[(DWORD)nType] = bShow;

	if (bUpdate)
		ReposControls();
}
*/

void CFilterBar::SetVisibleFilters(const CTDCColumnArray& aFilters)
{
	// clear first
	int nFilter = m_aVisibility.GetSize();

	while (nFilter--)
		m_aVisibility[nFilter] = FALSE;

	// then set the visible ones
	int nItem = aFilters.GetSize();

	while (nItem--)
	{
		nFilter = aFilters[nItem];
		m_aVisibility[nFilter] = TRUE;
	}

	ReposControls();
}

BOOL CFilterBar::WantShowFilter(TDC_COLUMN nType)
{
	if ((int)nType < 0) // first filter is -1 and is therefore always visible
		return TRUE;

	return (BOOL)m_aVisibility[(DWORD)nType];
}

void CFilterBar::EnableMultiCategorySelection(BOOL bEnable)
{
	m_cbCategoryFilter.EnableMultiSelection(bEnable);
}

int CFilterBar::ReposControls(int nWidth, BOOL bCalcOnly)
{
	CDeferWndMove dwm(bCalcOnly ? 0 : NUMFILTERCTRLS);

	if (nWidth <= 0)
	{
		CRect rClient;
		GetClientRect(rClient);

		nWidth = rClient.Width();
	}

	int nCtrlHeight = dwm.OffsetCtrl(this, IDC_FILTERCOMBO).Height();
	int nXPos = 0, nYPos = 0, nVisible = 0;

	CDlgUnits dlu(*this);
	
	// indent controls after the first line so that they
	// line up neatly with the first line. Unless we're only
	// at the second control in which case there is nothing
	// to line up with
	const FILTERCTRL& fc0 = FILTERCTRLS[0];
	const FILTERCTRL& fc1 = FILTERCTRLS[1];
	int nLineIndent = dlu.ToPixelsX(fc0.nLenLabelDLU + 
									fc0.nLenCtrlDLU -
									fc1.nLenLabelDLU - 
									fc1.nLenCtrlDLU - 1);

	for (int nCtrl = 0; nCtrl < NUMFILTERCTRLS; nCtrl++)
	{
		CRect rCtrl;
		const FILTERCTRL& fc = FILTERCTRLS[nCtrl];
		
		// display this control only if the corresponding column
		// is also showing
		BOOL bWantCtrl = WantShowFilter(fc.nType);
		
		if (bWantCtrl)
		{
			// if we're at the start of the line then just move ctrls
			// else we must check whether there's enough space to fit
			
			if (nXPos > 0 || (nYPos > 0 && nXPos > nLineIndent)) // we're not the first
			{
				// work out the total length of label + ctrl
				int nLen = dlu.ToPixelsX(fc.nLenLabelDLU + FILTERCTRLXSPACING + fc.nLenCtrlDLU);

				// do we fit?
				if ((nLen + nXPos) > nWidth) // no
				{
					// move to next line
					nXPos = nLineIndent;
					nYPos += dlu.ToPixelsY(FILTERCTRLYSPACING) + nCtrlHeight;
				}
			}
			
			// move label
			rCtrl.left = nXPos;
			rCtrl.right = nXPos + dlu.ToPixelsX(fc.nLenLabelDLU);
			rCtrl.top = nYPos;
			rCtrl.bottom = nYPos + nCtrlHeight;
			
			if (!bCalcOnly)
				dwm.MoveWindow(GetDlgItem(fc.nIDLabel), rCtrl);
			
			// update XPos
			nXPos = rCtrl.right + dlu.ToPixelsX(FILTERCTRLXSPACING);
			
			// move ctrl
			rCtrl.left = nXPos;
			rCtrl.right = nXPos + dlu.ToPixelsX(fc.nLenCtrlDLU);
			rCtrl.top = nYPos;
			rCtrl.bottom = nYPos + nCtrlHeight;
			
			if (!bCalcOnly)
				dwm.MoveWindow(GetDlgItem(fc.nIDCtrl), rCtrl);
			
			// update XPos
			nXPos = rCtrl.right + dlu.ToPixelsX(FILTERCTRLXSPACING);

			nVisible++;
		}
		
		GetDlgItem(fc.nIDLabel)->ShowWindow(bWantCtrl ? SW_SHOW : SW_HIDE);
		GetDlgItem(fc.nIDLabel)->EnableWindow(bWantCtrl);
		GetDlgItem(fc.nIDCtrl)->ShowWindow(bWantCtrl ? SW_SHOW : SW_HIDE);
		GetDlgItem(fc.nIDCtrl)->EnableWindow(bWantCtrl);
	}

	return nYPos + nCtrlHeight + 2;
}

void CFilterBar::SetFilter(const FTDCFILTER& filter)
{
	m_filter = filter;
	UpdateData(FALSE);

	// the droplists don't get set properly if they
	// are empty but were not previously so we do it manually
// 	if (m_filter.sAllocTo.IsEmpty())
// 		m_cbAllocToFilter.SetCurSel(0);
	
	if (m_filter.sAllocBy.IsEmpty())
		m_cbAllocByFilter.SetCurSel(0);
	
	if (m_filter.sStatus.IsEmpty())
		m_cbStatusFilter.SetCurSel(0);
}

BOOL CFilterBar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// one-time init for risk filter combo
	m_cbRiskFilter.InsertString(0, CEnString(IDS_TDC_ANY)); // add a blank item
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
