// NcGutter.cpp: implementation of the CNcGutter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NcGutter.h"
#include "holdredraw.h"
#include "themed.h"
#include "dlgunits.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT WM_NCG_FORCERESIZE = ::RegisterWindowMessage("WM_NCG_FORCERESIZE");
UINT HEADERHEIGHT = 0; // set in WM_NCCALCSIZE so not const
UINT BORDER = 0; // set in WM_NCCALCSIZE so not const
const UINT SORTWIDTH = 10;
const UINT MINCLIENTWIDTH = GetSystemMetrics(SM_CXVSCROLL) * 4; 

CNcGutter::CNcGutter() : 
	m_bSetRedraw(TRUE), 
	m_bShowHeader(TRUE), 
	m_bFirstRecalc(TRUE),
	m_nHeaderColDown(-1),
	m_dwButtonDownItem(0)
{
	// add client header hot rect placeholder
	if (m_bShowHeader && CThemed().AreControlsThemed())
		m_hotTrack.AddRect();

	// client column is always last so we add it first
	m_aColumns.Add(new COLUMNDESC(NCG_CLIENTCOLUMNID));
}

CNcGutter::~CNcGutter()
{
	// delete column defs
	int nCol = m_aColumns.GetSize();

	while (nCol--)
		delete m_aColumns[nCol];
}

BOOL CNcGutter::Initialize(HWND hwnd)
{
	if (CSubclassWnd::HookWindow(hwnd))
	{
		// reset calculated widths
		int nCol = m_aColumns.GetSize() - 1; // omit last column == client

		while (nCol--)
		{
			if (m_aColumns[nCol]->bCalcWidth)
				m_aColumns[nCol]->nWidth = 0;
		}
		
		RecalcGutter();

		// hot tracking
		if (m_bShowHeader && CThemed().AreControlsThemed())
			m_hotTrack.Initialize(GetCWnd());

		return TRUE;
	}

	return FALSE;
}

BOOL CNcGutter::AddColumn(UINT nColID, LPCTSTR szTitle, UINT nWidth, UINT nTextAlign)
{
	ASSERT (GetColumnIndex(nColID) == -1);

	if (GetColumnIndex(nColID) >= 0)
		return FALSE;

	COLUMNDESC* pCD = new COLUMNDESC(nColID);

	pCD->sTitle = szTitle;
	pCD->nWidth = nWidth;
	pCD->nTextAlign = nTextAlign;
	pCD->bCalcWidth = !nWidth;

	if (szTitle)
	{
		CWindowDC dc(GetCWnd());
		CFont* pOldFont = PrepareFont(&dc, TRUE);
		
		pCD->nTextWidth = dc.GetTextExtent(szTitle).cx;
		
		dc.SelectObject(pOldFont);
	}
	else
		pCD->nTextWidth = 0;
	
	if (nWidth)
		pCD->nWidth = max(nWidth, pCD->nTextWidth + 2 * NCG_COLPADDING);

	// client column is always last
	m_aColumns.InsertAt(m_aColumns.GetSize() - 1, pCD);

	// add a placeholder to the hot tracker
	if (CThemed().AreControlsThemed())
		m_hotTrack.AddRect();

	if (IsHooked())
		RecalcGutter();

	return TRUE;
}

void CNcGutter::PressHeader(UINT nColID, BOOL bPress)
{
	int nColumn = GetColumnIndex(nColID);

	UnpressAllColumnHeaders(nColumn);
	
	if (nColumn >= 0 && nColumn < m_aColumns.GetSize())
	{
		COLUMNDESC* pCD = m_aColumns[nColumn];

		if (pCD->bPressed != bPress)
		{
			pCD->bPressed = bPress;
			Redraw();
		}
	}
}

void CNcGutter::SetHeaderTitle(UINT nColID, LPCTSTR szTitle, LPCTSTR szFont, BOOL bSymbolFont)
{
	int nColumn = GetColumnIndex(nColID);

	if (nColumn >= 0 && nColumn < m_aColumns.GetSize())
	{
		BOOL bRedraw = FALSE;
		COLUMNDESC* pCD = m_aColumns[nColumn];

		if (!pCD->hFont && szFont)
		{
			LOGFONT lf;
			HFONT hDef = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

			if (GetObject(hDef, sizeof(lf), &lf))
			{
				lstrcpy(lf.lfFaceName, szFont);

				if (bSymbolFont)
				{
					lf.lfCharSet = SYMBOL_CHARSET;
					lf.lfQuality = ANTIALIASED_QUALITY;
					lf.lfHeight = MulDiv(lf.lfHeight, 12, 10);
				}
				
				pCD->hFont = CreateFontIndirect(&lf);

				if (pCD->hFont)
					pCD->bSymbolFont = bSymbolFont;

				bRedraw = (NULL != pCD->hFont);
			}
		}
		
		if (pCD->sTitle != szTitle)
		{
			pCD->sTitle = szTitle;

			if (szTitle)
			{
				CWindowDC dc(GetCWnd());
				CFont* pOldFont = PrepareFont(&dc, TRUE, pCD->hFont);
				
				pCD->nTextWidth = dc.GetTextExtent(szTitle).cx;
				dc.SelectObject(pOldFont);
			}
			else
				pCD->nTextWidth = 0;
			
			if (!pCD->bCalcWidth)
				pCD->nWidth = max(pCD->nWidth, pCD->nTextWidth + 2 * NCG_COLPADDING);

			bRedraw = TRUE;
		}

		if (bRedraw)
			Redraw();
	}
}

void CNcGutter::SetColumnSort(UINT nColID, NCGSORT nSortDir, BOOL bExclusive)
{
	BOOL bNeedsRecalc = FALSE;

	int nCol = m_aColumns.GetSize();

	while (nCol--)
	{
		COLUMNDESC* pCD = m_aColumns[nCol];

		if (pCD->nColID != nColID) // other column
		{
			if (bExclusive)
			{
				BOOL bChange = (pCD->nSortDir != NCGSORT_NONE);

				// adjust column width if fixed size and was sorted
				if (bChange)
				{
					if (!pCD->bCalcWidth)
						pCD->nWidth -= SORTWIDTH;
					else
						bNeedsRecalc = TRUE;
				}

				pCD->nSortDir = NCGSORT_NONE;
				bNeedsRecalc |= bChange;
			}
		}
		else
		{
			BOOL bChange = (pCD->nSortDir != NCGSORT_NONE && nSortDir == NCGSORT_NONE) ||
							(pCD->nSortDir == NCGSORT_NONE && nSortDir != NCGSORT_NONE);

			// adjust column width if fixed size and was sorted
			if (bChange)
			{
				if (!pCD->bCalcWidth)
					pCD->nWidth += SORTWIDTH;
				else
					bNeedsRecalc = TRUE;
			}

			pCD->nSortDir = nSortDir;
		}
	}
	
	if (bNeedsRecalc)
		RecalcGutter();
	else
		Redraw();
}

