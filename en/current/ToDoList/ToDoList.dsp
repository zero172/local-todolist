# Microsoft Developer Studio Project File - Name="ToDoList" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TODOLIST - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ToDoList.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ToDoList.mak" CFG="TODOLIST - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ToDoList - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ToDoList - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ToDoList - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zd /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "NO_DIAGRAM_TEMPLATE" /D "NO_XML_TEMPLATE" /D "USE_TRANSTEXT" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /map /machine:I386 /MAPINFO:LINES /MAPINFO:EXPORTS
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ToDoList - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_DIAGRAM_TEMPLATE" /D "NO_XML_TEMPLATE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ToDoList - Win32 Release"
# Name "ToDoList - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\autocombobox.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\AutoFlag.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Base64Coder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Shared\browserdlg.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\checkcombobox.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\colorbutton.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\colorcombobox.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\ContentCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ContentMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DateHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\deferWndMove.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DlgUnits.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DockManager.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\DragDrop.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\driveinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\EnBitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\EnBitmapEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\enbrowserctrl.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\encolordialog.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\EnCommandLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\enedit.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\EnRecentFileList.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\enstatic.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\EnString.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\entoolbar.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=.\ExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\filedialogex.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\fileedit.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\FILEMISC.CPP
# End Source File
# Begin Source File

SOURCE=..\Shared\FileRegister.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterBar.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FilteredToDoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FindBlankPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FindCategoryPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDatePage.cpp
# End Source File
# Begin Source File

SOURCE=.\FindNumPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FindPriorityPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FindRiskPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTextPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\fontcombobox.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupLine.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\GUI.CPP
# End Source File
# Begin Source File

SOURCE=..\Shared\hotkeyctrlex.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\HotTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ImageProcessors.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ImportExportMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Ini.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyboardShortcutDisplayDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\LockableHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\MASKEDIT.CPP
# End Source File
# Begin Source File

SOURCE=..\Shared\MenuEx.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\MenuIconMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeToDoList.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Misc.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\monthcombobox.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\MouseWheelMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\NcGutter.cpp
# End Source File
# Begin Source File

SOURCE=.\OffsetDatesDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\olericheditctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\OrderedTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\OSVersion.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\passworddialog.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=.\PreferencesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesExportPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesFilePage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesGenPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesMultiUserPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesShortcutsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesTaskDefPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesTaskPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesToolPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesUIPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesUITasklistColorsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesUITasklistPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\PropertyPageHost.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RCCtrlParser.cpp
# End Source File
# Begin Source File

SOURCE=.\RecurringTaskEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Regkey.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RichEditHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RichEditNcBorder.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RuntimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ShortcutManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SpellCheckDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\StatLink.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\Subclass.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SysImageList.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\tabbedcombobox.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=..\Shared\tabctrlex.cpp
# ADD CPP /I "..\ToDoList"
# End Source File
# Begin Source File

SOURCE=.\TaskFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskListCsvExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskListDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskListHtmlExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskListTxtExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskSelectionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskTimeLog.cpp
# End Source File
# Begin Source File

SOURCE=.\TDColumnListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TDImportExportMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TDLCategoryComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TDLContentMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TDLPrintDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TDLPriorityComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TDLRiskComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TDLTransformDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Themed.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TimeEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TimeHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\todocommentsctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ToDoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ToDoCtrlData.cpp
# End Source File
# Begin Source File

SOURCE=.\ToDoCtrlMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ToDoList.cpp
# End Source File
# Begin Source File

SOURCE=.\ToDoList.rc
# End Source File
# Begin Source File

SOURCE=.\ToDoListWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ToolbarHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolsCmdlineParser.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolsHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolsUserInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TRAYICON.CPP
# End Source File
# Begin Source File

SOURCE=..\Shared\TreeCtrlHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TreeDragDropHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TreeSelectionHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\urlricheditctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\webbrowserctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\WelcomeDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WinClasses.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WndPrompt.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\XHTMLStatic.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\XmlFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\xmlfileex.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\XmlNodeWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\XNamedColors.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Shared\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\autocombobox.h
# End Source File
# Begin Source File

SOURCE=..\Shared\AutoFlag.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Base64Coder.h
# End Source File
# Begin Source File

SOURCE=..\Shared\browserdlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\checkcombobox.h
# End Source File
# Begin Source File

SOURCE=..\Shared\colorbutton.h
# End Source File
# Begin Source File

SOURCE=..\Shared\colorcombobox.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ColorDef.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\COMUTIL.H
# End Source File
# Begin Source File

