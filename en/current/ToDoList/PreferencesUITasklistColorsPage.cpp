// PreferencesUITasklistColorsPage.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "PreferencesUITasklistColorsPage.h"

#include "..\shared\colordef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistColorsPage property page

// default priority colors
const COLORREF PRIORITYLOWCOLOR = RGB(30, 225, 0);
const COLORREF PRIORITYHIGHCOLOR = RGB(255, 0, 0);
const COLORREF CBMASKCOLOR = RGB(255, 0, 0);
const COLORREF ALTERNATELINECOLOR = RGB(245, 245, 255);
const COLORREF GRIDLINECOLOR = RGB(192, 192, 192);
const COLORREF TASKDONECOLOR = RGB(128, 128, 128);
const COLORREF TASKDUECOLOR = RGB(255, 0, 0);

IMPLEMENT_DYNCREATE(CPreferencesUITasklistColorsPage, CPreferencesPageBase)

CPreferencesUITasklistColorsPage::CPreferencesUITasklistColorsPage() : 
	CPreferencesPageBase(CPreferencesUITasklistColorsPage::IDD),
	m_nTextColorOption(COLOROPT_DEFAULT), m_cbCategories(TRUE)

{
	//{{AFX_DATA_INIT(CPreferencesUITasklistColorsPage)
	//}}AFX_DATA_INIT
	// priority colors
	m_nTextColorOption = COLOROPT_DEFAULT;
	m_nSelPriorityColor = 0;

}

CPreferencesUITasklistColorsPage::~CPreferencesUITasklistColorsPage()
{
}

void CPreferencesUITasklistColorsPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesUITasklistColorsPage)
	DDX_Control(pDX, IDC_CATEGORYCOLORS, m_cbCategories);
	DDX_Check(pDX, IDC_COLORTASKBKGND, m_bColorTaskBackground);
	DDX_Check(pDX, IDC_COMMENTSUSETREEFONT, m_bCommentsUseTreeFont);
	DDX_Check(pDX, IDC_USEHLSGRADIENT, m_bHLSColorGradient);
	DDX_Check(pDX, IDC_HIDEPRIORITYNUMBER, m_bHidePriorityNumber);
	DDX_Check(pDX, IDC_ALTERNATELINECOLOR, m_bAlternateLineColor);
	DDX_Radio(pDX, IDC_COLORTEXTBYCATEGORY, m_nTextColorOption);
	DDX_CBString(pDX, IDC_CATEGORYCOLORS, m_sSelCategory);
	DDX_Control(pDX, IDC_SETCATEGORYCOLOR, m_btCatColor);
	DDX_Check(pDX, IDC_DUETASKCOLOR, m_bSpecifyDueColor);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DUETODAYTASKCOLOR, m_bSpecifyDueTodayColor);
	DDX_Control(pDX, IDC_SETDUETASKCOLOR, m_btDueColor);
	DDX_Control(pDX, IDC_SETDUETODAYTASKCOLOR, m_btDueTodayColor);
	DDX_Control(pDX, IDC_SETDONECOLOR, m_btDoneColor);
	DDX_Control(pDX, IDC_SETGRIDLINECOLOR, m_btGridlineColor);
	DDX_Control(pDX, IDC_TREEFONTSIZE, m_cbTreeFontSize);
	DDX_Control(pDX, IDC_TREEFONTLIST, m_cbTreeFonts);
	DDX_Control(pDX, IDC_COMMENTSFONTSIZE, m_cbCommentsFontSize);
	DDX_Control(pDX, IDC_COMMENTSFONTLIST, m_cbCommentsFonts);
	DDX_Check(pDX, IDC_SPECIFYTREEFONT, m_bSpecifyTreeFont);
	DDX_Check(pDX, IDC_SPECIFYCOMMENTSFONT, m_bSpecifyCommentsFont);
	DDX_Check(pDX, IDC_SPECIFYGRIDLINECOLOR, m_bSpecifyGridColor);
	DDX_Check(pDX, IDC_SPECIFYDONECOLOR, m_bSpecifyDoneColor);
	DDX_Control(pDX, IDC_SETALTLINECOLOR, m_btAltLineColor);
	DDX_Control(pDX, IDC_SETPRIORITYCOLOR, m_btSetColor);
	DDX_Control(pDX, IDC_LOWPRIORITYCOLOR, m_btLowColor);
	DDX_Control(pDX, IDC_HIGHPRIORITYCOLOR, m_btHighColor);
	DDX_Check(pDX, IDC_COLORPRIORITY, m_bColorPriority);
	DDX_Radio(pDX, IDC_INDIVIDUALPRIORITYCOLORS, m_bGradientPriorityColors);
	DDX_CBIndex(pDX, IDC_PRIORITYCOLORS, m_nSelPriorityColor);

	if (pDX->m_bSaveAndValidate)
	{
		CString sSize;

		// tree
		m_sTreeFont = m_cbTreeFonts.GetSelectedFont();
		m_cbTreeFontSize.GetLBText(m_cbTreeFontSize.GetCurSel(), sSize);
		m_nTreeFontSize = atoi(sSize);

		// comments
		m_sCommentsFont = m_cbCommentsFonts.GetSelectedFont();
		m_cbCommentsFontSize.GetLBText(m_cbCommentsFontSize.GetCurSel(), sSize);
		m_nCommentsFontSize = atoi(sSize);
	}
	else
	{
		CString sFontSize;

		// tree
		m_cbTreeFonts.SetSelectedFont(m_sTreeFont);
		sFontSize.Format("%d", m_nTreeFontSize);

		if (CB_ERR == m_cbTreeFontSize.SelectString(-1, sFontSize))
		{
			m_nTreeFontSize = 9;
			m_cbTreeFontSize.SetCurSel(0);
		}

		// comments
		m_cbCommentsFonts.SetSelectedFont(m_sCommentsFont);
		sFontSize.Format("%d", m_nCommentsFontSize);

		if (CB_ERR == m_cbCommentsFontSize.SelectString(-1, sFontSize))
		{
			m_nCommentsFontSize = 9;
			m_cbCommentsFontSize.SetCurSel(0);
		}
	}
}