void CNcGutter::EnableHeaderClicking(UINT nColID, BOOL bEnable)
{
	int nColumn = GetColumnIndex(nColID);

	if (nColumn >= 0 && nColumn < m_aColumns.GetSize())
		m_aColumns[nColumn]->bClickable = bEnable;
}

int CNcGutter::GetColumnIndex(UINT nColID) const
{
	int nCol = m_aColumns.GetSize();

	while (nCol-- && (nColID != m_aColumns[nCol]->nColID));

	return nCol; // can be -1
}

void CNcGutter::UnpressAllColumnHeaders(int nExcludeCol)
{
	int nCol = m_aColumns.GetSize();

	while (nCol--)
	{
		if (nCol != nExcludeCol)
			m_aColumns[nCol]->bPressed = FALSE;
	}
}

void CNcGutter::Redraw()
{
	CNcRedraw hr(GetHwnd());
}

void CNcGutter::RecalcGutter(BOOL bForceRedraw)
{
	int nCurWidth = GetGutterWidth();
	int nNewWidth = RecalcGutterWidth();

	if (IsHooked())
	{
		if (nNewWidth != nCurWidth || m_bFirstRecalc)
		{
			m_bFirstRecalc = FALSE;
			PostMessage(WM_NCG_FORCERESIZE, nCurWidth, nNewWidth);

			// notify parent then hook window
			UINT nID = GetDlgCtrlID();

			::SendMessage(GetParent(), WM_NCG_WIDTHCHANGE, nID, MAKELPARAM(nCurWidth, nNewWidth));
			SendMessage(WM_NCG_WIDTHCHANGE, nID, MAKELPARAM(nCurWidth, nNewWidth));

			// update header tracking rects
			UpdateHeaderHotRects();
		}
		else if (bForceRedraw)
			Redraw();
	}
}

BOOL CNcGutter::RecalcColumn(UINT nColID)
{
	if (!IsHooked() || m_bFirstRecalc || !m_bSetRedraw)
		return FALSE;

	if (nColID == NCG_CLIENTCOLUMNID)
		return FALSE;

	int nCol = GetColumnIndex(nColID);

	if (nCol == -1)
		return FALSE;

	COLUMNDESC* pCD = m_aColumns[nCol];

	if (!pCD->bCalcWidth)
		return FALSE;

	// do the recalc
	int nCurWidth = GetGutterWidth(); // cache this

	CWindowDC dc(GetCWnd());
	CFont* pOldFont = PrepareFont(&dc, FALSE);

	NCGRECALCCOLUMN ncrc = { nColID, &dc, 0 };

	// try parent then hook window
	UINT nCtrlID = GetDlgCtrlID();

	if (!::SendMessage(GetParent(), WM_NCG_RECALCCOLWIDTH, nCtrlID, (LPARAM)&ncrc))
		SendMessage(WM_NCG_RECALCCOLWIDTH, nCtrlID, (LPARAM)&ncrc);

	if (ncrc.nWidth > 0)
	{
		pCD->nWidth = max(ncrc.nWidth, pCD->nTextWidth) + 2 * NCG_COLPADDING;
	 
		if (pCD->nSortDir != NCGSORT_NONE)
			pCD->nWidth += SORTWIDTH;
	}
	else
		pCD->nWidth = 0;

	if (pOldFont)
		dc.SelectObject(pOldFont);

	// check for a change
	int nNewWidth = GetGutterWidth();

	if (nNewWidth != nCurWidth) // there's been a change
	{
		PostMessage(WM_NCG_FORCERESIZE);
		
		// notify hooked wnd and parent
		SendMessage(WM_NCG_WIDTHCHANGE, nCtrlID, MAKELPARAM(nCurWidth, nNewWidth));
		::SendMessage(GetParent(), WM_NCG_WIDTHCHANGE, nCtrlID, MAKELPARAM(nCurWidth, nNewWidth));
		
		// update header tracking rects
		UpdateHeaderHotRects();

		return TRUE;
	}

	return FALSE;
}

void CNcGutter::ShowHeader(BOOL bShow)
{
	if (m_bShowHeader != bShow)
	{
		if (CThemed().AreControlsThemed())
		{
			if (!bShow)
				m_hotTrack.Reset();
			else
			{
				m_hotTrack.Initialize(GetCWnd());

				for (int nCol = 0; nCol < m_aColumns.GetSize(); nCol++)
					m_hotTrack.AddRect();

				UpdateHeaderHotRects();
			}
		}

		m_bShowHeader = bShow;
		PostMessage(WM_NCG_FORCERESIZE);
	}
}

int CNcGutter::GetGutterWidth() const
{
	int nWidth = 0;

	for (int nCol = 0; nCol < m_aColumns.GetSize() - 1; nCol++)
		nWidth += m_aColumns[nCol]->nWidth;

	return nWidth;
}

void CNcGutter::AddRecalcMessage(UINT nMessage, UINT nNotification)
{
	m_mapRecalcMessages.SetAt(MAKELONG((WORD)nMessage, (WORD)nNotification), 0);
}

void CNcGutter::AddRedrawMessage(UINT nMessage, UINT nNotification)
{
	m_mapRedrawMessages.SetAt(MAKELONG((WORD)nMessage, (WORD)nNotification), 0);
}

