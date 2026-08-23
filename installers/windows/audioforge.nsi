; AudioForge Windows Installer (NSIS)
; Installs VST3 plugins to the standard Windows plugin directory

!define PRODUCT_NAME "AudioForge"
!define PRODUCT_VERSION "1.1.0"
!define PRODUCT_PUBLISHER "AudioForge"
!define PRODUCT_WEB_SITE "https://audioforge.fluxstudio.art"
!define VST3_PATH "$COMMONFILES64\VST3"

; MUI Settings
!include "MUI2.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Directory page (for advanced users)
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Installer details
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "AudioForge-Setup-v${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES64\AudioForge"
InstallDirRegKey HKLM "Software\${PRODUCT_NAME}" "InstallDir"
ShowInstDetails show
ShowUnInstDetails show

; Version information
VIProductVersion "1.1.0.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "© 2026 AudioForge"
VIAddVersionKey "FileDescription" "AudioForge Plugin Installer"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"

Section "Main Installation" SEC01
  SetOutPath "${VST3_PATH}"
  SetOverwrite on

  ; Copy all VST3 plugins
  File /r "..\..\releases\staging\AudioForge Plugins\VST3\*.vst3"

  ; Create installation directory for README and uninstaller
  SetOutPath "$INSTDIR"
  File /oname=README.txt "..\..\releases\staging\README.txt"

  ; Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Add to Windows Add/Remove Programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "DisplayName" "${PRODUCT_NAME} v${PRODUCT_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "DisplayIcon" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
    "NoRepair" 1

  ; Store installation directory in registry
  WriteRegStr HKLM "Software\${PRODUCT_NAME}" "InstallDir" "$INSTDIR"

  ; Success message
  DetailPrint "AudioForge plugins installed successfully!"
  DetailPrint "VST3 plugins installed to: ${VST3_PATH}"
  DetailPrint "Please rescan plugins in your DAW."
SectionEnd

Section "Uninstall"
  ; Remove all plugins
  Delete "${VST3_PATH}\SimpleGain.vst3"
  Delete "${VST3_PATH}\PanUtil.vst3"
  Delete "${VST3_PATH}\BasicSynth.vst3"
  Delete "${VST3_PATH}\CleanDelay.vst3"
  Delete "${VST3_PATH}\SimpleEQ.vst3"
  Delete "${VST3_PATH}\SimpleComp.vst3"
  Delete "${VST3_PATH}\WaveShaper.vst3"
  Delete "${VST3_PATH}\Saturation.vst3"
  Delete "${VST3_PATH}\ChorusFlanger.vst3"
  Delete "${VST3_PATH}\Reverb.vst3"

  ; Remove uninstaller and README
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\README.txt"
  RMDir "$INSTDIR"

  ; Remove from registry
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  DeleteRegKey HKLM "Software\${PRODUCT_NAME}"

  DetailPrint "AudioForge plugins uninstalled successfully."
SectionEnd
