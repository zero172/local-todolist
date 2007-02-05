// MenuIconMgr.cpp: implementation of the CMenuIconMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuIconMgr.h"
#include "..\shared\enbitmapex.h"

typedef unsigned long ULONG_PTR; 

#ifndef COLOR_MENUHILIGHT
#	define COLOR_MENUHILIGHT 29
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuIconMgr::CMenuIconMgr()
{
   
}

CMenuIconMgr::~CMenuIconMgr()
{
   
}

BOOL CMenuIconMgr::Initialize(CWnd* pWnd)
{
   if (pWnd && !IsHooked())
      return HookWindow(pWnd->GetSafeHwnd());

   else if (!pWnd && IsHooked())
      return CSubclassWnd::HookWindow(NULL);

   // else
   return TRUE;
}


int CMenuIconMgr::AddImages(const CToolBar& toolbar)
{
   // iterate the non-separator items extracting their images 
   // from the imagelist
   const CToolBarCtrl& tbc = toolbar.GetToolBarCtrl();
   CImageList* pIL = tbc.GetImageList();
   int nBtnCount = tbc.GetButtonCount(), nImage = 0;

   for (int nBtn = 0; nBtn < nBtnCount; nBtn++)
   {
      UINT nCmdID = toolbar.GetItemID(nBtn);

      if (nCmdID != ID_SEPARATOR)
      {
         m_mapID2Icon[nCmdID] = pIL->ExtractIcon(nImage);
         nImage++;
      }
   }

   return nImage;
}

int CMenuIconMgr::AddImages(const CUIntArray& aCmdIDs, const CImageList& il)
{
	return AddImages(aCmdIDs, il, FALSE);
}

int CMenuIconMgr::AddImages(const CUIntArray& aCmdIDs, const CImageList& il, BOOL bSelected)
{
   ASSERT (aCmdIDs.GetSize() == il.GetImageCount());

   if (aCmdIDs.GetSize() != il.GetImageCount())
      return 0;

   int nBtnCount = aCmdIDs.GetSize();
   CImageList* pIL = const_cast<CImageList*>(&il);

   for (int nBtn = 0; nBtn < nBtnCount; nBtn++)
   {
	   if (bSelected)
		   m_mapID2IconSelected[aCmdIDs[nBtn]] = pIL->ExtractIcon(nBtn);
	   else
		   m_mapID2Icon[aCmdIDs[nBtn]] = pIL->ExtractIcon(nBtn);
   }	   
	   
   return nBtnCount;

}

int CMenuIconMgr::AddImages(const CUIntArray& aCmdIDs, UINT nIDBitmap, int /*nCx*/,
                            COLORREF crMask)
{
	CImageList il;
	CEnBitmapEx bm;
	
	if (il.Create(16, 16, ILC_COLOR32, 0, 1) && bm.LoadBitmap(nIDBitmap))
	{
		bm.ReplaceColor(crMask, GetSysColor(COLOR_MENU));
		il.Add(&bm, RGB(255, 255, 255));

		int nCount = AddImages(aCmdIDs, il, FALSE);

		if (nCount)
		{
			il.DeleteImageList();
			il.Create(16, 16, ILC_COLOR32, 0, 1);

			bm.ReplaceColor(GetSysColor(COLOR_MENU), GetSysColor(COLOR_MENUHILIGHT));
			il.Add(&bm, RGB(255, 255, 255));

			AddImages(aCmdIDs, il, TRUE);
		}

		return nCount;
	}
	
	return 0;
}

BOOL CMenuIconMgr::ChangeImageID(UINT nCmdID, UINT nNewCmdID)
{
	HICON hIcon = LoadItemImage(nCmdID);

	if (hIcon)
	{
		// normal icon
		m_mapID2Icon.RemoveKey(nCmdID);
		m_mapID2Icon[nNewCmdID] = hIcon;

		// selected icon
		hIcon = LoadItemImage(nCmdID, TRUE);

		if (hIcon)
		{
			m_mapID2IconSelected.RemoveKey(nCmdID);
			m_mapID2IconSelected[nNewCmdID] = hIcon;
		}

		return TRUE;
	}
	
	// else
	return FALSE;
}

HICON CMenuIconMgr::LoadItemImage(UINT nCmdID, BOOL bSelected)
{
   HICON hIcon = NULL;

   m_mapID2Icon.Lookup(nCmdID, hIcon); // always

   if (bSelected)
	   m_mapID2IconSelected.Lookup(nCmdID, hIcon); // always

   if (!hIcon)
      hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nCmdID), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_SHARED);
/*   
   if (!hIcon)
   {
      hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), (LPCTSTR)sItem, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_SHARED);
   }
*/

   return hIcon;
}

BOOL CMenuIconMgr::AddImage(UINT nCmdID, HICON hIcon)
{
   if (hIcon && nCmdID)
   {
      m_mapID2Icon[nCmdID] = hIcon;
      return TRUE;
   }

   return FALSE;
}

