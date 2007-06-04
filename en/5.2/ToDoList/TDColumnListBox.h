#if !defined(AFX_TDCOLUMNLISTBOX_H__C5D9EFCE_7B59_498F_88E4_890286A344A5__INCLUDED_)
#define AFX_TDCOLUMNLISTBOX_H__C5D9EFCE_7B59_498F_88E4_890286A344A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TDColumnListBox.h : header file
//

#include "tdcenum.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTDColumnListBox window

enum TDLB_COLUMN
{
	TDLBC_PRIORITY,
	TDLBC_PERCENT,
	TDLBC_TIMEEST,
	TDLBC_STARTDATE,
	TDLBC_DUEDATE,
	TDLBC_DONEDATE,
	TDLBC_ALLOCTO,
	TDLBC_FILEREF,
	TDLBC_POSITION,
	TDLBC_ID,
	TDLBC_DONE,
	TDLBC_ALLOCBY,
	TDLBC_STATUS,
	TDLBC_CATEGORY,
	TDLBC_TIMESPENT,
	TDLBC_TRACKTIME,
	TDLBC_FLAG,
	TDLBC_CREATIONDATE,
	TDLBC_CREATEDBY,
	TDLBC_MODIFYDATE,
	TDLBC_RISK,
	TDLBC_EXTERNALID,
	TDLBC_COST,
	TDLBC_DEPENDS,
	TDLBC_RECURRENCE,
	TDLBC_VERSION,
	// add new items _ONLY_ at end
};

class CTDColumnListBox : public CCheckListBox
{
// Construction
public:
	CTDColumnListBox();

	void SetColumnState(TDLB_COLUMN nCol, BOOL bOn);
	BOOL GetColumnState(TDLB_COLUMN nCol) const;

// Attributes
protected:
	struct COLUMNSTATE
	{
		COLUMNSTATE() {}
		COLUMNSTATE(UINT nIDName, TDLB_COLUMN col, BOOL on) 
		{ 
			sName.LoadString(nIDName); 
			nCol = col; 
			bOn = on; 
		}

		CString sName;
		TDLB_COLUMN nCol;
		BOOL bOn;
	};
	CArray<COLUMNSTATE, COLUMNSTATE&> m_aColumns;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDColumnListBox)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTDColumnListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTDColumnListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg BOOL OnReflectCheckChange();
	afx_msg LRESULT OnInitListBox(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDCOLUMNLISTBOX_H__C5D9EFCE_7B59_498F_88E4_890286A344A5__INCLUDED_)
