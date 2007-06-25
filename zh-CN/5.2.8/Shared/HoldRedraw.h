// SetRedraw.h: interface and implementation of the CSetRedraw class.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOLDREDRAW_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
#define AFX_HOLDREDRAW_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_


class CNcRedraw // note: there is no inheritance intentionally.
{
public:
	CNcRedraw(CWnd* pWnd)
	{
		m_hWnd = pWnd ? pWnd->GetSafeHwnd() : NULL; 
	}

	CNcRedraw(HWND hWnd)
	{
		m_hWnd = hWnd; 
	}

	virtual ~CNcRedraw()
	{
		if (m_hWnd)
			::SendMessage(m_hWnd, WM_NCPAINT, 0, 0);
	}

protected:
	HWND m_hWnd;
};

class CRedrawAll
{
public:
	CRedrawAll(CWnd* pWnd, BOOL bUpdateWindow = FALSE) : m_bUpdateWindow(bUpdateWindow)
	{
		m_hWnd = pWnd ? pWnd->GetSafeHwnd() : NULL; 
	}

	CRedrawAll(HWND hWnd, BOOL bUpdateWindow = FALSE) : m_bUpdateWindow(bUpdateWindow)
	{
		m_hWnd = hWnd; 
	}

	virtual ~CRedrawAll()
	{
		if (m_hWnd)
		{
			::SendMessage(m_hWnd, WM_NCPAINT, 0, 0);
			::InvalidateRect(m_hWnd, NULL, FALSE);

			if (m_bUpdateWindow)
				::UpdateWindow(m_hWnd);
		}
	}

protected:
	HWND m_hWnd;
	BOOL m_bUpdateWindow;
};

class CHoldRedraw : protected CRedrawAll
{
public:
	CHoldRedraw(CWnd* pWnd, BOOL bUpdateWindow = FALSE) : CRedrawAll(pWnd, bUpdateWindow)
	{
		m_hWnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
		
		::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
	}

	CHoldRedraw(HWND hWnd, BOOL bUpdateWindow = FALSE) : CRedrawAll(hWnd, bUpdateWindow)
	{
		m_hWnd = hWnd; 

		::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
	}

	virtual ~CHoldRedraw()
	{
		if (m_hWnd)
			::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
	}
};

#endif
