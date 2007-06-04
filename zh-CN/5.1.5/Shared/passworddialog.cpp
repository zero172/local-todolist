// passworddialog.cpp : implementation file
//

#include "stdafx.h"
#include "passworddialog.h"
#include "stringres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDialog dialog

enum
{
	IDC_PASSWORD = 101,
	IDC_CONFIRMATION,
};

CString CPasswordDialog::s_sTitle(PASSWORD_TITLE);
CString CPasswordDialog::s_sPasswordPrompt(PASSWORD_ENTERPWD);
CString CPasswordDialog::s_sConfirmPrompt(PASSWORD_CONFIRMPWD);
CString CPasswordDialog::s_sConfirmFailed(PASSWORD_CONFIRMFAILED);
CString CPasswordDialog::s_sOK(BTN_OK);
CString CPasswordDialog::s_sCancel(BTN_CANCEL);

#define WM_POSTINITDIALOG (WM_USER+1)

CPasswordDialog::CPasswordDialog(BOOL bConfirm, LPCTSTR szExplanation, CWnd* /*pParent*/)
	: CRuntimeDlg(), m_bConfirm(bConfirm)
{
	//{{AFX_DATA_INIT(CPasswordDialog)
	m_sPassword = _T("");
	m_sConfirmation = _T("");
	//}}AFX_DATA_INIT

	// figure out how long the prompts might be
	const int PROMPTLEN = max(m_bConfirm ? s_sConfirmPrompt.GetLength() : 0, s_sPasswordPrompt.GetLength()) * 4;
	const int EDITSTART = PROMPTLEN + 8;
	const int EDITEND = EDITSTART + 110;

	int nLines = CalcLinesRequired(szExplanation, EDITEND - 7);
	const int YOFFSET = m_bConfirm ? 19 : 0;
	const int YEXPOFFSET = (NULL == szExplanation) ? 0 : (nLines * 9 + 9);

	if (szExplanation)
	    AddRCControl("LTEXT", "", szExplanation, 0, 0,7,7, EDITEND - 7, (nLines * 9), (UINT)IDC_STATIC);

    AddRCControl("LTEXT", "", s_sPasswordPrompt, 0, 0,7,9 + YEXPOFFSET,PROMPTLEN,9, (UINT)IDC_STATIC);
    AddRCControl("EDITTEXT", "", "", WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL, 0, EDITSTART,7 + YEXPOFFSET,110,14, IDC_PASSWORD);

	if (m_bConfirm)
	{
		AddRCControl("LTEXT", "", s_sConfirmPrompt, 0, 0,7, 28 + YEXPOFFSET,PROMPTLEN,9, (UINT)IDC_STATIC);
		AddRCControl("EDITTEXT", "", "", WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL, 0, EDITSTART,25 + YEXPOFFSET,110,14, IDC_CONFIRMATION);
	}

    AddRCControl("CONTROL", "Static", "", SS_ETCHEDHORZ, 0, 7,30 + YOFFSET + YEXPOFFSET, EDITEND - 7,1, (UINT)IDC_STATIC);
    AddRCControl("DEFPUSHBUTTON", "", s_sOK, WS_DISABLED | WS_TABSTOP, 0, EDITEND - 50 - 4 - 50,40 + YOFFSET + YEXPOFFSET,50,14,IDOK);
    AddRCControl("PUSHBUTTON", "", s_sCancel, WS_TABSTOP, 0, EDITEND - 50,40 + YOFFSET + YEXPOFFSET,50,14,IDCANCEL);
}


void CPasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDialog)
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);

	if (m_bConfirm)
		DDX_Text(pDX, IDC_CONFIRMATION, m_sConfirmation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDialog, CRuntimeDlg)
	//{{AFX_MSG_MAP(CPasswordDialog)
	ON_EN_CHANGE(IDC_PASSWORD, OnChangePassword)
	ON_EN_CHANGE(IDC_CONFIRMATION, OnChangeConfirmation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDialog message handlers

void CPasswordDialog::SetUIStrings(UINT nIDTitle, UINT nIDPasswordPrompt, 
								   UINT nIDConfirmPrompt, UINT nIDConfirmFailed,
								   UINT nIDOK, UINT nIDCancel)
{
	s_sTitle.LoadString(nIDTitle);
	s_sPasswordPrompt.LoadString(nIDPasswordPrompt);
	s_sConfirmPrompt.LoadString(nIDConfirmPrompt);
	s_sConfirmFailed.LoadString(nIDConfirmFailed);
	s_sOK.LoadString(nIDOK);
	s_sCancel.LoadString(nIDCancel);
}

int CPasswordDialog::DoModal() 
{	
	CString sTitle;
	sTitle.Format("%s - %s", PASSWORD_TITLE, AfxGetAppName());

	return CRuntimeDlg::DoModal(sTitle, RTD_DEFSTYLE, RTD_DEFEXSTYLE | WS_EX_TOPMOST); 
}

BOOL CPasswordDialog::OnInitDialog()
{
	CRuntimeDlg::OnInitDialog();

	GetDlgItem(IDC_PASSWORD)->SetFocus();

	return FALSE;
}

// static method
BOOL CPasswordDialog::RetrievePassword(BOOL bConfirm, CString& sPassword, LPCTSTR szExplanation)
{
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		CPasswordDialog dialog(bConfirm, szExplanation);

		if (IDOK == dialog.DoModal())
			bContinue = dialog.GetPassword().IsEmpty();
		else
			return FALSE;

		// check confirmation too
		if (!bContinue)
		{
			if (bConfirm)
			{
				if (dialog.GetConfirmation() != dialog.GetPassword())
				{
					szExplanation = s_sConfirmFailed;
					bContinue = TRUE;
				}
				else
					sPassword = dialog.GetPassword();
			}
			else
				sPassword = dialog.GetPassword();
		}
	}

	return TRUE;
}

BOOL CPasswordDialog::VerifyPassword(LPCTSTR szPassword, LPCTSTR szExplanation, LPCTSTR szErrorMsg)
{
	while (TRUE)
	{
		CPasswordDialog dialog(FALSE, szExplanation);

		if (IDOK == dialog.DoModal())
		{
			if (dialog.GetPassword() == szPassword)
				return TRUE;

			// else
			if (szErrorMsg && *szErrorMsg)
				szExplanation = szErrorMsg;
		}
		else
			break; // user cancelled
	}

	return FALSE;
}

void CPasswordDialog::OnChangePassword() 
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(!m_sPassword.IsEmpty() && (!m_bConfirm || !m_sConfirmation.IsEmpty()));
}

void CPasswordDialog::OnChangeConfirmation() 
{
	UpdateData();

	if (m_bConfirm)
		GetDlgItem(IDOK)->EnableWindow(!m_sPassword.IsEmpty() && (!m_bConfirm || !m_sConfirmation.IsEmpty()));
}
