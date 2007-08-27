// RichEditSpellCheck.h: interface for the CRichEditSpellCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHEDITSPELLCHECK_H__9A6FC513_71CB_4207_9FED_7B1429010FE5__INCLUDED_)
#define AFX_RICHEDITSPELLCHECK_H__9A6FC513_71CB_4207_9FED_7B1429010FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISpellCheck.h"

class CRichEditSpellCheck : public ISpellCheck  
{
public:
	CRichEditSpellCheck(CRichEditCtrl& re);
	virtual ~CRichEditSpellCheck();

	const char* GetFirstWord() const;
	const char* GetNextWord() const;
	const char* GetCurrentWord() const;

	void SelectCurrentWord();
	void ReplaceCurrentWord(const char* szWord);
	
	void ClearSelection();

protected:
	CRichEditCtrl& m_re;
	mutable CHARRANGE m_crCurrentWord;

protected:
	const char* GetWord(const CHARRANGE& cr) const;
};

#endif // !defined(AFX_RICHEDITSPELLCHECK_H__9A6FC513_71CB_4207_9FED_7B1429010FE5__INCLUDED_)