BEGIN_MESSAGE_MAP(CPreferencesUITasklistColorsPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesUITasklistColorsPage)
	ON_BN_CLICKED(IDC_SETALTLINECOLOR, OnAltlinecolor)
	ON_BN_CLICKED(IDC_ALTERNATELINECOLOR, OnSpecifyAlternatelinecolor)
	ON_BN_CLICKED(IDC_COLORTEXTBYCATEGORY, OnChangeTextColorOption)
	ON_BN_CLICKED(IDC_SETCATEGORYCOLOR, OnSetcategorycolor)
	ON_CBN_EDITCHANGE(IDC_CATEGORYCOLORS, OnEditCategorycolors)
	ON_CBN_SELCHANGE(IDC_CATEGORYCOLORS, OnSelchangeCategorycolors)
	ON_BN_CLICKED(IDC_COMMENTSUSETREEFONT, OnCommentsusetreefont)
	ON_BN_CLICKED(IDC_SPECIFYCOMMENTSFONT, OnSpecifycommentsfont)
	ON_BN_CLICKED(IDC_DUETASKCOLOR, OnDuetaskcolor)
	ON_BN_CLICKED(IDC_COLORTASKSBYCOLOR, OnChangeTextColorOption)
	ON_BN_CLICKED(IDC_COLORTEXTBYPRIORITY, OnChangeTextColorOption)
	ON_BN_CLICKED(IDC_SETDUETASKCOLOR, OnSetduetaskcolor)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DUETODAYTASKCOLOR, OnDuetodaytaskcolor)
	ON_BN_CLICKED(IDC_SETDUETODAYTASKCOLOR, OnSetduetodaytaskcolor)
	ON_BN_CLICKED(IDC_SPECIFYTREEFONT, OnSpecifytreefont)
	ON_BN_CLICKED(IDC_SETGRIDLINECOLOR, OnSetgridlinecolor)
	ON_BN_CLICKED(IDC_SPECIFYGRIDLINECOLOR, OnSpecifygridlinecolor)
	ON_BN_CLICKED(IDC_SETDONECOLOR, OnSetdonecolor)
	ON_BN_CLICKED(IDC_SPECIFYDONECOLOR, OnSpecifydonecolor)
	ON_BN_CLICKED(IDC_LOWPRIORITYCOLOR, OnLowprioritycolor)
	ON_BN_CLICKED(IDC_HIGHPRIORITYCOLOR, OnHighprioritycolor)
	ON_BN_CLICKED(IDC_SETPRIORITYCOLOR, OnSetprioritycolor)
	ON_BN_CLICKED(IDC_GRADIENTPRIORITYCOLORS, OnChangePriorityColorOption)
	ON_BN_CLICKED(IDC_COLORPRIORITY, OnColorPriority)
	ON_CBN_SELCHANGE(IDC_PRIORITYCOLORS, OnSelchangePrioritycolors)
	ON_BN_CLICKED(IDC_INDIVIDUALPRIORITYCOLORS, OnChangePriorityColorOption)
   ON_REGISTERED_MESSAGE(WM_ACB_ITEMADDED, OnCategoryAdded)
   ON_REGISTERED_MESSAGE(WM_ACB_ITEMDELETED, OnCategoryDeleted)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistColorsPage message handlers

BOOL CPreferencesUITasklistColorsPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	GetDlgItem(IDC_GRADIENTPRIORITYCOLORS)->EnableWindow(m_bColorPriority);
	GetDlgItem(IDC_INDIVIDUALPRIORITYCOLORS)->EnableWindow(m_bColorPriority);
	GetDlgItem(IDC_PRIORITYCOLORS)->EnableWindow(m_bColorPriority && !m_bGradientPriorityColors);
	GetDlgItem(IDC_USEHLSGRADIENT)->EnableWindow(m_bColorPriority && m_bGradientPriorityColors);
	GetDlgItem(IDC_CATEGORYCOLORS)->EnableWindow(m_nTextColorOption == COLOROPT_CATEGORY);
	GetDlgItem(IDC_TREEFONTSIZE)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_TREEFONTSIZELABEL)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_TREEFONTLIST)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_COMMENTSUSETREEFONT)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_SPECIFYCOMMENTSFONT)->EnableWindow(!m_bCommentsUseTreeFont || !m_bSpecifyTreeFont);

	BOOL bEnableCommentsFont = m_bSpecifyCommentsFont && (!m_bSpecifyTreeFont || !m_bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZE)->EnableWindow(bEnableCommentsFont);
	GetDlgItem(IDC_COMMENTSFONTSIZELABEL)->EnableWindow(bEnableCommentsFont);
	GetDlgItem(IDC_COMMENTSFONTLIST)->EnableWindow(bEnableCommentsFont);

	m_btSetColor.EnableWindow(m_bColorPriority && !m_bGradientPriorityColors);
	m_btLowColor.EnableWindow(m_bColorPriority && m_bGradientPriorityColors);
	m_btHighColor.EnableWindow(m_bColorPriority && m_bGradientPriorityColors);
	m_btGridlineColor.EnableWindow(m_bSpecifyGridColor);
	m_btDoneColor.EnableWindow(m_bSpecifyDoneColor);
	m_btAltLineColor.EnableWindow(m_bAlternateLineColor);
	m_btDueColor.EnableWindow(m_bSpecifyDueColor);
	m_btDueTodayColor.EnableWindow(m_bSpecifyDueTodayColor);
	m_btCatColor.EnableWindow(m_nTextColorOption == COLOROPT_CATEGORY && !m_sSelCategory.IsEmpty());
	
	m_btGridlineColor.SetColor(m_crGridlines);
	m_btLowColor.SetColor(m_crLow);
	m_btHighColor.SetColor(m_crHigh);
	m_btSetColor.SetColor(m_aPriorityColors[0]);
	m_btDoneColor.SetColor(m_crDone);
	m_btAltLineColor.SetColor(m_crAltLine);
	m_btDueColor.SetColor(m_crDue);
	m_btDueTodayColor.SetColor(m_crDueToday);

	// category colors
	if (!m_sSelCategory.IsEmpty())
	{
		int nColor = m_aCategoryColors.GetSize();

		while (nColor--)
		{
			const CATCOLOR& cc = m_aCategoryColors[nColor];
			
			if (!cc.sCategory.IsEmpty())
				m_cbCategories.AddString(cc.sCategory);
		}

		nColor = FindCategoryColor(m_sSelCategory);

		if (nColor != -1)
			m_btCatColor.SetColor(m_aCategoryColors[nColor].color);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesUITasklistColorsPage::OnLowprioritycolor() 
{
	m_crLow = m_btLowColor.GetColor();
}

void CPreferencesUITasklistColorsPage::OnHighprioritycolor() 
{
	m_crHigh = m_btHighColor.GetColor();
}

void CPreferencesUITasklistColorsPage::OnSetprioritycolor() 
{
	VERIFY(m_nSelPriorityColor >= 0);

	m_aPriorityColors.SetAt(m_nSelPriorityColor, m_btSetColor.GetColor());
}

void CPreferencesUITasklistColorsPage::OnChangePriorityColorOption() 
{
	UpdateData();

	GetDlgItem(IDC_PRIORITYCOLORS)->EnableWindow(m_bColorPriority && !m_bGradientPriorityColors);
	GetDlgItem(IDC_USEHLSGRADIENT)->EnableWindow(m_bColorPriority && m_bGradientPriorityColors);

	m_btSetColor.EnableWindow(m_bColorPriority && !m_bGradientPriorityColors && m_nSelPriorityColor >= 0);
	m_btLowColor.EnableWindow(m_bColorPriority && m_bGradientPriorityColors);
	m_btHighColor.EnableWindow(m_bColorPriority && m_bGradientPriorityColors);
}

void CPreferencesUITasklistColorsPage::OnColorPriority() 
{
	UpdateData();

	GetDlgItem(IDC_GRADIENTPRIORITYCOLORS)->EnableWindow(m_bColorPriority);
	GetDlgItem(IDC_INDIVIDUALPRIORITYCOLORS)->EnableWindow(m_bColorPriority);

	OnChangePriorityColorOption(); // to handle the other controls

	// if the text color option is COLOROPT_PRIORITY and 
	// the user has turned off priority coloring then switch to default
	if (!m_bColorPriority && m_nTextColorOption == COLOROPT_PRIORITY)
	{
		m_nTextColorOption = COLOROPT_DEFAULT;
		UpdateData(FALSE);
	}
}

int CPreferencesUITasklistColorsPage::GetPriorityColors(CDWordArray& aColors) const 
{ 
	aColors.RemoveAll();

	if (m_bColorPriority)
	{
		if (!m_bGradientPriorityColors)
			aColors.Copy(m_aPriorityColors); 
		else
		{
			aColors.Add(m_crLow);

			if (m_bHLSColorGradient)
			{
				HLSX hlsLow, hlsHigh;
				RGBX rgbLow(m_crLow), rgbHigh(m_crHigh);

				// convert rgb limits to hls
				RGBX::RGB2HLS(rgbLow, hlsLow);
				RGBX::RGB2HLS(rgbHigh, hlsHigh);

				float fHueInc = (hlsHigh.fHue - hlsLow.fHue) / 10;
				float fSatInc = (hlsHigh.fSaturation - hlsLow.fSaturation) / 10;
				float fLumInc = (hlsHigh.fLuminosity - hlsLow.fLuminosity) / 10;

				HLSX hlsColor = hlsLow;
				int nPriority = 10; // aColors[0] added at top

				while (nPriority--)
				{
					hlsColor.fHue += fHueInc;
					hlsColor.fSaturation += fSatInc;
					hlsColor.fLuminosity += fLumInc;
					
					RGBX rgbColor;
					RGBX::HLS2RGB(hlsColor, rgbColor);

					COLORREF color = min(RGB(255, 255, 255), (COLORREF)rgbColor);
					aColors.Add((DWORD)color);
				}

			}
			else // RGB Gradient
			{
				BYTE redLow = GetRValue(m_crLow);
				BYTE greenLow = GetGValue(m_crLow);
				BYTE blueLow = GetBValue(m_crLow);

				BYTE redHigh = GetRValue(m_crHigh);
				BYTE greenHigh = GetGValue(m_crHigh);
				BYTE blueHigh = GetBValue(m_crHigh);

				for (int nPriority = 1; nPriority <= 10; nPriority++) // m_aPriorityColors[0] added at top
				{
					double dRed = (redLow * (10 - nPriority) / 10) + (redHigh * nPriority / 10);
					double dGreen = (greenLow * (10 - nPriority) / 10) + (greenHigh * nPriority / 10);
					double dBlue = (blueLow * (10 - nPriority) / 10) + (blueHigh * nPriority / 10);

					double dColor = dRed + (dGreen * 256) + (dBlue * 256 * 256);

					aColors.Add((DWORD)min(RGB(255, 255, 255), (COLORREF)(int)dColor));
				}
			}
		}
	}
	else
	{
		// grayscale
		aColors.Add(RGB(240, 240, 240));	// 0
		aColors.Add(RGB(216, 216, 216));	// 1
		aColors.Add(RGB(192, 192, 192));	// 2
		aColors.Add(RGB(168, 168, 168));	// 3
		aColors.Add(RGB(144, 144, 144));	// 4
		aColors.Add(RGB(120, 120, 120));	// 5
		aColors.Add(RGB(96, 96, 96));		// 6
		aColors.Add(RGB(72, 72, 72));		// 7
		aColors.Add(RGB(48, 48, 48));		// 8
		aColors.Add(RGB(24, 24, 24));		// 9
		aColors.Add(0);						// 10
	}
	
	return aColors.GetSize(); 
}

void CPreferencesUITasklistColorsPage::GetTaskDueColors(COLORREF& crDue, COLORREF& crDueToday) const 
{ 
	crDue = m_bSpecifyDueColor ? m_crDue : -1; 
	crDueToday = m_bSpecifyDueTodayColor ? m_crDueToday : -1; 
}

int CPreferencesUITasklistColorsPage::GetCategoryColors(CCatColorArray& aColors) const
{
	aColors.Copy(m_aCategoryColors);

	return aColors.GetSize();
}

void CPreferencesUITasklistColorsPage::OnSelchangePrioritycolors() 
{
	UpdateData();

	ASSERT (m_nSelPriorityColor >= 0);
	
	if (m_nSelPriorityColor >= 0)
	{
		m_btSetColor.SetColor(m_aPriorityColors[m_nSelPriorityColor]);
		m_btSetColor.EnableWindow(TRUE);
	}
	else
		m_btSetColor.EnableWindow(FALSE);
}

void CPreferencesUITasklistColorsPage::OnSpecifytreefont() 
{
	UpdateData();

	GetDlgItem(IDC_TREEFONTLIST)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_TREEFONTSIZE)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_TREEFONTSIZELABEL)->EnableWindow(m_bSpecifyTreeFont);
	GetDlgItem(IDC_COMMENTSUSETREEFONT)->EnableWindow(m_bSpecifyTreeFont);

	GetDlgItem(IDC_SPECIFYCOMMENTSFONT)->EnableWindow(!m_bCommentsUseTreeFont || !m_bSpecifyTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZE)->EnableWindow(m_bSpecifyCommentsFont && !m_bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZELABEL)->EnableWindow(m_bSpecifyCommentsFont && !m_bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTLIST)->EnableWindow(m_bSpecifyCommentsFont && !m_bCommentsUseTreeFont);

	if (m_bSpecifyTreeFont)
		m_cbTreeFonts.SetSelectedFont(m_sTreeFont);
}

