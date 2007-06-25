// SysImageList.h: interface for the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_)
#define AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSysImageList  
{
public:
	CSysImageList(BOOL bLargeIcons = FALSE);
	virtual ~CSysImageList();

	const CImageList* GetImageList() const; // temporary. should not be stored
	CImageList* GetImageList(); // temporary. should not be stored
	HIMAGELIST GetHImageList();

	BOOL Initialize();
	int GetFileImageIndex(LPCTSTR szFilePath, BOOL bFailUnKnown = FALSE); // will call Initialize if nec.
	int GetFolderImageIndex(); // will call Initialize if nec.

	// caller is responsible for clean up
	HICON ExtractAppIcon();
	HICON ExtractFileIcon(LPCTSTR szFilePath);
	HICON ExtractFolderIcon();

	static BOOL IsWebAddress(LPCTSTR szFilePath);

protected:
	BOOL m_bLargeIcons;
	int m_nFolderImage, m_nHtmlImage, m_nRemoteFolderImage;
	HIMAGELIST m_hImageList;

protected:
	// raw version that resolves directly to SHGetFileInfo
	int GetImageIndex(LPCTSTR szFile); // will call Initialize if nec.

};

#endif // !defined(AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_)