LRESULT CNcGutter::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CAPTURECHANGED:
		if (!lp || (HWND)lp == GetHwnd())
			break;

		// else
		m_nHeaderColDown = -1;
		// fall thru
	case WM_SETFOCUS:
	case WM_VSCROLL:
	case WM_KILLFOCUS:
		if (m_bSetRedraw)
		{
			CNcRedraw hr(hRealWnd);
			return Default();
		}
		break;

	case WM_MOUSEWHEEL:
		if (m_bSetRedraw)
		{
			CNcRedraw hr(hRealWnd);
			return Default();
		}
		break;

	case WM_KEYDOWN:
		switch (wp) // virtual key
		{
		// the standard scroll keypresses
		case VK_DOWN:
		case VK_UP:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_PRIOR: 
		case VK_NEXT:  
		case VK_END:   
		case VK_HOME:  
		case VK_ESCAPE:  
			if (m_bSetRedraw)
			{
				CNcRedraw hr(hRealWnd);
				return Default();
			}
			break;

		// keys that might modify the cursor
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
			// but only on the first event (ie not repeats)
			if (!(lp & 0x40000000))
				OnSetCursor();
		}
		break;

	case WM_KEYUP:
		switch (wp) // virtual key
		{
		// keys that might modify the cursor
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
			if (!OnSetCursor())
			{
				// draw the default cursor
				SendMessage(WM_SETCURSOR, (WPARAM)GetHwnd(), MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		OnButtonDown(msg, lp);
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		OnButtonUp(msg, lp);
		break;

	case WM_NCRBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONDBLCLK:
		if (wp == HTBORDER) // means gutter
			OnNcButtonDown(msg, lp);
		break;

	case WM_NCRBUTTONUP:
		if (wp == HTBORDER) // means gutter
		{
			OnNcButtonUp(msg, lp);
			return 0;
		}
		break;

	case WM_NCLBUTTONUP:
		if (wp == HTBORDER) // means gutter
			OnNcButtonUp(msg, lp);
		break;

	case WM_ERASEBKGND:
		// its not a neat solution but it does ensure that when expanding and collapsing
		// tree controls that any animations look correct
		{
			HDC hdc = (HDC)wp;
			UINT nFlags = PRF_CLIENT;
			OnPrint(hdc, nFlags);
		}
		return TRUE; // always

	case WM_NCHITTEST:
		return OnNcHitTest(lp);

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_NCCALCSIZE:
		if (wp)
			return OnNcCalcSize((LPNCCALCSIZE_PARAMS)lp);
		break;

	case WM_NCPAINT:
		Default();

		if (m_bSetRedraw)
			OnNcPaint();
		return 0;

	case WM_PRINT:
		if (m_bSetRedraw)
			OnPrint((HDC)wp, (UINT&)lp);
		break;

	case WM_SETREDRAW:
		m_bSetRedraw = wp;
		
		if (m_bSetRedraw)
		{
			RecalcGutter();
			Redraw();
		}
		break;

	case WM_SETFONT:
		Default();
		RecalcGutter();
		return 0;

	case WM_SETCURSOR:
		if (LOWORD(lp) == HTBORDER && OnSetCursor())
			return TRUE;
		break;

	case WM_SIZE:
		if (m_hotTrack.IsValid())
		{
			Default();
			UpdateHeaderHotRects();
			return 0;
		}
		break;
	}

	// registered messages must be handled explicitly
	if (msg == WM_NCG_FORCERESIZE)
		return SetWindowPos(hRealWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); 

	else if (msg == WM_HTHOTCHANGE)
		OnHotChange(wp, lp);

	// test for specific messages wanting recalcs/redraws
	LRESULT lr = 0;

	if (WantsRecalc(msg, wp, lp, lr) || WantsRedraw(msg, wp, lp, lr))
		return lr;

	return CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
}

LRESULT CNcGutter::OnNcCalcSize(LPNCCALCSIZE_PARAMS lpncsp)
{
	LRESULT lr = Default();
	
	if (BORDER == 0)
		BORDER = lpncsp->rgrc[0].left - lpncsp->rgrc[1].left; // diff between window and client

	// make sure the client width is _never_ reduced to zero
	CRect rWindow;
	GetWindowRect(rWindow);

	UINT nGutterWidth = GetGutterWidth();
	nGutterWidth = min(rWindow.Width() - MINCLIENTWIDTH - BORDER * 2, nGutterWidth);
	
	lpncsp->rgrc[0].left += nGutterWidth;
	
	if (m_bShowHeader)
    {
        if (HEADERHEIGHT == 0)
            HEADERHEIGHT = CDlgUnits(GetParent()).ToPixelsY(11); // handles font sizes

		lpncsp->rgrc[0].top += HEADERHEIGHT;
    }
	else
        HEADERHEIGHT = 0;
	
	return lr;
}

BOOL CNcGutter::WantsRecalc(UINT nMsg, WPARAM wp, LPARAM lp, LRESULT& lr)
{
	if (!m_bSetRedraw)
		return FALSE;

	// convert message to lookup key
	WORD nNotification = 0;

	if (nMsg == WM_COMMAND)
		nNotification = HIWORD(wp);

	else if (nMsg == WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lp;
		nNotification = (WORD)pNMHDR->code;
	}

	DWORD dwKey = MAKELONG((WORD)nMsg, nNotification);
	char dummy;

	if (m_mapRecalcMessages.Lookup(dwKey, dummy))
	{
		lr = Default(); // always

		// ask parent/derived if they still want to recalc
		UINT nID = GetDlgCtrlID();
		const MSG* pMsg = GetCurrentMessage();

		BOOL bCancel = ::SendMessage(GetParent(), WM_NCG_WANTRECALC, nID, (LPARAM)pMsg);

		if (!bCancel)
			bCancel = SendMessage(WM_NCG_WANTRECALC, nID, (LPARAM)pMsg);

		if (bCancel)
			return TRUE;

		RecalcGutter();

		return TRUE;
	}

	return FALSE;
}

BOOL CNcGutter::WantsRedraw(UINT nMsg, WPARAM wp, LPARAM lp, LRESULT& lr)
{
	if (!m_bSetRedraw)
		return FALSE;

	// convert message to lookup key
	WORD nNotification = 0;

	if (nMsg == WM_COMMAND)
		nNotification = HIWORD(wp);
	else if (nMsg == WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lp;
		nNotification = (WORD)pNMHDR->code;
	}

	DWORD dwKey = MAKELONG((WORD)nMsg, nNotification);
	char dummy;

	if (m_mapRedrawMessages.Lookup(dwKey, dummy))
	{
		lr = Default(); // always

		// ask parent/derived if they still want to redraw
		UINT nID = GetDlgCtrlID();
		const MSG* pMsg = GetCurrentMessage();

		BOOL bCancel = ::SendMessage(GetParent(), WM_NCG_WANTREDRAW, nID, (LPARAM)pMsg);

		if (!bCancel)
			bCancel = SendMessage(WM_NCG_WANTREDRAW, nID, (LPARAM)pMsg);

		if (bCancel)
			return TRUE;

		CNcRedraw hr(GetHwnd());

		return TRUE;
	}

	return FALSE;
}

void CNcGutter::OnPaint()
{
	PAINTSTRUCT ps;
			
	HDC hdc = ::BeginPaint(GetHwnd(), &ps); // device context for painting

	if (m_bSetRedraw)
	{
		UINT nFlags = PRF_CLIENT;
		OnPrint(hdc, nFlags);
	}

	::EndPaint(GetHwnd(), &ps);
}

LRESULT CNcGutter::OnNcHitTest(CPoint point)
{
	LRESULT lHitTest = Default();
	
	switch (lHitTest)
	{
	case HTNOWHERE:
		return HTBORDER;
		
	case HTCAPTION:
		if (HitTest(point) == NCGHT_HEADER)
			return HTBORDER;
		break;
	}
		
	// everything else
	return lHitTest;
}

void CNcGutter::OnButtonDown(UINT nMsg, CPoint point)
{
	m_dwButtonDownItem = ItemHitTest(point);
	
	SetFocus(GetHwnd());

	// notify parent then hook window
	NCGITEMCLICK ngic = { m_dwButtonDownItem, 
						  0, 
						  NCG_CLIENTCOLUMNID, 
						  nMsg, { point.x, point.y } };
	
	UINT nID = GetDlgCtrlID();
		
	if (!::SendMessage(GetParent(), WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic))
		SendMessage(WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic);
	
	CNcRedraw hr(GetHwnd());
}

void CNcGutter::OnButtonUp(UINT nMsg, CPoint point)
{
	CPoint ptScreen(point);
	ClientToScreen(&ptScreen);
	
	// before we proceed, check whether we should process the message
	// as a non-client message. we check for any header button that is 
	// down as well as out of client area clicks  
	CRect rClient;
	GetClientRect(rClient);
	
	if (m_nHeaderColDown >= 0 || !rClient.PtInRect(point))
	{
		if (nMsg == WM_LBUTTONUP)
			OnNcButtonUp(WM_NCLBUTTONUP, ptScreen);
		else
			OnNcButtonUp(WM_NCRBUTTONUP, ptScreen);
		
		return;
	}

	DWORD dwItem;
	int nCol;
	NCG_HITTEST nHitTest = HitTest(ptScreen, dwItem, nCol);
	
	if (nMsg == WM_LBUTTONUP && nHitTest == NCGHT_ITEM)
	{
		// check its the same item
		if (dwItem == m_dwButtonDownItem)
		{
			// notify parent then hook window
			NCGITEMCLICK ngic = { dwItem, 
									m_dwButtonDownItem, 
									NCG_CLIENTCOLUMNID, 
									nMsg, { point.x, point.y } };
			
			UINT nID = GetDlgCtrlID();
			
			if (!::SendMessage(GetParent(), WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic))
				SendMessage(WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic);
		}
	}
}

void CNcGutter::OnNcButtonDown(UINT nMsg, CPoint point)
{
	SetFocus(GetHwnd());
	
	DWORD dwItem;
	int nCol;
	NCG_HITTEST nHitTest = HitTest(point, dwItem, nCol);

	if (nHitTest == NCGHT_NOWHERE)
		return;

	COLUMNDESC* pCD = m_aColumns[nCol];

	// reset
	m_dwButtonDownItem = 0;
	
	switch (nHitTest)
	{
	case NCGHT_HEADER:
		// check its a clickable column
		if (nMsg == WM_NCLBUTTONDOWN && nCol >= 0 && pCD->bClickable)
		{
			SetCapture(GetHwnd());
				
			pCD->bPressed = TRUE;
			m_nHeaderColDown = nCol;
		}
		break;

	case NCGHT_ITEM:
		{
			m_dwButtonDownItem = dwItem;
				
			NCGITEMCLICK ngic = { m_dwButtonDownItem, 0, pCD->nColID, nMsg, { point.x, point.y } };
				
			// try parent then hook window
			UINT nID = GetDlgCtrlID();
				
			if (!::SendMessage(GetParent(), WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic))
				SendMessage(WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic);
		}
		break;
	}
	
	// redraw for good measure
	Redraw();
}

void CNcGutter::OnNcButtonUp(UINT nMsg, CPoint point)
{
	if (nMsg == WM_NCRBUTTONUP)
	{
		SendMessage(WM_CONTEXTMENU, (WPARAM)GetHwnd(), MAKELPARAM(point.x, point.y));
	}
	else if (nMsg == WM_NCLBUTTONUP)
	{
		DWORD dwItem = 0;
		int nCol;
		
		NCG_HITTEST nHitTest = HitTest(point, dwItem, nCol);
		
		// if the header column is set it means that we're in the middle
		// of a column click. note: we don't check hittest here as the cursor
		// may not still be on the header
		if (m_nHeaderColDown >= 0)
		{
			UINT nColID = m_aColumns[m_nHeaderColDown]->nColID;
			
			// send notification if same column header currently pressed
			if (nHitTest == NCGHT_HEADER && nCol == m_nHeaderColDown)
			{
				NCGHDRCLICK nghc = { nColID, nMsg, FALSE };
				
				// try parent then hook window
				UINT nID = GetDlgCtrlID();
				
				if (!::SendMessage(GetParent(), WM_NCG_NOTIFYHEADERCLICK, nID, (LPARAM)&nghc))
					SendMessage(WM_NCG_NOTIFYHEADERCLICK, nID, (LPARAM)&nghc);
				
				m_nHeaderColDown = -1;
				PressHeader(nColID, nghc.bPressed);
			}
			else
			{
				m_nHeaderColDown = -1;
				PressHeader(nColID, FALSE);
			}
			
			ReleaseCapture();
		}
		// else item click, so check its the same item
		else if (nHitTest == NCGHT_ITEM && dwItem && dwItem == m_dwButtonDownItem)
		{
			// notify parent then hook window
			NCGITEMCLICK ngic = { dwItem, 
				m_dwButtonDownItem, 
				m_aColumns[nCol]->nColID, 
				nMsg, { point.x, point.y } };
			
			UINT nID = GetDlgCtrlID();
			
			if (!::SendMessage(GetParent(), WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic))
				SendMessage(WM_NCG_NOTIFYITEMCLICK, nID, (LPARAM)&ngic);
		}
	}
}

void CNcGutter::OnHotChange(int nPrevHot, int nHot)
{
	ASSERT (m_bShowHeader);

	if (m_bShowHeader)
	{
		CRect rWindow, rClient;

		// convert rects to window coords
		GetWindowRect(rWindow);
		GetClientRect(rClient);

		CPoint ptWindowTL = rWindow.TopLeft();

		ClientToScreen(rClient);
		rClient.OffsetRect(-ptWindowTL);
		rWindow.OffsetRect(-ptWindowTL);

		// allow for client edge, etc
		rWindow.DeflateRect(BORDER, BORDER);

		// adjust top of window rect to be top of header (if showing)
		CRect rHeader(rWindow);
		rHeader.bottom = rClient.top;
		rHeader.top = rHeader.bottom - HEADERHEIGHT;

		CWindowDC dc(GetCWnd());
		CPoint ptCursor(::GetMessagePos());
		ptCursor.Offset(-ptWindowTL);

		// nc portion
		int nClientCol = m_aColumns.GetSize() - 1;
		BOOL bDrawNonClient = FALSE;

		if (nPrevHot >= 0 && nPrevHot != nClientCol)
		{
			const COLUMNDESC* pCD = m_aColumns[nPrevHot];
			bDrawNonClient = (pCD->bClickable);
		}

		if (!bDrawNonClient && nHot >= 0 && nHot != nClientCol)
		{
			const COLUMNDESC* pCD = m_aColumns[nHot];
			bDrawNonClient = (pCD->bClickable);
		}

		CFont* pOldFont = PrepareFont(&dc, TRUE);

		if (bDrawNonClient)
		{
			rHeader.right = rClient.left;
			NcDrawHeader(&dc, rHeader, NONCLIENT, &ptCursor);
		}

		// client bit
		if (nPrevHot == nClientCol || nHot == nClientCol)
		{
			const COLUMNDESC* pCD = m_aColumns[nClientCol];

			if (pCD->bClickable)
			{
				rHeader.left = rClient.left;
				rHeader.right = rWindow.right;
				NcDrawHeader(&dc, rHeader, CLIENT, &ptCursor);
			}
		}

		dc.SelectObject(pOldFont);
	}
}

CFont* CNcGutter::PrepareFont(CDC* pDC, BOOL bHeader, HFONT hFont)
{
	if (!hFont && IsHooked())
	{
		if (bHeader)
			hFont = (HFONT)::SendMessage(GetParent(), WM_GETFONT, 0, 0);
		else
			hFont = (HFONT)::SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	}

	if (hFont)
	{
		CFont* pFont = CFont::FromHandle(hFont);
		
		if (pFont)
			return pDC->SelectObject(pFont);
	}
	
	// else
	return (CFont*)pDC->SelectStockObject(DEFAULT_GUI_FONT);
}

DWORD CNcGutter::ItemHitTest(CPoint ptClient) const
{
	UINT nID = GetDlgCtrlID();
	LONG lPoint = MAKELONG(ptClient.x, ptClient.y);

	// try parent then hook window
	DWORD dwItem = (DWORD)::SendMessage(GetParent(), WM_NCG_HITTEST, nID, lPoint);

	if (!dwItem)
		dwItem = (DWORD)SendMessage(WM_NCG_HITTEST, nID, lPoint);

	// else
	return dwItem;
}

int CNcGutter::ColumnHitTest(CPoint ptScreen) const
{
	CRect rClient;
	GetClientRect(rClient);
	ClientToScreen(rClient);

	rClient.top -= HEADERHEIGHT;

	if (rClient.PtInRect(ptScreen))
		return m_aColumns.GetSize() - 1;  // last column == client

	// else must be a gutter column
	CRect rWindow;
	GetWindowRect(rWindow);

	ptScreen.Offset(-rWindow.TopLeft());

	if (ptScreen.x <= 0)
		return 0;

	int nCol = m_aColumns.GetSize() - 1;  // omit last column == client
	int nGutter = GetGutterWidth();

	while (nCol--)
	{
		int nColWidth = m_aColumns[nCol]->nWidth;

		if (ptScreen.x >= (LONG)(nGutter - nColWidth) && ptScreen.x <= (LONG)nGutter + 1)
			return nCol;

		nGutter -= nColWidth;
	}

	return nCol; // == -1 ?? 
}

CNcGutter::NCG_HITTEST CNcGutter::HitTest(CPoint ptScreen) const
{
	DWORD dwItem;
	int nColumn;

	return HitTest(ptScreen, dwItem, nColumn);
}

BOOL CNcGutter::PtInHeader(CPoint ptScreen) const
{
	return (HitTest(ptScreen) == NCGHT_HEADER);
}

CNcGutter::NCG_HITTEST CNcGutter::HitTest(CPoint ptScreen, DWORD& dwItem, int& nColumn) const
{
	CRect rClient, rHeader, rWindow, rGutter;

	GetWindowRect(rWindow);
	GetClientRect(rClient);
	ClientToScreen(rClient);

	rHeader = rClient;
	rHeader.bottom = rHeader.top;
	rHeader.top -= HEADERHEIGHT; // HEADERHEIGHT will be zero if the header is hidden
	rHeader.left = rWindow.left;

	rGutter = rClient;
	rGutter.right = rClient.left;
	rGutter.left = rWindow.left;
	
	if (rHeader.PtInRect(ptScreen))
	{
		nColumn = ColumnHitTest(ptScreen);
		return NCGHT_HEADER;
	}
	else if (rGutter.PtInRect(ptScreen) || rClient.PtInRect(ptScreen))
	{
		nColumn = ColumnHitTest(ptScreen);

		// get item
		CPoint point(ptScreen);
		ScreenToClient(&point);

		if (point.x < 0) // in the gutter
			point.x = 0; // client coords

		dwItem = ItemHitTest(point);
		return NCGHT_ITEM;
	}

	// else
	return NCGHT_NOWHERE;
}

BOOL CNcGutter::OnSetCursor()
{
	CPoint ptScreen(::GetMessagePos());
	DWORD dwItem;
	int nCol;

	NCG_HITTEST nHitTest = HitTest(ptScreen, dwItem, nCol);

	if (nHitTest == NCGHT_ITEM && dwItem)
	{
		NCGGETCURSOR ncgsc = { m_aColumns[nCol]->nColID, dwItem };
		UINT nID = GetDlgCtrlID();
		
		HCURSOR hCursor = (HCURSOR)::SendMessage(GetParent(), WM_NCG_GETCURSOR, nID, (LPARAM)&ncgsc);
		
		if (!hCursor)
			hCursor = (HCURSOR)SendMessage(WM_NCG_GETCURSOR, nID, (LPARAM)&ncgsc);
		
		if (hCursor)
		{
			::SetCursor(hCursor);
			return TRUE;
		}
	}

	return FALSE;
}

void CNcGutter::OnNcPaint() 
{
//	TRACE("CNcGutter::OnNcPaint()\n");
	// force client repaints to happen first because
	// of the time it takes to perform the non-client paint
	::UpdateWindow(GetHwnd());

	// see if we can avoid any unnecessary drawing
	if (GetGutterWidth() == 0 && !m_bShowHeader)
		return;

	CRect rWindow, rClient;
	CPoint ptWindowTL;

	GetWindowRect(rWindow);
	ptWindowTL = rWindow.TopLeft();
	rWindow.OffsetRect(-ptWindowTL);

	GetClientRect(rClient);
	ClientToScreen(rClient);
	rClient.OffsetRect(-ptWindowTL);

	// get cursor pos in window coords for header drawing
	CPoint ptCursor;

	if (m_bShowHeader)
	{
		::GetCursorPos(&ptCursor);
		ptCursor.Offset(-ptWindowTL);
	}

	// allow for client edge, etc
	rWindow.DeflateRect(BORDER, BORDER);

	// adjust top of window rect to be top of header (if showing)
	rWindow.top = rClient.top - (m_bShowHeader ? HEADERHEIGHT : 0);
	CRect rWindowOrg(rWindow); // and save

	// to avoid creating excessively large bitmaps we'll render the client header straight
	// into the window dc ie only the non-client gutter & header is rendered on the bitmap
	rWindow.bottom = rClient.bottom;
	rWindow.right = rClient.left;

	CWindowDC dc(GetCWnd());
	CDC dcMem;
	CDC* pOutputDC = &dc; // default backup plan in case mem dc or bitmap cannot be created
	CBitmap* pBMOld = NULL;

	if (GetGutterWidth() > 0 && dcMem.CreateCompatibleDC(NULL) && 
		PrepareBitmap(&dc, &m_bmNonClient, CRect(0, 0, rWindow.right, rWindow.bottom), FALSE))
	{		
		pOutputDC = &dcMem;
		pBMOld = dcMem.SelectObject(&m_bmNonClient);
	}

	pOutputDC->FillSolidRect(rWindow.left, rClient.top, rWindow.Width(), rWindow.Height(), ::GetSysColor(COLOR_WINDOW));

	// iterate the top level items
	CFont* pOldFont = PrepareFont(pOutputDC, FALSE);
				
	int nItem = 1;
	DWORD dwItem = GetFirstVisibleTopLevelItem(nItem);
				
	while (dwItem)
	{
		CRect rItem;
		CString sPrevPos;
		NcDrawItem(pOutputDC, dwItem, 0, 0, nItem, rWindow, rClient, rItem, TRUE);
					
		dwItem = GetNextItem(dwItem);
		nItem++;
					
		if (rItem.bottom >= rWindow.bottom)
			break;
	}
				
	PostNcDraw(pOutputDC, rWindow);

	// cleanup			
	pOutputDC->SelectObject(pOldFont);

	// non-client header
	if (m_bShowHeader)
	{
		CRect rHeader(rWindowOrg);
		rHeader.bottom = rHeader.top + HEADERHEIGHT;
		rHeader.right = rClient.left;
		NcDrawHeader(pOutputDC, rHeader, NONCLIENT, &ptCursor);
	}

	// blt to window dc if nec
	if (pOutputDC == &dcMem)
	{
		dc.BitBlt(rWindow.left, rWindow.top, rWindow.Width(), rWindow.Height(),
				  &dcMem, rWindow.left, rWindow.top, SRCCOPY);

		dcMem.SelectObject(pBMOld); // V.V.IMPORTANT
	}

	// two items we render direct to the window dc
	
	// 1. the client header
	if (m_bShowHeader)
	{
		CFont* pOldFont = PrepareFont(&dc, FALSE);

		CRect rHeader(rWindowOrg);
		rHeader.bottom = rHeader.top + HEADERHEIGHT;
		rHeader.left = rClient.left;
		NcDrawHeader(&dc, rHeader, CLIENT, &ptCursor);

		// cleanup			
		dc.SelectObject(pOldFont);
	}
				
	// 2. if the window base does not match the client bottom then 
	// paint the extra bit gray (means there is a horz scrollbar)
	if (rWindowOrg.bottom != rClient.bottom)
	{
		CRect rScroll(rWindowOrg.left, rClient.bottom, rClient.left, rWindowOrg.bottom);

		if (CThemed().IsNonClientThemed())
		{
			CThemed th;

			if (th.Open(GetCWnd(), "SCROLLBAR"))
				th.DrawBackground(&dc, SBP_LOWERTRACKHORZ, SCRBS_NORMAL, rScroll);
			else
				dc.FillSolidRect(rScroll, ::GetSysColor(COLOR_SCROLLBAR));
		}
		else
			dc.FillSolidRect(rScroll, ::GetSysColor(COLOR_SCROLLBAR));
	}
}

void CNcGutter::NcDrawHeaderColumn(CDC* pDC, int nColumn, CRect rColumn, CThemed* pTheme, LPPOINT pCursor)
{
	COLUMNDESC* pCD = m_aColumns[nColumn];

	BOOL bDown = (nColumn == m_nHeaderColDown);
	BOOL bPressed = (pCD->bPressed || bDown);
	BOOL bSorted = (pCD->nSortDir != NCGSORT_NONE);
	
	if (!pTheme)
	{
		if (bPressed)
		{
			pDC->FillSolidRect(rColumn, ::GetSysColor(COLOR_3DHIGHLIGHT));
			pDC->Draw3dRect(rColumn, ::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_3DSHADOW));
		}
		else
			pDC->Draw3dRect(rColumn, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DSHADOW));
	}
	else
	{
		BOOL bHot = rColumn.PtInRect(*pCursor);
		BOOL bClickable = (bHot && pCD->bClickable);

		if (!bPressed)
			rColumn.right++;
		
		pTheme->DrawBackground(pDC, HP_HEADERITEM, bPressed ? HIS_PRESSED : 
								(bClickable ? HIS_HOT : HIS_NORMAL), rColumn);
	}
	
	// text
	if (!pCD->sTitle.IsEmpty())
	{
		const UINT DEFFLAGS = DT_END_ELLIPSIS | DT_BOTTOM | DT_SINGLELINE;

		CRect rText(rColumn);

		rText.bottom -= (pCD->bSymbolFont ? 1 : 2) + (pTheme ? 1 : 0);

		switch (pCD->nTextAlign)
		{
		case DT_LEFT:
			rText.left += NCG_COLPADDING;
			break;
			
		case DT_RIGHT:
			rText.right -= NCG_COLPADDING;
			break;
		}
		
		if (bDown)
			rText.OffsetRect(1, 1);
		
		if (bSorted)
			rText.right -= (SORTWIDTH + 2);

		if (pCD->hFont)
		{
			CFont* pOldFont = PrepareFont(pDC, TRUE, pCD->hFont);
			pDC->DrawText(pCD->sTitle, rText, DEFFLAGS | pCD->nTextAlign);
			pDC->SelectObject(pOldFont);
		}
		else
			pDC->DrawText(pCD->sTitle, rText, DEFFLAGS | pCD->nTextAlign);

		// adjust for sort arrow
		if (bSorted)
		{
			if (pCD->nTextAlign & DT_CENTER)
				rColumn.left = ((rText.left + rText.right + pDC->GetTextExtent(pCD->sTitle).cx) / 2) + 2;

			else if (pCD->nTextAlign & DT_RIGHT)
				rColumn.left = rText.right + 2;
			else
				rColumn.left = rText.left + pDC->GetTextExtent(pCD->sTitle).cx + 2;
		}
	}

	// draw sort direction if required
	if (bSorted)
	{
		int nMidY = (bDown ? 1 : 0) + (rColumn.top + rColumn.bottom) / 2;
		POINT ptArrow[3] = { { 0, 0 }, { 3, (int)pCD->nSortDir * 3 }, { 7, -(int)pCD->nSortDir } };
		
		// translate the arrow to the appropriate location
		int nPoint = 3;
		
		while (nPoint--)
		{
			ptArrow[nPoint].x += rColumn.left + 3 + (bDown ? 1 : 0);
			ptArrow[nPoint].y += nMidY;
		}
		pDC->Polyline(ptArrow, 3);
	}
	
}