LRESULT CMenuIconMgr::WindowProc(HWND /*hRealWnd*/, UINT msg, WPARAM wp, LPARAM lp)
{
   switch (msg)
   {
   case WM_INITMENUPOPUP:
      OnInitMenuPopup(CMenu::FromHandle((HMENU)wp), LOWORD(lp), HIWORD(lp));
      break;

   case WM_DRAWITEM:
      if (OnDrawItem(wp, (LPDRAWITEMSTRUCT)lp))
		  return 0L;
      break;

   case WM_MEASUREITEM:
      if (OnMeasureItem(wp, (LPMEASUREITEMSTRUCT)lp))
		  return 0L;
      break;
   }
   
   return CSubclassWnd::Default();
}

BOOL CMenuIconMgr::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpdis)
{
    if (lpdis == NULL || lpdis->CtlType != ODT_MENU)
        return FALSE; // not for a menu

/*
    CString sItem;
    int len = ::GetMenuString((HMENU)lpdis->hwndItem, lpdis->itemID, NULL, 0, MF_BYCOMMAND);

    // use exact buffer length
    if (len > 0)
    {
        LPTSTR pstrString = sItem.GetBufferSetLength(len+1);
        ::GetMenuString((HMENU)lpdis->hwndItem, lpdis->itemID, pstrString, len+1, MF_BYCOMMAND);
        sItem.ReleaseBuffer();
    }

    //choose text colors
    if (lpdis->itemState & ODS_SELECTED)
    {
        COLORREF crText;
        if (lpdis->itemState & ODS_GRAYED)
            crText = ::SetTextColor(lpdis->hDC, ::GetSysColor(COLOR_GRAYTEXT)); //RGB(128, 128, 128));
        else
            crText = ::SetTextColor(lpdis->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
        ::SetBkColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHT));
    }
    // draw the background (if highlighted)
    ::ExtTextOut(lpdis->hDC, 0, 0, ETO_CLIPPED|ETO_OPAQUE, &(lpdis->rcItem), NULL, 0, (LPINT)NULL);
*/

    HICON hIcon = LoadItemImage(lpdis->itemID, (lpdis->itemState & ODS_SELECTED));

    if (hIcon)
    {
        ICONINFO iconinfo;
        GetIconInfo(hIcon, &iconinfo);

        BITMAP bitmap;
        GetObject(iconinfo.hbmColor, sizeof(bitmap), &bitmap);

        ::DrawIconEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, hIcon, bitmap.bmWidth, bitmap.bmHeight, 0, NULL, DI_NORMAL);
		return TRUE;
    }

	return FALSE;
}

void CMenuIconMgr::OnInitMenuPopup(CMenu* pMenu, UINT /*nIndex*/, BOOL bSysMenu)
{
    if (bSysMenu)
        pMenu = GetCWnd()->GetSystemMenu(FALSE);

    MENUINFO mnfo;
    mnfo.cbSize = sizeof(mnfo);
    mnfo.fMask = MIM_STYLE;
    mnfo.dwStyle = MNS_CHECKORBMP | MNS_AUTODISMISS;
   ::SetMenuInfo(pMenu->GetSafeHmenu(), &mnfo);

    MENUITEMINFO minfo;
    minfo.cbSize = sizeof(minfo);

    for (UINT pos=0; pos<pMenu->GetMenuItemCount(); pos++)
    {
        minfo.fMask = MIIM_FTYPE | MIIM_ID;
        pMenu->GetMenuItemInfo(pos, &minfo, TRUE);

        CString sItem;
        pMenu->GetMenuString(pos, sItem, MF_BYPOSITION);

        HICON hIcon = LoadItemImage(minfo.wID);

        if (hIcon && !(minfo.fType & MFT_OWNERDRAW))
        {
			TRACE ("CMenuIconMgr::OnInitMenuPopup(%s, %d)\n", sItem, minfo.wID);

            minfo.fMask = MIIM_BITMAP | MIIM_DATA;
            minfo.hbmpItem = HBMMENU_CALLBACK;

	        ::SetMenuItemInfo(pMenu->GetSafeHmenu(), pos, TRUE, &minfo);
        }
    }
}

BOOL CMenuIconMgr::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpmis)
{
    if ((lpmis==NULL)||(lpmis->CtlType != ODT_MENU))
        return FALSE;

    lpmis->itemWidth = 16;
    lpmis->itemHeight = 16;

    HICON hIcon = LoadItemImage(lpmis->itemID);

    if (hIcon)
    {

        ICONINFO iconinfo;
        ::GetIconInfo(hIcon, &iconinfo);

        BITMAP bitmap;
        ::GetObject(iconinfo.hbmColor, sizeof(bitmap), &bitmap);

        lpmis->itemWidth = bitmap.bmWidth;
        lpmis->itemHeight = bitmap.bmHeight;

		return TRUE;
    }

	return FALSE;
}
