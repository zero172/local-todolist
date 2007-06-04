#include "stdafx.h"
#include "filemisc.h"
#include "misc.h"

#include <sys/utime.h>
#include <sys/stat.h>
#include <direct.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

CFileBackup::CFileBackup(const CString& sFile, const CString& sExt)
{
	MakeBackup(sFile, sExt);
}

CFileBackup::~CFileBackup()
{
	if (FileMisc::FileExists(m_sBackup))
		::DeleteFile(m_sBackup);
}

BOOL CFileBackup::MakeBackup(const CString& sFile, const CString& sExt)
{
	ASSERT (m_sFile.IsEmpty() && m_sBackup.IsEmpty());

	if (!m_sFile.IsEmpty() || !m_sBackup.IsEmpty())
		return FALSE;

	if (!FileMisc::FileExists(sFile))
		return FALSE;

	m_sFile = sFile;
	m_sBackup = BuildBackupPath(sFile, sExt);

	return ::CopyFile(m_sFile, m_sBackup, FALSE);
}

BOOL CFileBackup::RestoreBackup()
{
	ASSERT (!m_sFile.IsEmpty() && !m_sBackup.IsEmpty());

	if (m_sFile.IsEmpty() || m_sBackup.IsEmpty())
		return FALSE;

	return ::CopyFile(m_sBackup, m_sFile, FALSE);
}

CString CFileBackup::BuildBackupPath(const CString& sFile, const CString& sExt)
{
	CString sBackup(sFile);

	sBackup.TrimRight();

	// add extension
	if (sExt.IsEmpty())
		sBackup += ".bak";
	else
	{
		if (sExt.Find('.') == -1)
			sBackup += '.';

		sBackup += sExt;
	}

	return sBackup;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FileMisc::TerminatePath(CString& sPath)
{
	sPath.TrimRight();

	if (sPath.ReverseFind('\\') != (sPath.GetLength() - 1))
		sPath += '\\';
}

void FileMisc::UnterminatePath(CString& sPath)
{
	sPath.TrimRight();

	int len = sPath.GetLength();

	if (sPath.ReverseFind('\\') == (len - 1))
		sPath = sPath.Left(len - 1);
}

void FileMisc::ReplaceExtension(CString& sFilePath, const char* szExt)
{
	CString sDrive, sDir, sFile;

	SplitPath(sFilePath, &sDrive, &sDir, &sFile);
	MakePath(sFilePath, sDrive, sDir, sFile, szExt);
}

CString& FileMisc::ValidateFilepath(CString& sFilepath)
{
	sFilepath.TrimLeft();
	sFilepath.TrimRight();

	sFilepath.Replace("/", "");
	sFilepath.Replace("*", "");
	sFilepath.Replace("?", "");
	sFilepath.Replace("\"", "");
	sFilepath.Replace("<", "");
	sFilepath.Replace(">", "");
	sFilepath.Replace("|", "");

	// make sure if a colon exists it is the 2nd pos
	int nColon = sFilepath.Find(':');

	while (nColon != -1 && nColon != 1)
	{
		// delete the colon
		sFilepath = sFilepath.Left(nColon) + sFilepath.Mid(nColon + 1);
		nColon = sFilepath.Find(':', nColon);
	}

	return sFilepath;
}

CString& FileMisc::ValidateFilename(CString& sFilename)
{
	sFilename.Replace("\\", "");
	sFilename.Replace(":", "");
	
	return ValidateFilepath(sFilename);
}

const char* FileMisc::GetFileNameFromPath(const char* szFilepath)
{
	const char* szFilename = strrchr(szFilepath, '\\');

	if (szFilename)
		return szFilename + 1;

	// else
	return szFilepath;
}

time_t FileMisc::GetLastModified(const char* szPath)
{
	struct _stat st;

	if (!szPath || _stat(szPath, &st) != 0)
		return 0;

	// files only
	if ((st.st_mode & _S_IFDIR) == _S_IFDIR)
		return 0;

	return st.st_mtime;
}

bool FileMisc::GetLastModified(const char* szPath, SYSTEMTIME& sysTime, bool bLocalTime)
{
	ZeroMemory(&sysTime, sizeof(SYSTEMTIME));

	DWORD dwAttr = ::GetFileAttributes(szPath);

	// files only
	if (dwAttr == 0xFFFFFFFF)
		return false;

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)szPath, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	FindClose(hFind);

	FILETIME ft = findFileData.ftLastWriteTime;

	if (bLocalTime)
		FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &ft);

	FileTimeToSystemTime(&ft, &sysTime);
	return true;
}