void CNcGutter::NcDrawHeader(CDC* pDC, const CRect& rHeader, HCHDRPART nPart, const LPPOINT pCursor)
{
	if (!m_bShowHeader)
		return;

	CThemed th;
	BOOL bThemed = th.AreControlsThemed() && th.Open(GetCWnd(), "HEADER");

	if (!bThemed)
		pDC->FillSolidRect(rHeader, ::GetSysColor(COLOR_3DFACE));

	pDC->SetBkMode(TRANSPARENT);

	CFont* pOldFont = PrepareFont(pDC, TRUE); // default header font

	CRect rColumn(rHeader);

	if (nPart == NONCLIENT)
	{
		int nNumCols = m_aColumns.GetSize() - 1; // omit last column == client
		rColumn.right = rColumn.left;

		for (int nCol = 0; nCol < nNumCols; nCol++)
		{
			COLUMNDESC* pCD = m_aColumns[nCol];

			if (!pCD->nWidth)
				continue;

			rColumn.left = rColumn.right;
			rColumn.right = min(rHeader.right, rColumn.left + (int)pCD->nWidth);

			NcDrawHeaderColumn(pDC, nCol, rColumn, bThemed ? &th : NULL, bThemed ? pCursor : NULL);

			// stop if we've gone passed the end of the available space
			if (rColumn.right >= rHeader.right)
				break;
		}
	}
	else // the rest is over the client area
	{
		int nCol = GetColumnIndex(NCG_CLIENTCOLUMNID);
		ASSERT (nCol != -1);

		NcDrawHeaderColumn(pDC, nCol, rColumn, bThemed ? &th : NULL, bThemed ? pCursor : NULL);
	}

	// cleanup
	pDC->SelectObject(pOldFont);
}

