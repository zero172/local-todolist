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


CFilterDlg::CFilterDlg(BOOL bMultiCategorySel, CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent), m_cbCategoryFilter(bMultiCategorySel)
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
	DDX_CBString(pDX, IDC_ALLOCTOFILTERCOMBO, m_filter.sAllocTo);
	DDX_CBString(pDX, IDC_ALLOCBYFILTERCOMBO, m_filter.sAllocBy);
	DDX_CBString(pDX, IDC_STATUSFILTERCOMBO, m_filter.sStatus);
	
	// special handling
	if (pDX->m_bSaveAndValidate)
	{
		int nPriority;
		DDX_CBIndex(pDX, IDC_PRIORITYFILTERCOMBO, nPriority);
		m_filter.nPriority = nPriority - 1;

		int nRisk;
		DDX_CBIndex(pDX, IDC_RISKFILTERCOMBO, nRisk);
		m_filter.nRisk = nRisk - 1;

		m_cbCategoryFilter.GetChecked(m_filter.aCategories);

		BOOL bAllCats;
		DDX_Check(pDX, IDC_ALLCATEGORIES, bAllCats);
		m_filter.SetFlag(FT_ANYCATEGORY, !bAllCats);
	}
	else
	{
		int nPriority = m_filter.nPriority + 1;
		DDX_CBIndex(pDX, IDC_PRIORITYFILTERCOMBO, nPriority);

		int nRisk = m_filter.nRisk + 1;
		DDX_CBIndex(pDX, IDC_RISKFILTERCOMBO, nRisk);

		m_cbCategoryFilter.SetChecked(m_filter.aCategories);

		BOOL bAllCats = !m_filter.HasFlag(FT_ANYCATEGORY);
		DDX_Check(pDX, IDC_ALLCATEGORIES, bAllCats);
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
	CStringArray aCategory;
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
	m_cbPriorityFilter.InsertColor(0, (COLORREF)-1, ""); // add a blank item

	// risk
	m_cbRiskFilter.InsertString(0, "");

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
