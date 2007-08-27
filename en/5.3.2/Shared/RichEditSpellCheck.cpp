// RichEditSpellCheck.cpp: implementation of the CRichEditSpellCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RichEditSpellCheck.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRichEditSpellCheck::CRichEditSpellCheck(CRichEditCtrl& re) : m_re(re)
{
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax = 0;
}

CRichEditSpellCheck::~CRichEditSpellCheck()
{

}

const char* CRichEditSpellCheck::GetFirstWord() const
{
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax = -1;

	return GetNextWord();
}

const char* CRichEditSpellCheck::GetNextWord() const
{
	CHARRANGE cr;

	cr.cpMin = m_re.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, m_crCurrentWord.cpMax + 1);
	cr.cpMax = m_re.SendMessage(EM_FINDWORDBREAK, WB_RIGHTBREAK, cr.cpMin + 1);

	const char* szWord = GetWord(cr);

	if (szWord && *szWord)
		m_crCurrentWord = cr;

	return szWord;
}

const char* CRichEditSpellCheck::GetCurrentWord() const
{
	return GetWord(m_crCurrentWord);
}

const char* CRichEditSpellCheck::GetWord(const CHARRANGE& cr) const
{
	if (cr.cpMax > cr.cpMin)
	{
		static CString sWord;
		
		TEXTRANGE tr;
		tr.chrg = cr;
		tr.lpstrText = sWord.GetBuffer(cr.cpMax - cr.cpMin);

		m_re.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);
		sWord.ReleaseBuffer();

		return sWord;
	}

	// else
	return NULL;
}

void CRichEditSpellCheck::SelectCurrentWord()
{
	m_re.SetSel(m_crCurrentWord);
	
	// need to make sure line is visible
	CPoint ptSel = m_re.GetCharPos(m_crCurrentWord.cpMax);
	
	CRect rClient;
	m_re.GetClientRect(rClient);
	
	if (ptSel.y >= rClient.bottom)
	{
		while (ptSel.y >= rClient.bottom)
		{
			m_re.LineScroll(1);
			ptSel = m_re.GetCharPos(m_crCurrentWord.cpMax);
		}
		
		// one more for good measure
		m_re.LineScroll(1);
	}
	else if (ptSel.y < rClient.top)
	{

		while (ptSel.y < rClient.top)
		{
			m_re.LineScroll(-1);
			ptSel = m_re.GetCharPos(m_crCurrentWord.cpMax);
		}
		
		// one more for good measure
		m_re.LineScroll(-1);
	}
}

void CRichEditSpellCheck::ReplaceCurrentWord(const char* szWord)
{
	m_re.SetSel(m_crCurrentWord);
	m_re.ReplaceSel(szWord, TRUE);
}

void CRichEditSpellCheck::ClearSelection()
{
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax = 0;
	m_re.SetSel(m_crCurrentWord);
}
