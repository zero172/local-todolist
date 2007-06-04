// XmlNodeWrapper.h: interface for the CXmlNodeWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLNODEWRAPPER_H__43622334_FDEB_4175_9E6D_19BBAA3992A5__INCLUDED_)
#define AFX_XMLNODEWRAPPER_H__43622334_FDEB_4175_9E6D_19BBAA3992A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "..\todolist\debug\msxml3.tlh"
#import "MSXML3.dll" named_guids 
using namespace MSXML2;

class CXmlNodeWrapper  
{
public:
	CXmlNodeWrapper();
	CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,BOOL bAutoRelease = TRUE);
	void operator=(MSXML2::IXMLDOMNodePtr pNode);
	virtual ~CXmlNodeWrapper();

	CString NodeType();
	int NodeTypeVal();
	CString GetAttribVal(int index);
	CString GetAttribName(int index);
	int NumAttributes();
	void ReplaceNode(MSXML2::IXMLDOMNode* pOldNode,MSXML2::IXMLDOMNode* pNewNode);
	CString GetText();
	void SetText(LPCTSTR text);
	MSXML2::IXMLDOMNode* InsertAfter(MSXML2::IXMLDOMNode *refNode, MSXML2::IXMLDOMNode *pNode);
	CString Name();
	MSXML2::IXMLDOMNodeList* FindNodes(LPCTSTR searchStr);
	MSXML2::IXMLDOMNode* Parent();
	void RemoveNodes(LPCTSTR searchStr);
	MSXML2::IXMLDOMNode* InsertAfter(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* InsertBefore(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* Interface();
	MSXML2::IXMLDOMDocument* ParentDocument();
	CString GetXML();
	MSXML2::IXMLDOMNode* RemoveNode(MSXML2::IXMLDOMNodePtr pNode);
	MSXML2::IXMLDOMNode* InsertNode(int index,LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* InsertNode(int index,MSXML2::IXMLDOMNodePtr pNode);
	MSXML2::IXMLDOMNode* AppendChild(MSXML2::IXMLDOMNodePtr pNode);
	long NumNodes();
	MSXML2::IXMLDOMNode* Detach();
	MSXML2::IXMLDOMNode* GetNode(LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* GetNode(int nodeIndex);
	MSXML2::IXMLDOMNode* FindNode(LPCTSTR searchString);
	MSXML2::IXMLDOMNode* GetPrevSibling();
	MSXML2::IXMLDOMNode* GetNextSibling();
	void SetValue(LPCTSTR valueName,LPCTSTR value);
	void SetValue(LPCTSTR valueName,int value);
	void SetValue(LPCTSTR valueName,short value);
	void SetValue(LPCTSTR valueName,double value);
	void SetValue(LPCTSTR valueName,float value);
	void SetValue(LPCTSTR valueName,bool value);
	BOOL IsValid();
	CString GetValue(LPCTSTR valueName);

private:
	BOOL m_bAutoRelease;
	MSXML2::IXMLDOMNodePtr m_xmlnode;
};

class CXmlDocumentWrapper
{
public:
	CXmlDocumentWrapper(LPCTSTR header = NULL, LPCTSTR szRootItem = NULL);
	CXmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc);
	virtual ~CXmlDocumentWrapper();

	CString GetUrl() const;
	CString GetXML(BOOL bPreserveWhiteSpace = TRUE) const;
	BOOL IsValid() const;
	BOOL Load(LPCTSTR path);
	CString Transform(LPCTSTR pathXSL) const;
	BOOL LoadXML(LPCTSTR xml);
	BOOL Save(LPCTSTR path = "", BOOL bPreserveWhiteSpace = TRUE);
	CString GetHeader(BOOL bAsXml = FALSE) const;
	
	static BOOL IsVersion3orGreater();

	MSXML2::IXMLDOMDocument* Detach();
	MSXML2::IXMLDOMDocument* Clone();
	MSXML2::IXMLDOMNode* AsNode();

protected:
	void SetHeader(LPCTSTR szHeader);

private:
	MSXML2::IXMLDOMDocumentPtr m_xmldoc;
	static BOOL s_bVer3orGreater;
};

class CXmlNodeListWrapper
{
public:
	CXmlNodeListWrapper();
	CXmlNodeListWrapper(MSXML2::IXMLDOMNodeListPtr pList);
	virtual ~CXmlNodeListWrapper();

	MSXML2::IXMLDOMDocument* AsDocument();
	MSXML2::IXMLDOMNode* Node(int index);
	void Start();
	MSXML2::IXMLDOMNode* Next();
	BOOL IsValid();
	int Count();
	void operator=(MSXML2::IXMLDOMNodeListPtr pList);

private:
	MSXML2::IXMLDOMNodeListPtr m_xmlnodelist;
};

#endif // !defined(AFX_XMLNODEWRAPPER_H__43622334_FDEB_4175_9E6D_19BBAA3992A5__INCLUDED_)