bool FileMisc::ResetLastModified(const char* szPath)
{
	::SetFileAttributes(szPath, FILE_ATTRIBUTE_NORMAL);

	return (_utime(szPath, NULL) == 0);
}

bool FileMisc::DeleteFolderContents(const char* szFolder, BOOL bIncludeSubFolders, const char* szFileMask, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	// if the dir does not exists just return
	if (!FolderExists(szFolder))
		return true;

	// if a file mask has been specified with subfolders we need to do 2 passes on each folder, 
	// one for the files and one for the sub folders
	int nPasses = (bIncludeSubFolders && (szFileMask && lstrlen(szFileMask))) ? 2 : 1;
		
	bool bResult = true;
	bool bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);

	for (int nPass = 0; !bStopped && nPass < nPasses; nPass++)
	{
		CString sSearchSpec(szFolder), sMask(szFileMask);

		if (sMask.IsEmpty() || nPass == 1) // (nPass == 1) == 2nd pass (for folders)
			sMask = "*.*";

		TerminatePath(sSearchSpec);
		sSearchSpec += sMask;

		WIN32_FIND_DATA finfo;
		HANDLE hSearch = NULL;

		if ((hSearch = FindFirstFile(sSearchSpec, &finfo)) != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if (bProcessMsgLoop)
					Misc::ProcessMsgLoop();

				if (finfo.cFileName[0] != '.') 
				{
					CString sItem(szFolder);
					sItem += "\\";
					sItem += finfo.cFileName;

					if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bIncludeSubFolders && (nPass == 1 || nPasses == 1))
						{
							if (DeleteFolderContents(sItem, TRUE, szFileMask, hTerminate, bProcessMsgLoop))
							{
								if (!szFileMask || !lstrlen(szFileMask))
									bResult = (RemoveDirectory(sItem) == TRUE);
							}
						}
					}
					else 
						bResult = (DeleteFile(sItem) == TRUE);
				}

				bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
			} 
			while (!bStopped && bResult && FindNextFile(hSearch, &finfo));
			
			FindClose(hSearch);
		}
	}

	return (!bStopped && bResult);
}

bool FileMisc::RemoveFolder(const char* szFolder, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	// if the dir does not exists just return
	if (!FolderExists(szFolder))
		return true;

	if (DeleteFolderContents(szFolder, TRUE, NULL, hTerminate, bProcessMsgLoop))
	{
		::SetFileAttributes(szFolder, FILE_ATTRIBUTE_NORMAL);
		return (RemoveDirectory(szFolder) == TRUE);
	}

	return false;
}

double FileMisc::GetFolderSize(const char* szFolder, BOOL bIncludeSubFolders, const char* szFileMask, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	// if the dir does not exists just return
	if (!FolderExists(szFolder))
		return 0;
	
	double dSize = 0;

	WIN32_FIND_DATA finfo;
	CString sSearchSpec(szFolder), sFileMask(szFileMask);

	if (sFileMask.IsEmpty())
		sFileMask = "*.*";

	TerminatePath(sSearchSpec);
	sSearchSpec += sFileMask;

	BOOL bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
	HANDLE h = NULL;
		
	if (!bStopped && (h = FindFirstFile(sSearchSpec, &finfo)) != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if (bProcessMsgLoop)
				Misc::ProcessMsgLoop();

			if (finfo.cFileName[0] != '.') 
			{
				if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bIncludeSubFolders)
					{
						CString sSubFolder(szFolder);
						sSubFolder += "\\";
						sSubFolder += finfo.cFileName;
						
						dSize += GetFolderSize(sSubFolder, TRUE, sFileMask, hTerminate, bProcessMsgLoop);
					}
				}
				else 
					dSize += (finfo.nFileSizeHigh * ((double)MAXDWORD + 1)) + finfo.nFileSizeLow;
			}

			bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
		}
		while (!bStopped && FindNextFile(h, &finfo));
		
		FindClose(h);
	} 

	return bStopped ? -1 : dSize;
}