void CNcGutter::OnPrint(HDC hdc, UINT& nFlags)
{
	if (nFlags & PRF_CLIENT)
	{
		// render the control to a bitmap for flicker free
		CRect rClient;
		GetClientRect(rClient);
		rClient.InflateRect(1, 1);

		CDC dcMem, dc;

		if (dcMem.CreateCompatibleDC(NULL))
		{
			dc.Attach(hdc);

			if (PrepareBitmap(&dc, &m_bmClient, rClient, TRUE))
			{
				CBitmap* pBMOld = dcMem.SelectObject(&m_bmClient);
				dcMem.FillSolidRect(rClient, GetSysColor(COLOR_WINDOW));

				CSubclassWnd::WindowProc(GetHwnd(), WM_PRINT, (WPARAM)(HDC)dcMem, PRF_CLIENT);

				dc.BitBlt(0, 0, rClient.right, rClient.bottom, &dcMem, 0, 0, SRCCOPY);

				nFlags &= ~PRF_CLIENT; // we just drawn this bit

				// cleanup
				dcMem.SelectObject(pBMOld); // V.V.IMPORTANT
			}

			dc.Detach();
		}
	}
}

DWORD CNcGutter::GetFirstVisibleTopLevelItem(int& nItem) const
{
	// try hook window then parent
	UINT nID = GetDlgCtrlID();

	DWORD dwItem = (DWORD)::SendMessage(GetParent(), WM_NCG_GETFIRSTVISIBLETOPLEVELITEM, nID, (LPARAM)(LPINT)&nItem);

	if (!dwItem)
		dwItem = (DWORD)SendMessage(WM_NCG_GETFIRSTVISIBLETOPLEVELITEM, nID, (LPARAM)(LPINT)&nItem);

	// else
	return dwItem;
}

