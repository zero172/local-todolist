// ToDoList.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ToDoList.h"
#include "ToDoListWnd.h"
#include "PreferencesGenPage.h"
#include "welcomedialog.h"
#include "tdcmsg.h"
#include "tdlprefmigrationdlg.h"

#include "..\shared\LimitSingleInstance.h"
#include "..\shared\encommandlineinfo.h"
#include "..\shared\driveinfo.h"
#include "..\shared\enfiledialog.h"
#include "..\shared\enstring.h"
#include "..\shared\regkey.h"
#include "..\shared\filemisc.h"

#include "..\3rdparty\xmlnodewrapper.h"
#include "..\3rdparty\ini.h"

#include <afxpriv.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLimitSingleInstance g_SingleInstanceObj(_T("{3A4EFC98-9BA9-473D-A3CF-6B0FE644470D}")); 

BOOL CALLBACK FindOtherInstance(HWND hwnd, LPARAM lParam);

const LPCTSTR REGKEY = "AbstractSpoon";
const LPCTSTR APPREGKEY = "Software\\AbstractSpoon\\ToDoList";

const LPCTSTR CONTACTUS = "mailto:abstractspoon2@optusnet.com.au"; 
const LPCTSTR FEEDBACKANDSUPPORT = "http://www.codeproject.com/tools/todolist2.asp"; 
const LPCTSTR LICENSE = "http://creativecommons.org/licenses/by-sa/2.5/"; 
const LPCTSTR ONLINE = "http://www.abstractspoon.com/tdl_resources.html"; 
const LPCTSTR WIKI = "http://abstractspoon.pbwiki.com/"; 
const LPCTSTR DONATE = "https://www.paypal.com/cgi-bin/webscr?cmd=_xclick&business=abstractspoon2%40optusnet%2ecom%2eau&item_name=Software"; 

/////////////////////////////////////////////////////////////////////////////
// CToDoListApp

BEGIN_MESSAGE_MAP(CToDoListApp, CWinApp)
	//{{AFX_MSG_MAP(CToDoListApp)
	ON_COMMAND(ID_HELP_CONTACTUS, OnHelpContactus)
	ON_COMMAND(ID_HELP_FEEDBACKANDSUPPORT, OnHelpFeedbackandsupport)
	ON_COMMAND(ID_HELP_LICENSE, OnHelpLicense)
	ON_COMMAND(ID_HELP_ONLINE, OnHelpOnline)
	ON_COMMAND(ID_HELP_WIKI, OnHelpWiki)
	ON_COMMAND(ID_HELP_COMMANDLINE, OnHelpCommandline)
	ON_COMMAND(ID_HELP_DONATE, OnHelpDonate)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_TOOLS_IMPORTPREFS, OnImportPrefs)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_IMPORTPREFS, OnUpdateImportPrefs)
	ON_COMMAND(ID_TOOLS_EXPORTPREFS, OnExportPrefs)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXPORTPREFS, OnUpdateExportPrefs)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoListApp construction

CToDoListApp::CToDoListApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CToDoListApp object

CToDoListApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CToDoListApp initialization

