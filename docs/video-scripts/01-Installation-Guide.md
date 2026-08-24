# Video Tutorial Script: AudioForge Installation Guide

**Target Length**: 5-7 minutes
**Audience**: Beginners to intermediate users
**Format**: Screen recording with voiceover

---

## Pre-Production Checklist

**Recording Setup**:
- [ ] Screen recording software ready (OBS Studio, ScreenFlow, Camtasia)
- [ ] Audio interface and microphone tested
- [ ] Browser open to GitHub releases page
- [ ] Fresh VMs ready (Windows 10, macOS, Ubuntu 22.04)
- [ ] DAW ready for plugin verification (Reaper or Ableton)

**Screen Resolution**: 1920x1080 (1080p)
**Frame Rate**: 30fps
**Audio**: 48kHz, 16-bit or higher

---

## Video Structure

**Total Time**: ~6 minutes

1. **Intro** (0:00-0:30) - 30 seconds
2. **macOS Installation** (0:30-2:00) - 90 seconds
3. **Windows Installation** (2:00-3:30) - 90 seconds
4. **Linux Installation** (3:30-4:30) - 60 seconds
5. **DAW Setup & Verification** (4:30-5:30) - 60 seconds
6. **Outro** (5:30-6:00) - 30 seconds

---

## Script

### INTRO (0:00-0:30)

**[VISUAL: AudioForge logo animation, fade to desktop]**

**NARRATION**:
> "Hey everyone! Welcome to the AudioForge installation guide. In this video, I'll show you how to install all 10 AudioForge plugins on macOS, Windows, and Linux. Whether you're on Mac, PC, or Linux, I've got you covered. Let's dive in!"

**[VISUAL: Text overlay - "AudioForge Installation Guide" - "macOS • Windows • Linux"]**

---

### PART 1: macOS INSTALLATION (0:30-2:00)

**[VISUAL: Browser showing GitHub releases page]**

**NARRATION**:
> "First, let's cover macOS. Head to the AudioForge releases page on GitHub - I'll put the link in the description. Download the DMG file for the latest version."

**[VISUAL: Click "AudioForge-v1.1.0-macOS.dmg", show download starting]**

**NARRATION**:
> "Once downloaded, double-click the DMG file to mount it."

**[VISUAL: Double-click DMG, wait for mount, Finder window opens]**

**NARRATION**:
> "You'll see the AudioForge installer window. Double-click 'Install AudioForge.command' to start the installation."

**[VISUAL: Double-click "Install AudioForge.command", Terminal window opens]**

**[VISUAL: Show Terminal running installation script]**

**NARRATION**:
> "The installer will automatically copy all VST3 and Audio Units plugins to your system folders. This takes about 10 seconds. You may be asked for your password to grant permission."

**[VISUAL: Type password (blur on screen), press Enter]**

**[VISUAL: Show "Installation complete!" message]**

**NARRATION**:
> "And we're done! AudioForge is now installed. You can close the installer and eject the DMG."

**[VISUAL: Close Terminal, eject DMG]**

**TIMING**: 90 seconds total

---

### PART 2: WINDOWS INSTALLATION (2:00-3:30)

**[VISUAL: Switch to Windows desktop, browser showing releases page]**

**NARRATION**:
> "Now let's install on Windows. Same process - go to the releases page and download 'AudioForge-Setup-v1.1.0.exe'."

**[VISUAL: Click download, show file downloading]**

**NARRATION**:
> "Once downloaded, run the installer. You might see a Windows SmartScreen warning since our certificate is new. Click 'More info', then 'Run anyway'."

**[VISUAL: Show SmartScreen dialog, click "More info", click "Run anyway"]**

**NARRATION**:
> "Click 'Yes' on the User Account Control prompt to allow installation."

**[VISUAL: UAC prompt appears, click "Yes"]**

**[VISUAL: AudioForge installer wizard appears]**

**NARRATION**:
> "The AudioForge installer will launch. Click 'Install' to begin."

**[VISUAL: Click "Install" button]**

**[VISUAL: Progress bar fills, plugins copying]**

**NARRATION**:
> "The installer copies all 10 VST3 plugins to the standard Windows plugin folder. This takes about 15 seconds."

**[VISUAL: Show "Installation complete" dialog]**

**NARRATION**:
> "Installation complete! Click 'Finish' to close the installer. AudioForge is now ready to use."

**[VISUAL: Click "Finish"]**

**TIMING**: 90 seconds total

---

### PART 3: LINUX INSTALLATION (3:30-4:30)

**[VISUAL: Switch to Ubuntu desktop, terminal open]**

**NARRATION**:
> "For Linux users, we have three options: .deb packages for Ubuntu and Debian, .rpm for Fedora and Red Hat, or AppImage for universal compatibility. I'll show the .deb installation here."

**[VISUAL: Terminal showing download command]**

**NARRATION**:
> "First, download the .deb package from the releases page. Then, in your terminal, run 'sudo dpkg -i audioforge-1.1.0-amd64.deb'."

**[VISUAL: Type command, press Enter]**

```bash
sudo dpkg -i audioforge-1.1.0-amd64.deb
```

**[VISUAL: Show password prompt, installation progress]**

**NARRATION**:
> "Enter your password, and the package manager will install all plugins to /usr/lib/vst3. That's it!"

**[VISUAL: Show "Setting up audioforge..." message, then completion]**

**NARRATION**:
> "If you prefer .rpm or AppImage, check out the installation guide linked in the description."

**TIMING**: 60 seconds total

---

### PART 4: DAW SETUP & VERIFICATION (4:30-5:30)

**[VISUAL: Open DAW (Reaper or Ableton Live)]**

