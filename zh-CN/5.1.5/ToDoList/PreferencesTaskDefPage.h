#if !defined(AFX_PREFERENCESTASKDEFPAGE_H__852964E3_4ABD_4B66_88BA_F553177616F2__INCLUDED_)
#define AFX_PREFERENCESTASKDEFPAGE_H__852964E3_4ABD_4B66_88BA_F553177616F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTaskDefPage.h : header file
//

#include "tdlprioritycombobox.h"
#include "tdlriskcombobox.h"

#include <afxtempl.h>

#include "..\shared\colorbutton.h"
#include "..\shared\timeedit.h"
#include "..\shared\wndPrompt.h"
#include "..\shared\autocombobox.h"
#include "..\shared\groupline.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\maskedit.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage dialog

enum PTP_ATTRIB
{
	PTPA_PRIORITY,
	PTPA_COLOR,
	PTPA_ALLOCTO,
	PTPA_ALLOCBY,
	PTPA_STATUS,
	PTPA_CATEGORY,
	PTPA_TIMEEST,
	PTPA_RISK,
	PTPA_DUEDATE,
	// add to end
};

class CPreferencesTaskDefPage : public CPropertyPage, public CDialogHelper
{
	DECLARE_DYNCREATE(CPreferencesTaskDefPage)

// Construction
public:
	CPreferencesTaskDefPage();
	~CPreferencesTaskDefPage();

	void SetPriorityColors(const CDWordArray& aColors);

	int GetDefaultPriority() const { return m_nDefPriority; }
	int GetDefaultRisk() const { return m_nDefRisk; }
	CString GetDefaultAllocTo() const { return m_sDefAllocTo; }
	CString GetDefaultAllocBy() const { return m_sDefAllocBy; }
	CString GetDefaultStatus() const { return m_sDefStatus; }
	int GetDefaultCategories(CStringArray& aCats) const;
	CString GetDefaultCreatedBy() const { return m_sDefCreatedBy; }
	double GetDefaultTimeEst(int& nUnits) const;
	double GetDefaultCost() const { return m_dDefCost; }
	COLORREF GetDefaultColor() const { return m_crDef; }
	BOOL GetAutoDefaultStartDate() const { return m_bUseCreationForDefStartDate; }
	BOOL GetUseParentAttrib(PTP_ATTRIB nAttrib) const;

	int GetCategoryList(CStringArray& aItems) const;
	int GetStatusList(CStringArray& aItems) const;
	int GetAllocToList(CStringArray& aItems) const;
	int GetAllocByList(CStringArray& aItems) const;

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesTaskDefPage)
	enum { IDD = IDD_PREFTASKDEF_PAGE };
	CTDLRiskComboBox	m_cbDefRisk;
	CTDLPriorityComboBox	m_cbDefPriority;
	CAutoComboBox	m_cbAllocByList;
	CAutoComboBox	m_cbAllocToList;
	CAutoComboBox	m_cbStatusList;
	CAutoComboBox	m_cbCategoryList;
	CTimeEdit	m_eTimeEst;
	CString	m_sDefCreatedBy;
	double	m_dDefCost;
	//}}AFX_DATA
	CMaskEdit m_eCost;
	CCheckListBox	m_lbAttribUse;
	int		m_nDefPriority;
	int		m_nDefRisk;
	double		m_dDefTimeEst;
	CString	m_sDefAllocTo;
	CString	m_sDefAllocBy;
	CString	m_sDefStatus;
	CString	m_sDefCategory;
	CColorButton	m_btDefColor;
	COLORREF m_crDef;
	BOOL	m_bUseParentAttributes;
	int		m_nSelAttribUse;
	BOOL	m_bUseCreationForDefStartDate;
	CWndPromptManager m_mgrPrompts;
	CGroupLineManager m_mgrGroupLines;

	struct ATTRIBPREF
	{
		ATTRIBPREF() {}
		ATTRIBPREF(UINT nIDName, PTP_ATTRIB attrib, BOOL use) { sName.LoadString(nIDName); nAttrib = attrib; bUse = use; }

		CString sName;
		PTP_ATTRIB nAttrib;
		BOOL bUse;
	};
	CArray<ATTRIBPREF, ATTRIBPREF&> m_aAttribPrefs;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTaskDefPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesTaskDefPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetdefaultcolor();
	afx_msg void OnUseparentattrib();
	afx_msg void OnAttribUseChange();
	DECLARE_MESSAGE_MAP()

	void SaveComboList(const CAutoComboBox& combo, LPCTSTR szRegKey) const;
	void RestoreComboList(CAutoComboBox& combo, LPCTSTR szRegKey);
	int GetComboListItems(LPCTSTR szRegKey, CStringArray& aItems) const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTASKDEFPAGE_H__852964E3_4ABD_4B66_88BA_F553177616F2__INCLUDED_)