bool FileMisc::FolderExists(const char* szFolder)
{
	DWORD dwAttrib = GetFileAttributes(szFolder);

	return (dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileMisc::FileExists(const char* szFile)
{
	DWORD dwAttrib = GetFileAttributes(szFile);

	return (dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileMisc::FolderFromFilePathExists(const char* szFilePath)
{
	return FolderExists(GetFolderFromFilePath(szFilePath));
}

CString FileMisc::GetCwd()
{
	char szCwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCwd);

	return szCwd;
}

CString FileMisc::GetFolderFromFilePath(const char* szFilePath)
{
	CString sFolder;

	// check if its a folder already
	if (FolderExists(szFilePath))
	{
		sFolder = szFilePath;
	}
	else
	{
		// remove file ending
		CString sDrive, sDir;

		SplitPath(szFilePath, &sDrive, &sDir);
		MakePath(sFolder, sDrive, sDir);
	}

	return sFolder;
}

bool FileMisc::CreateFolderFromFilePath(const char* szFilePath)
{
	return CreateFolder(GetFolderFromFilePath(szFilePath));
}

bool FileMisc::PathHasWildcard(const char* szFilePath)
{
	return (strchr(szFilePath, '?') || strchr(szFilePath, '*'));
}

bool FileMisc::CreateFolder(const char* szFolder)
{
	if (FolderExists(szFolder))
		return true;

	// start from the highest level folder working to the lowest
	CString sFolder, sRemaining(szFolder);
	UnterminatePath(sRemaining);

	bool bDone = false;
	bool bResult = true;

	// pull off the :\ or \\ start
	int nFind = sRemaining.Find(":\\");

	if (nFind != -1)
	{
		sFolder += sRemaining.Left(nFind + 2);
		sRemaining = sRemaining.Mid(nFind + 2);
	}
	else
	{
		nFind = sRemaining.Find("\\\\");
		
		if (nFind != -1)
		{
			sFolder += sRemaining.Left(nFind + 2);
			sRemaining = sRemaining.Mid(nFind + 2);
		}
	}

	while (!bDone && bResult)
	{
		nFind = sRemaining.Find('\\', 1);

		if (nFind == -1)
		{
			bDone = TRUE;
			sFolder += sRemaining;
		}
		else
		{
			sFolder += sRemaining.Left(nFind);
			sRemaining = sRemaining.Mid(nFind);
		}

		if (GetFileAttributes(sFolder) == 0xffffffff && _mkdir(sFolder) != 0)
			bResult = false;
	}

	return bResult;
}

bool FileMisc::MoveFolder(const char* szSrcFolder, const char* szDestFolder, BOOL bIncludeSubFolders, const char* szFileMask, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	if (CopyFolder(szSrcFolder, szDestFolder, bIncludeSubFolders, szFileMask, hTerminate, bProcessMsgLoop))
	{
		// don't pass on hTerminate to ensure the operation completes
		DeleteFolderContents(szSrcFolder, bIncludeSubFolders, szFileMask, NULL, bProcessMsgLoop);

		return true;
	}

	return false;
}

bool FileMisc::CopyFolder(const char* szSrcFolder, const char* szDestFolder, BOOL bIncludeSubFolders, const char* szFileMask, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	if (!CreateFolder(szDestFolder))
		return false;

	if (!FolderExists(szSrcFolder))
		return false;

	// if a file mask has been specified with subfolders we need to do 2 passes on each folder, 
	// one for the files and one for the sub folders
	int nPasses = (bIncludeSubFolders && (szFileMask && lstrlen(szFileMask))) ? 2 : 1;
		
	bool bResult = true;
	bool bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);

	for (int nPass = 0; !bStopped && nPass < nPasses; nPass++)
	{
		CString sSearchSpec(szSrcFolder), sMask(szFileMask);

		if (sMask.IsEmpty() || nPass == 1) // (nPass == 1) == 2nd pass (for folders)
			sMask = "*.*";

		TerminatePath(sSearchSpec);
		sSearchSpec += sMask;

		WIN32_FIND_DATA finfo;
		HANDLE hSearch = NULL;

		if ((hSearch = FindFirstFile(sSearchSpec, &finfo)) != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if (bProcessMsgLoop)
					Misc::ProcessMsgLoop();

				if (finfo.cFileName[0] != '.') 
				{
					CString sSource(szSrcFolder);
					sSource += "\\";
					sSource += finfo.cFileName;
					
					CString sDest(szDestFolder);
					sDest += "\\";
					sDest += finfo.cFileName;
					
					if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if ((nPass == 1 || nPasses == 1) && bIncludeSubFolders)
							bResult = CopyFolder(sSource, sDest, hTerminate);
					}
					else if (nPass == 0) // files 
					{
						bResult = (TRUE == CopyFile(sSource, sDest, FALSE));
					}
				}

				bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
			}
			while (!bStopped && bResult && FindNextFile(hSearch, &finfo));
			
			FindClose(hSearch);
		} 
	}

	return (!bStopped && bResult);
}

