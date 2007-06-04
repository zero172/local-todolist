// RichEditHelper.h: interface for the CRichEditHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHEDITHELPER_H__C498C86D_613F_42AD_9C93_6C773E6368E8__INCLUDED_)
#define AFX_RICHEDITHELPER_H__C498C86D_613F_42AD_9C93_6C773E6368E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <richedit.h>
#include <richole.h>

struct ITextDocument;

class CReBase
{
public:
	CReBase(HWND hwndRichEdit);
	virtual ~CReBase();

protected:
	HWND m_hwndRichedit;
};

class CTextDocument : public CReBase
{
public:
	CTextDocument(HWND hwndRichEdit);
	virtual ~CTextDocument();
	
	BOOL Valid() const { return (m_pDoc != NULL);	}
	operator ITextDocument*() { return m_pDoc; }

	BOOL Undo();
	BOOL Redo();
	
protected:
	ITextDocument* m_pDoc;
	LPRICHEDITOLE m_pRichEditOle;
};

class CRePauseUndo : public CTextDocument
{
public:
	CRePauseUndo(HWND hwndRichEdit);
	virtual ~CRePauseUndo();
};

class CReFileObject : public CReBase
{
public:
	CReFileObject(HWND hwndRichEdit);
	virtual ~CReFileObject();
	
	BOOL Insert(LPCTSTR szFilePath);
	
protected:
	LPRICHEDITOLE m_pRichEditOle;
	LPOLEOBJECT m_pObject;
	LPSTORAGE m_pStorage;
	LPOLECLIENTSITE m_pClientSite;
	
protected:
	void Reset();
	
	BOOL Create(LPCTSTR szFilePath);
	BOOL GetReObject(REOBJECT& reObj) const;
	
};

class CRichEditHelper
{
public:
	static BOOL CreateRichEdit20(CWnd& wnd, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	static BOOL InitRichEdit();
	static void ClearUndo(HWND hWnd);
	static BOOL Redo(HWND hWnd);
	
protected:
};

#endif // !defined(AFX_RICHEDITHELPER_H__C498C86D_613F_42AD_9C93_6C773E6368E8__INCLUDED_)
