// CDialogHelper.cpp: implementation of the CDialogHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DialogHelper.h"
#include "autoflag.h"
#include "winclasses.h"
#include "wclassdefines.h"

#include <afxpriv.h>
#include <float.h>
#include <locale.h>
#include <afxtempl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// MFC replacements to prevent popup error messages

BOOL DH_SimpleScanf(LPCTSTR lpszText, LPCTSTR lpszFormat, va_list pData)
{
	ASSERT(lpszText != NULL);
	ASSERT(lpszFormat != NULL);

	ASSERT(*lpszFormat == '%');
	lpszFormat++;        // skip '%'

	BOOL bLong = FALSE;
	BOOL bShort = FALSE;
	if (*lpszFormat == 'l')
	{
		bLong = TRUE;
		lpszFormat++;
	}
	else if (*lpszFormat == 's')
	{
		bShort = TRUE;
		lpszFormat++;
	}

	ASSERT(*lpszFormat == 'd' || *lpszFormat == 'u');
	ASSERT(lpszFormat[1] == '\0');

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	TCHAR chFirst = lpszText[0];
	long l, l2;
	if (*lpszFormat == 'd')
	{
		// signed
		l = _tcstol(lpszText, (LPTSTR*)&lpszText, 10);
		l2 = (int)l;
	}
	else
	{
		// unsigned
		if (*lpszText == '-')
			return FALSE;
		l = (long)_tcstoul(lpszText, (LPTSTR*)&lpszText, 10);
		l2 = (unsigned int)l;
	}
	if (l == 0 && chFirst != '0')
		return FALSE;   // could not convert

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	if (bShort)
	{
		if ((short)l != l)
			return FALSE;   // too big for short
		*va_arg(pData, short*) = (short)l;
	}
	else
	{
		ASSERT(sizeof(long) == sizeof(int));
		ASSERT(l == l2);
		*va_arg(pData, long*) = l;
	}

	// all ok
	return TRUE;
}

BOOL DH_SimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	return TRUE;
}

void DH_DDX_TextWithFormat(CDataExchange* pDX, int nIDC, LPCTSTR lpszFormat, UINT nIDPrompt, ...)
{
	va_list pData;
	va_start(pData, nIDPrompt);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szT[32];
	if (pDX->m_bSaveAndValidate)
	{
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, sizeof(szT) / sizeof(TCHAR));

// *******************************************************************
		if (strlen(szT) == 0)
			//fabio_2005
#if _MSC_VER >= 1400
			strcpy_s(szT, "0");
#else
			strcpy(szT, "0");
#endif

// *******************************************************************

		if (!DH_SimpleScanf(szT, lpszFormat, pData))
		{
// *******************************************************************
//			AfxMessageBox(nIDPrompt);
//			pDX->Fail();        // throws exception
// *******************************************************************
		}
	}
	else
	{
		wvsprintf(szT, lpszFormat, pData);
			// does not support floating point numbers - see dlgfloat.cpp
		SetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

void DH_TextFloatFormat(CDataExchange* pDX, int nIDC, void* pData, double value, int nSizeGcvt)
{
	// handle locale specific decimal separator
	setlocale(LC_NUMERIC, "");

	ASSERT(pData != NULL);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];

	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, sizeof(szBuffer) / sizeof(TCHAR));

		// *******************************************************************
		if (strlen(szBuffer) == 0)
			//fabio_2005
#if _MSC_VER >= 1400
			strcpy_s(szBuffer, "0");
#else
			strcpy(szBuffer, "0");
#endif

		// *******************************************************************

		double d;

		if (!DH_SimpleFloatParse(szBuffer, d))
		{
// *******************************************************************
//			AfxMessageBox(nIDPrompt);
//			pDX->Fail();        // throws exception
// *******************************************************************
		}
		if (nSizeGcvt == FLT_DIG)
			*((float*)pData) = (float)d;
		else
			*((double*)pData) = d;
	}
	else
	{
		//fabio_2005
#if _MSC_VER >= 1400
		_stprintf_s(szBuffer, _T("%.*g"), nSizeGcvt, value);
#else
		_stprintf(szBuffer, _T("%.*g"), nSizeGcvt, value);
#endif
		AfxSetWindowText(hWndCtrl, szBuffer);
	}

	// restore decimal separator to '.'
	setlocale(LC_NUMERIC, "English");
}