bool FileMisc::MoveFolder(const char* szSrcFolder, const char* szDestFolder, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	return MoveFolder(szSrcFolder, szDestFolder, TRUE, NULL, hTerminate, bProcessMsgLoop);
}

bool FileMisc::CopyFolder(const char* szSrcFolder, const char* szDestFolder, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	return CopyFolder(szSrcFolder, szDestFolder, TRUE, NULL, hTerminate, bProcessMsgLoop);
}

double FileMisc::GetFileSize(const char* szPath)
{
	HANDLE hFile = ::CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwHighSize = 0;
		DWORD dwLowSize = ::GetFileSize(hFile, &dwHighSize);
		
		::CloseHandle(hFile);
		
		if (dwLowSize != INVALID_FILE_SIZE)
		{
			return (dwHighSize * ((double)MAXDWORD + 1) + dwLowSize);
		}
	}

	// else
	return 0;
}

bool FileMisc::AppendLineToFile(LPCTSTR szPathname, LPCTSTR szLine)
{
	// make sure parent folder exists
	if (!CreateFolderFromFilePath(szPathname))
		return false;

	CStdioFile file;

	if (file.Open(szPathname, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		file.SeekToEnd();
		file.WriteString(szLine);

		if (!strstr(szLine, "\n"))
			file.WriteString("\n");

		return true;
	}

	return false;
}

bool FileMisc::SaveFile(const char* szPathname, const char* szText, int nLen)
{
	if (nLen == -1)
		nLen = strlen(szText);

	CStdioFile file;

	if (file.Open(szPathname, CFile::modeCreate | CFile::modeWrite))
	{
		file.Write(szText, nLen);
		return true;
	}

	return false;
}

bool FileMisc::LoadFile(const char* szPathname, CString& sText)
{
	CStdioFile file;

	if (file.Open(szPathname, CFile::modeRead))
	{
		if (file.GetLength())
		{
			// init size
			CString sTemp('\0', (int)file.GetLength());
			sText = sTemp;

			CString sLine;
			int nPos = 0;

			while (file.ReadString(sLine))
			{
				sText.Insert(nPos, sLine);
				nPos += sLine.GetLength();
			}
		}

		return true;
	}

	return false;
}

CString FileMisc::GetTempFileName(LPCTSTR szPrefix, UINT uUnique)
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
	
	if (::GetTempPath(MAX_PATH, szTempPath))
	{
		if (::GetTempFileName(szTempPath, szPrefix, uUnique, szTempFile))
			return szTempFile;
	}

	return "";
}

CString FileMisc::GetTempFileName(LPCTSTR szFilename, LPCTSTR szExt)
{
	CString sTempFile;
	char szTempPath[MAX_PATH];
	
	if (::GetTempPath(MAX_PATH, szTempPath))
		MakePath(sTempFile, NULL, szTempPath, szFilename, szExt);

	return sTempFile;
}

