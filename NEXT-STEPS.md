# AudioForge Roadmap: Next Steps

**Status**: Ready to begin Month 1 (Production Readiness - v1.1.0)
**Date**: 2026-08-27

---

## ✅ What's Already Complete

### Code & Infrastructure
- ✅ Code signing implementation in `build-installer.sh` (lines 140-170)
- ✅ DMG notarization script ready (`installers/notarize-dmg.sh`)
- ✅ Windows installer NSIS script ready (`installers/windows/audioforge.nsi`)
- ✅ Linux build scripts ready (`installers/linux/build-packages.sh`)
- ✅ Analytics approach chosen (GitHub built-in)
- ✅ Analytics script created (`scripts/check-downloads.sh`)

### Documentation
- ✅ macOS signing guide (`docs/signing-macos.md`)
- ✅ Community setup guide (`docs/COMMUNITY-SETUP.md`)
- ✅ Roadmap document with timeline and budget
- ✅ Analytics documentation (`docs/ANALYTICS.md`)

---

## 🔧 Manual Setup Required (You Need to Do This)

### 1. Enable GitHub Discussions (5 minutes)

**Why**: Unlock community engagement features

**Steps**:
1. Go to https://github.com/kentin0-fiz0l/AudioForge/settings
2. Scroll to "Features" section
3. Check "Discussions" checkbox
4. Click "Set up discussions"
5. Create categories:
   - 📢 **Announcements** (admin-only)
   - 💬 **General**
   - ❓ **Q&A**
   - 💡 **Ideas**
   - 🎵 **Show and Tell**
   - 🛠️ **Development**

**Verify**:
```bash
gh api repos/kentin0-fiz0l/AudioForge --jq .has_discussions
# Should return: true
```

**Status**: ⏳ Waiting for you to enable

---

### 2. Create Apple Developer ID Certificate (5 minutes)

**Why**: Required for macOS code signing

**Prerequisites**:
- ✅ Apple Developer Program membership ($99/year) - you have this
- ✅ Xcode installed

**Steps**:
1. Open **Xcode**
2. Go to **Xcode** → **Settings** (or Preferences)
3. Click **Accounts** tab
4. Select your Apple ID
5. Click **Manage Certificates**
6. Click **+** → **Developer ID Application**

**Verify**:
```bash
security find-identity -v -p codesigning
# Should show: "Developer ID Application: Your Name (TEAM_ID)"
```

**Set environment variable**:
```bash
# Copy the full identity name from the security command above
export CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"

# Add to .zshrc to persist
echo 'export CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"' >> ~/.zshrc
source ~/.zshrc
```

**Status**: ⏳ Waiting for you to create certificate

---

### 3. Set Up Notarization Credentials (10 minutes)

**Why**: Required for DMG notarization (eliminates Gatekeeper warnings)

**Steps**:

1. **Generate app-specific password**:
   - Go to https://appleid.apple.com
   - Sign in → Security → App-Specific Passwords
   - Click "Generate Password"
   - Name it "AudioForge Notarization"
   - Save the password securely

2. **Store credentials in Keychain**:
```bash
xcrun notarytool store-credentials "audioforge-notarize" \
    --apple-id "your@email.com" \
    --team-id "YOUR_TEAM_ID" \
    --password "your-app-specific-password"
```

Replace:
- `your@email.com` with your Apple ID
- `YOUR_TEAM_ID` with your team ID (from certificate name)
- `your-app-specific-password` with the generated password

**Verify**:
```bash
xcrun notarytool history --keychain-profile "audioforge-notarize"
# Should show your notarization history (may be empty initially)
```

**Status**: ⏳ Waiting for you to set up

---

### 4. Purchase Windows Code Signing Certificate (Optional but Recommended)

**Why**: Eliminates Windows SmartScreen warnings

**Options**:
- **Standard OV Certificate**: ~$200/year
  - Vendors: Sectigo, SSL.com
  - Initial SmartScreen warnings, builds reputation over time

- **EV Certificate**: ~$400/year (RECOMMENDED)
  - Vendors: DigiCert, GlobalSign
  - Instant SmartScreen trust, best user experience

**Recommendation**: EV certificate for professional image

**Status**: ⏳ Your decision on budget/timing

---

## 🚀 What Happens Next (Automated Once Setup is Complete)

### After You Complete Manual Setup Above:

1. **Build signed macOS installer**:
```bash
./build-installer.sh
# → Automatically signs all plugins with your certificate
# → Creates DMG with signed plugins
```

