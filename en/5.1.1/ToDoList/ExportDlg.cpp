// ExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "todolist.h"
#include "ExportDlg.h"

#include "..\shared\filemisc.h"
#include "..\shared\enstring.h"
#include "..\shared\dialoghelper.h"

#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

enum { ACTIVETASKLIST, ALLTASKLISTS };

int CExportDlg::s_nFormatOption = 0;

CExportDlg::CExportDlg(const CImportExportMgr& mgr, BOOL bSingleTaskList, LPCTSTR szFilePath, 
					   LPCTSTR szFolderPath, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EXPORT_DIALOG, pParent), 
	  m_mgrImportExport(mgr),
	  m_bSingleTaskList(bSingleTaskList), 
	  m_sFilePath(szFilePath),
	  m_sFolderPath(szFolderPath), 
	  m_taskSel("Exporting"),
	  m_eExportPath(FES_COMBOSTYLEBTN | FES_SAVEAS)
{
	//{{AFX_DATA_INIT(CExportDlg)
	m_sPathLabel = _T("");
	//}}AFX_DATA_INIT
	m_bExportOneFile = FALSE;

	if (m_bSingleTaskList)
		m_nExportOption = ACTIVETASKLIST;
	else
		m_nExportOption = AfxGetApp()->GetProfileInt("Exporting", "ExportOption", ACTIVETASKLIST);

	if (m_sFolderPath.IsEmpty())
		m_sFolderPath = AfxGetApp()->GetProfileString("Exporting", "LastFolder");

//	if (m_bSingleTaskList || m_nExportOption == ACTIVETASKLIST)
//		m_bExportOneFile = TRUE;
//	else 
//		m_bExportOneFile = AfxGetApp()->GetProfileInt("Exporting", "ExportOneFile", FALSE);

	if (m_bSingleTaskList || m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
	{
		m_sExportPath = m_sFilePath; // default

		if ((m_sFilePath.IsEmpty() || PathIsRelative(m_sFilePath)) && 
			!m_sFolderPath.IsEmpty())
		{
			char szFName[_MAX_FNAME];
			_splitpath(m_sFilePath, NULL, NULL, szFName, NULL);

			// handle empty filename
			if (!szFName[0])
			{
				CEnString sUntitled(IDS_TDC_UNTITLEDFILE);
				strcpy(szFName, sUntitled);
			}

			_makepath(m_sExportPath.GetBuffer(MAX_PATH), NULL, m_sFolderPath, szFName, NULL);
			m_sExportPath.ReleaseBuffer();
		}
				
		ReplaceExtension(m_sExportPath, s_nFormatOption);
		m_sPathLabel.LoadString(IDS_ED_FILEPATH);
	}
	else // multiple files
	{
		if (!m_sFolderPath.IsEmpty())
			m_sExportPath = m_sFolderPath;

		else if (!m_sFilePath.IsEmpty())
		{
			char szDrive[_MAX_DRIVE], szPath[MAX_PATH];
			_splitpath(m_sFilePath, szDrive, szPath, NULL, NULL);
			_makepath(m_sExportPath.GetBuffer(MAX_PATH), szDrive, szPath, NULL, NULL);
			m_sExportPath.ReleaseBuffer();
		}

		m_sPathLabel.LoadString(IDS_ED_FOLDER);
	}
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Control(pDX, IDC_FORMATOPTIONS, m_cbFormat);
	DDX_Control(pDX, IDC_EXPORTPATH, m_eExportPath);
	DDX_CBIndex(pDX, IDC_TASKLISTOPTIONS, m_nExportOption);
	DDX_Text(pDX, IDC_EXPORTPATH, m_sExportPath);
	DDX_Check(pDX, IDC_EXPORTONEFILE, m_bExportOneFile);
	DDX_Text(pDX, IDC_PATHLABEL, m_sPathLabel);
	DDX_CBIndex(pDX, IDC_FORMATOPTIONS, s_nFormatOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_CBN_SELCHANGE(IDC_TASKLISTOPTIONS, OnSelchangeTasklistoptions)
	ON_CBN_SELCHANGE(IDC_FORMATOPTIONS, OnSelchangeFormatoptions)
	ON_BN_CLICKED(IDC_EXPORTONEFILE, OnExportonefile)
	ON_EN_CHANGE(IDC_EXPORTPATH, OnChangeExportpath)
	//}}AFX_MSG_MAP
   ON_REGISTERED_MESSAGE(WM_TASKSELDLG_CHANGE, OnChangeTaskSelOption)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers

BOOL CExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    VERIFY(m_taskSel.Create(IDC_FRAME, this));
	
	// set initial control states
	GetDlgItem(IDC_TASKLISTOPTIONS)->EnableWindow(!m_bSingleTaskList);
	GetDlgItem(IDC_EXPORTONEFILE)->EnableWindow(!m_bSingleTaskList && m_nExportOption == ALLTASKLISTS);

	m_eExportPath.SetFilter(m_mgrImportExport.GetExporterFileFilter(s_nFormatOption));
	m_eExportPath.EnableStyle(FES_FOLDERS, (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile));

	// build the format comboxbox
	for (int nExp = 0; nExp < m_mgrImportExport.GetNumExporters(); nExp++)
		m_cbFormat.AddString(m_mgrImportExport.GetExporterMenuText(nExp));

	m_cbFormat.SetCurSel(s_nFormatOption);
	m_eExportPath.EnableWindow(m_mgrImportExport.ExporterHasFileExtension(s_nFormatOption));

	EnableOK();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportDlg::OnSelchangeTasklistoptions() 
{
	UpdateData();

	// can't do all tasklists and selected item
	if (m_taskSel.GetWantSelectedTasks() && m_nExportOption == ALLTASKLISTS)
      m_taskSel.SetWantSelectedTasks(FALSE);
	
	// only process this notification if something's _really_ changed
	if (!(m_nExportOption == ALLTASKLISTS && m_bExportOneFile))
	{
		BOOL bFolder = (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile);

		m_eExportPath.EnableStyle(FES_FOLDERS, bFolder);
		m_sPathLabel.LoadString(bFolder ? IDS_ED_FOLDER : IDS_ED_FILEPATH);

		// check file extension is correct
		if (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile)
		{
			m_sExportPath = m_sFolderPath;
		}
		else
		{
			m_sExportPath = m_sFilePath;
			ReplaceExtension(m_sExportPath, s_nFormatOption);
		}
	}

	GetDlgItem(IDC_EXPORTONEFILE)->EnableWindow(!m_bSingleTaskList && m_nExportOption == ALLTASKLISTS);

	UpdateData(FALSE);
}

void CExportDlg::OnSelchangeFormatoptions() 
{
	UpdateData();

	// check exporter has a file extension
	if (m_mgrImportExport.ExporterHasFileExtension(s_nFormatOption))
	{
		// enable path edit
		m_eExportPath.EnableWindow(TRUE);

		// check file extension is correct
		m_eExportPath.SetFilter(m_mgrImportExport.GetExporterFileFilter(s_nFormatOption));

		if ((m_nExportOption == ACTIVETASKLIST || m_bExportOneFile) && !m_sExportPath.IsEmpty())
		{
			ReplaceExtension(m_sExportPath, s_nFormatOption);
			UpdateData(FALSE);
		}
	}
	else // disable path edit
		m_eExportPath.EnableWindow(FALSE);

}

void CExportDlg::ReplaceExtension(CString& sPathName, int nFormat)
{
	if (!m_mgrImportExport.ExporterHasFileExtension(nFormat))
		return;
	
	char szDrive[_MAX_DRIVE], szFolder[MAX_PATH], szFName[_MAX_FNAME];

	CString sExt = m_mgrImportExport.GetExporterFileExtension(nFormat);

	sPathName.TrimRight();

	int nFind = sPathName.Find(sExt);

	if (nFind != sPathName.GetLength() - sExt.GetLength())
	{
		_splitpath(sPathName, szDrive, szFolder, szFName, NULL);
		_makepath(sPathName.GetBuffer(MAX_PATH), szDrive, szFolder, szFName, sExt);
	}

	sPathName.ReleaseBuffer();
}

void CExportDlg::OnOK()
{
	BOOL bExporterHasFileExt = m_mgrImportExport.ExporterHasFileExtension(s_nFormatOption);

	if (bExporterHasFileExt)
	{
		m_sExportPath.TrimLeft();
		m_sExportPath.TrimRight();

		// if the export path is relative we build a path based
		// on the exe path and check with the user
		if (::PathIsRelative(m_sExportPath))
		{
			char szPath[MAX_PATH], szDrive[_MAX_DRIVE], szFolder[MAX_PATH];

			::GetModuleFileName(NULL, szPath, MAX_PATH);

			_splitpath(szPath, szDrive, szFolder, NULL, NULL);
			_makepath(szPath, szDrive, szFolder, m_sExportPath, NULL);

			CString sMessage;
			
			if (m_nExportOption == ALLTASKLISTS)
				sMessage.Format(IDS_ED_CONFIRMEXPORTPATHMULTI, szPath);
			else
				sMessage.Format(IDS_ED_CONFIRMEXPORTPATH, szPath);
							
			UINT nRet = MessageBox(sMessage, CEnString(IDS_ED_CONFIRMEXPORTPATH_TITLE), MB_YESNO);

			if (nRet == IDNO)
			{
				// re-display dialog
				m_eExportPath.SetSel(0, -1);
				m_eExportPath.SetFocus();
				return;
			}
			else
				m_sExportPath = szPath;
		}

		// make sure the output folder is valid
		BOOL bBadFolder = (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile) ? 
							!FileMisc::CreateFolder(m_sExportPath) : 
							!FileMisc::CreateFolderFromFilePath(m_sExportPath);

		if (bBadFolder)
		{
			CEnString sMessage(IDS_ED_NOMAKEEXPORTPATH, m_sExportPath);
			
			UINT nRet = MessageBox(sMessage, CEnString(IDS_ED_NOMAKEEXPORTPATH_TITLE), MB_OKCANCEL);

			// re-display dialog
			if (nRet == IDOK)
			{
				m_eExportPath.SetSel(0, -1);
				m_eExportPath.SetFocus();
				return;
			}
			else
			{
				EndDialog(IDCANCEL);
				return;
			}
		}
	}

	CDialog::OnOK();

	// make sure extension is right
	if (bExporterHasFileExt)
	{
		if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
			ReplaceExtension(m_sExportPath, s_nFormatOption);
	}

	if (!m_bSingleTaskList)
	{
		AfxGetApp()->WriteProfileInt("Exporting", "ExportOption", m_nExportOption);
//		AfxGetApp()->WriteProfileInt("Exporting", "ExportOneFile", m_bExportOneFile);

		if (bExporterHasFileExt)
		{
			if (m_nExportOption == ALLTASKLISTS)
				AfxGetApp()->WriteProfileString("Exporting", "LastFolder", m_sExportPath);
			else
				AfxGetApp()->WriteProfileString("Exporting", "LastFolder", m_sFolderPath);
		}
	}
}

BOOL CExportDlg::GetExportAllTasklists()
{
	return (!m_bSingleTaskList && m_nExportOption == ALLTASKLISTS);
}

void CExportDlg::OnExportonefile() 
{
	UpdateData();

	m_eExportPath.EnableStyle(FES_FOLDERS, (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile));

	if (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile)
		m_sExportPath = m_sFolderPath;
	else
		m_sExportPath = m_sFilePath;

	if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
		ReplaceExtension(m_sExportPath, s_nFormatOption);

	UpdateData(FALSE);
}

void CExportDlg::EnableOK()
{
	m_sExportPath.TrimLeft();
	m_sExportPath.TrimRight();
	
	GetDlgItem(IDOK)->EnableWindow(!m_sExportPath.IsEmpty());
}

void CExportDlg::OnChangeExportpath() 
{
	UpdateData();
	EnableOK();

	if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
		m_sFilePath = m_sExportPath;
	else
		m_sFolderPath = m_sExportPath;
}

LRESULT CExportDlg::OnChangeTaskSelOption(WPARAM /*wp*/, LPARAM /*lp*/) 
{
	if (m_taskSel.GetWantSelectedTasks() && m_nExportOption == ALLTASKLISTS)
	{
		m_nExportOption = ACTIVETASKLIST;
		UpdateData(FALSE);

		OnSelchangeTasklistoptions();
	}
	
   return 0;
}

CString CExportDlg::GetExportPath()
{
	if (m_mgrImportExport.ExporterHasFileExtension(s_nFormatOption))
		return m_sExportPath;

	// else
	return "";
}
