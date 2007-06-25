// TimeEdit.h: interface for the CTimeEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEEDIT_H__2CCFE44D_9578_4E38_B2BF_091C172C85A5__INCLUDED_)
#define AFX_TIMEEDIT_H__2CCFE44D_9578_4E38_B2BF_091C172C85A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "enedit.h"
#include "timehelper.h"

const UINT WM_TEN_UNITSCHANGE = ::RegisterWindowMessage("WM_TEN_UNITSCHANGE"); // wParam == <CtrlID>, lParam = <prev units>

class CTimeEdit : public CEnEdit, public CTimeHelper
{
public:
	CTimeEdit(int nUnits = THU_HOURS, int nMaxDecPlaces = 9);
	virtual ~CTimeEdit();

	double GetTime() const;
	double GetTime(int nUnits) const;

	void SetTime(double dTime);
	void SetTime(double dTime, int nUnits);

	inline int GetUnits() const { return m_nUnits; }
	void SetUnits(int nUnits);

	inline int GetMaxDecimalPlaces() const { return m_nMaxDecPlaces; }
	void SetMaxDecimalPlaces(int nMaxDecPlaces);

	CString FormatTimeHMS() const; 
	CString FormatTime(BOOL bUnits) const; 

	static void SetUnits(int nUnits, LPCTSTR szLongUnits, LPCTSTR szAbbrevUnits);
	static void SetRecalcTimePrompt(LPCTSTR szTitle, LPCTSTR szPrompt);

protected:
	int m_nUnits, m_nMaxDecPlaces;
	static CString s_sRecalcTimePrompt, s_sRecalcTimeTitle;

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeEdit)
	//}}AFX_VIRTUAL
	virtual void PreSubclassWindow();
	virtual void OnBtnClick(UINT nID);
	virtual void OnSetReadOnly(BOOL bReadOnly);
//	virtual void DrawButton(CDC* pDC, const CRect& rWindow, int nBtn, const CPoint& ptCursor) const;

// Implementation

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	static void RemoveTrailingZeros(CString& sTime);
};

#endif // !defined(AFX_TIMEEDIT_H__2CCFE44D_9578_4E38_B2BF_091C172C85A5__INCLUDED_)
