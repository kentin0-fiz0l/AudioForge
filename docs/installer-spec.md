# AudioForge Installer Specification

## Overview

Professional installer packages for AudioForge plugin suite, supporting macOS and Windows platforms with automatic installation, version detection, and clean uninstallation.

## Version 1.0.0 Goals

### Primary Objectives
1. **Easy Installation**: One-click or drag-and-drop installation
2. **Complete Plugin Suite**: All 10 plugins in single installer
3. **Automatic Path Detection**: Install to correct system folders
4. **Version Management**: Detect existing installations and handle updates
5. **Clean Uninstallation**: Complete removal script included

### Non-Goals (Future Versions)
- Code signing (requires Apple Developer certificate - planned for v1.1.0)
- Notarization (requires signing first)
- Auto-update mechanism (planned for v2.0.0)
- Per-plugin installation options (install all or none for v1.0.0)

## macOS Installer (DMG)

### Package Contents

```
AudioForge-v1.0.0.dmg
├── AudioForge Plugins/
│   ├── VST3/
│   │   ├── SimpleGain.vst3
│   │   ├── PanUtil.vst3
│   │   ├── BasicSynth.vst3
│   │   ├── CleanDelay.vst3
│   │   ├── SimpleEQ.vst3
│   │   ├── SimpleComp.vst3
│   │   ├── WaveShaper.vst3
│   │   ├── Saturation.vst3
│   │   ├── ChorusFlanger.vst3
│   │   └── Reverb.vst3
│   └── AU/
│       ├── SimpleGain.component
│       ├── PanUtil.component
│       ├── BasicSynth.component
│       ├── CleanDelay.component
│       ├── SimpleEQ.component
│       ├── SimpleComp.component
│       ├── WaveShaper.component
│       ├── Saturation.component
│       ├── ChorusFlanger.component
│       └── Reverb.component
├── Install AudioForge.command
├── Uninstall AudioForge.command
├── README.txt
└── LICENSE.txt
```

### Installation Paths

- **VST3**: `~/Library/Audio/Plug-Ins/VST3/`
- **AU**: `~/Library/Audio/Plug-Ins/Components/`

### Installation Methods

#### Option A: Drag-and-Drop (Preferred for v1.0.0)
- User drags "AudioForge Plugins" folder to Applications alias
- Manual copy to plugin folders required
- Simple, no scripts needed
- **Pros**: Simple, transparent, no permissions needed
- **Cons**: User must know where to copy plugins

#### Option B: Install Script (Recommended)
- Double-click "Install AudioForge.command" script
- Automatically detects and creates plugin directories
- Copies all VST3 and AU plugins to correct locations
- Shows installation progress and success/error messages
- **Pros**: Automatic, user-friendly, handles missing directories
- **Cons**: Requires Terminal execution permission

### Installation Script Requirements

```bash
#!/bin/bash
# Install AudioForge.command

# Features:
1. Welcome message with version info
2. Check for existing installations
3. Create plugin directories if missing
4. Copy VST3 plugins to ~/Library/Audio/Plug-Ins/VST3/
5. Copy AU plugins to ~/Library/Audio/Plug-Ins/Components/
6. Verify installation
7. Success message with instructions to rescan plugins in DAW
8. Open installation log
```

### Uninstallation Script Requirements

```bash
#!/bin/bash
# Uninstall AudioForge.command

# Features:
1. Warning message (confirm before proceeding)
2. List installed AudioForge plugins
3. Remove all VST3 plugins
4. Remove all AU plugins
5. Kill AU cache processes (auval, AudioComponentRegistrar)
6. Success message
```

### DMG Customization

- **Background Image**: Custom background with drag instructions
- **Window Size**: 600x400px
- **Icon Positions**: Organized layout with clear copy instructions
- **Volume Name**: AudioForge v1.0.0
- **README Visibility**: Prominently displayed

## Windows Installer (NSIS)

### Package Structure

```
AudioForge-v1.0.0-Setup.exe
├── VST3/
│   └── (all 10 .vst3 plugins)
├── Install script
├── Uninstall script
├── README.txt
└── LICENSE.txt
```

### Installation Paths

- **VST3**: `C:\Program Files\Common Files\VST3\`
- **User VST3** (fallback): `C:\Users\{username}\AppData\Local\Programs\Common\VST3\`

### NSIS Script Requirements

```nsis
; Features:
1. Welcome page with AudioForge branding
2. License agreement (MIT)
3. Installation directory selection (default: VST3 common folder)
4. Progress bar during installation
5. Desktop shortcut to plugin folder (optional)
6. Registry entries for uninstaller
7. Add/Remove Programs entry
8. Completion page with success message
```

## File Sizes (Estimated)

- **macOS DMG**: ~50-80 MB (compressed)
  - VST3 plugins: ~30 MB
  - AU plugins: ~30 MB
  - Scripts + docs: <1 MB

- **Windows Installer**: ~30-40 MB (compressed)
  - VST3 plugins: ~30 MB
  - Installer overhead: ~5 MB

## Build Process

### Automated Build Script

```bash
#!/bin/bash
# build-installer.sh

