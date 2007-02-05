// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BEB85A7E_10B8_410B_BC4B_56649653A624__INCLUDED_)
#define AFX_STDAFX_H__BEB85A7E_10B8_410B_BC4B_56649653A624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Valik - WINVER needs defined for VC 7.1 will complain and define it to 0x0501 (Windows XP
#ifndef WINVER
//#define WINVER 0x0400	// Windows 95
#define WINVER 0x0500	// Windows 95
#endif	// WINVER

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include "..\3rdparty\comutil.h"
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning(disable:4239)
#pragma warning(disable:4505)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BEB85A7E_10B8_410B_BC4B_56649653A624__INCLUDED_)