BOOL CPreferencesUITasklistColorsPage::GetTreeFont(CString& sFaceName, int& nPointSize) const
{
	if (m_bSpecifyTreeFont)
	{
		sFaceName = m_sTreeFont;
		nPointSize = m_nTreeFontSize;
	}

	return m_bSpecifyTreeFont;
}

BOOL CPreferencesUITasklistColorsPage::GetCommentsFont(CString& sFaceName, int& nPointSize) const
{
	if ((m_bSpecifyTreeFont && m_bCommentsUseTreeFont) || !m_bSpecifyCommentsFont)
		return FALSE;

	sFaceName = m_sCommentsFont;
	nPointSize = m_nCommentsFontSize;

	return m_bSpecifyCommentsFont;
}

void CPreferencesUITasklistColorsPage::OnSetgridlinecolor() 
{
	m_crGridlines = m_btGridlineColor.GetColor();
}

void CPreferencesUITasklistColorsPage::OnSpecifygridlinecolor() 
{
	UpdateData();	

	m_btGridlineColor.EnableWindow(m_bSpecifyGridColor);
}

void CPreferencesUITasklistColorsPage::OnSetdonecolor() 
{
	m_crDone = m_btDoneColor.GetColor();
}

void CPreferencesUITasklistColorsPage::OnSpecifydonecolor() 
{
	UpdateData();	

	m_btDoneColor.EnableWindow(m_bSpecifyDoneColor);
}