void CNcGutter::NcDrawItem(CDC* pDC, DWORD dwItem, DWORD dwParentItem, int nLevel, int nPos, 
						   const CRect& rWindow, const CRect& rClient, CRect& rItem, BOOL bDrawChildren)
{
	rItem = GetItemRect(dwItem);
							
	if (!rItem.IsRectEmpty())
	{
		// convert rItem to window coords
		rItem.OffsetRect(0, rClient.TopLeft().y); // convert to window coords
		rItem.left = rWindow.left;
		rItem.right = rWindow.right;

		if (!bDrawChildren)
		{
			rItem.right = rClient.left;
			pDC->FillSolidRect(rItem, GetSysColor(COLOR_WINDOW));
		}

		NCGDRAWITEM ncgDI;
		
		ncgDI.pDC = pDC;
		ncgDI.dwItem = dwItem;
		ncgDI.dwParentItem = dwParentItem;
		ncgDI.nLevel = nLevel;
		ncgDI.nItemPos = nPos;
		ncgDI.rWindow = &rWindow;
		ncgDI.rItem = &rItem;
		ncgDI.bSelected = IsItemSelected(dwItem);
		
		UINT nID = GetDlgCtrlID();
		
		// draw each column in turn
		rItem.right = rItem.left;
		int nNumCols = m_aColumns.GetSize() - 1; // omit last column == client
		
		for (int nCol = 0; nCol < nNumCols; nCol++)
		{
			const COLUMNDESC* pCD = m_aColumns[nCol];

			ncgDI.nColID = pCD->nColID;
			ncgDI.nTextAlign = pCD->nTextAlign;

			rItem.left = rItem.right;
			
			// if we're out of the window rect then stop
			if (rItem.left > rWindow.right)
				break;
			
			// don't bother with zero width columns
			int nColWidth = (int)pCD->nWidth;
			
			if (!nColWidth)
				continue;
			
			rItem.right += nColWidth;
			int nSaveDC = pDC->SaveDC();
			
			// try parent then hook window
			if (!::SendMessage(GetParent(), WM_NCG_DRAWITEM, nID, (LPARAM)&ncgDI))
				SendMessage(WM_NCG_DRAWITEM, nID, (LPARAM)&ncgDI);
			
			pDC->RestoreDC(nSaveDC);
		}

		// post draw for this item
		rItem.left = rWindow.left;
		rItem.right = rClient.left;
		PostNcDrawItem(pDC, dwItem, rItem, nLevel, FALSE);

		// draw children
		if (bDrawChildren)
		{
			int nChild = 1;
			DWORD dwChild = GetFirstChildItem(dwItem);
			CRect rChild;
			
			while (dwChild)
			{
				NcDrawItem(pDC, dwChild, dwItem, nLevel + 1, nChild, rWindow, rClient, rChild, TRUE);
				
				dwChild = GetNextItem(dwChild);
				nChild++;
				
				// accumulate child rects into rItem
				rItem |= rChild;
				
				if (rItem.bottom >= rWindow.bottom)
					break;
			}
			
			// post draw only if we reached the last item
			CRect rItemTotal(rItem);
			rItemTotal.left = rWindow.left;
			rItemTotal.right = rWindow.right;
			
			if (!dwChild)
				PostNcDrawItem(pDC, dwItem, rItemTotal, nLevel, TRUE);
			
			rItem = rItemTotal;
		}
	}
}