//////////////////////////////////////////////////////////////////////

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, BYTE& value)
{
	int n = (int)value;
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, &n);

		if (n > 255)
		{
// *******************************************************************
//			AfxMessageBox(AFX_IDP_PARSE_BYTE);
//			pDX->Fail();        // throws exception
// *******************************************************************
		}
		value = (BYTE)n;
	}
	else
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, n);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, short& value)
{
	if (pDX->m_bSaveAndValidate)
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%sd"), AFX_IDP_PARSE_INT, &value);
	else
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%hd"), AFX_IDP_PARSE_INT, value);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, int& value)
{
	if (pDX->m_bSaveAndValidate)
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, &value);
	else
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, value);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, UINT& value)
{
	if (pDX->m_bSaveAndValidate)
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, &value);
	else
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, value);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, long& value)
{
	if (pDX->m_bSaveAndValidate)
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, &value);
	else
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, value);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, DWORD& value)
{
	if (pDX->m_bSaveAndValidate)
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, &value);
	else
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, value);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, CString& value)
{
	::DDX_Text(pDX, nIDC, value);
}

////////////////////////////////////////////////////////////////////////////////
// floats

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
	DH_TextFloatFormat(pDX, nIDC, &value, value, FLT_DIG);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
	DH_TextFloatFormat(pDX, nIDC, &value, value, DBL_DIG);
}

