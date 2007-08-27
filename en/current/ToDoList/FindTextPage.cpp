// FindTextPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "FindTextPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTextPage property page

IMPLEMENT_DYNCREATE(CFindTextPage, CPropertyPage)

CFindTextPage::CFindTextPage(DWORD dwShowFlags) : 
	CPropertyPage(CFindTextPage::IDD), m_dwShowFlags(dwShowFlags)
{
	//{{AFX_DATA_INIT(CFindTextPage)
	m_sText = _T("");
	m_bMatchWholeWord = FALSE;
	m_bMatchCase = FALSE;
	m_bMatchAll = FALSE;
	//}}AFX_DATA_INIT
}

CFindTextPage::~CFindTextPage()
{
}

void CFindTextPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTextPage)
	DDX_Control(pDX, IDC_TEXT, m_cbText);
	DDX_CBString(pDX, IDC_TEXT, m_sText);
	DDX_Check(pDX, IDC_MATCHWHOLEWORD, m_bMatchWholeWord);
	DDX_Check(pDX, IDC_MATCHCASE, m_bMatchCase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTextPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFindTextPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTextPage message handlers

CString CFindTextPage::GetText()
{
	if (GetSafeHwnd())
	{
		UpdateData();

		if (!m_sText.IsEmpty())
		{
			// move or add to top of combo
			int nFind = FindSearchInCombo(m_sText);

			if (nFind != CB_ERR)
				m_cbText.DeleteString(nFind);

			m_cbText.InsertString(0, m_sText);
			UpdateData(FALSE);
		}

		return m_sText;
	}

	return "";
}

BOOL CFindTextPage::GetMatchCase()
{
	if (GetSafeHwnd())
	{
		UpdateData();
		return (m_dwShowFlags & FTP_SHOWMATCHCASE) && m_bMatchCase;
	}

	return FALSE;
}

BOOL CFindTextPage::GetMatchWholeWord()
{
	if (GetSafeHwnd())
	{
		UpdateData();
		return (m_dwShowFlags & FTP_SHOWMATCHWHOLEWORD) && m_bMatchWholeWord;
	}

	return FALSE;
}

BOOL CFindTextPage::GetMatchAllItems()
{
	if (GetSafeHwnd())
	{
		UpdateData();
		return (m_dwShowFlags & FTP_SHOWMATCHALLWORDS) && m_bMatchAll;
	}

	return FALSE;
}

BOOL CFindTextPage::OnInitDialog()  
{
	CPropertyPage::OnInitDialog();

	CString sKey(GetRegKey());

	// restore previous combo entries
	int nCount = AfxGetApp()->GetProfileInt(sKey, "LookForCount", 0);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey;
		sItemKey.Format("LookFor%d", nItem);

		m_cbText.AddString(AfxGetApp()->GetProfileString(sKey, sItemKey));
	}
	
	m_sText = AfxGetApp()->GetProfileString(sKey, "LastLookFor");
	m_bMatchCase = AfxGetApp()->GetProfileInt(sKey, "MatchCase", FALSE);
	m_bMatchWholeWord = AfxGetApp()->GetProfileInt(sKey, "MatchWholeWord", FALSE);
	m_bMatchAll = AfxGetApp()->GetProfileInt(sKey, "MatchAllItems", FALSE);

	if (!(m_dwShowFlags & FTP_SHOWMATCHCASE))
	{
		GetDlgItem(IDC_MATCHCASE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MATCHCASE)->EnableWindow(FALSE);
	}

	if (!(m_dwShowFlags & FTP_SHOWMATCHWHOLEWORD))
	{
		GetDlgItem(IDC_MATCHWHOLEWORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MATCHWHOLEWORD)->EnableWindow(FALSE);
	}

	if (!(m_dwShowFlags & FTP_SHOWMATCHALLWORDS))
	{
		GetDlgItem(IDC_MATCHALLWORDS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MATCHALLWORDS)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindTextPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	UpdateData();

	CString sKey(GetRegKey());

	// save combo entries
	int nCount = m_cbText.GetCount();
	AfxGetApp()->WriteProfileInt(sKey, "LookForCount", nCount);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItem;
		m_cbText.GetLBText(nItem, sItem);

		CString sItemKey;
		sItemKey.Format("LookFor%d", nItem);

		AfxGetApp()->WriteProfileString(sKey, sItemKey, sItem);
	}

	// rest
	AfxGetApp()->WriteProfileString(sKey, "LastLookFor", m_sText);
	AfxGetApp()->WriteProfileInt(sKey, "MatchCase", m_bMatchCase);
	AfxGetApp()->WriteProfileInt(sKey, "MatchWholeWord", m_bMatchWholeWord);
	AfxGetApp()->WriteProfileInt(sKey, "MatchAllItems", m_bMatchAll);
}

CString CFindTextPage::GetRegKey() const
{
	CString sKey, sTitle;
	GetWindowText(sTitle);

	if (sTitle.IsEmpty())
		sTitle = m_psp.pszTitle;

	sKey.Format("FindTasks\\%s", sTitle);

	return sKey;
}

int CFindTextPage::FindSearchInCombo(LPCTSTR szLookFor) const
{
	// CComboBox::FindStringExact is not case sensitive 
	// and we need that so we must roll it ourselves
	int nItem = m_cbText.GetCount();

	while (nItem--)
	{
		CString sItem;
		m_cbText.GetLBText(nItem, sItem);

		if (sItem == szLookFor)
			return nItem;
	}

	return CB_ERR;
}

BOOL CFindTextPage::OnSetActive()
{
	m_cbText.SetFocus();
	m_cbText.SetEditSel(0, -1);

	return CPropertyPage::OnSetActive();
}