void CNcGutter::RedrawItem(DWORD dwItem)
{
	// for speed we draw straight to the window DC so _don't_ use
	// this internally.
	CRect rClient, rWindow, rItem;

	GetClientRect(rClient);
	GetWindowRect(rWindow);

	ClientToScreen(rClient);
	rClient.OffsetRect(-rWindow.TopLeft());

	rWindow.OffsetRect(-rWindow.TopLeft());
	rWindow.DeflateRect(BORDER, BORDER);

	// prepare the DC
	CWindowDC dc(GetCWnd());
	CFont* pOldFont = PrepareFont(&dc, FALSE);

	NcDrawItem(&dc, dwItem, GetParentItem(dwItem), -1, -1, rWindow, rClient, rItem, FALSE);

	// cleanup
	if (pOldFont)
		dc.SelectObject(pOldFont);
}

void CNcGutter::PostNcDrawItem(CDC* pDC, DWORD dwItem, const CRect& rItem, int nLevel, BOOL bParent)
{
	NCGDRAWITEM ncgDI;
		
	ncgDI.pDC = pDC;
	ncgDI.rItem = &rItem;
	ncgDI.nLevel = nLevel;

	if (bParent)
		ncgDI.dwParentItem = dwItem;
	else
		ncgDI.dwItem = dwItem;

	UINT nID = GetDlgCtrlID();
		
	// try parent then hook window
	if (!::SendMessage(GetParent(), WM_NCG_POSTDRAWITEM, nID, (LPARAM)&ncgDI))
		SendMessage(WM_NCG_POSTDRAWITEM, nID, (LPARAM)&ncgDI);
}