void CPreferencesUITasklistColorsPage::OnAltlinecolor() 
{
	m_crAltLine = m_btAltLineColor.GetColor();
}

void CPreferencesUITasklistColorsPage::OnSpecifyAlternatelinecolor() 
{
	UpdateData();	
	
	m_btAltLineColor.EnableWindow(m_bAlternateLineColor);
}

void CPreferencesUITasklistColorsPage::OnChangeTextColorOption() 
{
	UpdateData();

	// if the text color option is COLOROPT_PRIORITY and 
	// the user has got priority coloring turned off then switch it on
	if (!m_bColorPriority && m_nTextColorOption == COLOROPT_PRIORITY)
	{
		m_bColorPriority = TRUE;
		UpdateData(FALSE);

		OnColorPriority();
	}

	GetDlgItem(IDC_CATEGORYCOLORS)->EnableWindow(m_nTextColorOption == COLOROPT_CATEGORY);
	m_btCatColor.EnableWindow(m_nTextColorOption == COLOROPT_CATEGORY && 
								!m_sSelCategory.IsEmpty());
}

void CPreferencesUITasklistColorsPage::OnSetcategorycolor() 
{
	UpdateData();

	int nColor = FindCategoryColor(m_sSelCategory);

	if (nColor >= 0)
		m_aCategoryColors[nColor].color = m_btCatColor.GetColor();
	else
	{
		CATCOLOR cc;
		cc.sCategory = m_sSelCategory;
		cc.color = m_btCatColor.GetColor();

		m_aCategoryColors.Add(cc);
	}
}

