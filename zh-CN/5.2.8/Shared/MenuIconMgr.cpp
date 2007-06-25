// MenuIconMgr.cpp: implementation of the CMenuIconMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuIconMgr.h"
#include "enbitmapex.h"
#include "themed.h"

typedef unsigned long ULONG_PTR; 

#ifndef COLOR_MENUHILIGHT
#	define COLOR_MENUHILIGHT 29
#endif

#define NOCOLOR ((COLORREF)-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuIconMgr::CMenuIconMgr()
{
   
}

CMenuIconMgr::~CMenuIconMgr()
{
	// cleanup icons
	POSITION pos = m_mapID2Icon.GetStartPosition();

	while (pos)
	{
		UINT nCmdID;
		HICON hIcon = NULL;

		m_mapID2Icon.GetNextAssoc(pos, nCmdID, hIcon);
		::DestroyIcon(hIcon);
	}
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
	ASSERT (aCmdIDs.GetSize() == il.GetImageCount());
	
	if (aCmdIDs.GetSize() != il.GetImageCount())
		return 0;
	
	int nBtnCount = aCmdIDs.GetSize();
	CImageList* pIL = const_cast<CImageList*>(&il);
	
	for (int nBtn = 0; nBtn < nBtnCount; nBtn++)
		SetImage(aCmdIDs[nBtn], pIL->ExtractIcon(nBtn));
	   
	return nBtnCount;
	
}

int CMenuIconMgr::AddImages(const CUIntArray& aCmdIDs, UINT nIDBitmap, int nCx,
                            COLORREF crMask)
{
	CImageList il;
	CEnBitmapEx bm;
	
	if (il.Create(nCx, 16, ILC_COLOR32 | ILC_MASK, 0, 1) && bm.LoadBitmap(nIDBitmap))
	{
		il.Add(&bm, crMask);

		return AddImages(aCmdIDs, il);;
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
		
		return TRUE;
	}
	
	// else
	return FALSE;
}

HICON CMenuIconMgr::LoadItemImage(UINT nCmdID) 
{
	HICON hIcon = NULL;
	
	m_mapID2Icon.Lookup(nCmdID, hIcon);
	
	return hIcon;
}

BOOL CMenuIconMgr::SetImage(UINT nCmdID, HICON hIcon)
{
	if (nCmdID)
	{
		if (hIcon)
			m_mapID2Icon[nCmdID] = hIcon;
		else
		{
			// cleanup icon
			if (m_mapID2Icon.Lookup(nCmdID, hIcon))
				::DestroyIcon(hIcon);

			m_mapID2Icon.RemoveKey(nCmdID);
		}

		return TRUE;
	}
	
	return FALSE;
}

LRESULT CMenuIconMgr::WindowProc(HWND /*hRealWnd*/, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITMENUPOPUP:
		if (!HIWORD(lp)) // let windows look after the system menu 
		{
			LRESULT lr = CSubclassWnd::Default();
			OnInitMenuPopup(CMenu::FromHandle((HMENU)wp));
			return lr;
		}
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
	
    HICON hIcon = LoadItemImage(lpdis->itemID);
	
    if (hIcon)
    {
        ICONINFO iconinfo;
        GetIconInfo(hIcon, &iconinfo);
		
        BITMAP bitmap;
        GetObject(iconinfo.hbmColor, sizeof(bitmap), &bitmap);
		
        ::DrawIconEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, hIcon, 
					bitmap.bmWidth, bitmap.bmHeight, 0, NULL, DI_IMAGE | DI_MASK);
	
		// cleanup
		::DeleteObject(iconinfo.hbmColor);
		::DeleteObject(iconinfo.hbmMask);

		return TRUE;
    }
	
	return FALSE;
}

void CMenuIconMgr::OnInitMenuPopup(CMenu* pMenu)
{
	ASSERT (pMenu);

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
	
		// cleanup
		::DeleteObject(iconinfo.hbmColor);
		::DeleteObject(iconinfo.hbmMask);
		
		return TRUE;
    }
	
	return FALSE;
}