DWORD FileMisc::Run(HWND hwnd, LPCSTR lpFile, LPCSTR lpDirectory, int nShowCmd)
{
	DWORD dwRes = (DWORD)ShellExecute(hwnd, NULL, lpFile, NULL, lpDirectory, nShowCmd);

	if (dwRes <= 32) // failure
	{
		// try CreateProcess
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.wShowWindow = (WORD)nShowCmd;
		si.dwFlags = STARTF_USESHOWWINDOW;

		ZeroMemory( &pi, sizeof(pi) );

		// Start the child process.
		if (CreateProcess( NULL,			// No module name (use command line).
							(char*)lpFile,	// Command line.
							NULL,			// Process handle not inheritable.
							NULL,			// Thread handle not inheritable.
							FALSE,			// Set handle inheritance to FALSE.
							0,				// No creation flags.
							NULL,			// Use parent's environment block.
							lpDirectory,	// starting directory.
							&si,			// Pointer to STARTUPINFO structure.
							&pi ))			// Pointer to PROCESS_INFORMATION structure.
		{
			dwRes = 32; // success
		}

		// Close process and thread handles.
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
	return dwRes;
}

bool FileMisc::ExtractResource(UINT nID, LPCTSTR szType, const CString& sTempFilePath, HINSTANCE hInst)
{
	// compare time with that of module from which it was loaded
	CString sTempPath;
	CFileStatus fsRes, fsModule;
	CString sModulePath = GetModuleFileName(hInst);

	if (!CFile::GetStatus(sModulePath, fsModule))
		return FALSE;
	
	// see if the file has been created before
	if (!CFile::GetStatus(sTempFilePath, fsRes) || fsRes.m_mtime < fsModule.m_mtime)
	{
		// Load the resource into memory
		HRSRC hRes = FindResource(hInst, (LPCSTR)nID, szType);
		
		if (!hRes) 
		{
			TRACE("Couldn't find %s resource %d!\n", szType, nID);
			return FALSE;
		}
		
		DWORD len = SizeofResource(hInst, hRes);
		
		BYTE* lpRes = (BYTE*)LoadResource(hInst, hRes);
		ASSERT(lpRes);
		
		CFile file;
		
		if (file.Open(sTempFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			file.Write(lpRes, len);
			file.Close();
			FreeResource((HANDLE)lpRes);
		}
		else
		{
			FreeResource((HANDLE)lpRes);
			return FALSE;
		}
	}
	
	return TRUE;
}

CString FileMisc::GetModuleFileName(HMODULE hMod)
{
	CString sModulePath;

	::GetModuleFileName(hMod, sModulePath.GetBuffer(MAX_PATH + 1), MAX_PATH);
	sModulePath.ReleaseBuffer();

	return sModulePath;
}

bool FileMisc::ExtractResource(LPCTSTR szModulePath, UINT nID, LPCTSTR szType, const CString& sTempFilePath)
{
	HMODULE hModule = LoadLibrary(szModulePath);

	if (!hModule)
		return false;

	// else
	return ExtractResource(nID, szType, sTempFilePath, hModule);
}

void FileMisc::SplitPath(const char* szPath, CString* pDrive, CString* pDir, CString* pFName, CString* pExt)
{
	char szDrive[_MAX_DRIVE], szFolder[_MAX_DIR], szFile[_MAX_FNAME], szExt[_MAX_EXT];

#if _MSC_VER >= 1400
	_splitpath_s(szPath, szDrive,_MAX_DRIVE, szFolder,_MAX_DIR, szFile, _MAX_FNAME, szExt, _MAX_EXT);
#else
	_splitpath(szPath, szDrive, szFolder, szFile, szExt);
#endif

	if (pDrive)
		*pDrive = szDrive;

	if (pDir)
		*pDir = szFolder;

	if (pFName)
		*pFName = szFile;

	if (pExt)
		*pExt = szExt;
}

CString& FileMisc::MakePath(CString& sPath, const char* szDrive, const char* szDir, const char* szFName, const char* szExt)
{
	char szPath[MAX_PATH];

#if _MSC_VER >= 1400
	_makepath_s(szPath, MAX_PATH, szDrive, szDir, szFName, szExt);
#else
	_makepath(szPath, szDrive, szDir, szFName, szExt);
#endif

	sPath = szPath;

	return sPath;
}
