# AudioForge Code Signing Quick Start Guide

Complete workflow for signing and notarizing AudioForge plugins and DMG installer on macOS.

## Prerequisites

- ✅ Active Apple Developer Program membership ($99/year)
- ✅ Xcode installed
- ✅ AudioForge repository cloned

## One-Time Setup (15 minutes)

### Step 1: Create Developer ID Certificate (5 min)

1. Open **Xcode** → **Settings** → **Accounts**
2. Select your Apple ID → **Manage Certificates**
3. Click **+** → **Developer ID Application**
4. Certificate will be created automatically

**Verify:**
```bash
security find-identity -v -p codesigning | grep "Developer ID Application"
```

You should see: `Developer ID Application: Your Name (TEAM_ID)`

### Step 2: Configure Code Signing (2 min)

Run the automated setup script:

```bash
cd /path/to/AudioForge
./installers/setup-codesign.sh
```

This script will:
- ✅ Find your Developer ID certificate
- ✅ Add `CODESIGN_IDENTITY` to `~/.zshrc`
- ✅ Verify configuration

**After running:**
- Close terminal and open a new one (to load new environment)
- Or run: `source ~/.zshrc`

### Step 3: Set Up Notarization Credentials (5 min)

**3a. Create App-Specific Password:**
1. Go to https://appleid.apple.com
2. Sign in → **Security** → **App-Specific Passwords**
3. Click **+** → Name it "AudioForge Notarization"
4. Copy the generated password (save it!)

**3b. Store Credentials in Keychain:**

Run the automated setup script:

```bash
./installers/setup-notarization.sh
```

You'll need:
- Apple ID email (your developer account email)
- Team ID (found at developer.apple.com/account)
- App-specific password (from step 3a)

**Verify:**
```bash
xcrun notarytool store-credentials --list
```

You should see: `audioforge-notarize`

---

## Build and Sign Workflow (Every Release)

### Step 4: Build and Sign Plugins (5 min)

```bash
cd /path/to/AudioForge
./build-installer.sh
```

This script automatically:
- ✅ Builds all 10 plugins in Release mode
- ✅ Collects VST3 and AU plugins
- ✅ Signs all plugins with Developer ID (if `CODESIGN_IDENTITY` is set)
- ✅ Creates staging directory with install scripts

**Verify signing:**
```bash
codesign --verify --deep --strict --verbose=2 \
    releases/staging/AudioForge\ Plugins/VST3/SimpleGain.vst3
```

Expected: `valid on disk` and `satisfies its Designated Requirement`

### Step 5: Create Signed DMG (2 min)

```bash
./installers/create-dmg.sh
```

This creates: `releases/installers/AudioForge-v1.1.0-macOS.dmg`

The DMG will be automatically signed if `CODESIGN_IDENTITY` is set.

**Verify DMG signature:**
```bash
codesign --verify --verbose releases/installers/AudioForge-v1.1.0-macOS.dmg
```

### Step 6: Notarize DMG (15-30 min wait)

```bash
./installers/notarize-dmg.sh releases/installers/AudioForge-v1.1.0-macOS.dmg
```

This script:
1. Submits DMG to Apple for notarization
2. Waits for Apple approval (typically 5-30 minutes)
3. Staples notarization ticket to DMG
4. Verifies notarization

**Progress:** You'll see real-time status updates. Wait for "Accepted" status.

**Verify notarization:**
```bash
spctl --assess --type install --verbose releases/installers/AudioForge-v1.1.0-macOS.dmg
```

Expected: `source=Notarized Developer ID`

### Step 7: Test on Clean macOS (5 min)

**Best practice:** Test on a clean macOS system or VM before distributing.

1. Copy DMG to test Mac
2. Double-click DMG
3. **Expected:** No Gatekeeper warnings
4. Run `Install AudioForge.command`
5. **Expected:** Smooth installation
6. Open DAW and scan plugins
7. **Expected:** All 10 plugins load without errors

### Step 8: Upload to GitHub Release

```bash
# Create GitHub release
gh release create v1.1.0 \
    --title "AudioForge v1.1.0 - Signed and Notarized" \
    --notes "First release with code signing and notarization. Zero Gatekeeper warnings on macOS." \
    releases/installers/AudioForge-v1.1.0-macOS.dmg
```

Or upload manually via GitHub web interface.

---

## Troubleshooting

### "No Developer ID certificate found"

**Solution:**
1. Verify you have an active Apple Developer account
2. Create certificate in Xcode (see Step 1)
3. Run `./installers/setup-codesign.sh` again

### "CODESIGN_IDENTITY not set"

**Solution:**
1. Run `./installers/setup-codesign.sh`
2. Close terminal and open a new one
3. Verify: `echo $CODESIGN_IDENTITY`

### "Notarization failed: Invalid credentials"

**Solution:**
1. Create app-specific password at appleid.apple.com
2. Run `./installers/setup-notarization.sh` again
3. Verify: `xcrun notarytool store-credentials --list`

### "Notarization rejected: Invalid signature"

**Solution:**
1. Verify all plugins are signed: `codesign --verify --deep releases/staging/AudioForge\ Plugins/VST3/*.vst3`
2. Verify DMG is signed: `codesign --verify releases/installers/AudioForge-v1.1.0-macOS.dmg`
3. Rebuild if needed: `./build-installer.sh && ./installers/create-dmg.sh`

### "DMG shows Gatekeeper warning"

**Solution:**
- DMG was not notarized, or notarization ticket was not stapled
- Run: `./installers/notarize-dmg.sh releases/installers/AudioForge-v1.1.0-macOS.dmg`
- Verify: `spctl --assess --type install --verbose <dmg>`

---

## Quick Reference

### One-Time Setup Checklist

- [ ] Create Developer ID certificate in Xcode
- [ ] Run `./installers/setup-codesign.sh`
- [ ] Create app-specific password at appleid.apple.com
- [ ] Run `./installers/setup-notarization.sh`

### Every Release Checklist

- [ ] Run `./build-installer.sh` (builds and signs plugins)
- [ ] Run `./installers/create-dmg.sh` (creates and signs DMG)
- [ ] Run `./installers/notarize-dmg.sh <dmg>` (notarizes DMG)
- [ ] Test DMG on clean macOS (no Gatekeeper warnings)
- [ ] Upload to GitHub Release

### Verification Commands

```bash
# Verify certificate exists
security find-identity -v -p codesigning

# Verify environment variable
echo $CODESIGN_IDENTITY

# Verify plugin signature
codesign --verify --deep --strict releases/staging/AudioForge\ Plugins/VST3/SimpleGain.vst3

# Verify DMG signature
codesign --verify --verbose releases/installers/AudioForge-v1.1.0-macOS.dmg

# Verify notarization
spctl --assess --type install --verbose releases/installers/AudioForge-v1.1.0-macOS.dmg
```

---

## Time Estimates

| Task | First Time | Subsequent |
|------|-----------|------------|
| One-time setup | 15 min | - |
| Build & sign plugins | 10 min | 5 min |
| Create DMG | 3 min | 2 min |
| Notarize DMG | 30 min wait | 15 min wait |
| Test | 10 min | 5 min |
| **Total** | **~70 min** | **~30 min** |

**Note:** Notarization wait time varies (5-60 minutes). Plan accordingly.

---

## Support

- **Documentation:** `docs/signing-macos.md` (detailed guide)
- **Issues:** https://github.com/kentin0-fiz0l/AudioForge/issues
- **Apple Developer:** https://developer.apple.com/support

---

## License

This guide is part of the AudioForge project (MIT License).
