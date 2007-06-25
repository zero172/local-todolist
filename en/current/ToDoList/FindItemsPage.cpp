// FindTextPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindItemsPage.h"

#include "..\shared\Misc.h"
#include "..\shared\dialoghelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindItemsPage property page

IMPLEMENT_DYNCREATE(CFindItemsPage, CPropertyPage)

CFindItemsPage::CFindItemsPage() : CPropertyPage(CFindItemsPage::IDD)
{
	//{{AFX_DATA_INIT(CFindItemsPage)
	m_sItems = _T("");
	//}}AFX_DATA_INIT
	m_bMatchAllItems = FALSE;
}

CFindItemsPage::~CFindItemsPage()
{
}

void CFindItemsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindItemsPage)
	DDX_Control(pDX, IDC_ITEMS, m_cbItems);
	DDX_CBString(pDX, IDC_ITEMS, m_sItems);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_MATCHALLITEMS, m_bMatchAllItems);
}


BEGIN_MESSAGE_MAP(CFindItemsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindItemsPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindItemsPage message handlers

int CFindItemsPage::GetItems(CStringArray& aItems)
{
	aItems.RemoveAll();

	if (GetSafeHwnd())
	{
		UpdateData();
		Misc::ParseIntoArray(m_sItems, aItems, TRUE);

		if (!m_sItems.IsEmpty())
		{
			// move or add to top of combo
			int nFind = FindSearchInCombo(m_sItems);

			if (nFind != CB_ERR)
				m_cbItems.DeleteString(nFind);

			m_cbItems.InsertString(0, m_sItems);
			UpdateData(FALSE);
		}

	}

	return aItems.GetSize();
}

/*
void CFindItemsPage::SetItems(const CStringArray& aItems)
{
	if (!GetSafeHwnd())
		return;

	CStringArray aCurItems;
	GetItems(aCurItems);

	if (Misc::ArraysMatch(aItems, aCurItems))
		return;

	CDialogHelper::SetComboBoxItems(m_cbItems, aItems);
}
*/

BOOL CFindItemsPage::GetMatchAllItems()
{
	if (GetSafeHwnd())
	{
		UpdateData();
		return m_bMatchAllItems;
	}

	return TRUE;
}

BOOL CFindItemsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString sKey(GetRegKey());

	// restore previous combo entries
	int nCount = AfxGetApp()->GetProfileInt(sKey, "LookForCount", 0);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey;
		sItemKey.Format("LookFor%d", nItem);

		m_cbItems.AddString(AfxGetApp()->GetProfileString(sKey, sItemKey));
	}
	
	m_sItems = AfxGetApp()->GetProfileString(sKey, "LastLookFor");
	m_bMatchAllItems = AfxGetApp()->GetProfileInt(sKey, "LastMatchAllItems", FALSE);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindItemsPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	UpdateData();

	CString sKey(GetRegKey());

	// save combo entries
	int nCount = m_cbItems.GetCount();
	AfxGetApp()->WriteProfileInt(sKey, "LookForCount", nCount);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItem;
		m_cbItems.GetLBText(nItem, sItem);

		CString sItemKey;
		sItemKey.Format("LookFor%d", nItem);

		AfxGetApp()->WriteProfileString(sKey, sItemKey, sItem);
	}

	// rest
	AfxGetApp()->WriteProfileString(sKey, "LastLookFor", m_sItems);
	AfxGetApp()->WriteProfileInt(sKey, "LastMatchAllItems", m_bMatchAllItems);
}

CString CFindItemsPage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

int CFindItemsPage::FindSearchInCombo(LPCTSTR szLookFor) const
{
	// CComboBox::FindStringExact is not case sensitive 
	// and we need that so we must roll it ourselves
	int nItem = m_cbItems.GetCount();

	while (nItem--)
	{
		CString sItem;
		m_cbItems.GetLBText(nItem, sItem);

		if (sItem == szLookFor)
			return nItem;
	}

	return CB_ERR;
}
