// colorbutton.cpp : implementation file
//

#include "stdafx.h"
#include "colorbutton.h"
#include "encolordialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

#define WM_SHOWACCELERATORS 0x0128

CColorButton::CColorButton() : m_color(0)
{
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_ENABLE()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOWACCELERATORS, OnShowAccelerators)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// create a temp dc to paint on
	CDC dcTemp;

	if (dcTemp.CreateCompatibleDC(&dc))
	{
		CBitmap bmTemp;
		CRect rClient;

		GetClientRect(rClient);

		if (bmTemp.CreateCompatibleBitmap(&dc, rClient.right, rClient.bottom))
		{
			CBitmap* pOld = dcTemp.SelectObject(&bmTemp);

			// default draw to temp dc
			DefWindowProc(WM_PAINT, (WPARAM)(HDC)dcTemp, 0);

			// draw color on top
			CRect rColor(rClient);
			rColor.DeflateRect(5, 4, 5, 5);

			DWORD dwStyle = GetStyle();

			if (dwStyle & BS_LEFT)
				rColor.left = rColor.right - max(12, rColor.Height());

			else if (dwStyle & BS_RIGHT)
				rColor.right = rColor.left + max(12, rColor.Height());

			else
				rColor.SetRectEmpty();

			if (!rColor.IsRectEmpty())
			{
				dcTemp.FillSolidRect(rColor, m_color);

				// black border
				if (m_color > 0)
				{
					if (IsWindowEnabled())
						::FrameRect(dcTemp, rColor, (HBRUSH)::GetStockObject(BLACK_BRUSH));
					else
						::FrameRect(dcTemp, rColor, (HBRUSH)::GetStockObject(GRAY_BRUSH));
				}

				dc.BitBlt(0, 0, rClient.right, rClient.bottom, &dcTemp, 0, 0, SRCCOPY);

				// cleanup
				dc.SelectObject(pOld);

				return;
			}
			
			// cleanup
			dc.SelectObject(pOld);
		}
	}
	
	// else draw to default dc
	DefWindowProc(WM_PAINT, (WPARAM)(HDC)dc, 0);
}

void CColorButton::SetColor(COLORREF color) 
{ 
	m_color = color; 

	if (GetSafeHwnd())
		Invalidate();
}

void CColorButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	Invalidate();
	UpdateWindow();

	CButton::OnLButtonUp(nFlags, point);
}

void CColorButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CButton::OnMouseMove(nFlags, point);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDblClk(nFlags, point);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	Invalidate();
	UpdateWindow();
}

void CColorButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	Invalidate();
	UpdateWindow();
}

void CColorButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	Invalidate();
	UpdateWindow();
}

BOOL CColorButton::OnClicked() 
{
	Invalidate();
	UpdateWindow();

	CEnColorDialog dialog(m_color, CC_FULLOPEN | CC_RGBINIT);

	if (dialog.DoModal() == IDOK)
	{
		m_color = dialog.m_cc.rgbResult;
	
		Invalidate();
		UpdateWindow();

		return FALSE; // pass to parent
	}

	return TRUE; // no need to bother parent
}

LRESULT CColorButton::OnShowAccelerators(WPARAM /*wp*/, LPARAM /*lp*/)
{
	LRESULT lr = Default();
	RedrawWindow();

	return lr;
}
