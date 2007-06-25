#if !defined(AFX_HTMLCHARMAP_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
#define AFX_HTMLCHARMAP_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// HtmlCharMap.h: interface and implementation
//
// ///////////////////////////////////////////////////////////////////////////////

#include <afxtempl.h>

struct HTMLCHARMAPITEM
{
    unsigned char c; // the character required
    LPCTSTR szHtmlRep; // html representation
};

///
/// Standard ASCII set, HTML Entity names, ISO 10646, ISO 8879, ISO 8859-1 Latin alphabet No. 1
/// Browser support: All browsers (http://www.w3.org/TR/html401/sgml/entities.html)
///
static HTMLCHARMAPITEM HTMLCHARARRAY[] = 
{
    // Special Regular ASCII HTML Characters (under 127)
    { '&', "&amp;"   }, //     ampersand
    { '<', "&lt;"    }, //     less than sign
    { '>', "&gt;"    }, //     greater than sign
    { '\"', "&quot;" }, //     double quotes
    { '\'', "&#39;"  }, //     single quote

    // 128-159 Special Characters not defined in HTML4
    { 150, "&#150;"  }, // '–'
    { 151, "&#151;"  }, // '—'
    { 153, "&#153;"  }, // '˜'

    // 160-255 Special Characters
    { 160, "&nbsp;"  }, // ' ' non-breaking space
    { 161, "&iexcl;" }, // '¡' inverted exclamation mark
    { 162, "&cent;"  }, // '¢' cent sign
    { 163, "&pound;" }, // '£' point sign
    { 164, "&curren;"}, // '¤' currency sing
    { 165, "&yen;"   }, // '¥' yen sign
    { 166, "&brvbar;"}, // '¦' broken vertical bar
    { 167, "&sect;"  }, // '§' section sign
    { 168, "&uml;"   }, // '¨' spacing diaeresis - umlaut
    { 169, "&copy;"  }, // '©' copyright sign
    { 170, "&ordf;"  }, // 'ª' femenine ordinal indicator
    { 171, "&laquo;" }, // '«' left double angle quotes
    { 172, "&not;"   }, // '¬' not sign 
    { 173, "&shy;"   }, // '­' soft hyphen
    { 174, "&reg;"   }, // '®' registered trade mark sign
    { 175, "&macr;"  }, // '¯' spacing macron - overline
    { 176, "%deg;"   }, // '°' degree sign
    { 177, "&plusmn;"}, // '±' plus-or-minus sign
    { 178, "&sup2;"  }, // '²' superscript two - squared
    { 179, "&sup3;"  }, // '³' superscript three - cubed
    { 180, "&acute;" }, // '´' acute accent - spacing acute
    { 181, "&micro;" }, // 'µ' micro sign
    { 182, "&para;"  }, // '¶' pilcrow sign - paragraph sign
    { 183, "&middot;"}, // '·' middle dot
    { 184, "&cedil;" }, // '¸' spacing cedilla
    { 185, "&sup1;"  }, // '¹' superscript one
    { 186, "&ordm;"  }, // 'º' masculine ordinal indicator
    { 187, "&raquo;" }, // '»' right double angle quotes
    { 188, "&frac14;"}, // '¼' fraction one quarter
    { 189, "&frac12;"}, // '½' fraction one half
    { 190, "&frac34;"}, // '¾' fraction three quarters
    { 191, "&iquest;"}, // '¿' inverted question mark
    { 192, "&Agrave;"}, // '?' latin capital letter A with grave
    { 193, "&Aacute;"}, // 'Á' latin capital letter A with acute
    { 194, "&Acirc;" }, // 'Â' latin capital letter A with circumflex
    { 195, "&Atilde;"}, // '?' latin capital letter A with tilde
    { 196, "&Auml;"  }, // 'Ä' latin capital letter A with diaeresis
    { 197, "&Aring;" }, // 'Å' latin capital letter A with ring above
    { 198, "&AElig;" }, // 'Æ' latin capital letter AE
    { 199, "&Ccedil;"}, // 'Ç' latin capital letter C with cedilla
    { 200, "&Egrave;"}, // 'È' latin capital letter E with grave
    { 201, "&Eacute;"}, // 'É' latin capital letter E with acute
    { 202, "&Ecirc;" }, // 'Ê'  latin capital letter E with circumflex
    { 203, "&Euml;"  }, // 'Ë' latin capital letter E with diaeresis
    { 204, "&Igrave;"}, // '?' latin capital letter I with grave
    { 205, "&Iacute;"}, // 'Í' latin capital letter I with acute
    { 206, "&Icirc;" }, // 'Î' latin capital letter I with circumflex
    { 207, "&Iuml;"  }, // 'Ï' latin capital letter I with diaeresis
    { 208, "&ETH;"   }, // '?' latin capital letter ETH
    { 209, "&Ntilde;"}, // 'Ñ' latin capital letter N with tilde
    { 210, "&Ograve;"}, // '?' latin capital letter O with grave
    { 211, "&Oacute;"}, // 'Ó' latin capital letter O with acute
    { 212, "&Ocirc;" }, // 'Ô' latin capital letter O with circumflex
    { 213, "&Otilde;"}, // '?' latin capital letter O with tilde
    { 214, "&Ouml;"  }, // 'Ö' latin capital letter U with diaeresis
    { 215, "&times;" }, // '×' multiplication sign
    { 216, "&Oslash;"}, // 'Ø' latin capital letter O with slash
    { 217, "&Ugrave;"}, // 'Ù' latin capital letter U with grave
    { 218, "&Uacute;"}, // 'Ú' latin capital letter U with acute
    { 219, "&Ucirc;" }, // 'Û' latin capital letter U with circumflex
    { 220, "&Uuml;"  }, // 'Ü' latin capital letter U with diaeresis
    { 221, "&Yacute;"}, // '?' latin capital letter Y with acute
    { 222, "&THORN;" }, // '?' latin capital letter THORN 
    { 223, "&szlig;" }, // 'ß' latin small letter sharp s - ess-zed
    { 224, "&agrave;"}, // 'à' latin small letter a with grave
    { 225, "&aacute;"}, // 'á' latin small letter a with acute
    { 226, "&acirc;" }, // 'â' latin small letter a with circumflex
    { 227, "&atilde;"}, // '?' latin small letter a with tilde
    { 228, "&auml;"  }, // 'ä' latin small letter a with diaeresis
    { 229, "&aring;" }, // 'å' latin small letter a with ring above
    { 230, "&aelig;" }, // 'æ' latin small letter ae
    { 231, "&ccedil;"}, // 'ç' latin small letter c with cedilla
    { 232, "&egrave;"}, // 'è' latin small letter e with grave
    { 233, "&eacute;"}, // 'é' latin small letter e with acute
    { 234, "&ecirc;" }, // 'ê' latin small letter e with circumflex
    { 235, "&euml;"  }, // 'ë' latin small letter e with diaeresis
    { 236, "&igrave;"}, // '?' latin small letter i with grave 
    { 237, "&iacute;"}, // 'í' latin small letter i with acute
    { 238, "&icirc;" }, // 'î' latin small letter i with circumflex
    { 239, "&iuml;"  }, // 'ï' latin small letter i with diaeresis
    { 240, "&eth;"   }, // '?' small eth, Icelandic
    { 241, "&ntilde;"}, // 'ñ' latin small letter n with tilde
    { 242, "&ograve;"}, // '?' latin small letter o with grave
    { 243, "&oacute;"}, // 'ó' latin small letter o with acute
    { 244, "&ocirc;" }, // 'ô' latin small letter o with circumflex
    { 245, "&otilde;"}, // '?' latin small letter o with tilde
    { 246, "&ouml;"  }, // 'ö' latin small letter o with diaeresis
    { 247, "&divide;"}, // '÷' division sign
    { 248, "&oslash;"}, // 'ø' latin small letter o with slash
    { 249, "&ugrave;"}, // 'ú' latin small letter u with acute
    { 250, "&uacute;"}, // 'ú' latin small letter u with acute
    { 251, "&ucirc;" }, // 'û' latin small letter u with circumflex
    { 252, "&uuml;"  }, // 'ü' latin small letter u with diaeresis
    { 253, "&yacute;"}, // '?' latin small letter y with acute
    { 254, "&thorn;" }, // '?' latin small letter thorn
    { 255, "&yuml;"  }, // 'ÿ' latin small letter y with diaeresis

};

