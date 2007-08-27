// IContentControl.h: IContentControl interface.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONTENTCONTROL_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_ICONTENTCONTROL_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>

// function to be exported from dll to create instance of interface
#ifdef _EXPORTING // declare this in project settings for dll _only_
#	define DLL_DECLSPEC __declspec(dllexport)
#else
#	define DLL_DECLSPEC __declspec(dllimport)
#endif 

#define ICONTENTCTRL_VERSION 0x0002

const UINT WM_ICC_WANTSPELLCHECK = ::RegisterWindowMessage("WM_ICC_WANTSPELLCHECK");

class IContent;
class IContentControl;

typedef IContent* (*PFNCREATECONTENT)(); // function prototype

extern "C" DLL_DECLSPEC IContent* CreateContentInterface();

typedef int (*PFNGETVERSION)(); // function prototype
extern "C" DLL_DECLSPEC int GetInterfaceVersion();

#pragma warning(disable:4505)

// helper method
static IContent* CreateContentInterface(const char* szDllPath, int* pVer = 0)
{
    IContent* pInterface = NULL;
    HMODULE hDll = LoadLibrary(szDllPath);
	
    if (hDll)
    {
        PFNCREATECONTENT pCreate = (PFNCREATECONTENT)GetProcAddress(hDll, "CreateContentInterface");
		
        if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");
			int nInterfaceVer = ICONTENTCTRL_VERSION;

			// pass version back to caller
			if (pVer)
				*pVer = pVersion ? pVersion() : 0;

			if (!nInterfaceVer || (pVersion && pVersion() >= nInterfaceVer))
				pInterface = pCreate();
		}

		if (hDll && !pInterface)
			FreeLibrary(hDll);
    }
	
    return pInterface;
}

static BOOL IsContentDll(const char* szDllPath)
{
    HMODULE hDll = LoadLibrary(szDllPath);
	
    if (hDll)
    {
		PFNCREATECONTENT pCreate = (PFNCREATECONTENT)GetProcAddress(hDll, "CreateContentInterface");
		FreeLibrary(hDll);

		return (pCreate != NULL);
	}

	return FALSE;
}

class IContent  
{
public:
	virtual const char* GetTypeID() const = 0;
	virtual const char* GetTypeDescription() const = 0;

	virtual IContentControl* CreateCtrl(unsigned short nCtrlID, unsigned long nStyle, 
						long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent) = 0;

	virtual void Release() = 0;
	
	// returns the length of the html or zero if not supported
	virtual int ConvertToHtml(const unsigned char* pContent, int nLength,
							  char*& pHtml) = 0;
};

class ISpellCheck;

class IContentControl  
{
public:
	// custom/binary data format
	virtual int GetContent(unsigned char* pContent) const = 0;
	virtual bool SetContent(unsigned char* pContent, int nLength) = 0;
	virtual const char* GetTypeID() const = 0;

	// text content if supported. return false if not supported
	virtual int GetTextContent(char* szContent, int nLength = -1) const = 0;
	virtual bool SetTextContent(const char* szContent) = 0;

	virtual void SetReadOnly(bool bReadOnly) = 0;
	virtual HWND GetHwnd() const = 0;

	virtual void Release() = 0;
	virtual bool ProcessMessage(MSG* pMsg) = 0;

	virtual ISpellCheck* GetSpellCheckInterface() = 0;
};

#endif // AFX_ICONTENTCONTROL_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_
