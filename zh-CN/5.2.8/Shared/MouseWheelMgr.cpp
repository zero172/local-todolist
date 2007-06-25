// MouseWheelMgr.cpp: implementation of the CMouseWheelMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MouseWheelMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMouseWheelMgr::CMouseWheelMgr()
{

}

CMouseWheelMgr::~CMouseWheelMgr()
{

}

BOOL CMouseWheelMgr::Initialize()
{
	return GetInstance().InitHooks(HM_MOUSE);
}

BOOL CMouseWheelMgr::OnMouse(UINT uMouseMsg, const MOUSEHOOKSTRUCTEX& info)
{
   if (uMouseMsg == WM_MOUSEWHEEL)
   {
	   //fabio_2005
#if _MSC_VER >= 1300

      HWND hwndPt = ::WindowFromPoint(info.pt);

      if (info.hwnd != hwndPt)  // does the window under the mouse have the focus.
      {
		  ::PostMessage(hwndPt, WM_MOUSEWHEEL, (info.mouseData & 0xffff0000), 
						MAKELPARAM(info.pt.x, info.pt.y));

		  return TRUE; // eat
      }
   }
#else

      HWND hwndPt = ::WindowFromPoint(info.MOUSEHOOKSTRUCT.pt);

      if (info.MOUSEHOOKSTRUCT.hwnd != hwndPt)  // does the window under the mouse have the focus.
      {
		  ::PostMessage(hwndPt, WM_MOUSEWHEEL, (info.mouseData & 0xffff0000), 
						MAKELPARAM(info.MOUSEHOOKSTRUCT.pt.x, info.MOUSEHOOKSTRUCT.pt.y));

		  return TRUE; // eat
      }
   }

#endif
   // all else
   return FALSE;
}