////////////////////////////////////////////////////////////////////////////////

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, BYTE& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, short& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, int& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	// this can be used for a variety of control types so we need
	// to figure out what nIDC points to and then code accordingly
	HWND hCtrl = dx.PrepareCtrl(nIDC);

	if (!hCtrl)
		return FALSE;

	CString sClass;

	::GetClassName(hCtrl, sClass.GetBuffer(100), 100);
	sClass.ReleaseBuffer();

	if (CWinClasses::IsEditControl(hCtrl))
		DDX_Text(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_COMBOBOX))
		DDX_CBIndex(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_LISTBOX))
		DDX_LBIndex(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_SCROLLBAR))
		DDX_Scroll(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_BUTTON))
	{
		DWORD dwStyle = GetWindowLong(hCtrl, GWL_STYLE);

		if ((dwStyle & BS_CHECKBOX) == BS_CHECKBOX)
			DDX_Check(&dx, nIDC, value);
		else if ((dwStyle & BS_RADIOBUTTON) == BS_RADIOBUTTON)
			DDX_Radio(&dx, nIDC, value);
		else
		{
			ASSERT (0);
			return FALSE;
		}
	}
	else
	{
		ASSERT (0);
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, UINT& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, long& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, DWORD& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	// this can be used for a variety of control types so we need
	// to figure out what nIDC points to and then code accordingly
	HWND hCtrl = dx.PrepareCtrl(nIDC);

	if (!hCtrl)
		return FALSE;

	if (CWinClasses::IsEditControl(hCtrl))
		DDX_Text(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_COMBOBOX))
		DDX_CBString(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_LISTBOX))
		DDX_LBString(&dx, nIDC, value);

	else
	{
//		ASSERT (0);
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogHelper::UpdateDataExact(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	// this can be used for a variety of control types so we need
	// to figure out what nIDC points to and then code accordingly
	HWND hCtrl = dx.PrepareCtrl(nIDC);

	if (!hCtrl)
		return FALSE;

	else if (CWinClasses::IsClass(hCtrl, WC_COMBOBOX))
		DDX_CBStringExact(&dx, nIDC, value);

	else if (CWinClasses::IsClass(hCtrl, WC_LISTBOX))
		DDX_LBStringExact(&dx, nIDC, value);

	else
	{
		ASSERT (0);
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, float& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, double& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, CWnd& ctrl, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Control(&dx, nIDC, ctrl);

	return TRUE;
}

void CDialogHelper::SetFont(CWnd* pWnd, HFONT hFont, BOOL bRedraw)
{
	// don't send to toolbar as it causes all sorts of problems with drop buttons
	// but do send to a toolbar's children
	if (!pWnd->IsKindOf(RUNTIME_CLASS(CToolBar)))
		pWnd->SendMessage(WM_SETFONT, (WPARAM)hFont, bRedraw);

	// children
	CWnd* pChild = pWnd->GetWindow(GW_CHILD);

	while (pChild)
	{
		SetFont(pChild, hFont, bRedraw);
		pChild = pChild->GetNextWindow(GW_HWNDNEXT);
	}
}

HFONT CDialogHelper::GetFont(CWnd* pWnd)
{
   if (pWnd)
      return GetFont(pWnd->GetSafeHwnd());

   return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}
 
HFONT CDialogHelper::GetFont(HWND hWnd)
{
   if (hWnd)
   {
      HFONT hFont = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);

      if (hFont)
         return hFont;
   }

   return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

int CDialogHelper::SetComboBoxItems(CComboBox& combo, const CStringArray& aItems)
{
	combo.ResetContent();
	
	for (int nItem = 0; nItem < aItems.GetSize(); nItem++)
		combo.AddString(aItems[nItem]);

	return combo.GetCount();
}

BOOL CDialogHelper::ProcessDialogControlShortcut(const MSG* pMsg)
{
	// message id must be WM_KEYDOWN and alt key must be pressed
	if (pMsg->message == WM_SYSKEYDOWN && (GetKeyState(VK_MENU) & 0x8000) &&
		pMsg->wParam != VK_MENU)
	{
		// convert shortcut from virtual key to char
		UINT nShortcut = MapVirtualKey(pMsg->wParam, 2);
		
		// find the next control having this accelerator
		CWnd* pNext = FindNextMatch(CWnd::GetFocus(), nShortcut);

		if (pNext)
		{
			pNext->SetFocus();
			return TRUE;
		}
	}

	return FALSE;
}

void CDialogHelper::ClipChild(CDC* pDC, CWnd* pWnd, UINT nChildID)
{
	CWnd* pChild = pWnd->GetDlgItem(nChildID);

	if (pChild)
	{
		CRect rChild;
	
		pChild->GetWindowRect(rChild);
		pWnd->ScreenToClient(rChild);
		pDC->ExcludeClipRect(rChild);
	}
}

void CDialogHelper::ClipChild(CDC* pDC, CWnd* pChild)
{
	CWnd* pParent = pChild->GetParent();

	if (pParent)
	{
		CRect rChild;
	
		pChild->GetWindowRect(rChild);
		pParent->ScreenToClient(rChild);
		pDC->ExcludeClipRect(rChild);
	}
}

CWnd* CDialogHelper::FindNextMatch(CWnd* pCurrent, UINT nShortcut)
{
	// only the brute force method would appear to work here
	nShortcut = toupper(nShortcut);

	// 1. find the first parent popup window
	CWnd* pTop = pCurrent->GetParent();

	while (pTop && !(pTop->GetStyle() & WS_CAPTION))
		pTop = pTop->GetParent();

	if (pTop && pTop != pCurrent)
	{
		// get a list of all tabbable items within this parent
		CTypedPtrList<CPtrList, CWnd*> lstWnds;

		CWnd* pFirst = pTop->GetNextDlgTabItem(pTop->GetWindow(GW_CHILD));
		CWnd* pChild = pFirst;

		while (pChild)
		{
			lstWnds.AddTail(pChild);
			pChild = pTop->GetNextDlgTabItem(pChild);

			if (pChild == pFirst)
				break;
		}

		if (lstWnds.GetCount() > 1)
		{
			// keep moving the head to the tail until pCurrent is at the head
			CWnd* pHead = lstWnds.GetHead();

			while (pHead != pCurrent && !pHead->IsChild(pCurrent))
			{
				lstWnds.AddTail(lstWnds.RemoveHead());
				pHead = lstWnds.GetHead();
			}

			// remove pCurrent
			lstWnds.RemoveHead();

			// now traverse the list looking for preceding static
			// labels with a matching accelerator
			POSITION pos = lstWnds.GetHeadPosition();

			while (pos)
			{
				pChild = lstWnds.GetNext(pos);

				CWnd* pPrev = pChild->GetWindow(GW_HWNDPREV);

				if (CWinClasses::IsClass(pPrev->GetSafeHwnd(), WC_STATIC) &&
					pPrev->IsWindowVisible())
				{
					CString sText;
					pPrev->GetWindowText(sText);

					if (!sText.IsEmpty() && GetShortcut(sText) == nShortcut)
						return pChild;
				}
			}
		}
	}

	// all else
	return NULL;
}

UINT CDialogHelper::GetShortcut(const CString& sText)
{
	for (int nChar = 0; nChar < sText.GetLength() - 1; nChar++)
	{
		if (sText[nChar] == '&' && sText[nChar + 1] != '&')
			return toupper(sText[nChar + 1]);
	}

	// no shortcut
	return 0;
}

UINT CDialogHelper::MessageBoxEx(CWnd* pWnd, UINT nIDText, UINT nIDCaption, UINT nType)
{
	CString sText;
	
	sText.LoadString(nIDText);

	return MessageBoxEx(pWnd, sText, nIDCaption, nType);
}

UINT CDialogHelper::MessageBoxEx(CWnd* pWnd, LPCTSTR szText, UINT nIDCaption, UINT nType)
{
	CString sCaption;
	
	sCaption.LoadString(nIDCaption);

	return pWnd->MessageBox(szText, sCaption, nType);
}

int CDialogHelper::RefreshMaxDropWidth(CComboBox& combo, CDC* pDCRef)
{
   int nWidth = CalcMaxTextWidth(combo, 0, TRUE, pDCRef);
   combo.SetDroppedWidth(nWidth);

   return nWidth;
}
 
int CDialogHelper::CalcMaxTextWidth(CComboBox& combo, int nMinWidth, BOOL bDropped, CDC* pDCRef)
{
   CDC* pDC = pDCRef;
   CFont* pOldFont = NULL;
   
   if (!pDC)
   {
      pDC = combo.GetDC();
      pOldFont = pDC->SelectObject(combo.GetFont());
   }
      
	CString sText;
	int nMaxWidth = nMinWidth;
	int nItem = combo.GetCount();
	
	while (nItem--)
	{
		combo.GetLBText(nItem, sText);
		
		int nWidth = pDC->GetTextExtent(sText).cx;
		nMaxWidth = max(nMaxWidth, nWidth);
	}

	// check window text too
	combo.GetWindowText(sText);

	int nWidth = pDC->GetTextExtent(sText).cx;
	nMaxWidth = max(nMaxWidth, nWidth);

   // dropped width needs some more space
   if (bDropped)
   {
      // Add the avg width to prevent clipping
      TEXTMETRIC tm;
      pDC->GetTextMetrics(&tm);
      nMaxWidth += tm.tmAveCharWidth;

      // Adjust the width for the vertical scroll bar and the left and right border.
      if (combo.GetStyle() & WS_VSCROLL)
         nMaxWidth += ::GetSystemMetrics(SM_CXVSCROLL);
      
      nMaxWidth += (2 * ::GetSystemMetrics(SM_CXEDGE));
   }

   // cleanup
   if (!pDCRef)
   {
      pDC->SelectObject(pOldFont);
      combo.ReleaseDC(pDC);
   }

   return nMaxWidth;
}

BOOL CDialogHelper::IsChildOrSame(HWND hWnd, HWND hwndChild)
{
	return (hWnd == hwndChild || ::IsChild(hWnd, hwndChild));
}