SOURCE=..\Shared\ContentCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ContentMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DateHelper.h
# End Source File
# Begin Source File

SOURCE=..\Shared\deferWndMove.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogHelper.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DlgUnits.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DockManager.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\DragDrop.h
# End Source File
# Begin Source File

SOURCE=..\Shared\driveinfo.h
# End Source File
# Begin Source File

SOURCE=..\Shared\EnBitmap.h
# End Source File
# Begin Source File

SOURCE=..\Shared\EnBitmapEx.h
# End Source File
# Begin Source File

SOURCE=..\Shared\enbrowserctrl.h
# End Source File
# Begin Source File

SOURCE=..\Shared\encolordialog.h
# End Source File
# Begin Source File

SOURCE=..\Shared\EnCommandLineInfo.h
# End Source File
# Begin Source File

SOURCE=..\Shared\enedit.h
# End Source File
# Begin Source File

SOURCE=..\Shared\EnRecentFileList.h
# End Source File
# Begin Source File

SOURCE=..\Shared\enstatic.h
# End Source File
# Begin Source File

SOURCE=..\Shared\EnString.h
# End Source File
# Begin Source File

SOURCE=..\Shared\entoolbar.h
# End Source File
# Begin Source File

SOURCE=.\ExportDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\filedialogex.h
# End Source File
# Begin Source File

SOURCE=..\Shared\fileedit.h
# End Source File
# Begin Source File

SOURCE=..\Shared\FILEMISC.H
# End Source File
# Begin Source File

SOURCE=..\Shared\FileRegister.h
# End Source File
# Begin Source File

SOURCE=.\FilterBar.h
# End Source File
# Begin Source File

SOURCE=.\FilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\FilteredToDoCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FindBlankPage.h
# End Source File
# Begin Source File

SOURCE=.\FindCategoryPage.h
# End Source File
# Begin Source File

SOURCE=.\FindDatePage.h
# End Source File
# Begin Source File

SOURCE=.\FindNumPage.h
# End Source File
# Begin Source File

SOURCE=.\FindPriorityPage.h
# End Source File
# Begin Source File

SOURCE=.\FindRiskPage.h
# End Source File
# Begin Source File

SOURCE=.\FindTaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\FindTextPage.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\FIXALLOC.H
# End Source File
# Begin Source File

SOURCE=..\Shared\FolderDialog.h
# End Source File
# Begin Source File

SOURCE=..\Shared\fontcombobox.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupLine.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\GUI.H
# End Source File
# Begin Source File

SOURCE=..\Shared\HoldRedraw.h
# End Source File
# Begin Source File

SOURCE=..\Shared\HookMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\hotkeyctrlex.h
# End Source File
# Begin Source File

SOURCE=..\Shared\HotTracker.h
# End Source File
# Begin Source File

SOURCE=..\Shared\HtmlCharMap.h
# End Source File
# Begin Source File

SOURCE=..\Shared\IContentControl.h
# End Source File
# Begin Source File

SOURCE=..\Shared\IEncryption.h
# End Source File
# Begin Source File

SOURCE=..\Shared\IImportExport.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ImageProcessors.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ImportExportMgr.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Ini.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ISpellCheck.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ITaskList.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ITransText.h
# End Source File
# Begin Source File

SOURCE=.\KeyboardShortcutDisplayDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\LimitSingleInstance.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\LockableHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Shared\MASKEDIT.H
# End Source File
# Begin Source File

SOURCE=..\Shared\MenuEx.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\MenuIconMgr.h
# End Source File
# Begin Source File

SOURCE=.\MergeToDoList.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Misc.h
# End Source File
# Begin Source File

SOURCE=..\Shared\monthcombobox.h
# End Source File
# Begin Source File

SOURCE=..\Shared\MouseWheelMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\NcGutter.h
# End Source File
# Begin Source File

SOURCE=.\OffsetDatesDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\olericheditctrl.h
# End Source File
# Begin Source File

SOURCE=..\Shared\OrderedTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Shared\OSVersion.h
# End Source File
# Begin Source File

SOURCE=..\Shared\passworddialog.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesDlg.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesExportPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesFilePage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesGenPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesMultiUserPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesShortcutsPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesTaskDefPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesTaskPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesToolPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesUIPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesUITasklistColorsPage.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesUITasklistPage.h
# End Source File
# Begin Source File

SOURCE=..\Shared\PropertyPageHost.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RCCtrlParser.h
# End Source File
# Begin Source File

SOURCE=.\RecurringTaskEdit.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Regkey.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RichEditHelper.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RichEditNcBorder.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RuntimeDlg.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Schemadef.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ShortcutManager.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SpellCheckDlg.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\StatLink.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Shared\stringres.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Subclass.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SysImageList.h
# End Source File
# Begin Source File

