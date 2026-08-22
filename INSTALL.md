# AudioForge Installation Guide

## Quick Start (macOS)

1. **Download** `AudioForge-v1.0.0-macOS.dmg` from the [Releases page](https://github.com/kentin0-fiz0l/AudioForge/releases)
2. **Double-click** the DMG file to mount it
3. **Double-click** "Install AudioForge.command"
4. **Rescan** plugins in your DAW
5. **Done!** All 10 AudioForge plugins are ready to use

## What's Included

### Plugins (10 total)

1. **SimpleGain** - Clean gain/volume control with metering
2. **PanUtil** - Stereo panning and width control
3. **BasicSynth** - Simple subtractive synthesizer
4. **CleanDelay** - Stereo delay with ping-pong mode
5. **SimpleEQ** - 3-band parametric equalizer
6. **SimpleComp** - Compressor with auto makeup gain
7. **WaveShaper** - Waveshaping distortion
8. **Saturation** - Multi-mode analog saturation
9. **ChorusFlanger** - LFO-based modulation effects
10. **Reverb** - Freeverb-inspired algorithmic reverb

### Formats

- **VST3**: Compatible with most DAWs (Reaper, Ableton, FL Studio, Pro Tools, Studio One, etc.)
- **AU (Audio Units)**: macOS native format (Logic Pro, GarageBand, MainStage)

## Installation Methods

### Method 1: Automatic Installation (Recommended)

1. Mount the DMG file
2. Double-click **"Install AudioForge.command"**
3. Terminal will open and show installation progress
4. All plugins automatically installed to:
   - **VST3**: `~/Library/Audio/Plug-Ins/VST3/`
   - **AU**: `~/Library/Audio/Plug-Ins/Components/`
5. Press Enter to close Terminal when done

### Method 2: Manual Installation

1. Mount the DMG file
2. Navigate to "AudioForge Plugins" folder
3. **For VST3**:
   - Copy all .vst3 files from "VST3" folder
   - Paste to `~/Library/Audio/Plug-Ins/VST3/`
4. **For AU**:
   - Copy all .component files from "AU" folder
   - Paste to `~/Library/Audio/Plug-Ins/Components/`

## System Requirements

### macOS
- **OS Version**: macOS 10.15 (Catalina) or later
- **Architecture**: Intel x86_64 and Apple Silicon (Universal Binary)
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 200MB for all plugins

### Compatible DAWs

**VST3 Compatible:**
- Reaper
- Ableton Live
- FL Studio
- Bitwig Studio
- Studio One
- Pro Tools (2021.6+)
- Cubase / Nuendo
- And any other VST3-compatible DAW

**AU Compatible (macOS only):**
- Logic Pro / Logic Pro X
- GarageBand
- MainStage
- Final Cut Pro (for audio processing)

## First Time Use

### Rescanning Plugins

After installation, you need to rescan plugins in your DAW:

**Logic Pro / GarageBand:**
1. Go to Preferences → Plug-in Manager
2. Click "Reset & Rescan Selection"
3. AudioForge plugins will appear in the plugin browser

**Reaper:**
1. Go to Options → Preferences → Plug-ins → VST
2. Click "Re-scan"
3. Or restart Reaper (auto-scans on startup)

**Ableton Live:**
1. Go to Preferences → Plug-ins
2. Click "Rescan"
3. Or restart Ableton (auto-scans on startup)

**FL Studio:**
1. Go to Options → Manage Plugins
2. Click "Find Plugins"
3. FL Studio will discover new plugins

### Security Note (macOS)

AudioForge plugins are **unsigned** in v1.0.0 (code signing coming in v1.1.0).

If you see a security warning:
1. Go to **System Settings → Privacy & Security**
2. Scroll down to find the blocked plugin message
3. Click **"Open Anyway"**
4. Rescan plugins in your DAW

Or use the Terminal installation method which bypasses Gatekeeper.

## Uninstallation

### Automatic Uninstall (Recommended)

1. Mount the AudioForge DMG file
2. Double-click **"Uninstall AudioForge.command"**
3. Type `yes` to confirm removal
4. All AudioForge plugins will be removed
5. Rescan plugins in your DAW

### Manual Uninstall

Remove these directories:

**VST3 Plugins:**
```
~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3
~/Library/Audio/Plug-Ins/VST3/PanUtil.vst3
~/Library/Audio/Plug-Ins/VST3/BasicSynth.vst3
~/Library/Audio/Plug-Ins/VST3/CleanDelay.vst3
~/Library/Audio/Plug-Ins/VST3/SimpleEQ.vst3
~/Library/Audio/Plug-Ins/VST3/SimpleComp.vst3
~/Library/Audio/Plug-Ins/VST3/WaveShaper.vst3
~/Library/Audio/Plug-Ins/VST3/Saturation.vst3
~/Library/Audio/Plug-Ins/VST3/ChorusFlanger.vst3
~/Library/Audio/Plug-Ins/VST3/Reverb.vst3
```

**AU Plugins:**
```
~/Library/Audio/Plug-Ins/Components/SimpleGain.component
~/Library/Audio/Plug-Ins/Components/PanUtil.component
~/Library/Audio/Plug-Ins/Components/BasicSynth.component
~/Library/Audio/Plug-Ins/Components/CleanDelay.component
~/Library/Audio/Plug-Ins/Components/SimpleEQ.component
~/Library/Audio/Plug-Ins/Components/SimpleComp.component
~/Library/Audio/Plug-Ins/Components/WaveShaper.component
~/Library/Audio/Plug-Ins/Components/Saturation.component
~/Library/Audio/Plug-Ins/Components/ChorusFlanger.component
~/Library/Audio/Plug-Ins/Components/Reverb.component
```

Clear AU cache (optional):
```bash
killall -9 AudioComponentRegistrar
```

## Troubleshooting

### Plugins Don't Appear in DAW

1. **Verify Installation**:
   - Check `~/Library/Audio/Plug-Ins/VST3/` for VST3 plugins
   - Check `~/Library/Audio/Plug-Ins/Components/` for AU plugins

2. **Rescan Plugins**: Follow the "Rescanning Plugins" steps above

3. **Check Blacklist**: Some DAWs blacklist plugins that fail to load
   - **Logic Pro**: Preferences → Plug-in Manager → check if plugins are disabled
   - **Reaper**: Options → Preferences → Plug-ins → VST → check blacklist

4. **Check DAW Compatibility**: Make sure your DAW version supports VST3 or AU

### Plugin Crashes or Fails to Load

1. **Check System Requirements**: macOS 10.15+ required
2. **Try Different Plugin**: If one plugin fails, try another to isolate the issue
3. **Check Console**: Open Console.app and look for crash reports
4. **Report Issue**: [Create a GitHub issue](https://github.com/kentin0-fiz0l/AudioForge/issues)

### Security Warning on macOS

**If you see "cannot be opened because the developer cannot be verified":**

1. Right-click the plugin (in Finder)
2. Select "Open"
3. Click "Open" in the dialog
4. Or use System Settings → Privacy & Security → Open Anyway

**For AU plugins in Logic Pro:**

1. Open Logic Pro
2. Go to Preferences → Plug-in Manager
3. Find AudioForge plugins
4. Click "Reset" if they're marked as incompatible
5. Restart Logic Pro

## Updating AudioForge

When a new version is released:

1. Download the new DMG
2. Run "Uninstall AudioForge.command" from old DMG (optional)
3. Run "Install AudioForge.command" from new DMG
4. Rescan plugins in your DAW

Plugins with the same name will be overwritten. Your DAW projects will continue to work with updated plugins.

## Getting Help

### Documentation
- **Website**: https://audioforge.fluxstudio.art
- **Plugin Guides**: https://audioforge.fluxstudio.art/docs
- **GitHub**: https://github.com/kentin0-fiz0l/AudioForge

### Support
- **Issues**: https://github.com/kentin0-fiz0l/AudioForge/issues
- **Discussions**: https://github.com/kentin0-fiz0l/AudioForge/discussions

## License

AudioForge is free and open source under the **MIT License**.

- ✅ Free for personal use
- ✅ Free for commercial use
- ✅ No registration required
- ✅ No limitations
- ✅ Source code available

See LICENSE.txt for full license text.

## Building from Source

Want to build AudioForge yourself?

```bash
# Clone the repository
git clone --recursive https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge

# Build all plugins
./build-installer.sh

# Create DMG
./create-dmg-simple.sh
```

See README.md for detailed build instructions.
