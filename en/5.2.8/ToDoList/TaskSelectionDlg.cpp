// TaskSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "TaskSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskSelectionDlg dialog


CTaskSelectionDlg::CTaskSelectionDlg(LPCTSTR szRegKey) : CDialog(), m_sRegKey(szRegKey)
{
	//{{AFX_DATA_INIT(CTaskSelectionDlg)
	//}}AFX_DATA_INIT
	m_bCompletedTasks = AfxGetApp()->GetProfileInt(m_sRegKey, "CompletedTasks", TRUE);
	m_bIncompleteTasks = AfxGetApp()->GetProfileInt(m_sRegKey, "IncompleteTasks", TRUE);
	m_bSelectedTasks = AfxGetApp()->GetProfileInt(m_sRegKey, "SelectedTasks", FALSE);
	m_bSelectedSubtasks = AfxGetApp()->GetProfileInt(m_sRegKey, "SelectedSubtasks", TRUE);
}


void CTaskSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskSelectionDlg)
	DDX_Check(pDX, IDC_INCLUDESUBTASKS, m_bSelectedSubtasks);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_INCLUDEDONE, m_bCompletedTasks);
	DDX_Check(pDX, IDC_INCLUDENOTDONE, m_bIncompleteTasks);
	DDX_Radio(pDX, IDC_ALLTASKS, m_bSelectedTasks);
}


BEGIN_MESSAGE_MAP(CTaskSelectionDlg, CDialog)
//{{AFX_MSG_MAP(CTaskSelectionDlg)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_ALLTASKS, OnChangetasksOption)
ON_BN_CLICKED(IDC_INCLUDEDONE, OnIncludeDone)
ON_BN_CLICKED(IDC_SELTASK, OnChangetasksOption)
ON_BN_CLICKED(IDC_INCLUDENOTDONE, OnIncludeNotDone)
ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskSelectionDlg message handlers

BOOL CTaskSelectionDlg::Create(UINT nIDRefFrame, CWnd* pParent, UINT nID)
{
	ASSERT (nIDRefFrame && pParent);
	
	if (CDialog::Create(IDD_TASKSELECTION_DIALOG, pParent))
	{
		if (nID != IDC_STATIC)
			SetDlgCtrlID(nID);
		
		CWnd* pFrame = pParent->GetDlgItem(nIDRefFrame);
		
		if (pFrame)
		{
			CRect rFrame;
			pFrame->GetWindowRect(rFrame);
			pParent->ScreenToClient(rFrame);
			
			MoveWindow(rFrame);
		}

		return TRUE;
	}
	
	return FALSE;
}

void CTaskSelectionDlg::OnDestroy() 
{
	UpdateData();
	
	CDialog::OnDestroy();
	
	// save settings
	AfxGetApp()->WriteProfileInt(m_sRegKey, "CompletedTasks", m_bCompletedTasks);
	AfxGetApp()->WriteProfileInt(m_sRegKey, "IncompleteTasks", m_bIncompleteTasks);
	AfxGetApp()->WriteProfileInt(m_sRegKey, "SelectedTasks", m_bSelectedTasks);
	AfxGetApp()->WriteProfileInt(m_sRegKey, "SelectedSubtasks", m_bSelectedSubtasks);
}

void CTaskSelectionDlg::OnChangetasksOption() 
{
	UpdateData();
	
	GetDlgItem(IDC_INCLUDEDONE)->EnableWindow(!m_bSelectedTasks);
	GetDlgItem(IDC_INCLUDENOTDONE)->EnableWindow(!m_bSelectedTasks);
	GetDlgItem(IDC_INCLUDESUBTASKS)->EnableWindow(m_bSelectedTasks);
	
	GetParent()->SendMessage(WM_TASKSELDLG_CHANGE);
}

void CTaskSelectionDlg::OnIncludeDone() 
{
	UpdateData();
	
	// prevent the user unchecking both tick boxes
	if (!m_bCompletedTasks && !m_bIncompleteTasks)
	{
		m_bIncompleteTasks = TRUE;
		UpdateData(FALSE);
	}
	
	GetParent()->SendMessage(WM_TASKSELDLG_CHANGE);
}
void CTaskSelectionDlg::OnIncludeNotDone() 
{
	UpdateData();
	
	// prevent the user unchecking both tick boxes
	if (!m_bCompletedTasks && !m_bIncompleteTasks)
	{
		m_bCompletedTasks = TRUE;
		UpdateData(FALSE);
	}
	
	GetParent()->SendMessage(WM_TASKSELDLG_CHANGE);
}

BOOL CTaskSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_INCLUDEDONE)->EnableWindow(!m_bSelectedTasks);
	GetDlgItem(IDC_INCLUDENOTDONE)->EnableWindow(!m_bSelectedTasks);
	GetDlgItem(IDC_INCLUDESUBTASKS)->EnableWindow(m_bSelectedTasks);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskSelectionDlg::SetWantSelectedTasks(BOOL bWant)
{
	m_bSelectedTasks = bWant;
	UpdateData(FALSE);
	
	GetDlgItem(IDC_INCLUDEDONE)->EnableWindow(!m_bSelectedTasks);
	GetDlgItem(IDC_INCLUDENOTDONE)->EnableWindow(!m_bSelectedTasks);
}

void CTaskSelectionDlg::SetWantCompletedTasks(BOOL bWant)
{
	// prevent the user unchecking both tick boxes
	if (!bWant && !m_bIncompleteTasks)
		m_bIncompleteTasks = TRUE;
	
	m_bCompletedTasks = bWant;
	UpdateData(FALSE);
}

void CTaskSelectionDlg::SetWantInCompleteTasks(BOOL bWant)
{
	// prevent the user unchecking both tick boxes
	if (!bWant && !m_bCompletedTasks)
		m_bCompletedTasks = TRUE;
	
	m_bIncompleteTasks = bWant;
	UpdateData(FALSE);
}