**NARRATION**:
> "Now let's verify the installation in your DAW. I'm using Reaper here, but the process is similar in any DAW."

**[VISUAL: Go to Preferences → VST]**

**NARRATION**:
> "First, make sure your DAW has scanned the plugin folders. In Reaper, go to Preferences, then Plug-ins, VST."

**[VISUAL: Click "Re-scan"]**

**NARRATION**:
> "Click 'Re-scan' to refresh the plugin list."

**[VISUAL: Scanning progress, plugins found]**

**NARRATION**:
> "Reaper will scan for new plugins. Once complete, you should see all 10 AudioForge plugins in the list."

**[VISUAL: Show plugin list with AudioForge plugins highlighted]**
- SimpleGain
- PanUtil
- BasicSynth
- CleanDelay
- SimpleEQ
- SimpleComp
- WaveShaper
- Saturation
- ChorusFlanger
- Reverb

**NARRATION**:
> "Perfect! Let's quickly test one. I'll insert SimpleGain on a track."

**[VISUAL: Create audio track, insert SimpleGain]**

**[VISUAL: SimpleGain UI appears]**

**NARRATION**:
> "And there we go - SimpleGain loaded successfully! All 10 plugins are now ready to use."

**[VISUAL: Close plugin]**

**TIMING**: 60 seconds total

---

### OUTRO (5:30-6:00)

**[VISUAL: Fade to title card with plugin icons]**

**NARRATION**:
> "And that's it! AudioForge is now installed on your system. In the next videos, I'll walk you through each plugin and show you how to use them in your productions."

**[VISUAL: Text overlay - "Coming Soon: SimpleGain Tutorial, BasicSynth Guide, SimpleComp Walkthrough"]**

**NARRATION**:
> "If you found this helpful, give the video a like and subscribe for more tutorials. Links to the manuals and documentation are in the description. Thanks for watching, and happy producing!"

**[VISUAL: Fade to AudioForge logo with website URL]**

**ON-SCREEN TEXT**:
- Website: audioforge.fluxstudio.art
- GitHub: github.com/username/AudioForge
- Docs: audioforge.fluxstudio.art/docs

**TIMING**: 30 seconds total

---

## B-Roll & Graphics Needed

**Lower Third Graphics** (use throughout):
- Name: "AudioForge Installation"
- Section titles: "macOS", "Windows", "Linux", "DAW Setup"

**Text Overlays**:
- Download URL: github.com/username/AudioForge/releases
- System paths (when relevant):
  - macOS VST3: ~/Library/Audio/Plug-Ins/VST3/
  - macOS AU: ~/Library/Audio/Plug-Ins/Components/
  - Windows: C:\Program Files\Common Files\VST3\
  - Linux: /usr/lib/vst3/

**Plugin List Graphic** (for DAW verification section):
```
AudioForge Plugins (10 total):
✓ SimpleGain
✓ PanUtil
✓ BasicSynth
✓ CleanDelay
✓ SimpleEQ
✓ SimpleComp
✓ WaveShaper
✓ Saturation
✓ ChorusFlanger
✓ Reverb
```

---

## Post-Production Notes

**Editing**:
- Cut dead air and long waits (speed up installation progress bars to 2x-4x)
- Add subtle background music (royalty-free, low volume)
- Add sound effects for clicks and successful actions
- Color grade to match brand colors

**Annotations/Cards**:
- Add YouTube cards at 1:00, 3:00, 5:00 linking to:
  - User manuals playlist
  - Next tutorial (SimpleGain)
  - Installation docs

**Chapters/Timestamps** (add in description):
```
0:00 - Introduction
0:30 - macOS Installation
2:00 - Windows Installation
3:30 - Linux Installation
4:30 - DAW Setup & Verification
5:30 - Outro
```

---

## Video Description Template

```
How to install AudioForge plugins on macOS, Windows, and Linux. This tutorial covers the complete installation process for all 10 professional audio plugins.

🎵 AudioForge is a free, open-source plugin suite featuring:
• SimpleGain - Gain control
• PanUtil - Stereo panning
• BasicSynth - Synthesizer
• CleanDelay - Delay effect
• SimpleEQ - Equalizer
• SimpleComp - Compressor
• WaveShaper - Distortion
• Saturation - Analog saturation
• ChorusFlanger - Modulation
• Reverb - Reverb effect

📥 Download: https://github.com/username/AudioForge/releases

📚 Documentation:
• Installation Guide (macOS): [link]
• Installation Guide (Windows): [link]
• Installation Guide (Linux): [link]
• User Manuals: [link]

🔗 Links:
• Website: https://audioforge.fluxstudio.art
• GitHub: https://github.com/username/AudioForge
• Report Issues: https://github.com/username/AudioForge/issues

⏱️ Chapters:
0:00 - Introduction
0:30 - macOS Installation
2:00 - Windows Installation
3:30 - Linux Installation
4:30 - DAW Setup & Verification
5:30 - Outro

🎬 Next Video: SimpleGain Tutorial - [link]

#audioforge #freeplugins #vst3 #audioplugins #musicproduction #daw #tutorial
```

---

## Filming Tips

**Do's**:
- ✅ Speak clearly and at a moderate pace
- ✅ Pause between sections for easy editing
- ✅ Demonstrate each step visually (don't just narrate)
- ✅ Keep cursor movements smooth and deliberate
- ✅ Test audio levels before recording (aim for -12 dB peaks)

**Don'ts**:
- ❌ Don't rush through steps
- ❌ Don't assume knowledge (explain everything)
- ❌ Don't use jargon without explanation
- ❌ Don't include personal info (blur passwords, emails)
- ❌ Don't skip error messages (show how to resolve them)

---

**Production Date**: TBD
**Last Updated**: 2026-08-23
