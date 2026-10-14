; AudioForge installer for Windows, built by the release workflow:
;   makensis /DVERSION=<version> /DSTAGE=<stage dir> /DOUTFILE=<exe> AudioForge.nsi
; Installs every VST3 in the stage to Common Files\VST3\AudioForge and the standalone
; apps to Program Files\AudioForge, with Start Menu shortcuts and an uninstaller.
; Not code-signed: SmartScreen may ask for "More info" > "Run anyway".

Unicode true
!include "MUI2.nsh"
!include "x64.nsh"

!ifndef VERSION
  !error "Pass /DVERSION=<version>"
!endif
!ifndef STAGE
  !error "Pass /DSTAGE=<stage dir>"
!endif
!ifndef OUTFILE
  !define OUTFILE "AudioForge-${VERSION}-Windows-Setup.exe"
!endif

!define PRODUCT "AudioForge"
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\AudioForge"
!define VST3_DIR "$COMMONFILES64\VST3\AudioForge"

Name "${PRODUCT} ${VERSION}"
OutFile "${OUTFILE}"
InstallDir "$PROGRAMFILES64\AudioForge"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TEXT "Free, open-source audio plugins (MIT License).$\r$\n$\r$\nThis installs the VST3 plugins to Common Files\VST3\AudioForge and the standalone apps to Program Files\AudioForge.$\r$\n$\r$\nRestart your DAW and rescan plugins afterwards."
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

Function .onInit
  ${IfNot} ${RunningX64}
    MessageBox MB_OK|MB_ICONSTOP "AudioForge needs 64-bit Windows."
    Abort
  ${EndIf}
  SetRegView 64
FunctionEnd

Section "VST3 plugins" SecVST3
  SectionIn RO
  SetOutPath "${VST3_DIR}"
  File /r "${STAGE}\VST3\*.vst3"
SectionEnd

Section "Standalone apps" SecApps
  SetOutPath "$INSTDIR\Standalone"
  File /nonfatal "${STAGE}\Standalone\*.exe"
  ; A Start Menu shortcut for each app
  CreateDirectory "$SMPROGRAMS\AudioForge"
  FindFirst $0 $1 "$INSTDIR\Standalone\*.exe"
  loop:
    StrCmp $1 "" done
    CreateShortcut "$SMPROGRAMS\AudioForge\$1.lnk" "$INSTDIR\Standalone\$1"
    FindNext $0 $1
    Goto loop
  done:
  FindClose $0
SectionEnd

Section "-Uninstaller"
  SetOutPath "$INSTDIR"
  File "/oname=README.txt" "${STAGE}\README.txt"
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayName" "${PRODUCT}"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "Publisher" "AudioForge (open source)"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "URLInfoAbout" "https://github.com/kentin0-fiz0l/AudioForge"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoRepair" 1
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecVST3} "Every AudioForge plugin, for any VST3 host."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecApps} "Standalone versions that run without a DAW, with Start Menu shortcuts."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "Uninstall"
  SetRegView 64
  RMDir /r "${VST3_DIR}"
  RMDir /r "$SMPROGRAMS\AudioForge"
  RMDir /r "$INSTDIR\Standalone"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"
  DeleteRegKey HKLM "${UNINSTALL_KEY}"
SectionEnd
