#if !defined(AFX_TDCMSG_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TDCMSG_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// tdcmsg.h : header file
//
const LPCTSTR TDL_PROTOCOL = "tdl://";

// notification messages
const UINT WM_TDCN_MODIFY = ::RegisterWindowMessage("WM_TDCN_MODIFY"); // lParam == <TDC_ATTRIBUTE>
const UINT WM_TDCN_SORT = ::RegisterWindowMessage("WM_TDCN_SORT"); 
const UINT WM_TDCN_MINWIDTHCHANGE = ::RegisterWindowMessage("WM_TDCN_MINWIDTHCHANGE"); 
const UINT WM_TDCN_COMMENTSCHANGE = ::RegisterWindowMessage("WM_TDCN_COMMENTSCHANGE");
const UINT WM_TDCN_TIMETRACK = ::RegisterWindowMessage("WM_TDCN_TIMETRACK"); // lParam = 0/1 => stop/start

// from the filterbar
const UINT WM_FBN_FILTERCHNG = ::RegisterWindowMessage("WM_FBN_FILTERCHNG"); 

// sent when one of the auto dropdown lists is changed
const UINT WM_TDCN_LISTCHANGE = ::RegisterWindowMessage("WM_TDCN_LISTCHANGE"); // lParam == <TDC_ATTRIBUTE>

// request messages
const UINT WM_TDCM_GETCLIPBOARD = ::RegisterWindowMessage("WM_TDCM_GETCLIPBOARD"); // lParam == match hwnd
const UINT WM_TDCM_HASCLIPBOARD = ::RegisterWindowMessage("WM_TDCM_HASCLIPBOARD"); // lParam == match hwnd
const UINT WM_TDCM_TASKISDONE = ::RegisterWindowMessage("WM_TDCM_TASKISDONE"); // format as WM_TDCM_TASKLINK

// instruction messages
// sent when a task outside 'this' todoctrl needs displaying
const UINT WM_TDCM_TASKLINK = ::RegisterWindowMessage("WM_TDCM_TASKLINK"); // wParam == taskID, lParam = taskfile

#endif // AFX_TDCMSG_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_