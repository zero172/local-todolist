// TDLContentMgr.cpp: implementation of the CTDLContentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "todolist.h"
#include "TDLContentMgr.h"
#include "ToDoCommentsCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

class CDefaultContent : public IContent
{
	bool GetTypeID(GUID& /*id*/) { return false; }
	const char* GetTypeDescription() { static LPCTSTR sDesc = "Simple Text"; return sDesc; }

	IContentControl* CreateCtrl(unsigned short nCtrlID, unsigned long nStyle, 
						long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent)
	{
		CToDoCommentsCtrl* pComments = new CToDoCommentsCtrl;

		nStyle |= ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL;
		CRect rect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);

		if (pComments->Create(nStyle, rect, CWnd::FromHandle(hwndParent), nCtrlID))
			return pComments;

		// else
		delete pComments;
		return NULL;
	}

	void Release() { delete this; }
	void SetIniLocation(bool /*bRegistry*/, const char* /*szIniPathName*/) {} // do nothing

	int ConvertToHtml(const unsigned char* /*pContent*/, int /*nLength*/,
						char*& /*szHtml*/) { return 0; } // not supported

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDLContentMgr::CTDLContentMgr() 
{

}

CTDLContentMgr::~CTDLContentMgr()
{

}

BOOL CTDLContentMgr::Initialize()
{
	BOOL bWasInitialized = m_bInitialized;

	if (CContentMgr::Initialize())
	{
		if (!bWasInitialized)
			m_aContent.InsertAt(0, new CDefaultContent);
		
		return TRUE;
	}

	return FALSE;
}
