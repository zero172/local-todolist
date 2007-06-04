// XmlNodeWrapper.cpp: implementation of the CXmlNodeWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlNodeWrapper.h"
#include "comutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// fix for bad typedef in <wtypes.h>
#ifdef VARIANT_TRUE
#	undef VARIANT_TRUE
#	define VARIANT_TRUE ((VARIANT_BOOL)(-1))
#endif

const LPCTSTR DEFAULT_HEADER = "version=\"1.0\" encoding=\"windows-1252\"";

CXmlNodeWrapper::CXmlNodeWrapper()
{
}

CXmlNodeWrapper::CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,BOOL bAutoRelease)
{
	m_xmlnode = pNode;
	m_bAutoRelease = bAutoRelease;
}
void CXmlNodeWrapper::operator=(MSXML2::IXMLDOMNodePtr pNode)
{
	if (IsValid())
		m_xmlnode.Release();
	m_xmlnode = pNode;
}

CXmlNodeWrapper::~CXmlNodeWrapper()
{
	if (!m_bAutoRelease)
		m_xmlnode.Detach();
}


CString CXmlNodeWrapper::GetValue(LPCTSTR valueName)
{
	if (!IsValid())
		return "";
	
	MSXML2::IXMLDOMNodePtr attribute = m_xmlnode->Getattributes()->getNamedItem(valueName);
	if (attribute)
	{
		return (LPCSTR)attribute->Gettext();
	}
	return "";
}

