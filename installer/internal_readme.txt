------
Readme
------
Steps:

Modify version number in TdlSetup.nsi. Include ToDoList version and plugins' versions.
Prepare the DOS shell script to run the whole flow of creating release packages.
Run the scripts.
Release


-----------------------
Sample DOS shell script
-----------------------
rem ScriptName: make-release-package.cmd
rem The file is ignored by SVN automatically.

@echo off

setlocal ENABLEDELAYEDEXPANSION

rem set TDL_ORIGINAL=original-todolist~
rem set TDL_PLUGINS=original-plugins~
set TDL_ZH_CN_DLL=E:\James\workspace\todolist\trans-zh-work\ToDoList\release
set TDL_ZH_CN_DOC=E:\James\workspace\todolist\todolist-docs\zh-CN\work
set TDL_UPX="D:\Program Files\jamesbin\upx.exe"
set TDL_NSIS="D:\Program Files\NSIS\makensis.exe"
set TDL_ZIP="D:\Program Files\7-Zip\7z.exe" a -r -tzip -mx=9
set TDL_RELEASE=F:\

cscript package-release.wsf

endlocal