const INT SIZEOFHTMLCHARARRAY = sizeof(HTMLCHARARRAY) / sizeof(HTMLCHARMAPITEM);

class CHtmlCharMap
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
                Translate(sRep, sResult);

                sText = sText.Mid(nEnd + 1);
                nStart = sText.Find('&', 0);
            }
            else
                break;
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
				Translate(c, sResult);
		}

        sText = sResult;
        return sText;
    }

protected:
    static void Translate(LPCTSTR szHtmlRep, CString& sAppendTo)
    {
        static CMap<CString, LPCTSTR, unsigned char, unsigned char&> mapHtmlRep;

        // init map once only
        if (!mapHtmlRep.GetCount())
        {
            int nItem = SIZEOFHTMLCHARARRAY;

            while (nItem--)
                mapHtmlRep.SetAt(HTMLCHARARRAY[nItem].szHtmlRep, HTMLCHARARRAY[nItem].c);
        }

        unsigned char c = 0;

        if (mapHtmlRep.Lookup(szHtmlRep, c))
            sAppendTo += c;
        else
            sAppendTo += szHtmlRep;
    }


    static void Translate(unsigned char c, CString& sAppendTo)
    {
        static CMap<unsigned char, unsigned char, CString, LPCTSTR> mapChar;

        // init map once only
        if (!mapChar.GetCount())
        {
            int nItem = SIZEOFHTMLCHARARRAY;

            while (nItem--)
                mapChar.SetAt(HTMLCHARARRAY[nItem].c, HTMLCHARARRAY[nItem].szHtmlRep);
        }

        CString sHtmlRep;

        if (mapChar.Lookup(c, sHtmlRep))
            sAppendTo += sHtmlRep;
        else
            sAppendTo += c;
    }
};


#endif // AFX_HTMLCHARMAP_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_
