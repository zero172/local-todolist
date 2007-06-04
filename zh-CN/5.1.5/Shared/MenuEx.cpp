// MenuEx.cpp: implementation of the CMenuEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuEx.h"
#include "Themed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef HBMMENU_MBAR_CLOSE
#	define HBMMENU_MBAR_RESTORE        ((HBITMAP)  2)
#	define HBMMENU_MBAR_MINIMIZE       ((HBITMAP)  3)
#	define HBMMENU_MBAR_CLOSE          ((HBITMAP)  5)
#	define HBMMENU_MBAR_CLOSE_D        ((HBITMAP)  6)
#	define HBMMENU_MBAR_MINIMIZE_D     ((HBITMAP)  7)
#endif

#ifndef ODS_HOTLIGHT
#	define ODS_HOTLIGHT        0x0040
#	define ODS_INACTIVE        0x0080
#endif

#define BTNBORDER 0
#define FUDGE 8

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuEx::CMenuEx()
{

}

CMenuEx::~CMenuEx()
{
}

BOOL CMenuEx::AddMDIButton(MENUEX_BTN nBtn, UINT nCmdID, UINT nThemeBMID, BOOL bRightJustify)
{
	ASSERT (GetSafeHmenu());

	if (!GetSafeHmenu())
		return FALSE;

	HBITMAP hbm = NULL;
	BOOL bOwnerdraw = (IsThemed() && (int)nThemeBMID < 0);

	if (!bOwnerdraw)
	{
		switch (nBtn)
		{
		case MEB_MINIMIZE:
			if (LoadThemeBitmap(m_bmMinimize, nThemeBMID))
				hbm = m_bmMinimize;
			else
				hbm = HBMMENU_MBAR_MINIMIZE;
			break;
			
		case MEB_RESTORE:
			if (LoadThemeBitmap(m_bmRestore, nThemeBMID))
				hbm = m_bmRestore;
			else
				hbm = HBMMENU_MBAR_RESTORE;
			break;
			
		case MEB_CLOSE:
			if (LoadThemeBitmap(m_bmClose, nThemeBMID))
				hbm = m_bmClose;
			else
				hbm = HBMMENU_MBAR_CLOSE;
			break;
			
		default:
			return FALSE;
		}
	}
	
	UINT nFlags = (bOwnerdraw ? MFT_OWNERDRAW : MFT_BITMAP) | 
					(bRightJustify ? MFT_RIGHTJUSTIFY : 0);
		
	return InsertMenu((UINT)-1, nFlags, nCmdID, CBitmap::FromHandle(hbm));
}

BOOL CMenuEx::DeleteMDIMenu(UINT nCmdID)
{
	// CMenu::DeleteMenu won't work on bitmap buttons directly
	// so we must traverse all menu items looking for nCmdID
	int nItem = GetMenuItemCount();

	while (nItem--)
	{
		UINT nMenuCmdID = GetMenuItemID(nItem);

		if (nCmdID == nMenuCmdID)
		{
			DeleteMenu(nItem, MF_BYPOSITION);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMenuEx::LoadThemeBitmap(CEnBitmapEx& bitmap, UINT nThemeBMID)
{
	if (bitmap.GetSafeHandle())
		return TRUE;

	if (!IsThemed() || (int)nThemeBMID <= 0)
		return FALSE;

	if (!bitmap.LoadBitmap(nThemeBMID))
		return FALSE;

	bitmap.RemapSysColors();
	return TRUE;
}

BOOL CMenuEx::IsThemed()
{
	return CThemed().IsNonClientThemed();
}

BOOL CMenuEx::DrawMDIButton(UINT nIDBitmap, LPDRAWITEMSTRUCT lpDrawItemStruct, BOOL bDrawEdges, COLORREF crMask)
{
	// draw the button
	CEnBitmapEx bitmap;

   if (crMask == (COLORREF)-1)
	   bitmap.LoadSysBitmap(nIDBitmap);
   else
   {
      bitmap.LoadBitmap(nIDBitmap);
      bitmap.ReplaceColor(crMask, GetSysColor(COLOR_3DFACE));
   }
	
	if (bitmap.GetSafeHandle())
	{
		CRect rect(lpDrawItemStruct->rcItem);


		
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CDC memDC;
      
      pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

		if (memDC.CreateCompatibleDC(pDC))
		{
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			
			// deflate rect to suit image size
			rect.DeflateRect((rect.Width() - bm.bmWidth) / 2, (rect.Height() - bm.bmHeight) / 2);

			// offset if selected
			UINT nState = lpDrawItemStruct->itemState;
			BOOL bSelected = (nState & ODS_SELECTED);

			if (bSelected)
				rect.OffsetRect(1, 1);

			// draw it			
			CBitmap* pOldBM = memDC.SelectObject(&bitmap);
			pDC->BitBlt(rect.left, rect.top, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY);

			if (bDrawEdges)
			{
				rect = lpDrawItemStruct->rcItem;
				rect.InflateRect(BTNBORDER, BTNBORDER);

				//BOOL bHilited = (nState & ODS_HOTLIGHT);

				if (nState & ODS_HOTLIGHT)   
					pDC->Draw3dRect(rect, GetSysColor(COLOR_3DHIGHLIGHT), GetSysColor(COLOR_3DSHADOW));

				else if (nState & ODS_SELECTED)
					pDC->Draw3dRect(rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));
				else
					pDC->Draw3dRect(rect, GetSysColor(COLOR_MENU), GetSysColor(COLOR_MENU));
			}

			// cleanup
			memDC.SelectObject(pOldBM);

			return TRUE;
		}
	}

	return FALSE;
}


BOOL CMenuEx::MeasureMDIButton(UINT nIDBitmap, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CBitmap bitmap;
	
	if (bitmap.LoadBitmap(nIDBitmap))
	{
		BITMAP bm;
		bitmap.GetBitmap(&bm);

		lpMeasureItemStruct->itemHeight = bm.bmHeight + BTNBORDER * 2;
		lpMeasureItemStruct->itemWidth = bm.bmWidth + BTNBORDER * 2 - FUDGE;

		return TRUE;
	}

	return FALSE;
}
