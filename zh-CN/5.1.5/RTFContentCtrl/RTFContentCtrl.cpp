// RTFContentCtrl.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "RTFContentCtrl.h"
#include "RTFContentControl.h"
#include "..\shared\filemisc.h"

#include "..\3rdparty\rtf2html\rtf2htmlconverter.h"

#include "..\shared\filemisc.h"
#include "..\shared\misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// from r2h.h
typedef int (*PFNCONVERTRTF2HTML)(const char*, const char*, unsigned long, const char*);

static AFX_EXTENSION_MODULE AfxdllDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("AFXDLL.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(AfxdllDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(AfxdllDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("AFXDLL.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(AfxdllDLL);
	}
	return 1;   // ok
}

DLL_DECLSPEC IContent* CreateContentInterface()
{
	return new CRTFContentCtrlApp;
}

DLL_DECLSPEC int GetInterfaceVersion() 
{ 
	return ICONTENTCTRL_VERSION; 
}

CRTFContentCtrlApp::CRTFContentCtrlApp()
{
}

bool CRTFContentCtrlApp::GetTypeID(GUID& id) 
{
	id = RTF_TYPEID;
	return true;
}

const char* CRTFContentCtrlApp::GetTypeDescription()
{
	return _T("带格式文本");
}

IContentControl* CRTFContentCtrlApp::CreateCtrl(unsigned short nCtrlID, unsigned long nStyle, 
						long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent)
{
	// load localized resources
	HINSTANCE hResDll = AfxLoadLibrary("RTFContentCtrlLOC.dll");

	if (hResDll)
		AfxSetResourceHandle(hResDll);

	CRTFContentControl* pControl = new CRTFContentControl;

	if (pControl)
	{
		CRect rect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);

		if (pControl->Create(nStyle, rect, CWnd::FromHandle(hwndParent), nCtrlID, TRUE))
		{
			return pControl;
		}
	}

	delete pControl;
	return NULL;
}

void CRTFContentCtrlApp::Release()
{
	delete this;
}

void CRTFContentCtrlApp::SetIniLocation(bool /*bRegistry*/, const char* /*szIniPathName*/)
{
	// do nothing because we're an extension dll so its all sorted
}

int CRTFContentCtrlApp::ConvertToHtml(const unsigned char* pContent, 
									  int nLength, char*& szHtml)
{
	if (nLength == 0)
		return 0; // nothing to convert

	CString sRtf(pContent), sHtml;

	// scan the string looking for anything that smells like a 
	// multi-byte character, because CRTF_HTMLConverter can't
	// handle that at the moment
	if (CRTF_HTMLConverter::HasMultiByteChars(sRtf))
		return 0;

/*
	// check the code page to see if it represents a multi-byte
	// char set because we can't handle that at present
	int nCodePage = CRTF_HTMLConverter::GetCodePage(sRtf);

	switch (nCodePage)
	{
	case 874:	// Thai
	case 932:	// Japanese
	case 936:	// Simplified Chinese
	case 949:	// Korean
	case 950:	// Traditional Chinese:
		return 0;
	}
*/

	// scan the string looking for anything that smells like a 
	// multi-byte character, because CRTF_HTMLConverter can't
	// handle that at the moment
	if (Misc::IsMultibyteString(sRtf))
		return 0;

	if (CRTF_HTMLConverter::Convert(sRtf, sHtml, FALSE))
	{
		nLength = sHtml.GetLength();
		szHtml = new char[nLength + 1];

		memcpy(szHtml, sHtml, nLength);
		szHtml[nLength] = 0;
	}
	else
		nLength = 0; // reuse

	return nLength;
}