BOOL CToDoListApp::InitInstance()
{
	AfxOleInit(); // for initializing COM and handling drag and drop via explorer
	AfxEnableControlContainer(); // embedding IE

	// load localized resources
	CEnString sResVersion(IDS_RES_VERSION);
	HINSTANCE hResDll = AfxLoadLibrary("ToDoListLOC.dll");

	if (hResDll)
	{
		// check resource version
		TCHAR szResVer[10];
		::LoadString(hResDll, IDS_RES_VERSION, szResVer, sizeof(szResVer) - 1);

		if (atoi(sResVersion) == atoi(szResVer))
			AfxSetResourceHandle(hResDll);
		else
			AfxMessageBox(IDS_MISMATCHEDRESOURCES);
	}

	CEnCommandLineInfo cmdInfo;
	ParseCommandLine(&cmdInfo);
	
	// see if the user just wants to see the commandline options
	if (cmdInfo.GetOption("h") || cmdInfo.GetOption("help") || 
		cmdInfo.GetOption("?"))
	{
		AfxMessageBox(IDS_COMMANDLINEOPTIONS, MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	// before anything else make sure we've got MSXML3 installed
	if (!CXmlDocumentWrapper::IsVersion3orGreater())
	{
		AfxMessageBox(IDS_BADMSXML);
		return FALSE;
	}

	if (!InitPreferences(&cmdInfo))
		return FALSE;

	// does the user want single instance only
	BOOL bSingleInstance = !CPreferencesGenPage().GetMultiInstance();

	if (bSingleInstance && g_SingleInstanceObj.IsAnotherInstanceRunning())
	{
		HWND hWnd = NULL;
		EnumWindows(FindOtherInstance, (LPARAM)&hWnd);
		
		// make sure its not closing
		if (hWnd && !::SendMessage(hWnd, WM_TDL_ISCLOSING, 0, 0))
		{
			// check version
			int nVer = ::SendMessage(hWnd, WM_TDL_GETVERSION, 0, 0);
			
			if (nVer < CToDoListWnd::GetVersion())
				MessageBox(NULL, CEnString(IDS_EARLIERVERRUNNING), CEnString(IDS_COPYRIGHT), MB_OK);
			
			::SendMessage(hWnd, WM_TDL_SHOWWINDOW, 0, 0);
			
			// pass on file to open
			if (!cmdInfo.m_strFileName.IsEmpty())
				SendDataMessage(hWnd, OPENTASKLIST, cmdInfo.m_strFileName);
			
			::SetForegroundWindow(hWnd);
			
			// new task?
			CString sValue;
			
			if (cmdInfo.GetOption("nt", sValue))
			{
				SendDataMessage(hWnd, ADDNEWTASK, sValue);
				
				if (cmdInfo.GetOption("cm", sValue))
				{
					// replace [\][n] with [\n]
					sValue.Replace("\\n", "\n");
					SendDataMessage(hWnd, SETCOMMENTS, sValue);
				}
			}
			// or select task
			else if (cmdInfo.GetOption("tid", sValue))
			{
				DWORD dwID = atoi(sValue);

				if (dwID)
					SendDataMessage(hWnd, SELECTTASK, dwID);
			}
			// or merge/import
			else if (cmdInfo.GetOption("m", sValue))
			{
				if (!sValue.IsEmpty())
					SendDataMessage(hWnd, IMPORTFILE, sValue);
			}
			
			return FALSE;
		}
	}

	BOOL bForceVisible = cmdInfo.GetOption("v");
	BOOL bPasswordPrompting = !cmdInfo.GetOption("x");

	DWORD dwFlags = (bForceVisible ? TLD_FORCEVISIBLE : 0) |
					(bPasswordPrompting ? TLD_PASSWORDPROMPTING : 0);

	CToDoListWnd* pTDL = new CToDoListWnd;
	
	if (pTDL && pTDL->Create(dwFlags, cmdInfo.m_strFileName))
	{
		HWND hWnd = pTDL->GetSafeHwnd();
		m_pMainWnd = pTDL;
		
		// new task?
		CString sValue;
		
		if (cmdInfo.GetOption("nt", sValue))
		{
			SendDataMessage(hWnd, ADDNEWTASK, sValue);

			if (cmdInfo.GetOption("cm", sValue))
			{
				// replace [\][n] with [\n]
				sValue.Replace("\\n", "\n");
				SendDataMessage(hWnd, SETCOMMENTS, sValue);
			}
		}
		// or select task
		else if (cmdInfo.GetOption("tid", sValue))
		{
			DWORD dwID = atoi(sValue);
			
			if (dwID)
				SendDataMessage(hWnd, SELECTTASK, dwID);
		}
		// or merge/import
		else if (cmdInfo.GetOption("m", sValue))
		{
			if (!sValue.IsEmpty())
				SendDataMessage(hWnd, IMPORTFILE, sValue);
		}

		return TRUE;
	}

	// else
	return FALSE;
}

void CToDoListApp::ParseCommandLine(CEnCommandLineInfo* pInfo)
{
	ASSERT (pInfo);

	CWinApp::ParseCommandLine(*pInfo); // default

	// check for task link
	if (pInfo->m_strFileName.Find(TDL_PROTOCOL) != -1)
	{
		CString sFilePath;
		DWORD dwID = 0;

		CToDoCtrl::ParseTaskLink(pInfo->m_strFileName, dwID, sFilePath);

		if (!sFilePath.IsEmpty() && dwID)
		{
			// replace possible %20 by spaces
			sFilePath.Replace("%20", " ");
			pInfo->m_strFileName = sFilePath;
			pInfo->SetOption("tid", dwID);
		}
	}

	// validate file path
	if (GetFileAttributes(pInfo->m_strFileName) == 0xffffffff)
		pInfo->m_strFileName.Empty();
}

BOOL CToDoListApp::SendDataMessage(HWND hWnd, int nType, int nSize, void* pData)
{
   COPYDATASTRUCT cds;
   
   cds.dwData = nType;
   cds.cbData = nSize;
   cds.lpData = pData;
   
   return SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
}

BOOL CToDoListApp::SendDataMessage(HWND hwnd, int nType, DWORD dwData)
{
   return SendDataMessage(hwnd, nType, sizeof(dwData), (LPVOID)&dwData);
}

BOOL CToDoListApp::SendDataMessage(HWND hwnd, int nType, LPCTSTR szData)
{
   return SendDataMessage(hwnd, nType, strlen(szData) + 1, (LPVOID)szData);
}

BOOL CALLBACK FindOtherInstance(HWND hwnd, LPARAM lParam)
{
	CString sCaption;

	int nLen = ::GetWindowTextLength(hwnd);
	::GetWindowText(hwnd, sCaption.GetBuffer(nLen + 1), nLen + 1);
	sCaption.ReleaseBuffer();

	if (sCaption.Find(CEnString(IDS_COPYRIGHT)) != -1)
	{
		HWND* pWnd = (HWND*)lParam;
		*pWnd = hwnd;
		return FALSE;
	}

	return TRUE;
}

BOOL CToDoListApp::PreTranslateMessage(MSG* pMsg) 
{
	// give first chance to main window for handling accelerators
	if (m_pMainWnd && m_pMainWnd->PreTranslateMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CToDoListApp::OnHelp() 
{ 
	DoHelp();
}

void CToDoListApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	if (nCmd == HELP_CONTEXT)
		DoHelp((LPCTSTR)dwData);
}

void CToDoListApp::DoHelp(const CString& /*sHelpRef*/)
{
	// 1. look for todolist_help.tdl in the registry
	CString sHelpTopic, sHelpFile = GetProfileString("Help", "TDLFile");

	if (!FileMisc::FileExists(sHelpFile))
	{
		// 2. try exe folder
		CString sFolder, sDrive, sHelpPath = FileMisc::GetModuleFileName();

		FileMisc::SplitPath(sHelpPath, &sDrive, &sFolder);
		FileMisc::MakePath(sHelpPath, sDrive, sFolder, "ToDoListDocumentation.tdl");

		// try again
		if (!FileMisc::FileExists(sHelpFile))
		{
			// 3. ask the user
			CEnString sMsg(IDS_LOCATEHELP), sTitle(IDS_LOCATEHELP_TITLE);

			if (MessageBox(*m_pMainWnd, sMsg, sTitle, MB_OKCANCEL) == IDOK)
			{
				CEnString sFilter(IDS_HELPFILEFILTER);
				//fabio_2005
				CEnFileDialog dialog(TRUE, "tdl", sHelpFile, OFN_PATHMUSTEXIST, "Help Files (*.tdl)|*.tdl||");

				dialog.m_ofn.lpstrTitle = (LPCSTR)(LPCTSTR)sTitle;

				if (dialog.DoModal() == IDOK)
					sHelpFile = dialog.GetPathName();
				else
					return;
			}
			else
				return;
		}
	}

	ASSERT (!sHelpFile.IsEmpty());

	if (32 < (int)ShellExecute(*m_pMainWnd, NULL, sHelpFile, NULL, NULL, SW_SHOWNORMAL))
		WriteProfileString("Help", "TDLFile", sHelpFile);

/*
    CString sHelpFile = sHelpRef.IsEmpty() ? 
						"http://www.abstractspoon.com/help.html" :
						"http://www.abstractspoon.com/help_prefs.html";

	ShellExecute(*m_pMainWnd, NULL, sHelpFile, NULL, NULL, SW_SHOWNORMAL);
*/
}

BOOL CToDoListApp::InitPreferences(const CEnCommandLineInfo* pInfo)
{
	BOOL bUseIni = FALSE;

	// get the app file path
	CString sTemp = FileMisc::GetModuleFileName(), sDrive, sFolder, sAppName;
	FileMisc::SplitPath(sTemp, &sDrive, &sFolder, &sAppName);

    // try command line first
    CString sIniPath;

    if (pInfo->GetOption("i", sIniPath) && !sIniPath.IsEmpty())
    {
		// prefix application path if path is relative
		if (PathIsRelative(sIniPath))
			FileMisc::MakePath(sIniPath, sDrive, sFolder, sIniPath);

        bUseIni = FileMisc::FileExists(sIniPath);
    }

	// else if there is a tasklist on the commandline then try for an
	// ini file of the same name
	if (!bUseIni && !pInfo->m_strFileName.IsEmpty())
	{
	    sIniPath = pInfo->m_strFileName;
		FileMisc::ReplaceExtension(sIniPath, "ini");

        bUseIni = FileMisc::FileExists(sIniPath);

		// if we found one then make sure it does not have single 
		// instance specified
		if (bUseIni)
			AfxGetApp()->WriteProfileInt("Preferences", "MultiInstance", TRUE);
	}

    // else try for an ini file having the same name as the executable
	if (!bUseIni)
    {
        // then try current working dir followed by app folder
		FileMisc::MakePath(sIniPath, NULL, FileMisc::GetCwd(), sAppName, ".ini");
		bUseIni = FileMisc::FileExists(sIniPath);

		// followed by app folder
        if (!bUseIni)
		{
			FileMisc::MakePath(sIniPath, sDrive, sFolder, sAppName, ".ini");
    		bUseIni = FileMisc::FileExists(sIniPath);
		}
   }

	// then try registry
	if (!bUseIni)
	{
      BOOL bFirstTime = !CRegKey::KeyExists(HKEY_CURRENT_USER, "Software\\Abstractspoon\\ToDoList");

		if (bFirstTime) // first time
		{
			// don't bother the user if we're being run off removeable media
			if (CDriveInfo::IsRemovablePath(sIniPath))
				bUseIni = TRUE;
			else
			{
				CWelcomeDialog dialog;

				if (dialog.DoModal() == IDOK)
					bUseIni = dialog.GetUseIniFile();
				else
					return FALSE;
			}
		}
	}

	// finally make the choice
	if (bUseIni)
	{
		free((void*)m_pszProfileName);
		m_pszProfileName = _strdup(sIniPath);
	}
	else
		SetRegistryKey(_T(REGKEY));

    UpgradePreferences(bUseIni);

	return TRUE;
}

void CToDoListApp::UpgradePreferences(BOOL bUseIni)
{
	// put later upgrades at top
	// HERE
	
	// unification of file states
	// do splits separately because it uses the unmodified filespec as the
	// value names whereas the others replace backslashes with underscores
	// for use as the key name
	// and as an optimization, if the splitpos key does not exist then 
	// assume we've already done the upgrade
	CTDLPrefMigrationDlg feedbackDlg;
	
	if (RelocateSplitPosSettings(bUseIni, &feedbackDlg))
	{
		CWaitCursor cursor;
		RelocateFileStateSettings("Filter", bUseIni);
		RelocateFileStateSettings("ExpandedState", bUseIni);
		RelocateFileStateSettings("SortState", bUseIni);
	}
}

BOOL CToDoListApp::RelocateSplitPosSettings(BOOL bUseIni, CTDLPrefMigrationDlg* pFeedbackDlg)
{
	if (bUseIni)
	{
		CIni ini(m_pszProfileName);

		if (!ini.IsSectionExist("SplitPos"))
			return FALSE;

		// at this point notify the user that
		// it can take some time to transfer all their settings
		AfxMessageBox(IDS_WARN_CONVERT_INI);

		if (pFeedbackDlg)
			pFeedbackDlg->Create(NULL);

		CStringArray aFileKeys;
		ini.GetKeyNames("SplitPos", &aFileKeys);

		for (int nItem = 0; nItem < aFileKeys.GetSize(); nItem++)
		{
			Misc::ProcessMsgLoop(); // for feedback dialog

			CString sItem = aFileKeys[nItem];
			
			int nSplit = ini.GetInt("SplitPos", sItem, -1);
			
			if (nSplit != -1)
			{
				CString sNewKey, sValueName("SplitPos");
				
				// handle 'Shared' differently
				if (sItem.CompareNoCase("Shared") == 0)
				{
					sNewKey = "FileStates";
					sValueName = "SharedSplitPos";
				}
				else
				{
					sItem.Replace('\\', '_');
					sNewKey.Format("FileStates\\%s", sItem);
				}

				ini.WriteInt(sNewKey, sValueName, nSplit);
			}
		}
		
		// delete old key
		ini.DeleteSection("SplitPos");
	}
	else // registry
	{
		CString sAppKey = CRegKey::GetAppRegPath(), sOldKey;
		sOldKey.Format("%sSplitPos", sAppKey);
		
		if (!CRegKey::KeyExists(HKEY_CURRENT_USER, sOldKey))
			return FALSE;

		CRegKey regOld;
		CStringArray aFileKeys;
		
		regOld.Open(HKEY_CURRENT_USER, sOldKey);
		regOld.GetValueNames(aFileKeys);
		
		for (int nItem = 0; nItem < aFileKeys.GetSize(); nItem++)
		{
			CString sItem = aFileKeys[nItem];
			
			int nSplit = -1;
			regOld.Read(sItem, (DWORD&)nSplit);
			
			if (nSplit != -1)
			{
				CString sNewKey, sValueName("SplitPos");
				
				// handle 'Shared' differently
				if (sItem.CompareNoCase("Shared") == 0)
				{
					sNewKey.Format("%sFileStates", sAppKey);
					sValueName = "SharedSplitPos";
				}
				else
				{
					sItem.Replace('\\', '_');
					sNewKey.Format("%sFileStates\\%s", sAppKey, sItem);
				}
				
				CRegKey regNew;
				
				regNew.Open(HKEY_CURRENT_USER, sNewKey);
				regNew.Write(sValueName, (DWORD)nSplit);
				regNew.Close();
			}
		}
		
		// delete old key
		CRegKey::Delete(HKEY_CURRENT_USER, sOldKey);
	}

	return TRUE;
}

BOOL CToDoListApp::RelocateFileStateSettings(const CString& sSection, BOOL bUseIni)
{
	if (bUseIni)
	{
		CIni ini(m_pszProfileName);

		CString sNewFileKey, sOldFileKey;
		CStringArray aSections;
		ini.GetSectionNames(&aSections);

		for (int nItem = 0; nItem < aSections.GetSize(); nItem++)
		{
			Misc::ProcessMsgLoop(); // for feedback dialog
			
			CString sItem = aSections[nItem];

			// look for match at start of section name
			if (sItem.Find(sSection) == 0 && sItem.GetLength() > sSection.GetLength())
			{
				CString sFileKey = sItem.Mid(sSection.GetLength() + 1); // name + backslash

				if (!sFileKey.IsEmpty())
				{
					sNewFileKey.Format("FileStates\\%s\\%s", sFileKey, sSection);
					
					ini.CopySection(sItem, sNewFileKey, TRUE); // copy over

					Misc::ProcessMsgLoop(); // for feedback dialog

					ini.DeleteSection(sItem); // and delete
				}
			}
		}
		
		// delete old key
		ini.DeleteSection(sSection);
	}
	else // registry
	{
		// check for existence of old key
		CString sAppKey = CRegKey::GetAppRegPath(), sOldKey;
		sOldKey.Format("%s%s", sAppKey, sSection);
		
		if (!CRegKey::KeyExists(HKEY_CURRENT_USER, sOldKey))
			return FALSE;

		CRegKey regOld;
		CString sNewFileKey, sOldFileKey;
		CStringArray aFileKeys;
		
		regOld.Open(HKEY_CURRENT_USER, sOldKey);
		regOld.GetSubkeyNames(aFileKeys);
		regOld.Close();
		
		// copy values to new location
		for (int nItem = 0; nItem < aFileKeys.GetSize(); nItem++)
		{
			CString sItem = aFileKeys[nItem];
			sItem.Replace('\\', '_');
			
			sOldFileKey.Format("%s\\%s", sOldKey, sItem);
			sNewFileKey.Format("%sFileStates\\%s\\%s", sAppKey, sItem, sSection);
			
			CRegKey::CopyKey(HKEY_CURRENT_USER, sOldFileKey, HKEY_CURRENT_USER, sNewFileKey);
		}
		
		// then delete old key
		CRegKey::Delete(HKEY_CURRENT_USER, sOldKey);
	}

	return TRUE;
}

int CToDoListApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt) 
{
	// make sure app window is visible
	if (m_pMainWnd && !m_pMainWnd->IsWindowVisible())
		m_pMainWnd->SendMessage(WM_TDL_SHOWWINDOW, 0, 0);
	
	return CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
}

void CToDoListApp::OnImportPrefs() 
{
	// default location is always app folder
	CString sIniPath = FileMisc::GetModuleFileName();
	sIniPath.MakeLower();
	sIniPath.Replace("exe", "ini");
	
	CEnFileDialog dialog(TRUE, "ini", sIniPath, 
						OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
						CEnString(IDS_INIFILEFILTER));
	
	if (dialog.DoModal() == IDOK)
	{
		CRegKey reg;
		
		if (reg.Open(HKEY_CURRENT_USER, APPREGKEY) == ERROR_SUCCESS)
		{
			sIniPath = dialog.GetPathName();
			
			if (reg.ImportFromIni(sIniPath)) // => import ini to registry
			{
				// use them now?
				// only ask if we're not already using the registry
				BOOL bUsingIni = (m_pszRegistryKey == NULL);

				if (bUsingIni)
				{
					if (AfxMessageBox(IDS_POSTIMPORTPREFS, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						// renames existing prefs file
						CString sNewName(sIniPath);
						sNewName += ".bak";
						
						if (GetFileAttributes(sNewName) != 0xffffffff)
							DeleteFile(sNewName);
						
						if (MoveFile(sIniPath, sNewName))
						{
							// and initialize the registry 
							SetRegistryKey(_T(REGKEY));
							
							// reset prefs
							m_pMainWnd->SendMessage(WM_TDL_REFRESHPREFS);
						}
					}
				}
				else // reset prefs
					m_pMainWnd->SendMessage(WM_TDL_REFRESHPREFS);
			}
			else // notify user
			{
				CEnString sMessage(IDS_INVALIDPREFFILE, dialog.GetFileName());
				AfxMessageBox(sMessage, MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}
}

void CToDoListApp::OnUpdateImportPrefs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CToDoListApp::OnExportPrefs() 
{
	ASSERT (m_pszRegistryKey != NULL);

	CRegKey reg;

	if (reg.Open(HKEY_CURRENT_USER, APPREGKEY) == ERROR_SUCCESS)
	{
		// default location is always app folder
		CString sAppPath = FileMisc::GetModuleFileName();

		CString sIniPath(sAppPath);
		sIniPath.MakeLower();
		sIniPath.Replace("exe", "ini");
		
		CEnFileDialog dialog(FALSE, "ini", sIniPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
							CEnString(IDS_INIFILEFILTER));
		
		if (dialog.DoModal() == IDOK)
		{
			BOOL bUsingReg = (m_pszRegistryKey != NULL);
			sIniPath = dialog.GetPathName();

			if (bUsingReg && reg.ExportToIni(sIniPath))
			{
				// use them now? 
				CString sAppFolder, sIniFolder;
				
				FileMisc::SplitPath(sAppPath, NULL, &sAppFolder);
				FileMisc::SplitPath(sIniPath, NULL, &sIniFolder);
				
				// only if they're in the same folder as the exe
				if (sIniFolder.CompareNoCase(sAppFolder) == 0)
				{
					if (AfxMessageBox(IDS_POSTEXPORTPREFS, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						free((void*)m_pszRegistryKey);
						m_pszRegistryKey = NULL;
						
						free((void*)m_pszProfileName);
						m_pszProfileName = _strdup(sIniPath);
						
						// reset prefs
						m_pMainWnd->SendMessage(WM_TDL_REFRESHPREFS);
					}
				}
			}
		}
	}
}

void CToDoListApp::OnUpdateExportPrefs(CCmdUI* pCmdUI) 
{
	BOOL bUsingReg = (m_pszRegistryKey != NULL);
	pCmdUI->Enable(bUsingReg);
}

void CToDoListApp::OnHelpContactus() 
{
	::ShellExecute(*m_pMainWnd, NULL, CONTACTUS, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpFeedbackandsupport() 
{
	::ShellExecute(*m_pMainWnd, NULL, FEEDBACKANDSUPPORT, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpLicense() 
{
	::ShellExecute(*m_pMainWnd, NULL, LICENSE, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpOnline() 
{
	::ShellExecute(*m_pMainWnd, NULL, ONLINE, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpWiki() 
{
	::ShellExecute(*m_pMainWnd, NULL, WIKI, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpCommandline() 
{
	AfxMessageBox(IDS_COMMANDLINEOPTIONS, MB_OK | MB_ICONINFORMATION);
}

void CToDoListApp::OnHelpDonate() 
{
	::ShellExecute(*m_pMainWnd, NULL, DONATE, NULL, NULL, SW_SHOWNORMAL);
}

int CToDoListApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::ExitInstance();
}
