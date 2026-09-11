# AudioForge Installation Guide

This guide covers installing and managing your AudioForge plugins.

## Quick Start

### Install All Built Plugins

```bash
./install-all-plugins.sh
```

This will:
- ✅ Find all successfully built plugins
- 📦 Backup existing versions automatically
- 📥 Install to system VST3 directory
- 🎹 Make plugins available in your DAW

### Install a Specific Plugin

```bash
./install-plugin.sh ElectricPiano
```

### Verify Installation

```bash
./verify-installation.sh
```

Shows:
- Which plugins are installed
- File sizes
- Preset support status
- Missing plugins

## Installation Scripts

### `install-all-plugins.sh`
**Purpose:** Batch install all successfully built plugins

**Features:**
- Automatic backup of existing versions
- Only installs plugins that built successfully
- Detailed progress reporting
- Summary statistics

**Output:**
```
✅ Installed: 39 plugins
📦 Backed up: 12 plugins
⊙ Skipped: 0 plugins
```

### `install-plugin.sh <PluginName>`
**Purpose:** Install a single plugin

**Usage:**
```bash
./install-plugin.sh Polysynth
./install-plugin.sh FMSynth
./install-plugin.sh MIDIArpeggiator
```

**Features:**
- Timestamp-based backup
- Build verification
- Size reporting

### `verify-installation.sh`
**Purpose:** Check what's installed

**Shows:**
- Installation status for all 39 plugins
- File sizes
- Preset support detection
- Total disk usage

**Example output:**
```
✅ ElectricPiano          12 MB   ✓ Presets
✅ Polysynth              15 MB   ✓ Presets
❌ BasicSynth - NOT INSTALLED
```

### `cleanup-backups.sh`
**Purpose:** Remove old plugin backups

**Features:**
- Lists all backups with sizes
- Shows total disk usage
- Confirmation before deletion
- Preserves current installations

**Safe to run:** Only removes backups, never current plugins

## Installation Paths

### macOS
```
VST3: ~/Library/Audio/Plug-Ins/VST3/
Backups: ~/Library/Audio/Plug-Ins/VST3_Backup_YYYYMMDD_HHMMSS/
```

### Windows
```
VST3: C:\Program Files\Common Files\VST3\
```

### Linux
```
VST3: ~/.vst3/
```

## Preset Locations

User presets are stored per-plugin:

```
~/Library/Application Support/AudioForge/{PluginName}/Presets/
```

Factory presets ship with the plugin bundle.

## DAW Setup

### Logic Pro
1. Run installation script
2. Logic → Preferences → Plug-in Manager
3. Click "Reset & Rescan Selection"
4. Look for "AudioForge" manufacturer

### Ableton Live
1. Run installation script
2. Preferences → Plug-ins → Rescan
3. Find under "AudioForge" in browser

### Reaper
1. Run installation script
2. Options → Preferences → Plug-ins → VST
3. Re-scan
4. Plugins appear in FX browser

### FL Studio
1. Run installation script
2. Options → Manage plugins → Find more plugins
3. Scan for new plugins
4. Look in plugin database

## Troubleshooting

### "Plugin not found in DAW"
1. Verify installation: `./verify-installation.sh`
2. Check plugin path in DAW preferences
3. Force rescan in DAW

### "Plugin won't load"
1. Check macOS Gatekeeper: `xattr -cr ~/Library/Audio/Plug-Ins/VST3/PluginName.vst3`
2. Verify architecture matches DAW (Intel/Apple Silicon)
3. Check Console.app for crash logs

### "Old version still appears"
1. Clear DAW plugin cache
2. Restart DAW
3. Run `./cleanup-backups.sh` to remove old versions

## Build → Install Workflow

Complete workflow from source to DAW:

```bash
# 1. Build all plugins
./build-all-presets.sh

# 2. Install successfully built plugins
./install-all-plugins.sh

# 3. Verify installation
./verify-installation.sh

# 4. (Optional) Clean up old backups
./cleanup-backups.sh
```

## Uninstallation

To remove a plugin:

```bash
rm -rf ~/Library/Audio/Plug-Ins/VST3/PluginName.vst3
```

To restore from backup:

```bash
# Find backup
ls -lt ~/Library/Audio/Plug-Ins/VST3_Backup_*/

# Restore
cp -R ~/Library/Audio/Plug-Ins/VST3_Backup_TIMESTAMP/PluginName.vst3 \
      ~/Library/Audio/Plug-Ins/VST3/
```

## System Requirements

- **macOS:** 10.13+ (High Sierra)
- **CPU:** Intel or Apple Silicon
- **DAW:** VST3-compatible host
- **Disk:** ~500 MB for all 39 plugins

## What's Included

### Synthesizers (14)
- AcousticBass, BrassSection, ClassicMonosynth, DrumSynth
- ElectricPiano, FMSynth, HiHat, Koto
- OrganEmulator, PadSynth, Polysynth, Shakuhachi
- Sitar, StringEnsemble, WavetableSynth

### Effects (18)
- AutoPanner, FreezeFX, Gate, Limiter
- MultibandCompressor, PhaserFlanger, PlateReverb
- SamplerPlugin, ShimmerReverb, SnareSynth
- StereoChorus, TapeDelay, TomSynth
- TremoloVibrato, VintageCompressor, Vocoder

### MIDI Tools (7)
- MacroController, MIDIArpeggiator, MIDICCMapper
- MIDIChordGenerator, MIDIGrooveQuantizer
- MIDIHarmonizer, XYPadController

## Features

All 39 plugins include:
- ✅ Universal preset management (.afpreset format)
- ✅ MIDI Learn for all parameters
- ✅ Professional UI with AudioForge theme
- ✅ Factory and user preset support
- ✅ VST3 format (modern, efficient)

## Next Steps

After installation:
1. **Test in DAW** - Load a few plugins to verify
2. **Create presets** - Save your favorite sounds
3. **Share feedback** - Report any issues
4. **Explore** - Try all 39 plugins!

---

**Need help?** Check build logs in `plugins/PluginName/build_compile.log`