# Steps:
1. Build all 10 plugins in Release mode
2. Collect VST3 artifacts from build directories
3. Collect AU artifacts from build directories
4. Create installer staging directory
5. Copy plugins to staging
6. Copy README, LICENSE, scripts
7. Create DMG (macOS)
8. Create NSIS installer (Windows - if on Windows)
9. Checksum generation (SHA256)
10. Move to releases/ directory
```

### Version Management

- Version embedded in DMG filename: `AudioForge-v1.0.0.dmg`
- Version in README and scripts
- Version detection script checks installed plugin versions
- Warn if overwriting newer version with older version

## User Experience Flow

### macOS Installation (Script Method)

1. Download `AudioForge-v1.0.0.dmg`
2. Double-click to mount
3. Double-click "Install AudioForge.command"
4. Terminal opens, shows installation progress
5. Plugins installed automatically
6. Success message displayed
7. User rescans plugins in DAW
8. Eject DMG

### macOS Uninstallation

1. Mount DMG (or run from backup)
2. Double-click "Uninstall AudioForge.command"
3. Confirm removal
4. All plugins removed
5. Success message

### Windows Installation

1. Download `AudioForge-v1.0.0-Setup.exe`
2. Double-click installer
3. Follow wizard (Next → Agree → Install)
4. Plugins installed to VST3 folder
5. Completion page
6. User rescans plugins in DAW

### Windows Uninstallation

1. Open Add/Remove Programs
2. Find "AudioForge v1.0.0"
3. Click Uninstall
4. Confirm removal
5. All plugins removed

## Testing Requirements

### Pre-Release Testing

1. **Clean System Test**: Install on system with no existing AudioForge plugins
2. **Update Test**: Install on system with older AudioForge version
3. **Reinstall Test**: Install over existing v1.0.0 installation
4. **Uninstall Test**: Verify complete removal
5. **DAW Scan Test**: Verify plugins appear in Logic Pro, Reaper, Ableton

### Test Matrix

| OS | Version | VST3 Host | AU Host | Result |
|----|---------|-----------|---------|--------|
| macOS 14 | Sonoma | Reaper | Logic Pro | ? |
| macOS 13 | Ventura | Ableton | GarageBand | ? |
| Windows 11 | 23H2 | FL Studio | N/A | ? |
| Windows 10 | 22H2 | Reaper | N/A | ? |

## Distribution

### Release Artifacts

- `AudioForge-v1.0.0-macOS.dmg` (macOS installer)
- `AudioForge-v1.0.0-Windows.exe` (Windows installer)
- `AudioForge-v1.0.0-macOS.dmg.sha256` (checksum)
- `AudioForge-v1.0.0-Windows.exe.sha256` (checksum)
- `INSTALL.md` (installation guide)

### Hosting

- GitHub Releases (primary)
- AudioForge website download page
- FluxStudio marketplace

### Download Page Requirements

```markdown
# AudioForge v1.0.0 Downloads

## macOS
- [Download AudioForge v1.0.0 for macOS (DMG, 60 MB)](link)
  - Supports: macOS 10.15+
  - Formats: VST3, AU
  - SHA256: abc123...

## Windows
- [Download AudioForge v1.0.0 for Windows (EXE, 35 MB)](link)
  - Supports: Windows 10/11
  - Formats: VST3
  - SHA256: def456...

## Installation Instructions
1. Download installer for your platform
2. Run installer (macOS: double-click Install script, Windows: run .exe)
3. Rescan plugins in your DAW
4. Plugins ready to use!

## Compatibility
- Logic Pro X / Pro 11
- GarageBand
- Ableton Live
- FL Studio
- Reaper
- Pro Tools (VST3)
- Studio One
```

## Future Enhancements (v1.1.0+)

### Code Signing (v1.1.0)
- Apple Developer certificate ($99/year)
- Code sign all .vst3 and .component files
- Notarize DMG for macOS Gatekeeper
- Windows code signing certificate

### Advanced Features (v2.0.0)
- Auto-update mechanism (Sparkle for macOS)
- Per-plugin installation options
- Plugin activation/licensing system
- Installation analytics (privacy-respecting)
- Multi-language support

## Security Considerations

### v1.0.0 (Unsigned)
- **macOS**: Users must right-click → Open to bypass Gatekeeper warning
- **Windows**: SmartScreen warning may appear
- **Mitigation**: Clear installation instructions in README
- **Risk**: Low (open-source, MIT license, source code available)

### v1.1.0 (Signed)
- **macOS**: Code signed + notarized = no warnings
- **Windows**: Code signed = no SmartScreen warning
- **Benefit**: Professional distribution, user trust

## Success Metrics

### v1.0.0 Goals
- [ ] DMG installer created successfully
- [ ] Install script works on macOS 10.15+
- [ ] All 10 plugins install correctly
- [ ] Uninstall script removes all traces
- [ ] Plugins load in 3+ DAWs
- [ ] Installation time < 2 minutes
- [ ] Zero errors in testing matrix

## Timeline

- **Day 1**: Create installer build script
- **Day 2**: Create macOS DMG with install/uninstall scripts
- **Day 3**: Test on multiple macOS versions
- **Day 4**: Create Windows NSIS installer (optional for v1.0.0)
- **Day 5**: Documentation and release preparation

## Priority for v1.0.0

**Focus**: macOS DMG installer with install/uninstall scripts
**Defer**: Windows installer (can be added in v1.0.1)

This allows us to:
1. Deliver macOS installer quickly
2. Test with macOS users first (primary platform)
3. Iterate based on feedback before Windows version