DWORD CNcGutter::GetParentItem(DWORD dwItem) const
{
	UINT nID = GetDlgCtrlID();
		
	// try parent then hook window
	DWORD dwParentID = ::SendMessage(GetParent(), WM_NCG_GETPARENTITEM, nID, dwItem);

	if (!dwParentID)
		dwParentID = SendMessage(WM_NCG_GETPARENTITEM, nID, dwItem);

	return dwParentID;
}

void CNcGutter::PostNcDraw(CDC* pDC, const CRect& rWindow)
{
	NCGDRAWITEM ncgDI;
		
	ncgDI.pDC = pDC;
	ncgDI.rWindow = &rWindow;

	UINT nID = GetDlgCtrlID();
		
	// try parent then hook window
	if (!::SendMessage(GetParent(), WM_NCG_POSTNCDRAW, nID, (LPARAM)&ncgDI))
		SendMessage(WM_NCG_POSTNCDRAW, nID, (LPARAM)&ncgDI);
}

DWORD CNcGutter::GetNextItem(DWORD dwItem) const
{
	UINT nID = GetDlgCtrlID();

	// try parent then hook window
	DWORD dwNextItem = (DWORD)::SendMessage(GetParent(), WM_NCG_GETNEXTITEM, nID, dwItem);

	if (!dwNextItem)
		dwNextItem = (DWORD)SendMessage(WM_NCG_GETNEXTITEM, nID, dwItem);

	return dwNextItem;
}

int CNcGutter::RecalcGutterWidth()
{
	if (!IsHooked())
		return 0;

	// optimization
	if (!m_bSetRedraw)
		return GetGutterWidth();

//	DWORD dwTick = GetTickCount();

	CWindowDC dc(GetCWnd());

	NCGRECALCCOLUMN ncrc;
	ncrc.pDC = &dc;

	// try hook window then parent
	UINT nID = GetDlgCtrlID();
	int nGutter = 0;
	int nCol = m_aColumns.GetSize() - 1; // omit last column == client

	while (nCol--)
	{
		// check if its a fixed size column
		COLUMNDESC* pCD = m_aColumns[nCol];

		if (pCD->bCalcWidth)
		{
			CFont* pOldFont = PrepareFont(&dc, FALSE);
			
			ncrc.nColID = pCD->nColID;
			ncrc.nWidth = 0;
			
			// try parent then hook window
			if (!::SendMessage(GetParent(), WM_NCG_RECALCCOLWIDTH, nID, (LPARAM)&ncrc))
				SendMessage(WM_NCG_RECALCCOLWIDTH, nID, (LPARAM)&ncrc);
			
			if (ncrc.nWidth > 0)
			{
				pCD->nWidth = max(ncrc.nWidth, pCD->nTextWidth) + 2 * NCG_COLPADDING;
				
				if (pCD->nSortDir != NCGSORT_NONE)
					pCD->nWidth += SORTWIDTH;
			}
			else
				pCD->nWidth = 0;
			
			if (pOldFont)
				dc.SelectObject(pOldFont);
		}
		nGutter += pCD->nWidth;
	}

//	TRACE ("CNcGutter::RecalcGutterWidth(%d ms)\n", GetTickCount() - dwTick);

	return nGutter;
}

CRect CNcGutter::GetItemRect(DWORD dwItem) const
{
	UINT nID = GetDlgCtrlID();
	NCGITEMRECT ncgGI;
	ncgGI.dwItem = dwItem;

	// try parent then hook window
	if (!::SendMessage(GetParent(), WM_NCG_GETITEMRECT, nID, (LPARAM)&ncgGI))
		SendMessage(WM_NCG_GETITEMRECT, nID, (LPARAM)&ncgGI);

	return ncgGI.rItem;
}

DWORD CNcGutter::GetFirstChildItem(DWORD dwItem) const
{
	UINT nID = GetDlgCtrlID();

	// try parent then hook window
	DWORD dwChildItem = (DWORD)::SendMessage(GetParent(), WM_NCG_GETFIRSTCHILDITEM, nID, dwItem);

	if (!dwChildItem)
		dwChildItem = (DWORD)SendMessage(WM_NCG_GETFIRSTCHILDITEM, nID, dwItem);

	return dwChildItem;
}

BOOL CNcGutter::IsItemSelected(DWORD dwItem) const
{
	UINT nID = GetDlgCtrlID();
	NCGITEMSELECTION ncgIS = { dwItem, FALSE };

	// try parent then hook window
	if (!::SendMessage(GetParent(), WM_NCG_ISITEMSELECTED, nID, (LPARAM)&ncgIS))
		SendMessage(WM_NCG_ISITEMSELECTED, nID, (LPARAM)&ncgIS);

	// else
	return ncgIS.bSelected;
}

int CNcGutter::GetSelectedCount() const
{
	UINT nID = GetDlgCtrlID();

	// try parent then hook window
	int nCount = 0;
	
	if (!::SendMessage(GetParent(), WM_NCG_GETSELECTEDCOUNT, nID, (LPARAM)&nCount))
		SendMessage(WM_NCG_GETSELECTEDCOUNT, nID, (LPARAM)&nCount);

	return nCount;
}

BOOL CNcGutter::PrepareBitmap(CDC* pDC, CBitmap* pBitmap, const CRect& rect, BOOL /*bClient*/)
{
	BOOL bRecreate = !pBitmap->GetSafeHandle();

	if (!bRecreate)
	{
		BITMAP BM;

		bRecreate = (!pBitmap->GetBitmap(&BM));

		if (!bRecreate)
		{
			int nBitDepth = pDC->GetDeviceCaps(BITSPIXEL);

			bRecreate = (nBitDepth != BM.bmBitsPixel || 
						rect.Width() > BM.bmWidth ||
						rect.Height() > BM.bmHeight);
		}
	}

	if (bRecreate)
	{
		pBitmap->DeleteObject();
		pBitmap->CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	}

	return (NULL != pBitmap->GetSafeHandle());
}

void CNcGutter::UpdateHeaderHotRects()
{
	if (!m_bShowHeader || !CThemed().AreControlsThemed())
		return;

	CRect rWindow, rClient;

	// convert rects to client coords
	GetWindowRect(rWindow);
	ScreenToClient(rWindow);

	GetClientRect(rClient);

	// allow for client edge, etc
	rWindow.DeflateRect(BORDER, BORDER);

	// adjust top of window rect to be top of header (if showing)
	CRect rHeader(rWindow);
	rHeader.bottom = rClient.top;
	rHeader.top = rHeader.bottom - HEADERHEIGHT;

	CRect rItem(rHeader);
	rItem.right = rItem.left;

	int nNumCols = m_aColumns.GetSize() - 1;  // omit last column == client

	for (int nCol = 0; nCol < nNumCols; nCol++)
	{
		rItem.left = rItem.right;
		rItem.right += m_aColumns[nCol]->nWidth;
		rItem.right = min(rItem.right, rWindow.right);
		
		m_hotTrack.UpdateRect(nCol, rItem);
		
		if (rItem.right == rWindow.right)
			break;
	}

	// client bit
	rItem.left = rClient.left;
	rItem.right = rClient.right;

	m_hotTrack.UpdateRect(nNumCols, rItem);
}
