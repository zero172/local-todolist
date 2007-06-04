#ifndef _MISCFILE_FUNCTIONS_H_
#define _MISCFILE_FUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>

class CFileBackup
{
public:
	CFileBackup(const CString& sFile = "", const CString& sExt = ".bak");
	~CFileBackup();

	BOOL MakeBackup(const CString& sFile, const CString& sExt = ".bak");
	BOOL RestoreBackup();

	static CString BuildBackupPath(const CString& sFile, const CString& sExt = ".bak");

protected:
	CString m_sFile, m_sBackup;
};

namespace FileMisc
{
	void TerminatePath(CString& sPath);
	void UnterminatePath(CString& sPath);

	time_t GetLastModified(const char* szPath);
	bool GetLastModified(const char* szPath, SYSTEMTIME& sysTime, bool bLocalTime = true); // files only
	bool ResetLastModified(const char* szPath); // resets to current time
	double GetFileSize(const char* szPath);

	bool RemoveFolder(const char* szFolder, 
					  HANDLE hTerminate = NULL, 
					  BOOL bProcessMsgLoop = TRUE);

	bool DeleteFolderContents(const char* szFolder, 
							  BOOL bIncludeSubFolders, 
							  const char* szFileMask, 
							  HANDLE hTerminate = NULL, 
							  BOOL bProcessMsgLoop = TRUE);

	double GetFolderSize(const char* szFolder, 
						 BOOL bIncludeSubFolders = TRUE, 
						 const char* szFileMask = NULL, 
						 HANDLE hTerminate = NULL, 
						 BOOL bProcessMsgLoop = TRUE);

	bool CreateFolder(const char* szFolder);
	bool CreateFolderFromFilePath(const char* szFilePath);
	bool FolderExists(const char* szFolder);
	bool FileExists(const char* szFile);
	bool FolderFromFilePathExists(const char* szFilePath);
	bool PathHasWildcard(const char* szFilePath);

	CString GetCwd();

	CString& ValidateFilename(CString& sFilename);
	CString& ValidateFilepath(CString& sFilepath);
	void ReplaceExtension(CString& szFilePath, const char* szExt);


	CString GetTempFileName(LPCTSTR szPrefix, UINT uUnique = 0);
	CString GetTempFileName(LPCTSTR szFilename, LPCTSTR szExt);
	bool SaveFile(const char* szPathname, const char* szText, int nLen = -1);
	bool LoadFile(const char* szPathname, CString& sText);

	bool ExtractResource(UINT nID, LPCTSTR szType, const CString& sTempFilePath, HINSTANCE hInst = NULL);
	bool ExtractResource(LPCTSTR szModulePath, UINT nID, LPCTSTR szType, const CString& sTempFilePath);

	CString GetModuleFileName(HMODULE hMod = NULL);
	void SplitPath(const char* szPath, CString* pDrive, CString* pDir = NULL, CString* pFName = NULL, CString* pExt = NULL);
	CString& MakePath(CString& sPath, const char* szDrive, const char* szDir = NULL, const char* szFName = NULL, const char* szExt = NULL);

	CString GetFolderFromFilePath(const char* szFilePath);
	const char* GetFileNameFromPath(const char* szFilepath);

	// will delete the source folder on success
	bool MoveFolder(const char* szSrcFolder, 
					const char* szDestFolder, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	bool CopyFolder(const char* szSrcFolder, 
					const char* szDestFolder, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	// will delete the source folder only if file mask was "*.*"
	bool MoveFolder(const char* szSrcFolder, 
					const char* szDestFolder, 
					BOOL bIncludeSubFolders, 
					const char* szFileMask, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	bool CopyFolder(const char* szSrcFolder, 
					const char* szDestFolder, 
					BOOL bIncludeSubFolders, 
					const char* szFileMask, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	// append a line of text to a text file
	bool AppendLineToFile(LPCTSTR szPathname, LPCTSTR szLine);

	DWORD Run(HWND hwnd, LPCSTR lpFile, LPCSTR lpDirectory = NULL, int nShowCmd = SW_SHOW);

}

#endif // _MISCFILE_FUNCTIONS_H_