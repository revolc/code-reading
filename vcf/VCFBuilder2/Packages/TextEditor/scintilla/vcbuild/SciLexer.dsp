# Microsoft Developer Studio Project File - Name="SciLexer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SciLexer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SciLexer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SciLexer.mak" CFG="SciLexer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SciLexer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SciLexer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SciLexer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SciLexer_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /O2 /I "..\include" /I "..\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCI_LEXER" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib comctl32.lib /nologo /dll /machine:I386 /out:"..\..\scite\bin\SciLexer.dll"

!ELSEIF  "$(CFG)" == "SciLexer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SciLexer_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /ZI /Od /I "..\include" /I "..\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCI_LEXER" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib comctl32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\..\Bin\SciLexer.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SciLexer - Win32 Release"
# Name "SciLexer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=..\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=..\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=..\src\ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=..\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=..\src\DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=..\src\Editor.cxx
# End Source File
# Begin Source File

SOURCE=..\win32\ExternalLexer.cxx
# End Source File
# Begin Source File

SOURCE=..\src\Indicator.cxx
# End Source File
# Begin Source File

SOURCE=..\src\KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=..\src\KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=..\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=..\win32\PlatWin.cxx
# End Source File
# Begin Source File

SOURCE=..\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=..\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=..\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=..\win32\ScintillaWin.cxx
# End Source File
# Begin Source File

SOURCE=..\win32\ScintRes.rc
# End Source File
# Begin Source File

SOURCE=..\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=..\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=..\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=..\src\WindowAccessor.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\Platform.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\win32\Margin.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project