2. **Notarize DMG**:
```bash
./installers/notarize-dmg.sh AudioForge-v1.1.0-macOS.dmg
# → Submits to Apple (5-15 min wait)
# → Staples ticket to DMG
# → Verifies notarization
```

3. **Build Windows installer**:
```bash
# On Windows machine (or VM)
makensis installers\windows\audioforge.nsi
# → Creates AudioForge-Setup-v1.1.0.exe

# Sign (after purchasing certificate)
.\installers\windows\sign.ps1
```

4. **Build Linux packages**:
```bash
cd installers/linux
./build-packages.sh
# → Creates .deb, .rpm, AppImage
```

5. **Test on VMs** (see Task #9 in task list)

6. **Upload to GitHub Release v1.1.0** (see Task #10 in task list)

---

## 📋 Task List Summary

**Total Tasks**: 10 (Month 1: Production Readiness)

**Immediate (Can Do Now)**:
- ✅ Task #1: Enable GitHub Discussions (manual, 5 min)
- ✅ Task #2: Choose analytics approach (COMPLETE)
- ⏳ Task #3: macOS code signing (waiting for certificate)

**Blocked (Waiting on Prerequisites)**:
- Task #4: DMG notarization (blocked by Task #3)
- Task #5: Windows NSIS installer (ready to build)
- Task #6: Windows code signing certificate (your decision)
- Task #7: Sign Windows installer (blocked by Tasks #5, #6)
- Task #8: Linux packages (ready to build)
- Task #9: Test all installers (blocked by Tasks #4, #7, #8)
- Task #10: Update GitHub Release (blocked by Task #9)

**View full task list**:
```bash
# I've created a task tracking system for the roadmap
# You can see progress in the current Claude Code session
```

---

## ⏱️ Time Estimates

### Manual Setup (You)
- Enable Discussions: 5 minutes
- Create certificate: 5 minutes
- Set up notarization: 10 minutes
- Purchase Windows cert: 30-60 minutes (research + purchase)
- **Total**: ~1 hour

### Automated Execution (Scripts)
- Build macOS installer: 5 minutes
- Notarize DMG: 10-20 minutes (Apple's service)
- Build Windows installer: 5 minutes
- Sign Windows installer: 2 minutes
- Build Linux packages: 10 minutes
- **Total**: ~30-45 minutes

### Testing (You + Me)
- Test on macOS VMs: 20-30 minutes
- Test on Windows VMs: 20-30 minutes
- Test on Linux VMs: 15-20 minutes
- **Total**: ~1-1.5 hours

### **Grand Total for Month 1**: ~3-4 hours of active work

---

## 💡 Recommendations

### Do This Week
1. ✅ Enable GitHub Discussions (5 min)
2. ✅ Create Developer ID certificate (5 min)
3. ✅ Set up notarization (10 min)
4. ✅ Test macOS signing workflow (15 min)

**Result**: Signed macOS DMG ready by end of week

### Do This Month
5. Purchase Windows EV certificate ($400)
6. Build and sign Windows installer
7. Build Linux packages
8. Test all installers on VMs
9. Update GitHub Release v1.1.0

**Result**: v1.1.0 Complete with signed installers for all platforms

### Do Next Month (Month 2: Content Creation)
- Video tutorials (15-20 hours)
- Screenshot capture (3-5 hours)
- Website updates (2-3 hours)

**Result**: v1.2.0 Complete with professional documentation

---

## 🎯 Success Criteria

**v1.1.0 Success** (End of Month 1):
- ✅ macOS DMG signed and notarized (zero Gatekeeper warnings)
- ✅ Windows .exe signed (zero SmartScreen warnings with EV cert)
- ✅ Linux packages available (.deb, .rpm, AppImage)
- ✅ All installers tested on clean VMs
- ✅ GitHub Release updated with all platform installers

**Current Blockers**:
1. You need to create Apple Developer ID certificate
2. You need to set up notarization credentials
3. You need to decide on Windows code signing (budget/timing)

---

## 📞 Questions?

**If you need help with**:
- Certificate creation → See `docs/signing-macos.md`
- Notarization setup → See `docs/signing-macos.md` (lines 40-59)
- Windows signing → See `installers/windows/README.md`
- Any other questions → Just ask me!

**Ready to proceed?**
1. Complete the manual setup steps above
2. Let me know when done
3. I'll help execute the automated build/test workflow

---

**Last Updated**: 2026-08-27
