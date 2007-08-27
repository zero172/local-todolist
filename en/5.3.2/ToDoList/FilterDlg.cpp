// FilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FilterDlg.h"
#include "tdstringres.h"

#include "..\shared\enstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog


CFilterDlg::CFilterDlg(DWORD dwFlags, CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent), 
	  m_cbCategoryFilter(dwFlags & FB_MULTISELCAT, IDS_NOCATEGORY, IDS_TDC_ANY),
	  m_cbAllocToFilter(dwFlags & FB_MULTISELALLOCTO, IDS_NOALLOCTO, IDS_TDC_ANYONE),
	  m_cbAllocByFilter(FALSE, 0, IDS_TDC_ANYONE),
	  m_cbStatusFilter(FALSE, 0, IDS_TDC_ANY)
{
	//{{AFX_DATA_INIT(CFilterDlg)
	//}}AFX_DATA_INIT
}


void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterDlg)
	DDX_Control(pDX, IDC_CATEGORYFILTERCOMBO, m_cbCategoryFilter);
	DDX_Control(pDX, IDC_STATUSFILTERCOMBO, m_cbStatusFilter);
	DDX_Control(pDX, IDC_PRIORITYFILTERCOMBO, m_cbPriorityFilter);
	DDX_Control(pDX, IDC_RISKFILTERCOMBO, m_cbRiskFilter);
	DDX_Control(pDX, IDC_ALLOCTOFILTERCOMBO, m_cbAllocToFilter);
	DDX_Control(pDX, IDC_ALLOCBYFILTERCOMBO, m_cbAllocByFilter);
	DDX_Control(pDX, IDC_FILTERCOMBO, m_cbTaskFilter);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_FILTERCOMBO, (int&)m_filter.nFilter);
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

		BOOL bAll = FALSE;
		DDX_Check(pDX, IDC_ALLCATEGORIES, bAll);
		m_filter.SetFlag(FT_ANYCATEGORY, !bAll);

		// alloc to
		m_cbAllocToFilter.GetChecked(m_filter.aAllocTo);

		DDX_Check(pDX, IDC_ALLALLOCTO, bAll);
		m_filter.SetFlag(FT_ANYALLOCTO, !bAll);
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

		BOOL bAll = !m_filter.HasFlag(FT_ANYCATEGORY);
		DDX_Check(pDX, IDC_ALLCATEGORIES, bAll);

		// alloc to
		m_cbAllocToFilter.SetChecked(m_filter.aAllocTo);

		bAll = !m_filter.HasFlag(FT_ANYALLOCTO);
		DDX_Check(pDX, IDC_ALLALLOCTO, bAll);
	}
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterDlg)
	ON_BN_CLICKED(IDC_CLEARFILTER, OnClearfilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg message handlers

int CFilterDlg::DoModal(const CFilteredToDoCtrl& tdc, 
						const CDWordArray& aPriorityColors)
{
	// main filter
	tdc.GetFilter(m_filter);
	
	// alloc filters
	tdc.GetAllocToNames(m_aAllocTo);
	tdc.GetAllocByNames(m_aAllocBy);
	m_aAllocTo.Add(""); // add blank item to top
	m_aAllocBy.Add(""); // add blank item to top
	
	// category filter
	tdc.GetCategoryNames(m_aCategory);
	m_aCategory.Add(""); // add blank item to top
	
	// status filter
	CStringArray aStatus;
	tdc.GetStatusNames(m_aStatus);
	m_aStatus.Add(""); // add blank item to top

	m_aPriorityColors.Copy(aPriorityColors);

	return CDialog::DoModal();
}

void CFilterDlg::GetFilter(FTDCFILTER& filter)
{
	filter = m_filter;
}

BOOL CFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// alloc filters
	SetComboBoxItems(m_cbAllocToFilter, m_aAllocTo);
	SetComboBoxItems(m_cbAllocByFilter, m_aAllocBy);
	SetComboBoxItems(m_cbCategoryFilter, m_aCategory);
	SetComboBoxItems(m_cbStatusFilter, m_aStatus);

	// priority
	m_cbPriorityFilter.SetColors(m_aPriorityColors);
	m_cbPriorityFilter.InsertColor(0, (COLORREF)-1, CEnString(IDS_TDC_ANY)); // add a blank item

	// risk
	m_cbRiskFilter.InsertString(0, CEnString(IDS_TDC_ANY)); // add a blank item

	// update UI
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterDlg::OnClearfilter() 
{
	m_filter.Reset();
	
	UpdateData(FALSE);
	
	// the allocto and category don't get set properly if they
	// are empty but were not previously so we do it manually
	m_cbAllocToFilter.SetCurSel(0);
	m_cbAllocByFilter.SetCurSel(0);
	m_cbCategoryFilter.SetCurSel(0);
}
