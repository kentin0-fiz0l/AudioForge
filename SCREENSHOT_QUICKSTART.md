# Screenshot Capture - Quick Start Guide

**Goal:** Capture professional screenshots for 18 remaining AudioForge plugins  
**Time:** 4-5 hours (or split into 5 one-hour sessions)  
**Tools:** Your DAW + macOS Screenshot tool

---

## 🎯 Two Capture Methods

### **Method A: Automated Script (Recommended)**

The script guides you step-by-step through all 18 plugins:

```bash
cd ~/Projects/Active/AudioForge
./capture-screenshots.sh
```

**What it does:**
- ✅ Prompts you for each plugin one-by-one
- ✅ Automatically finds your screenshot on Desktop
- ✅ Moves and renames it to the correct location
- ✅ Optimizes file size (reduces by ~80%)
- ✅ Creates backup in docs/screenshots/
- ✅ Cleans up Desktop

**Workflow:**
1. Script says "Capture [PluginName]"
2. You open plugin in DAW
3. You take screenshot (Cmd+Shift+4 + Space + Click)
4. You press Enter in terminal
5. Script processes it automatically
6. Repeat for next plugin!

---

### **Method B: Manual Batch Capture**

Capture all screenshots first, organize later:

```bash
# 1. Open your DAW once
# 2. Load all 18 plugins one by one
# 3. Screenshot each (Cmd+Shift+4 + Space + Click)
# 4. All screenshots save to Desktop automatically
# 5. Then organize them all at once:

cd ~/Projects/Active/AudioForge
./organize-screenshots.sh  # (coming next if you want this method)
```

**Pros:** Faster (stay in DAW the whole time)  
**Cons:** Have to organize 18 files afterward

---

## 📸 Screenshot Technique

### **Best Way to Capture Plugin Window:**

1. **Cmd + Shift + 4** (crosshair appears)
2. **Press Spacebar** (crosshair becomes camera icon)
3. **Hover over plugin window** (it highlights blue)
4. **Click once** (screenshot saved to Desktop)

✅ **Result:** Clean screenshot with subtle drop shadow, no extra space

### **Screenshot Settings (Check These First):**

```bash
# Check where screenshots save (should be Desktop)
defaults read com.apple.screencapture location

# If not Desktop, set it:
defaults write com.apple.screencapture location ~/Desktop
killall SystemUIServer
```

---

## 🎛️ DAW-Specific Tips

### **Logic Pro X**
```
1. File → New → Empty Project
2. Create Software Instrument track
3. Click empty instrument slot in channel strip
4. Choose "Audio Units" → "AudioForge" → [Plugin]
5. Double-click to open GUI
6. Screenshot the plugin window
```

### **Ableton Live**
```
1. Create MIDI track
2. Drag AudioForge plugin from browser onto track
3. Click "Plug-In" button to show GUI
4. Screenshot the plugin window
```

### **Reaper**
```
1. Insert → Virtual Instrument on New Track
2. Select "VST3: AudioForge - [Plugin]"
3. Click "UI" button to show GUI
4. Screenshot the plugin window
```

---

## ⚡ Pro Tips for Speed

### **Batch Loading Strategy:**

**Session 1: Mixing Plugins (5 plugins, 1 hour)**
Create 5 audio tracks in your DAW, load all 5 mixing plugins on the tracks, screenshot each.

**Session 2: Effects (3 plugins, 45 min)**
Same approach - 3 tracks, 3 effect plugins.

**Session 3: Synths (8 plugins, 2 hours)**
Create 8 instrument tracks, load all synths, screenshot each. Load an interesting preset first!

### **Make Plugins Look Good:**

**For Effects (EQ, Comp, etc.):**
- Play some audio through it
- Adjust controls so you can see them doing something
- EQ: Show a frequency boost/cut
- Compressor: Show gain reduction meter active

**For Synths:**
- Load a factory preset (shows preset name)
- Play a chord (shows notes active)
- Set interesting modulation values

---

## 🔧 Troubleshooting

### **Screenshot too large (>3 MB)?**
```bash
# Optimize manually:
sips -Z 1200 ~/Desktop/Screenshot*.png
```

### **Plugin won't load in DAW?**
```bash
# Rebuild and reinstall:
cd ~/Projects/Active/AudioForge/plugins/[PluginName]
cmake --build build --target install
```

### **Wrong screenshot captured?**
```bash
# Delete from Desktop and re-capture:
rm ~/Desktop/Screenshot*.png
# Then take screenshot again
```

---

## 📋 Organized Workflow (Recommended)

### **Phase 1: Setup (5 min)**
```bash
cd ~/Projects/Active/AudioForge
./capture-screenshots.sh
# Leave terminal open
```

### **Phase 2: Open DAW (2 min)**
- Launch Logic Pro / Ableton / Reaper
- Create a new empty project
- You'll load plugins one-by-one as script prompts

### **Phase 3: Capture Loop (3-4 hours)**
For each of 18 plugins:
1. Terminal says: "Capture [PluginName]"
2. Load plugin in DAW
3. Adjust window/controls to look good
4. Screenshot (Cmd+Shift+4 + Space + Click)
5. Press Enter in terminal (script processes it)
6. Close plugin in DAW
7. Repeat!

### **Phase 4: Verification (5 min)**
```bash
# Check all screenshots captured:
ls -1 website/public/screenshots/plugins/
# Should show 22 directories (was 4, now 22)
```

---

## 🎨 Visual Consistency Checklist

Make sure your new 18 screenshots match the existing 4:

- [ ] **Same macOS appearance** (Light or Dark mode)
- [ ] **Clean window capture** (no background windows visible)
- [ ] **Plugin UI fully visible** (no cut-off edges)
- [ ] **Readable text** (not blurry or pixelated)
- [ ] **Consistent file size** (~1-2 MB after optimization)
- [ ] **Proper naming** (`[pluginname]-main.png` in lowercase)

---

## 🚀 Ready to Start?

**Recommended approach for first-time:**

```bash
# 1. Run the automated script
cd ~/Projects/Active/AudioForge
./capture-screenshots.sh

# 2. Follow the on-screen prompts
# 3. Check SCREENSHOT_CHECKLIST.md to track progress
# 4. Take breaks every 5-6 plugins!
```

**Time estimate by session:**
- Session 1 (5 plugins): 60 minutes
- Session 2 (3 plugins): 45 minutes  
- Session 3 (2 plugins): 30 minutes
- Session 4 (4 plugins): 60 minutes
- Session 5 (4 plugins): 60 minutes

**Total: ~4.5 hours** (can pause/resume anytime!)

---

## 📞 Questions?

- Script not working? Check that you're in AudioForge root directory
- Screenshots look bad? Try adjusting plugin window size before capturing
- Want to skip a plugin? Press 's' when prompted
- Need to restart? Script remembers what's already captured

**Let's go! 📸**
