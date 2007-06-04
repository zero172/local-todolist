#if !defined(AFX_XMLCHARMAP_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
#define AFX_XMLCHARMAP_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMLCharMap.h: interface and implementation
//
/////////////////////////////////////////////////////////////////////////////////

#include <afxtempl.h>

struct XMLCHARMAPITEM
{
	unsigned char c; // the character required
	LPCTSTR szXMLRep; // XML representation
};
static XMLCHARMAPITEM XMLCHARARRAY[] = 
{
	{ '<', "&lt;" },
	{ '>', "&gt;" },
	{ '\"', "&quot;" },
	{ '&', "&amp;" },
	// smart quotes
	{ 0x91, "&#8216;" },
	{ 0x92, "&#8217;" },
	{ 0x93, "&#8220;" },
	{ 0x94, "&#8221;" },
	// ellipsis
	{ 0x85, "..." },

};

const INT SIZEOFXMLCHARARRAY = sizeof(XMLCHARARRAY) / sizeof(XMLCHARMAPITEM);

class CXmlCharMap
{
public:
	static CString& ConvertFromRep(CString& sText)
	{
		CString sResult;

		// look for '&...;' pairs
		int nStart = sText.Find('&', 0);

		while (nStart != -1)
		{
			int nEnd = sText.Find(';', nStart);

			if (nEnd != -1)
			{
				sResult += sText.Left(nStart);

				CString sRep = sText.Mid(nStart, nEnd - nStart + 1);
				Translate(sRep, &sResult);

				sText = sText.Mid(nEnd + 1);
				nStart = sText.Find('&', 0);
			}
		}

		// add whatevers left
		sResult += sText;

		sText = sResult;
		return sText;
	}

	static CString& ConvertToRep(CString& sText)
	{
		CString sResult;

		for (int nChar = 0; nChar < sText.GetLength(); nChar++)
		{
			unsigned char c = sText[nChar];

			// don't translate multibyte chars
			if (IsDBCSLeadByte(c))
			{
				// add 2 chars to result because they're really
				// a single unicode char
				sResult += sText.Mid(nChar, 2);
				nChar++; // bump pos
			}
			else
				Translate(c, &sResult);
		}

		sText = sResult;
		return sText;
	}

	static int GetConvertToRepLength(const CString& sText)
	{
		int nLen = 0;

		for (int nChar = 0; nChar < sText.GetLength(); nChar++)
			nLen += Translate(sText[nChar], NULL);

		return nLen;
	}

protected:
	static int Translate(LPCTSTR szXMLRep, CString* pAppendTo)
	{
		static CMap<CString, LPCTSTR, unsigned char, unsigned char&> mapXMLRep;

		// init map once only
		if (!mapXMLRep.GetCount())
		{
			int nItem = SIZEOFXMLCHARARRAY;

			while (nItem--)
				mapXMLRep.SetAt(XMLCHARARRAY[nItem].szXMLRep, XMLCHARARRAY[nItem].c);
		}

		unsigned char c = 0;

		if (mapXMLRep.Lookup(szXMLRep, c))
		{
			if (pAppendTo)
				(*pAppendTo) += c;

			return 1;
		}
		else
		{
			if (pAppendTo)
				(*pAppendTo) += szXMLRep;

			return lstrlen(szXMLRep);
		}
	}

	static int Translate(unsigned char c, CString* pAppendTo)
	{
		static CMap<unsigned char, unsigned char, CString, LPCTSTR> mapChar;

		// init map once only
		if (!mapChar.GetCount())
		{
			int nItem = SIZEOFXMLCHARARRAY;

			while (nItem--)
				mapChar.SetAt(XMLCHARARRAY[nItem].c, XMLCHARARRAY[nItem].szXMLRep);
		}

		CString sXMLRep;

		if (mapChar.Lookup(c, sXMLRep))
		{
			if (pAppendTo)
				(*pAppendTo) += sXMLRep;

			return sXMLRep.GetLength();
		}
		else
		{
			if (pAppendTo)
				(*pAppendTo) += c;

			return 1;
		}
	}
};

#endif // AFX_XMLCHARMAP_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_
