#if !defined(AFX_TDLSENDTASKSDLG_H__02DB12DD_A125_4754_AE1C_F8BB9EE45903__INCLUDED_)
#define AFX_TDLSENDTASKSDLG_H__02DB12DD_A125_4754_AE1C_F8BB9EE45903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TDLSendTasksDlg.h : header file
//

enum TD_SENDWHAT 
{
	TDSW_ACTIVETASKLIST,
	TDSW_SELECTEDTASKS,	
};

enum TD_SENDAS
{
	TDSA_TASKLIST,
	TDSA_BODYTEXT,	
};

/////////////////////////////////////////////////////////////////////////////
// CTDLSendTasksDlg dialog

class CTDLSendTasksDlg : public CDialog
{
// Construction
public:
	CTDLSendTasksDlg(TD_SENDWHAT nWhat = TDSW_ACTIVETASKLIST, TD_SENDAS nSendAs = TDSA_TASKLIST, CWnd* pParent = NULL);   // standard constructor

	void GetSendOptions(TD_SENDWHAT& nWhat, TD_SENDAS& nSendAs) const 
	{
		nWhat = (TD_SENDWHAT)m_nSendWhat;
		nSendAs = (m_nSendWhat == TDSW_ACTIVETASKLIST) ? TDSA_TASKLIST : (TD_SENDAS)m_nSendTasksAsOption;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CTDLSendTasksDlg)
	enum { IDD = IDD_SENDTASKS_DIALOG };
	int		m_nSendWhat;
	int		m_nSendTasksAsOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLSendTasksDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLSendTasksDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSendWhat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLSENDTASKSDLG_H__02DB12DD_A125_4754_AE1C_F8BB9EE45903__INCLUDED_)