BOOL CXmlNodeWrapper::IsValid()
{
	if (m_xmlnode == NULL)
		return FALSE;
	if (m_xmlnode.GetInterfacePtr() == NULL)
		return FALSE;
   
	return TRUE;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetPrevSibling()
{
	if (!IsValid())
		return NULL;
	return m_xmlnode->GetpreviousSibling().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNextSibling()
{
	if (!IsValid())
		return NULL;
	return m_xmlnode->GetnextSibling().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNode(LPCTSTR nodeName)
{
	if (!IsValid())
		return NULL;
	try{
		return m_xmlnode->selectSingleNode(nodeName).Detach();
	}
	catch (_com_error e)
	{
		CString err = e.ErrorMessage();
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNode(int nodeIndex)
{
	if (!IsValid())
		return NULL;
	
	return m_xmlnode->GetchildNodes()->Getitem(nodeIndex).Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::FindNode(LPCTSTR searchString)
{
	if (!IsValid())
		return NULL;

	try
	{
		return m_xmlnode->selectSingleNode(searchString).Detach();
	}
	catch (_com_error e)
	{
		CString err = e.ErrorMessage();
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Detach()
{
	if (IsValid())
		return m_xmlnode.Detach();
	else
		return NULL;
}

long CXmlNodeWrapper::NumNodes()
{
	if (IsValid())
	{
		return m_xmlnode->GetchildNodes()->Getlength();
	}
	else
		return 0;
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,LPCTSTR value)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();

	if (xmlDocument)
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attributes = m_xmlnode->Getattributes();

		if (attributes)
		{
			MSXML2::IXMLDOMAttributePtr attribute = xmlDocument->createAttribute(valueName);
			
			if (attribute)
			{
				attribute->Puttext(value);
				attributes->setNamedItem(attribute);
			}
		}
	}
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,int value)
{
	CString str;
	str.Format("%ld",value);
	SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,short value)
{
	CString str;
	str.Format("%hd",value);
	SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,double value)
{
	CString str;
	str.Format("%f",value);
	SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,float value)
{
	CString str;
	str.Format("%f",value);
	SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,bool value)
{
	CString str;
	if (value)
		str = "True";
	else
		str = "False";
	SetValue(valueName,str);
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::AppendChild(MSXML2::IXMLDOMNodePtr pNode)
{
	return m_xmlnode->appendChild(pNode);
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertNode(int index,LPCTSTR nodeName)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();

	if (xmlDocument)
	{
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName,"");
		MSXML2::IXMLDOMNode* refNode = GetNode(index);
		MSXML2::IXMLDOMNode* pNewNode = NULL;
		
		if (refNode)
			pNewNode = m_xmlnode->insertBefore(newNode.Detach(),refNode);
		else
			pNewNode = m_xmlnode->appendChild(newNode.Detach());
		
		return pNewNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertNode(int index, MSXML2::IXMLDOMNodePtr pNode)
{
	MSXML2::IXMLDOMNodePtr newNode = pNode->cloneNode(VARIANT_TRUE);

	if (newNode)
	{
		MSXML2::IXMLDOMNode* refNode = GetNode(index);
		MSXML2::IXMLDOMNode* pNewNode = NULL;
		
		if (refNode)
			pNewNode = m_xmlnode->insertBefore(newNode.Detach(),refNode);
		else
			pNewNode = m_xmlnode->appendChild(newNode.Detach());
		
		return pNewNode;
	}
	else
		return NULL;
}

CString CXmlNodeWrapper::GetXML()
{
	if (IsValid())
		return (LPCSTR)m_xmlnode->Getxml();
	else
		return "";
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::RemoveNode(MSXML2::IXMLDOMNodePtr pNode)
{
	if (!IsValid())
		return NULL;
	return m_xmlnode->removeChild(pNode).Detach();
}

/* ********************************************************************************************************* */

BOOL CXmlDocumentWrapper::s_bVer3orGreater = -1;

CXmlDocumentWrapper::CXmlDocumentWrapper(LPCTSTR header, LPCTSTR szRootItem)
{
	try
	{
		m_xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);
		m_xmldoc->put_validateOnParse(VARIANT_FALSE);
		m_xmldoc->put_async(VARIANT_FALSE);
		
		if (szRootItem && *szRootItem)
		{
			CString sRoot;
			sRoot.Format("<%s/>", szRootItem);
			LoadXML(sRoot);
		}

	   // set header afterwards so it not overwritten
		if (header && *header)
			SetHeader(header);
	}
	catch (...)
	{
	}
}

CXmlDocumentWrapper::CXmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc)
{
	m_xmldoc = pDoc;
}

CXmlDocumentWrapper::~CXmlDocumentWrapper()
{
}

BOOL CXmlDocumentWrapper::IsValid() const
{
	if (m_xmldoc == NULL)
		return FALSE;

	if (m_xmldoc.GetInterfacePtr() == NULL)
		return FALSE;

	return TRUE;
}

CString CXmlDocumentWrapper::GetHeader(BOOL bAsXml) const
{
	CString sHeader;
	
	if (IsValid())
	{
		CXmlNodeWrapper nodeHdr(m_xmldoc->childNodes->item[0]);
		
		if (nodeHdr.IsValid())
		{
			if (nodeHdr.GetXML().Find("?xml") == 1) // <?xml
			{
				int nAttribs = nodeHdr.NumAttributes();
				
				for (int nAttrib = 0; nAttrib < nAttribs; nAttrib++)
				{
					CString sAttrib;
					sAttrib.Format("%s=\"%s\" ", nodeHdr.GetAttribName(nAttrib),
						nodeHdr.GetAttribVal(nAttrib));
					sHeader += sAttrib;
				}
			}
		}
		
		if (sHeader.IsEmpty())
		{
			sHeader = DEFAULT_HEADER;
			
			// get active code page
			CString sCodePage;
			
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, sCodePage.GetBuffer(7), 6);
			sCodePage.ReleaseBuffer();
			
			// and replace in header
			if (atoi(sCodePage) > 0)
				sHeader.Replace("1252", sCodePage);
		}
		
		if (bAsXml)
		{
			sHeader = "<?xml " + sHeader;
			sHeader += "?>";
		}
	}
	
	return sHeader;
}

void CXmlDocumentWrapper::SetHeader(LPCTSTR szHeader)
{
	if (IsValid())
	{
		_bstr_t name(_com_util_fix::ConvertStringToBSTR("xml"), FALSE);
		_bstr_t bstr(_com_util_fix::ConvertStringToBSTR(szHeader), FALSE);
		
		MSXML2::IXMLDOMProcessingInstructionPtr pHdr = m_xmldoc->createProcessingInstruction(name, bstr);
		
		// always insert header right at the start
		MSXML2::IXMLDOMNodePtr pNode = m_xmldoc->childNodes->item[0];

      if (pNode)
      {
         CString sXml = (LPCTSTR)pNode->Getxml();
         _variant_t varFirst(pNode.GetInterfacePtr());

         m_xmldoc->insertBefore(pHdr, varFirst);
      }
      else
		m_xmldoc->appendChild(pHdr);
	}
}

MSXML2::IXMLDOMDocument* CXmlDocumentWrapper::Detach()
{
	if (!IsValid())
		return NULL;
	return m_xmldoc.Detach();
}

MSXML2::IXMLDOMDocument* CXmlDocumentWrapper::Clone()
{
	if (!IsValid())
		return NULL;
	MSXML2::IXMLDOMDocumentPtr xmldoc;
	xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);

	_variant_t v(xmldoc.GetInterfacePtr());
	m_xmldoc->save(v);

	return xmldoc.Detach();
}

BOOL CXmlDocumentWrapper::Load(LPCTSTR path)
{
	if (!IsValid())
		return FALSE;
	
	_bstr_t bstr(_com_util_fix::ConvertStringToBSTR(path), FALSE);
	m_xmldoc->put_async(VARIANT_FALSE);
	
	return (VARIANT_TRUE == m_xmldoc->load(bstr));
}

BOOL CXmlDocumentWrapper::LoadXML(LPCTSTR xml)
{
	if (!IsValid())
		return FALSE;
	
	_bstr_t bstr(_com_util_fix::ConvertStringToBSTR(xml), FALSE); 
	
	return (VARIANT_TRUE == m_xmldoc->loadXML(bstr));
}

BOOL CXmlDocumentWrapper::Save(LPCTSTR path, BOOL bPreserveWhiteSpace)
{
	if (!IsValid())
		return FALSE;
	
	try
	{
		_bstr_t bPath(_com_util_fix::ConvertStringToBSTR(path), FALSE);
		
		if (bPath.length() == 0)
			bPath = m_xmldoc->Geturl();
		
		m_xmldoc->put_preserveWhiteSpace(bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE);
		
		return (VARIANT_TRUE == m_xmldoc->save(bPath));
	}
	catch(...)
	{
		return FALSE;
	}
}

CString CXmlDocumentWrapper::Transform(LPCTSTR pathXSL) const
{
	CXmlDocumentWrapper xsl;
	
    try
    {
        if (xsl.Load(pathXSL))
            return (LPSTR)m_xmldoc->transformNode(xsl.AsNode());
    }
    catch( const _com_error & err)
    {
        AfxMessageBox(err.ErrorMessage(), MB_OK | MB_ICONERROR);
    }

    // else
    return "";
}

MSXML2::IXMLDOMNode* CXmlDocumentWrapper::AsNode()
{
	if (!IsValid())
		return NULL;

	return m_xmldoc->GetdocumentElement().Detach();
}

CString CXmlDocumentWrapper::GetXML(BOOL bPreserveWhiteSpace) const
{
	CString sXml;
	
	if (IsValid())
	{
		m_xmldoc->put_preserveWhiteSpace(bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE);
		sXml = (LPCSTR)m_xmldoc->Getxml();
		
      // Getxml doesn't return entire header so we need to fix it up here
      CString sHeader = GetHeader(TRUE);
      int nStart = sXml.Find("?xml");
		
      if (nStart > 0)
      {
         // find start brace
         while (sXml[nStart] != '<')
            nStart--;

         // find end brace
         int nEnd = sXml.Find(">") + 1; // closing brace

         sXml = sXml.Left(nStart) + sHeader + sXml.Mid(nEnd);
      }
      else // just add header
         sXml = sHeader + sXml;
	}
	
	return sXml;
}

CString CXmlDocumentWrapper::GetUrl() const
{
	return (LPSTR)m_xmldoc->Geturl();
}

BOOL CXmlDocumentWrapper::IsVersion3orGreater()
{
	if (s_bVer3orGreater == -1)
	{
		try
		{
			// try to load some dummy xml
			CXmlDocumentWrapper doc;
			CString sXml;

			sXml.Format("<?xml %s ?><NodeList/>", DEFAULT_HEADER);
			s_bVer3orGreater = doc.LoadXML(sXml);
		}
		catch (CException* /*e*/)
		{
			s_bVer3orGreater = FALSE;
		}
	}

	return s_bVer3orGreater;
}

MSXML2::IXMLDOMDocument* CXmlNodeWrapper::ParentDocument()
{
	return m_xmlnode->GetownerDocument().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Interface()
{
	if (IsValid())
		return m_xmlnode;
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertBefore(MSXML2::IXMLDOMNode *refNode, LPCTSTR nodeName)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName,"");
		MSXML2::IXMLDOMNode* pNode = m_xmlnode->insertBefore(newNode.Detach(),(IUnknown*)refNode);
		
		return pNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertAfter(MSXML2::IXMLDOMNode *refNode, LPCTSTR nodeName)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName,"");
		MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
		MSXML2::IXMLDOMNode* pNode = NULL;
		
		if (nextNode.GetInterfacePtr() != NULL)
			pNode = m_xmlnode->insertBefore(newNode.Detach(),(IUnknown*)nextNode);
		else
			pNode = m_xmlnode->appendChild(newNode.Detach());
		
		return pNode;
	}
	return NULL;
}

void CXmlNodeWrapper::RemoveNodes(LPCTSTR searchStr)
{
	if (!IsValid())
		return;

	MSXML2::IXMLDOMNodeListPtr nodeList = m_xmlnode->selectNodes(searchStr);
	for (int i = 0; i < nodeList->Getlength(); i++)
	{
		try
		{
			MSXML2::IXMLDOMNode* pNode = nodeList->Getitem(i).Detach();
			pNode->GetparentNode()->removeChild(pNode);
		}
		catch (_com_error er)
		{
			AfxMessageBox(er.ErrorMessage());
		}
	}
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Parent()
{
	if (IsValid())
		return m_xmlnode->GetparentNode().Detach();

	return NULL;
}

CXmlNodeListWrapper::CXmlNodeListWrapper()
{
}
CXmlNodeListWrapper::CXmlNodeListWrapper(MSXML2::IXMLDOMNodeListPtr pList)
{
	m_xmlnodelist = pList;
}

void CXmlNodeListWrapper::operator=(MSXML2::IXMLDOMNodeListPtr pList)
{
	if (IsValid())
		m_xmlnodelist.Release();
	
	m_xmlnodelist = pList;
}

CXmlNodeListWrapper::~CXmlNodeListWrapper()
{
	
}

int CXmlNodeListWrapper::Count()
{
	if (IsValid())
		return m_xmlnodelist->Getlength();
	else
		return 0;
	
}

BOOL CXmlNodeListWrapper::IsValid()
{
	if (m_xmlnodelist == NULL)
		return FALSE;

	if (m_xmlnodelist.GetInterfacePtr() == NULL)

		return FALSE;
	return TRUE;
}

MSXML2::IXMLDOMNode* CXmlNodeListWrapper::Next()
{
	if (IsValid())
		return m_xmlnodelist->nextNode().Detach();
	else
		return NULL;
}

void CXmlNodeListWrapper::Start()
{
	if (IsValid())
		m_xmlnodelist->reset();
}

MSXML2::IXMLDOMNode* CXmlNodeListWrapper::Node(int index)
{
	if (IsValid())
		return m_xmlnodelist->Getitem(index).Detach();
	else
		return NULL;
}

MSXML2::IXMLDOMDocument* CXmlNodeListWrapper::AsDocument()
{
	if (IsValid())
	{
		CXmlDocumentWrapper doc;
		doc.LoadXML("<NodeList></NodeList>");
		
		CXmlNodeWrapper root(doc.AsNode());
		
		for (int i = 0; i < m_xmlnodelist->Getlength(); i++)
		{
			root.InsertNode(root.NumNodes(),m_xmlnodelist->Getitem(i)->cloneNode(VARIANT_TRUE));
		}
		return doc.Detach();
	}
	else 
		return NULL;
}

MSXML2::IXMLDOMNodeList* CXmlNodeWrapper::FindNodes(LPCTSTR searchStr)
{
	if(IsValid())
	{
		try
		{
			return m_xmlnode->selectNodes(searchStr).Detach();
		}
		catch (_com_error e)
		{
			CString err = e.ErrorMessage();
			return NULL;
		}
	}
	else
		return NULL;
}

CString CXmlNodeWrapper::Name()
{
	if (IsValid())
		return (LPCSTR)m_xmlnode->GetbaseName();
	return "";
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertAfter(MSXML2::IXMLDOMNode *refNode, MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
	MSXML2::IXMLDOMNodePtr newNode;
	// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
	MSXML2::IXMLDOMNodePtr pNewNode = NULL;

	if (nextNode.GetInterfacePtr() != NULL)
		pNewNode = m_xmlnode->insertBefore(pNode,(IUnknown*)nextNode);
	else
		pNewNode = m_xmlnode->appendChild(pNode);

	return pNewNode;
}

void CXmlNodeWrapper::SetText(LPCTSTR text)
{
	if (IsValid())
		m_xmlnode->Puttext(text);
}

CString CXmlNodeWrapper::GetText()
{
	if (IsValid())
		return (LPCSTR)m_xmlnode->Gettext();
	else
		return "";
}

void CXmlNodeWrapper::ReplaceNode(MSXML2::IXMLDOMNode *pOldNode, MSXML2::IXMLDOMNode *pNewNode)
{
	if (IsValid())
	{
		m_xmlnode->replaceChild(pNewNode,pOldNode);
	}
}

int CXmlNodeWrapper::NumAttributes()
{
	if (IsValid())
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();
		
		if (attribs)
			return attribs->Getlength();
	}
	
	return 0;
}

CString CXmlNodeWrapper::GetAttribName(int index)
{
	CString sName;
	
	if (IsValid())
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

		if (attribs)
		{
			MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);

			if (attrib)
			{
				sName = (LPCSTR)attrib->Getname();
				attrib.Release();
			}
			
			attribs.Release();
		}
	}
	
	return sName;
}

CString CXmlNodeWrapper::GetAttribVal(int index)
{
	CString sVal;
	
	if (IsValid())
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

		if (attribs)
		{
			MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);

			if (attrib)
			{
				sVal = (LPCSTR)attrib->Gettext();
				attrib.Release();
			}
			
			attribs.Release();
		}
	}
	
	
	return sVal;
}

CString CXmlNodeWrapper::NodeType()
{
	if (IsValid())
		return (LPCSTR)m_xmlnode->GetnodeTypeString();

	return "";
}

int CXmlNodeWrapper::NodeTypeVal()
{
	if (IsValid())
		return (int)m_xmlnode->GetnodeType();

	// Valik - Fully qualify the name of NODE_INVALID to prevent an ambiguous symbol error (C2872) in VC 7.1
	return MSXML2::NODE_INVALID;
}
