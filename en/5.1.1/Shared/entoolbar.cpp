// entoolbar.cpp : implementation file
//

#include "stdafx.h"
#include "entoolbar.h"
#include "enbitmapex.h"
#include "imageprocessors.h"
#include "osversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnToolBar

CEnToolBar::CEnToolBar()
{
}

CEnToolBar::~CEnToolBar()
{
}


BEGIN_MESSAGE_MAP(CEnToolBar, CToolBar)
	//{{AFX_MSG_MAP(CEnToolBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnToolBar message handlers

BOOL CEnToolBar::LoadToolBar(LPCTSTR lpszResourceName, LPCTSTR szImagePath)
{
	if (CToolBar::LoadToolBar(lpszResourceName))
	{
		if (szImagePath && ::GetFileAttributes(szImagePath) != 0xffffffff)
			SetImage(szImagePath);
		else
		{
			GetToolBarCtrl().SetDisabledImageList(NULL);
			m_ilDisabled.DeleteImageList();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CEnToolBar::LoadToolBar(UINT nIDResource, LPCTSTR szImagePath)
{
	return LoadToolBar(MAKEINTRESOURCE(nIDResource), szImagePath);
}

BOOL CEnToolBar::LoadToolBar(UINT nIDResource, UINT nIDImage)
{
	if (CToolBar::LoadToolBar(MAKEINTRESOURCE(nIDResource)))
	{
		if (nIDImage)
			SetImage(nIDImage);
		else
		{
			GetToolBarCtrl().SetDisabledImageList(NULL);
			m_ilDisabled.DeleteImageList();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CEnToolBar::SetImage(UINT nIDImage)
{
	CEnBitmapEx bitmap;

   if (!bitmap.LoadBitmap(nIDImage))
		return FALSE;

   return SetImage(&bitmap);
}

BOOL CEnToolBar::SetImage(const CString& sImagePath)
{
	CEnBitmapEx bitmap;

	if (!bitmap.LoadImage(sImagePath))
		return FALSE;

   return SetImage(&bitmap);
}

BOOL CEnToolBar::SetImage(CEnBitmapEx* pBitmap)
{
	// map 3d colors
	pBitmap->RemapSysColors();
	
	CEnBitmapEx bmDis;
	bmDis.CopyImage(pBitmap); // for later
	
	// button size
	BITMAP BM;
	pBitmap->GetBitmap(&BM);
	
	int nCount = GetButtonCount(TRUE);
	ASSERT (nCount);
	
	if (!nCount)
		return FALSE;
	
	CSize sizeBmp((BM.bmWidth / nCount), BM.bmHeight);
	CSize sizeBtn(sizeBmp.cx + 7, sizeBmp.cy + 7);
	
	SetSizes(sizeBtn, sizeBmp);
	
	if (SetBitmap((HBITMAP)pBitmap->Detach()))
	{
		RefreshDisabledImageList();
		return TRUE;
	}
	
	// else 
	return FALSE;
}

BOOL CEnToolBar::GrayScale(CEnBitmapEx* pBitmap)
{
	// create 'nice' disabled imagelist 
	COLORREF cr3DFACE = GetSysColor(COLOR_3DFACE);

	C32BIPArray aProcessors;
	CImageSysColorMapper ip1;
	CImageGrayer ip2(0.33, 0.33, 0.33);
	CImageLightener ip3(0.2);
	CImageTinter ip4(GetSysColor(COLOR_3DSHADOW), 10);
	CImageContraster ip5(-50);

	aProcessors.Add(&ip1);
	aProcessors.Add(&ip2);
	aProcessors.Add(&ip3);
	aProcessors.Add(&ip5);
	aProcessors.Add(&ip4);

	return pBitmap->ProcessImage(aProcessors, cr3DFACE);
}

int CEnToolBar::GetButtonCount(BOOL bIgnoreSeparators)
{
	int nCount = GetToolBarCtrl().GetButtonCount();

	if (bIgnoreSeparators)
	{
		int nBtn = nCount;

		while (nBtn--)
		{
			if (GetItemID(nBtn) == ID_SEPARATOR)
				nCount--;
		}
	}

	return nCount;
}

void CEnToolBar::RefreshButtonStates()
{
	OnUpdateCmdUI((CFrameWnd*)GetParent(), FALSE);
}

void CEnToolBar::RefreshDisabledImageList() 
{
	// not under win9x
	if (COSVersion() >= OSV_NT4)
	{
		CEnBitmapEx bmDis;
		
		if (!bmDis.CopyImage(m_hbmImageWell))
			return;
		
		// create 'nice' disabled imagelist 
		if (GrayScale(&bmDis))
		{
			// button size
			int nCx = m_sizeImage.cx, nCy = m_sizeImage.cy;
			
			m_ilDisabled.DeleteImageList();
			m_ilDisabled.Create(nCx, nCy, ILC_COLOR32, 0, 1);
			m_ilDisabled.Add(&bmDis, GetSysColor(COLOR_3DFACE));
			
			GetToolBarCtrl().SetDisabledImageList(&m_ilDisabled);
			
			Invalidate();
		}
	}
}

BOOL CEnToolBar::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    *pResult = CDRF_DODEFAULT;
    LPNMTBCUSTOMDRAW lpNMCustomDraw = ( LPNMTBCUSTOMDRAW )pNMHDR;
    
    switch ( lpNMCustomDraw->nmcd.dwDrawStage )
    {
    case CDDS_PREPAINT:
       FillRect(lpNMCustomDraw->nmcd.hdc,
                  &(lpNMCustomDraw->nmcd.rc),
                  GetSysColorBrush(COLOR_3DFACE));

       *pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
       break;
       
    case CDDS_POSTPAINT:
/*
       FillRect(lpNMCustomDraw->nmcd.hdc,
                  &(lpNMCustomDraw->nmcd.rc),
                  GetSysColorBrush(COLOR_3DFACE));
*/

       *pResult = CDRF_DODEFAULT | CDRF_NOTIFYITEMDRAW;
       break;
       
    case CDDS_PREERASE:
       *pResult = CDRF_SKIPDEFAULT | CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
       break;
       
    case CDDS_ITEMPREPAINT:
       *pResult = OnItemPrePaint(lpNMCustomDraw) | CDRF_NOTIFYPOSTPAINT;
       break;

    case CDDS_ITEMPOSTPAINT:
       *pResult = OnItemPostPaint(lpNMCustomDraw);
       break;

    default:
       break;
    }

    return FALSE; // continue routing
}

void CEnToolBar::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{

	CToolBar::OnSettingChange(uFlags, lpszSection);
}