SOURCE=..\Shared\tabbedcombobox.h
# End Source File
# Begin Source File

SOURCE=..\Shared\tabctrlex.h
# End Source File
# Begin Source File

SOURCE=.\TaskFile.h
# End Source File
# Begin Source File

SOURCE=.\TaskListCsvExporter.h
# End Source File
# Begin Source File

SOURCE=.\TaskListDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\TaskListHtmlExporter.h
# End Source File
# Begin Source File

SOURCE=.\TaskListTxtExporter.h
# End Source File
# Begin Source File

SOURCE=.\TaskSelectionDlg.h
# End Source File
# Begin Source File

SOURCE=.\TaskTimeLog.h
# End Source File
# Begin Source File

SOURCE=.\tdcenum.h
# End Source File
# Begin Source File

SOURCE=.\tdcmsg.h
# End Source File
# Begin Source File

SOURCE=.\TDColumnListBox.h
# End Source File
# Begin Source File

SOURCE=.\tdcstruct.h
# End Source File
# Begin Source File

SOURCE=.\TDImportExportMgr.h
# End Source File
# Begin Source File

SOURCE=.\TDLCategoryComboBox.h
# End Source File
# Begin Source File

SOURCE=.\TDLContentMgr.h
# End Source File
# Begin Source File

SOURCE=.\TDLPrintDialog.h
# End Source File
# Begin Source File

SOURCE=.\TDLPriorityComboBox.h
# End Source File
# Begin Source File

SOURCE=.\TDLRiskComboBox.h
# End Source File
# Begin Source File

SOURCE=.\tdlschemadef.h
# End Source File
# Begin Source File

SOURCE=.\TDLTransformDialog.h
# End Source File
# Begin Source File

SOURCE=.\tdstringres.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Themed.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TimeEdit.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TimeHelper.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Tmschema.h
# End Source File
# Begin Source File

SOURCE=.\todocommentsctrl.h
# End Source File
# Begin Source File

SOURCE=.\ToDoCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ToDoCtrlData.h
# End Source File
# Begin Source File

SOURCE=.\ToDoCtrlMgr.h
# End Source File
# Begin Source File

SOURCE=.\ToDoList.h
# End Source File
# Begin Source File

SOURCE=.\ToDoListWnd.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\TOM.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ToolbarHelper.h
# End Source File
# Begin Source File

SOURCE=.\ToolsCmdlineParser.h
# End Source File
# Begin Source File

SOURCE=.\ToolsHelper.h
# End Source File
# Begin Source File

SOURCE=.\ToolsUserInputDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TRAYICON.H
# End Source File
# Begin Source File

SOURCE=..\Shared\TreeCtrlHelper.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TreeDragDropHelper.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TreeSelectionHelper.h
# End Source File
# Begin Source File

SOURCE=..\Shared\urlricheditctrl.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\Uxtheme.h
# End Source File
# Begin Source File

SOURCE=..\Shared\wclassdefines.h
# End Source File
# Begin Source File

SOURCE=..\Shared\webbrowserctrl.h
# End Source File
# Begin Source File

SOURCE=.\WelcomeDialog.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WinClasses.h
# End Source File
# Begin Source File

SOURCE=..\Shared\winstyles.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WndPrompt.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\XHTMLStatic.h
# End Source File
# Begin Source File

SOURCE=..\Shared\XmlCharMap.h
# End Source File
# Begin Source File

SOURCE=..\Shared\XmlFile.h
# End Source File
# Begin Source File

SOURCE=..\Shared\xmlfileex.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\XmlNodeWrapper.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\XNamedColors.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\cc_somerights.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\checklistbox.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\close.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\find.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\findtoolbar16.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\findtoolbar24.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mainfram.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mainfrm.ico
# End Source File
# Begin Source File

SOURCE=.\res\ToDoList.ico
# End Source File
# Begin Source File

SOURCE=.\res\ToDoList.rc2
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\toolbar16.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\toolbar24.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\trayicon.ico
# End Source File
# Begin Source File

SOURCE=.\RES\XPcheckboxes.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\license.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\RES\todolist.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\todolist_xml_schema.txt
# End Source File
# End Target
# End Project
# Section ToDoList : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowserCtrl
# 	2:10:HeaderFile:webbrowserctrl.h
# 	2:8:ImplFile:webbrowserctrl.cpp
# End Section
# Section ToDoList : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowserctrl.h
# 	2:16:DefaultSinkClass:CWebBrowserCtrl
# End Section
