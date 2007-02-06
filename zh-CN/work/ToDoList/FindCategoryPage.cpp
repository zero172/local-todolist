// FindTextPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindCategoryPage.h"

#include "..\shared\Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindCategoryPage property page

IMPLEMENT_DYNCREATE(CFindCategoryPage, CPropertyPage)

CFindCategoryPage::CFindCategoryPage() : CPropertyPage(CFindCategoryPage::IDD)
{
	//{{AFX_DATA_INIT(CFindCategoryPage)
	m_sCategories = _T("");
	//}}AFX_DATA_INIT
	m_bMatchAllCategories = TRUE;
}

CFindCategoryPage::~CFindCategoryPage()
{
}

void CFindCategoryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindCategoryPage)
	DDX_Control(pDX, IDC_CATEGORIES, m_cbCategory);
	DDX_CBString(pDX, IDC_CATEGORIES, m_sCategories);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_MATCHALLCATEGORIES, m_bMatchAllCategories);
}


BEGIN_MESSAGE_MAP(CFindCategoryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindCategoryPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindCategoryPage message handlers

int CFindCategoryPage::GetCategories(CStringArray& aCats)
{
	aCats.RemoveAll();

	if (GetSafeHwnd())
	{
		UpdateData();
		Misc::ParseIntoArray(m_sCategories, aCats, TRUE);

		if (!m_sCategories.IsEmpty())
		{
			// move or add to top of combo
			int nFind = FindSearchInCombo(m_sCategories);

			if (nFind != CB_ERR)
				m_cbCategory.DeleteString(nFind);

			m_cbCategory.InsertString(0, m_sCategories);
			UpdateData(FALSE);
		}

	}

	return aCats.GetSize();
}

BOOL CFindCategoryPage::GetMatchAllCategories()
{
	if (GetSafeHwnd())
	{
		UpdateData();
		return m_bMatchAllCategories;
	}

	return TRUE;
}

BOOL CFindCategoryPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString sKey(GetRegKey());

	// restore previous combo entries
	int nCount = AfxGetApp()->GetProfileInt(sKey, "LookForCount", 0);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey;
		sItemKey.Format("LookFor%d", nItem);

		m_cbCategory.AddString(AfxGetApp()->GetProfileString(sKey, sItemKey));
	}
	
	m_sCategories = AfxGetApp()->GetProfileString(sKey, "LastLookFor");
	m_bMatchAllCategories = AfxGetApp()->GetProfileInt(sKey, "LastMatchAllCategories", TRUE);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindCategoryPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	UpdateData();

	CString sKey(GetRegKey());

	// save combo entries
	int nCount = m_cbCategory.GetCount();
	AfxGetApp()->WriteProfileInt(sKey, "LookForCount", nCount);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItem;
		m_cbCategory.GetLBText(nItem, sItem);

		CString sItemKey;
		sItemKey.Format("LookFor%d", nItem);

		AfxGetApp()->WriteProfileString(sKey, sItemKey, sItem);
	}

	// rest
	AfxGetApp()->WriteProfileString(sKey, "LastLookFor", m_sCategories);
	AfxGetApp()->WriteProfileInt(sKey, "LastMatchAllCategories", m_bMatchAllCategories);
}

CString CFindCategoryPage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

int CFindCategoryPage::FindSearchInCombo(LPCTSTR szLookFor) const
{
	// CComboBox::FindStringExact is not case sensitive 
	// and we need that so we must roll it ourselves
	int nItem = m_cbCategory.GetCount();

	while (nItem--)
	{
		CString sItem;
		m_cbCategory.GetLBText(nItem, sItem);

		if (sItem == szLookFor)
			return nItem;
	}

	return CB_ERR;
}
