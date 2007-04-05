#ifndef _MISCFILE_FUNCTIONS_H_
#define _MISCFILE_FUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>

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
	bool FolderFromFilePathExists(const char* szFilePath);
	bool PathHasWildcard(const char* szFilePath);

	CString& ValidateFilename(CString& sFilename);
	CString& ValidateFilepath(CString& sFilepath);
	void ReplaceExtension(char szFilePath[MAX_PATH + 1], const char* szExt);

	const char* FileNameFromPath(const char* szFilepath);

	CString GetTempFileName(LPCTSTR szPrefix, UINT uUnique = 0);
	CString GetTempFileName(LPCTSTR szFilename, LPCTSTR szExt);
	bool SaveFile(const char* szPathname, const char* szText, int nLen = -1);
	bool LoadFile(const char* szPathname, CString& sText);

	bool GetFolderFromFilePath(const char* szFilePath, CString& sFolder);

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