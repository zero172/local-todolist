#if !defined(AFX_PREFERENCESUITASKLISTCOLORSPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_)
#define AFX_PREFERENCESUITASKLISTCOLORSPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesUITasklistPageColors.h : header file
//

#include "..\shared\fileedit.h"
#include "..\shared\colorbutton.h"
#include "..\shared\fontcombobox.h"
#include "..\shared\autocombobox.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistColorsPage dialog

const COLORREF TASKDONECOLOR = RGB(192, 192, 192);
const COLORREF TASKDUECOLOR = RGB(255, 0, 0);
enum { COLOROPT_CATEGORY, COLOROPT_PRIORITY, COLOROPT_DEFAULT };

struct CATCOLOR
{
	CString sCategory;
	COLORREF color;
};

typedef CArray<CATCOLOR, CATCOLOR&> CCatColorArray;

class CPreferencesUITasklistColorsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreferencesUITasklistColorsPage)

// Construction
public:
	CPreferencesUITasklistColorsPage();
	~CPreferencesUITasklistColorsPage();

	BOOL GetColorPriority() const { return m_bColorPriority; }
	int GetTextColorOption() const { return m_nTextColorOption; }
	BOOL GetHidePriorityNumber() const { return m_bHidePriorityNumber && m_bColorPriority; }
	int GetPriorityColors(CDWordArray& aColors) const;
	int GetCategoryColors(CCatColorArray& aColors) const;
	BOOL GetTreeFont(CString& sFaceName, int& nPointSize) const;
	BOOL GetCommentsFont(CString& sFaceName, int& nPointSize) const;
	COLORREF GetGridlineColor() const { return m_bSpecifyGridColor ? m_crGridlines : -1; }
	COLORREF GetTaskDoneColor() const { return m_bSpecifyDoneColor ? m_crDone : TASKDONECOLOR; }
	COLORREF GetAlternateLineColor() const { return m_bAlternateLineColor ? m_crAltLine : -1; }
	COLORREF GetTaskDueColor() const { return m_bSpecifyDueColor ? m_crDue : -1; }
	BOOL GetColorTaskBackground() const { return m_bColorTaskBackground; }
	BOOL GetCommentsUseTreeFont() const { return m_bSpecifyTreeFont && m_bCommentsUseTreeFont; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesUITasklistColorsPage)
	enum { IDD = IDD_PREFUITASKLISTCOLORS_PAGE };
	CAutoComboBox	m_cbCategories;
	BOOL	m_bColorTaskBackground;
	BOOL	m_bCommentsUseTreeFont;
	BOOL	m_bHLSColorGradient;
	BOOL	m_bHidePriorityNumber;
	BOOL	m_bAlternateLineColor;
	int		m_nTextColorOption;
	CString	m_sSelCategory;
	BOOL	m_bSpecifyDueColor;
	//}}AFX_DATA
	BOOL	m_bSpecifyGridColor;
	BOOL	m_bSpecifyDoneColor;
	CColorButton	m_btCatColor;
	CColorButton	m_btDoneColor;
	CColorButton	m_btGridlineColor;
	CColorButton	m_btDueColor;
	CComboBox	m_cbTreeFontSize, m_cbCommentsFontSize;
	CFontComboBox	m_cbTreeFonts, m_cbCommentsFonts;
	BOOL	m_bSpecifyTreeFont;
	BOOL	m_bSpecifyCommentsFont;
	CFileEdit	m_eCheckboxImagePath;
	CColorButton	m_btSetColor;
	CColorButton	m_btLowColor;
	CColorButton	m_btHighColor;
	BOOL	m_bColorPriority;
	int		m_bGradientPriorityColors;
	int		m_nSelPriorityColor;
	BOOL	m_bShowTimeColumn;
	CDWordArray m_aPriorityColors;
	CCatColorArray m_aCategoryColors;
	COLORREF m_crLow, m_crHigh;
	CString m_sTreeFont;
	int		m_nTreeFontSize;
	CString m_sCommentsFont;
	int		m_nCommentsFontSize;
	COLORREF m_crGridlines, m_crDone;
	CColorButton	m_btAltLineColor;
	COLORREF m_crAltLine;
	COLORREF m_crDue;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesUITasklistColorsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesUITasklistColorsPage)
	afx_msg void OnAltlinecolor();
	afx_msg void OnSpecifyAlternatelinecolor();
	afx_msg void OnChangeTextColorOption();
	afx_msg void OnSetcategorycolor();
	afx_msg void OnEditCategorycolors();
	afx_msg void OnSelchangeCategorycolors();
	afx_msg void OnCommentsusetreefont();
	afx_msg void OnSpecifycommentsfont();
	afx_msg void OnDuetaskcolor();
	afx_msg void OnSetduetaskcolor();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	afx_msg void OnSpecifytreefont();
	afx_msg void OnSetgridlinecolor();
	afx_msg void OnSpecifygridlinecolor();
	afx_msg void OnSetdonecolor();
	afx_msg void OnSpecifydonecolor();
	afx_msg void OnLowprioritycolor();
	afx_msg void OnHighprioritycolor();
	afx_msg void OnSetprioritycolor();
	afx_msg void OnChangePriorityColorOption();
	afx_msg void OnColorPriority();
	afx_msg void OnSelchangePrioritycolors();
	afx_msg LRESULT OnCategoryAdded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCategoryDeleted(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	int FindCategoryColor(LPCTSTR szCategory);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESUITASKLISTPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_)