int CPreferencesUITasklistColorsPage::FindCategoryColor(LPCTSTR szCategory)
{
	int nColor = m_aCategoryColors.GetSize();

	while (nColor--)
	{
		CATCOLOR& cc = m_aCategoryColors[nColor];

		if (cc.sCategory.CompareNoCase(szCategory) == 0)
			return nColor;
	}

	return -1;
}

void CPreferencesUITasklistColorsPage::OnEditCategorycolors() 
{
	UpdateData();

	m_btCatColor.EnableWindow(m_nTextColorOption == COLOROPT_CATEGORY && 
								!m_sSelCategory.IsEmpty());
}

void CPreferencesUITasklistColorsPage::OnSelchangeCategorycolors() 
{
	UpdateData();

	int nSel = m_cbCategories.GetCurSel();
	m_cbCategories.GetLBText(nSel, m_sSelCategory);
	
	int nColor = FindCategoryColor(m_sSelCategory);

	if (nColor >= 0)
		m_btCatColor.SetColor(m_aCategoryColors[nColor].color);

	m_btCatColor.EnableWindow(m_nTextColorOption == COLOROPT_CATEGORY && 
								!m_sSelCategory.IsEmpty());
}

LRESULT CPreferencesUITasklistColorsPage::OnCategoryAdded(WPARAM /*wParam*/, LPARAM lParam)
{
   CString sCategory((LPCTSTR)lParam);

   if (!sCategory.IsEmpty() && FindCategoryColor(sCategory) != -1)
   {
		CATCOLOR cc;
		cc.sCategory = sCategory;
		cc.color = 0;

		m_aCategoryColors.Add(cc);
   }
  
   return 0L;
}

LRESULT CPreferencesUITasklistColorsPage::OnCategoryDeleted(WPARAM /*wParam*/, LPARAM lParam)
{
   CString sCategory((LPCTSTR)lParam);

   if (!sCategory.IsEmpty())
   {
      int nDel = FindCategoryColor(sCategory);

      if (nDel != -1)
         m_aCategoryColors.RemoveAt(nDel);
   }
  
   return 0L;
}

void CPreferencesUITasklistColorsPage::OnCommentsusetreefont() 
{
	UpdateData();

	BOOL bCommentsUseTreeFont = (m_bCommentsUseTreeFont && m_bSpecifyTreeFont);
	GetDlgItem(IDC_SPECIFYCOMMENTSFONT)->EnableWindow(!bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZE)->EnableWindow(m_bSpecifyCommentsFont && !bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZELABEL)->EnableWindow(m_bSpecifyCommentsFont && !bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTLIST)->EnableWindow(m_bSpecifyCommentsFont && !bCommentsUseTreeFont);
}

void CPreferencesUITasklistColorsPage::OnSpecifycommentsfont() 
{
	UpdateData();
	
	BOOL bCommentsUseTreeFont = (m_bCommentsUseTreeFont && m_bSpecifyTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZE)->EnableWindow(m_bSpecifyCommentsFont && !bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTSIZELABEL)->EnableWindow(m_bSpecifyCommentsFont && !bCommentsUseTreeFont);
	GetDlgItem(IDC_COMMENTSFONTLIST)->EnableWindow(m_bSpecifyCommentsFont && !bCommentsUseTreeFont);

	if (m_bSpecifyCommentsFont && !bCommentsUseTreeFont)
		m_cbCommentsFonts.SetSelectedFont(m_sCommentsFont);
}

void CPreferencesUITasklistColorsPage::OnDuetaskcolor() 
{
	UpdateData();	
	
	m_btDueColor.EnableWindow(m_bSpecifyDueColor);
}

void CPreferencesUITasklistColorsPage::OnDuetodaytaskcolor() 
{
	UpdateData();	
	
	m_btDueTodayColor.EnableWindow(m_bSpecifyDueTodayColor);
}

void CPreferencesUITasklistColorsPage::OnSetduetaskcolor() 
{
	m_crDue = m_btDueColor.GetColor();
}

