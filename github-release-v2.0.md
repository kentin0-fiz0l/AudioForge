# AudioForge v2.0 - Preset & MIDI Learn Update 🎉

The biggest AudioForge update yet! Every plugin now includes **universal preset management** and **MIDI Learn** capabilities.

## ✨ Highlights

### 💾 Universal Preset System
- **Save & load presets** for all 39 plugins
- **Factory presets** included out of the box
- **Category organization** - Browse by Bass, Lead, Pad, FX, etc.
- **User presets** saved to your library
- **One-click loading** from dropdown menu

### 🎹 Universal MIDI Learn
- **Right-click any parameter** → Assign MIDI CC
- **Instant mapping** - Move your controller to assign
- **Visual feedback** - See what's mapped
- **Saved with projects** - Mappings persist
- **Easy removal** - Clear mappings with right-click

## 📦 What's Included

**39 Plugins with Full Preset Support:**
- 14 Synthesizers (Polysynth, ElectricPiano, FMSynth, OrganEmulator, ...)
- 18 Effects (PlateReverb, VintageCompressor, TapeDelay, Vocoder, ...)
- 7 MIDI Tools (MIDIArpeggiator, MIDIChordGenerator, MacroController, ...)

## 🚀 Installation

### macOS (Intel & Apple Silicon)

```bash
# Download and extract
unzip AudioForge-v2.0.zip
cd AudioForge-v2.0

# Install all plugins
./install-all-plugins.sh

# Verify installation
./verify-installation.sh
```

**Then:**
1. Open your DAW (Logic, Ableton, Reaper, etc.)
2. Rescan plugins
3. Look for "AudioForge" manufacturer
4. Load a plugin and try the preset browser!

See `INSTALLATION.md` for detailed DAW-specific instructions.

## 📋 Full Release Notes

See [`RELEASE_NOTES_v2.0.md`](./RELEASE_NOTES_v2.0.md) for complete documentation including:
- Detailed feature descriptions
- Migration guide from v1.x
- System requirements
- Troubleshooting
- Known issues
- Future roadmap

## ⚠️ Breaking Changes

- **Preset format changed** - v1.x presets not compatible
- **MIDI mappings** - New system (old mappings won't migrate)
- **UI size** - Plugins are 50px taller (preset browser added)

## 🐛 Known Issues

- **macOS Gatekeeper:** May show "damaged" warning on first load
  - **Fix:** `xattr -cr ~/Library/Audio/Plug-Ins/VST3/*.vst3`

## 📊 System Requirements

- **macOS:** 10.13+ (High Sierra)
- **CPU:** Intel or Apple Silicon
- **DAW:** Any VST3-compatible host
- **Disk:** ~500 MB for full collection

## 🙏 Thank You

Thank you for using AudioForge! This release represents months of development to bring professional preset management and MIDI Learn to the entire plugin collection.

**Enjoy creating!** 🎵

---

## 📥 Downloads

Choose your platform:

### macOS
- **[AudioForge-v2.0-macOS-Universal.zip]** (500 MB)
  - Universal Binary (Intel + Apple Silicon)
  - Includes all 39 plugins with presets
  - Installation scripts
  - Documentation

### Checksums
```
SHA256: [to be generated]
MD5: [to be generated]
```

---

## 📚 Documentation

- [Installation Guide](./INSTALLATION.md)
- [Full Release Notes](./RELEASE_NOTES_v2.0.md)
- [Changelog](./CHANGELOG.md)

## 🔗 Resources

- **Issues:** Report bugs or request features
- **Discussions:** Share presets and tips
- **Email:** jkino.ji@gmail.com

---

**Version:** 2.0.0  
**Release Date:** September 11, 2026  
**License:** Proprietary (free for use, no redistribution)
