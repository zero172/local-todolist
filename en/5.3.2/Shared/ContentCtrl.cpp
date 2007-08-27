// ContentCtrl.cpp: implementation of the CContentCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ContentCtrl.h"
#include "autoflag.h"
#include "misc.h"

#include "IContentControl.h"
#include "ISpellCheck.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

BOOL CONTENTFORMAT::FormatIsText() const
{
	GUID id;
	return (Misc::GuidFromString(*this, id) == FALSE);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentCtrl::CContentCtrl(IContentControl* pContentCtrl) : 
	m_pContentCtrl(pContentCtrl), m_bSettingContent(FALSE)
{

}

CContentCtrl::~CContentCtrl()
{
	if (m_pContentCtrl)
	{
		m_pContentCtrl->Release();
		m_pContentCtrl = NULL;
	}
}

CContentCtrl::operator HWND() const
{ 
	if (m_pContentCtrl)
		return m_pContentCtrl->GetHwnd();
	
	// else
	return NULL;
}

BOOL CContentCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_pContentCtrl)
	{
		// allow tooltip handling thru
		CWnd* pWnd = CWnd::FromHandle(GetSafeHwnd());
		pWnd->FilterToolTipMessage(pMsg);

		// only process if we have the focus
		HWND hFocus = ::GetFocus();

		if (hFocus == GetSafeHwnd() || ::IsChild(GetSafeHwnd(), hFocus))
			return m_pContentCtrl->ProcessMessage(pMsg);
	}

	return FALSE;
}

BOOL CContentCtrl::Attach(IContentControl* pContentCtrl)
{
	ASSERT (pContentCtrl && pContentCtrl->GetHwnd());

	if (pContentCtrl && pContentCtrl->GetHwnd())
	{
		// release existing control
		if (m_pContentCtrl)
			m_pContentCtrl->Release();

		m_pContentCtrl = pContentCtrl;
		return TRUE;
	}

	// else
	return FALSE;
}

ISpellCheck* CContentCtrl::GetSpellCheckInterface()
{
	if (m_pContentCtrl)
		return m_pContentCtrl->GetSpellCheckInterface();

	// else
	return NULL;
}

BOOL CContentCtrl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	if (GetSafeHwnd())
		return CWnd::ModifyStyle(*this, dwRemove, dwAdd, nFlags);

	// else
	return FALSE;
}

BOOL CContentCtrl::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	if (GetSafeHwnd())
		return CWnd::ModifyStyleEx(*this, dwRemove, dwAdd, nFlags);

	// else
	return FALSE;
}

LRESULT CContentCtrl::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pContentCtrl)
		return ::SendMessage(*this, message, wParam, lParam);

	return 0L;
}

BOOL CContentCtrl::SetReadOnly(BOOL bReadOnly)
{
	if (m_pContentCtrl)
	{
		m_pContentCtrl->SetReadOnly(bReadOnly != FALSE);
		return TRUE;
	}

	return FALSE;
}

BOOL CContentCtrl::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pContentCtrl)
		return ::PostMessage(*this, message, wParam, lParam);

	return FALSE;
}

int CContentCtrl::GetContent(unsigned char* pContent) const
{
	if (m_pContentCtrl)
		return m_pContentCtrl->GetContent(pContent);

	// else
	return 0;
}

BOOL CContentCtrl::SetContent(unsigned char* pContent, int nLength)
{
	CAutoFlag af(m_bSettingContent, TRUE);

	if (m_pContentCtrl)
		return m_pContentCtrl->SetContent(pContent, nLength);

	// else
	return false;
}

BOOL CContentCtrl::SetContent(const CString& sContent)
{
	return SetContent((unsigned char*)(LPCTSTR)sContent, sContent.GetLength());
}

int CContentCtrl::GetContent(CString& sContent) const
{
	if (m_pContentCtrl)
	{
		int nLen = m_pContentCtrl->GetContent(NULL);

		if (nLen) // excludes NULL
		{
			unsigned char* szContent = (unsigned char*)sContent.GetBufferSetLength(nLen);
			m_pContentCtrl->GetContent(szContent);
			sContent.ReleaseBuffer(nLen);
			return nLen;
		}
	}

	// else
	sContent.Empty();
	return 0;
}

int CContentCtrl::GetTextContent(CString& sContent) const
{
	if (m_pContentCtrl)
	{
		int nLen = m_pContentCtrl->GetTextContent(NULL);

		if (nLen)
		{
			char* szContent = sContent.GetBufferSetLength(nLen);
			VERIFY (nLen + 1 == m_pContentCtrl->GetTextContent(szContent, nLen + 1));
			sContent.ReleaseBuffer(nLen);
			return nLen;
		}
	}

	// else
	sContent.Empty();
	return 0;
}

BOOL CContentCtrl::SetTextContent(const char* szContent)
{
	CAutoFlag af(m_bSettingContent, TRUE);

	if (m_pContentCtrl)
		return m_pContentCtrl->SetTextContent(szContent);

	// else
	return false;
}

LPCTSTR CContentCtrl::GetTypeID() const
{
	if (m_pContentCtrl)
		return m_pContentCtrl->GetTypeID();

	// else
	return "";
}

BOOL CContentCtrl::IsFormat(const CONTENTFORMAT& cf) const
{
	return (!cf.IsEmpty() && cf == GetContentFormat());
}

CONTENTFORMAT CContentCtrl::GetContentFormat() const
{
	return GetTypeID();
}