void CPreferencesUITasklistColorsPage::OnSetduetodaytaskcolor() 
{
	m_crDueToday = m_btDueTodayColor.GetColor();
}

void CPreferencesUITasklistColorsPage::LoadPreferences(const CPreferencesStorage& prefs)
{
	m_crLow = prefs.GetProfileInt("Preferences\\Colors", "Low", PRIORITYLOWCOLOR);
	m_crHigh = prefs.GetProfileInt("Preferences\\Colors", "High", PRIORITYHIGHCOLOR);

	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P0", RGB(30, 225, 0)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P1", RGB(30, 225, 0)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P2", RGB(30, 225, 0)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P3", RGB(30, 225, 0)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P4", RGB(0, 0, 255)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P5", RGB(0, 0, 255)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P6", RGB(0, 0, 255)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P7", RGB(0, 0, 255)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P8", RGB(255, 0, 0)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P9", RGB(255, 0, 0)));
	m_aPriorityColors.Add(prefs.GetProfileInt("Preferences\\Colors", "P10", RGB(255, 0, 0)));

	// category colors
	int nColor = prefs.GetProfileInt("Preferences\\CatColors", "Count", 0);

	while (nColor--)
	{
		CString sKey;
		sKey.Format("Preferences\\CatColors\\P%d", nColor);

		CATCOLOR cc;
		cc.color = prefs.GetProfileInt(sKey, "Color", 0);
		cc.sCategory = prefs.GetProfileString(sKey, "Category");

		if (!cc.sCategory.IsEmpty())
		{
			m_sSelCategory = cc.sCategory;
			m_aCategoryColors.Add(cc);
		}
	}

	// add in the users default categories
	CStringArray aDefCats;
	prefs.GetArrayItems("Preferences\\CategoryList", aDefCats);

	int nDefColor = aDefCats.GetSize();

	while (nDefColor--)
	{
		const CString& sDefCat = aDefCats[nDefColor];

		if (sDefCat.IsEmpty())
			continue;

		// make sure this category is not already specified
		nColor = m_aCategoryColors.GetSize();

		while (nColor--)
		{
			if (m_aCategoryColors[nColor].sCategory.CompareNoCase(sDefCat) == 0) // match
				break;
		}

		if (nColor >= 0) // match
			continue; // skip

		CATCOLOR cc;
		cc.color = 0;
		cc.sCategory = sDefCat;

		m_sSelCategory = sDefCat;
		m_aCategoryColors.Add(cc);
	}
	
	// prefs
	m_bColorPriority = prefs.GetProfileInt("Preferences", "ColorPriority", TRUE);
	m_bGradientPriorityColors = !prefs.GetProfileInt("Preferences", "IndividualPriorityColors", FALSE);
	m_sTreeFont = prefs.GetProfileString("Preferences", "TreeFont", "Arial");
	m_nTreeFontSize = prefs.GetProfileInt("Preferences", "FontSize", 8);
	m_bSpecifyTreeFont = prefs.GetProfileInt("Preferences", "SpecifyTreeFont", FALSE);
	m_sCommentsFont = prefs.GetProfileString("Preferences", "CommentsFont", "Arial");
	m_nCommentsFontSize = prefs.GetProfileInt("Preferences", "CommentsFontSize", 8);
	m_bSpecifyCommentsFont = prefs.GetProfileInt("Preferences", "SpecifyCommentsFont", FALSE);
	m_bSpecifyGridColor = prefs.GetProfileInt("Preferences", "SpecifyGridColor", TRUE);
	m_crGridlines = prefs.GetProfileInt("Preferences\\Colors", "Gridlines", GRIDLINECOLOR);
	m_bSpecifyDoneColor = prefs.GetProfileInt("Preferences", "SpecifyDoneColor", TRUE);
	m_bSpecifyDueColor = prefs.GetProfileInt("Preferences", "SpecifyDueColor", FALSE);
	m_bSpecifyDueTodayColor = prefs.GetProfileInt("Preferences", "SpecifyDueTodayColor", FALSE);
	m_crDone = prefs.GetProfileInt("Preferences\\Colors", "TaskDone", TASKDONECOLOR);
	m_crDue = prefs.GetProfileInt("Preferences\\Colors", "TaskDue", TASKDUECOLOR);
	m_crDueToday = prefs.GetProfileInt("Preferences\\Colors", "TaskDueToday", TASKDUECOLOR);
	m_bColorTaskBackground = prefs.GetProfileInt("Preferences", "ColorTaskBackground", FALSE);
	m_bCommentsUseTreeFont = prefs.GetProfileInt("Preferences", "CommentsUseTreeFont", FALSE);
	m_bHLSColorGradient = prefs.GetProfileInt("Preferences", "HLSColorGradient", TRUE);
	m_bHidePriorityNumber = prefs.GetProfileInt("Preferences", "HidePriorityNumber", FALSE);
	m_bAlternateLineColor = prefs.GetProfileInt("Preferences", "AlternateLineColor", TRUE);
	m_crAltLine = prefs.GetProfileInt("Preferences\\Colors", "AlternateLines", ALTERNATELINECOLOR);

	// bkwds compatibility
	if (prefs.GetProfileInt("Preferences", "ColorByPriority", FALSE))
		m_nTextColorOption = COLOROPT_PRIORITY;

	m_nTextColorOption = prefs.GetProfileInt("Preferences", "TextColorOption", m_nTextColorOption);
}

