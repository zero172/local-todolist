// MenuEx.h: interface for the CMenuEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUEX_H__5AB11CC8_CCF5_4D52_ADC7_27FDC151F3FE__INCLUDED_)
#define AFX_MENUEX_H__5AB11CC8_CCF5_4D52_ADC7_27FDC151F3FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EnBitmapEx.h"

enum MENUEX_BTN
{
	MEB_MINIMIZE,
	MEB_RESTORE,
	MEB_CLOSE,
};

class CMenuEx : public CMenu  
{
public:
	CMenuEx();
	virtual ~CMenuEx();

	// pass -1 as nThemeBMID is you want ownerdraw
	BOOL AddMDIButton(MENUEX_BTN nBtn, UINT nCmdID, UINT nThemeBMID = 0, BOOL bRightJustify = TRUE);
	BOOL DeleteMDIMenu(UINT nCmdID);

	static BOOL DrawMDIButton(UINT nIDBitmap, LPDRAWITEMSTRUCT lpDrawItemStruct, BOOL bDrawEdges = TRUE); 
	static BOOL MeasureMDIButton(UINT nIDBitmap, LPMEASUREITEMSTRUCT lpMeasureItemStruct); 

protected:
	CEnBitmapEx m_bmMinimize, m_bmRestore, m_bmClose;

protected:
	static BOOL LoadThemeBitmap(CEnBitmapEx& bitmap, UINT nThemeBMID);
	static BOOL IsThemed();
};

#endif // !defined(AFX_MENUEX_H__5AB11CC8_CCF5_4D52_ADC7_27FDC151F3FE__INCLUDED_)