void CPreferencesUITasklistColorsPage::SavePreferences(CPreferencesStorage& prefs)
{
	// save settings
	// priority colors
	prefs.WriteProfileInt("Preferences\\Colors", "Low", m_crLow);
	prefs.WriteProfileInt("Preferences\\Colors", "High", m_crHigh);

	int nColor = 11;

	while (nColor--)
	{
		CString sKey;
		sKey.Format("P%d", nColor);
		prefs.WriteProfileInt("Preferences\\Colors", sKey, m_aPriorityColors[nColor]);
	}

	// category colors
	nColor = m_aCategoryColors.GetSize();
	prefs.WriteProfileInt("Preferences\\CatColors", "Count", nColor);

	while (nColor--)
	{
		CString sKey;
		sKey.Format("Preferences\\CatColors\\P%d", nColor);

		const CATCOLOR& cc = m_aCategoryColors[nColor];
		prefs.WriteProfileInt(sKey, "Color", cc.color);
		prefs.WriteProfileString(sKey, "Category", cc.sCategory);
	}

	// save settings
	prefs.WriteProfileInt("Preferences", "TextColorOption", m_nTextColorOption);
	prefs.WriteProfileInt("Preferences", "ColorPriority", m_bColorPriority);
	prefs.WriteProfileInt("Preferences", "IndividualPriorityColors", !m_bGradientPriorityColors);
	prefs.WriteProfileString("Preferences", "TreeFont", m_sTreeFont);
	prefs.WriteProfileInt("Preferences", "FontSize", m_nTreeFontSize);
	prefs.WriteProfileInt("Preferences", "SpecifyTreeFont", m_bSpecifyTreeFont);
	prefs.WriteProfileString("Preferences", "CommentsFont", m_sCommentsFont);
	prefs.WriteProfileInt("Preferences", "CommentsFontSize", m_nCommentsFontSize);
	prefs.WriteProfileInt("Preferences", "SpecifyCommentsFont", m_bSpecifyCommentsFont);
	prefs.WriteProfileInt("Preferences", "SpecifyGridColor", m_bSpecifyGridColor);
	prefs.WriteProfileInt("Preferences\\Colors", "Gridlines", m_crGridlines);
	prefs.WriteProfileInt("Preferences", "SpecifyDoneColor", m_bSpecifyDoneColor);
	prefs.WriteProfileInt("Preferences\\Colors", "TaskDone", m_crDone);
	prefs.WriteProfileInt("Preferences", "SpecifyDueColor", m_bSpecifyDueColor);
	prefs.WriteProfileInt("Preferences\\Colors", "TaskDue", m_crDue);
	prefs.WriteProfileInt("Preferences", "SpecifyDueTodayColor", m_bSpecifyDueTodayColor);
	prefs.WriteProfileInt("Preferences\\Colors", "TaskDueToday", m_crDueToday);
	prefs.WriteProfileInt("Preferences", "ColorTaskBackground", m_bColorTaskBackground);
	prefs.WriteProfileInt("Preferences", "CommentsUseTreeFont", m_bCommentsUseTreeFont);
	prefs.WriteProfileInt("Preferences", "HLSColorGradient", m_bHLSColorGradient);
	prefs.WriteProfileInt("Preferences", "HidePriorityNumber", m_bHidePriorityNumber);
	prefs.WriteProfileInt("Preferences\\Colors", "AlternateLines", m_crAltLine);
	prefs.WriteProfileInt("Preferences", "AlternateLineColor", m_bAlternateLineColor);
}
