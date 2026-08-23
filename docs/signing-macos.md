# macOS Code Signing and Notarization Guide

This guide covers how to sign AudioForge plugins and notarize the DMG installer for macOS, eliminating Gatekeeper security warnings.

## Prerequisites

### 1. Apple Developer Program Membership

**Cost**: $99/year

**Steps**:
1. Go to https://developer.apple.com
2. Click "Account" → "Enroll"
3. Complete enrollment (requires Apple ID)
4. Wait 24-48 hours for approval

**Why needed**: Code signing and notarization require an active Apple Developer account.

### 2. Developer ID Application Certificate

**Steps** (after enrollment approved):
1. Open Xcode
2. Go to **Xcode** → **Settings** (or **Preferences**)
3. Select **Accounts** tab
4. Click your Apple ID
5. Click **Manage Certificates**
6. Click **+** → **Developer ID Application**
7. Certificate will be created and stored in Keychain

**Verify certificate**:
```bash
security find-identity -v -p codesigning
```

You should see an entry like:
```
1) ABC123DEF456 "Developer ID Application: Your Name (TEAM_ID)"
```

### 3. App-Specific Password (for notarization)

**Steps**:
1. Go to https://appleid.apple.com
2. Sign in with your Apple ID
3. Go to **Security** → **App-Specific Passwords**
4. Click **Generate Password**
5. Name it "AudioForge Notarization"
6. Save the generated password securely

**Store password in Keychain** (recommended):
```bash
xcrun notarytool store-credentials "audioforge-notarize" \
    --apple-id "your@email.com" \
    --team-id "TEAM_ID" \
    --password "app-specific-password"
```

This stores credentials in Keychain so you don't need to enter them every time.

---

## Signing Workflow

### 1. Sign Individual Plugins

The `build-installer.sh` script automatically signs plugins after building them (if `CODESIGN_IDENTITY` is set).

**Environment variable**:
```bash
export CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"
```

Add to your `~/.zshrc` or `~/.bash_profile` to persist across sessions:
```bash
echo 'export CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"' >> ~/.zshrc
source ~/.zshrc
```

**Manual signing** (if needed):
```bash
# Sign VST3 plugin
codesign --force --deep --sign "$CODESIGN_IDENTITY" \
    --options runtime \
    ~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3

# Sign AU plugin
codesign --force --deep --sign "$CODESIGN_IDENTITY" \
    --options runtime \
    ~/Library/Audio/Plug-Ins/Components/SimpleGain.component
```

**Verify signature**:
```bash
codesign --verify --deep --strict --verbose=2 \
    ~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3
```

Expected output:
```
~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3: valid on disk
~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3: satisfies its Designated Requirement
```

### 2. Sign DMG Installer

After creating the DMG (via `create-dmg-simple.sh`), sign the DMG itself:

```bash
codesign --force --sign "$CODESIGN_IDENTITY" \
    --options runtime \
    AudioForge-v1.1.0-macOS.dmg

# Verify
codesign --verify --deep --strict --verbose=2 AudioForge-v1.1.0-macOS.dmg
```

---

## Notarization Workflow

Notarization is required for macOS 10.15 Catalina and later. It proves to Apple that your software doesn't contain malware.

### 1. Submit for Notarization

**Using stored credentials** (recommended):
```bash
xcrun notarytool submit AudioForge-v1.1.0-macOS.dmg \
    --keychain-profile "audioforge-notarize" \
    --wait
```

**Using inline credentials** (not recommended):
```bash
xcrun notarytool submit AudioForge-v1.1.0-macOS.dmg \
    --apple-id "your@email.com" \
    --password "app-specific-password" \
    --team-id "TEAM_ID" \
    --wait
```

**Expected output**:
```
Conducting pre-submission checks for AudioForge-v1.1.0-macOS.dmg and initiating connection to the Apple notary service...
Submission ID received
  id: abc-123-def-456
Successfully uploaded file
  id: abc-123-def-456
  path: AudioForge-v1.1.0-macOS.dmg
Waiting for processing to complete...
Current status: Accepted
```

**Processing time**: Usually 1-5 minutes, can take up to 1 hour during peak times.

### 2. Check Notarization Status

**Get status**:
```bash
xcrun notarytool info abc-123-def-456 \
    --keychain-profile "audioforge-notarize"
```

**Get detailed log** (if rejected):
```bash
xcrun notarytool log abc-123-def-456 \
    --keychain-profile "audioforge-notarize" \
    notarization-log.json

# View log
cat notarization-log.json | jq
```

### 3. Staple Notarization Ticket

After successful notarization, **staple** the ticket to the DMG:

```bash
xcrun stapler staple AudioForge-v1.1.0-macOS.dmg
```

**Verify stapling**:
```bash
xcrun stapler validate AudioForge-v1.1.0-macOS.dmg
```

Expected output:
```
The validate action worked!
```

**Check Gatekeeper assessment**:
```bash
spctl --assess --type install --verbose AudioForge-v1.1.0-macOS.dmg
```

Expected output:
```
AudioForge-v1.1.0-macOS.dmg: accepted
source=Notarized Developer ID
```

---

## Automated Signing in build-installer.sh

The `build-installer.sh` script has been updated to automatically sign plugins if `CODESIGN_IDENTITY` is set.

**Key sections**:

```bash
# After building all plugins, sign them
if [ -n "$CODESIGN_IDENTITY" ]; then
    echo "→ Signing macOS plugins..."
    for plugin in "${VST3_DIR}"/*.vst3; do
        codesign --force --deep --sign "$CODESIGN_IDENTITY" \
            --options runtime "$plugin"
        echo "  ✓ Signed $(basename "$plugin")"
    done

    for plugin in "${AU_DIR}"/*.component; do
        codesign --force --deep --sign "$CODESIGN_IDENTITY" \
            --options runtime "$plugin"
        echo "  ✓ Signed $(basename "$plugin")"
    done
fi
```

**Usage**:
```bash
export CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"
./build-installer.sh
```

---

## Automated Notarization Script

Create `notarize-dmg.sh` in the installers directory:

```bash
#!/bin/bash
set -e

DMG_FILE="$1"
KEYCHAIN_PROFILE="audioforge-notarize"

if [ -z "$DMG_FILE" ]; then
    echo "Usage: ./notarize-dmg.sh AudioForge-v1.1.0-macOS.dmg"
    exit 1
fi

if [ ! -f "$DMG_FILE" ]; then
    echo "Error: DMG file not found: $DMG_FILE"
    exit 1
fi

echo "→ Submitting $DMG_FILE for notarization..."
xcrun notarytool submit "$DMG_FILE" \
    --keychain-profile "$KEYCHAIN_PROFILE" \
    --wait

echo "→ Stapling notarization ticket..."
xcrun stapler staple "$DMG_FILE"

echo "→ Verifying notarization..."
xcrun stapler validate "$DMG_FILE"
spctl --assess --type install --verbose "$DMG_FILE"

echo "✓ $DMG_FILE is now notarized and ready for distribution!"
```

**Make executable**:
```bash
chmod +x installers/notarize-dmg.sh
```

**Usage**:
```bash
./installers/notarize-dmg.sh releases/AudioForge-v1.1.0-macOS.dmg
```

---

## CI/CD Integration (GitHub Actions)

To automate signing and notarization in GitHub Actions, you need to:

### 1. Export Certificate from Keychain

```bash
# Export Developer ID Application certificate
security export -k login.keychain -t identities \
    -f pkcs12 -P "password" \
    -o DeveloperIDApplication.p12
```

### 2. Add GitHub Secrets

Go to **Settings** → **Secrets and variables** → **Actions** → **New repository secret**:

- `MACOS_CERTIFICATE`: Base64-encoded .p12 file
  ```bash
  base64 -i DeveloperIDApplication.p12 | pbcopy
  ```
- `MACOS_CERTIFICATE_PWD`: Password for .p12 file
- `APPLE_ID`: Your Apple ID email
- `APPLE_APP_PASSWORD`: App-specific password
- `APPLE_TEAM_ID`: Your Team ID (e.g., ABC123DEF4)

### 3. Update .github/workflows/release.yml

Add signing and notarization steps:

```yaml
- name: Import Code Signing Certificate
  if: runner.os == 'macOS'
  run: |
    echo "${{ secrets.MACOS_CERTIFICATE }}" | base64 --decode > certificate.p12
    security create-keychain -p actions temp.keychain
    security default-keychain -s temp.keychain
    security unlock-keychain -p actions temp.keychain
    security import certificate.p12 -k temp.keychain -P "${{ secrets.MACOS_CERTIFICATE_PWD }}" -T /usr/bin/codesign
    security set-key-partition-list -S apple-tool:,apple: -s -k actions temp.keychain
    export CODESIGN_IDENTITY="Developer ID Application"

- name: Sign Plugins
  if: runner.os == 'macOS'
  run: |
    for plugin in ~/Library/Audio/Plug-Ins/VST3/*.vst3; do
      codesign --force --deep --sign "$CODESIGN_IDENTITY" --options runtime "$plugin"
    done

- name: Notarize DMG
  if: runner.os == 'macOS'
  run: |
    xcrun notarytool submit releases/AudioForge-*.dmg \
      --apple-id "${{ secrets.APPLE_ID }}" \
      --password "${{ secrets.APPLE_APP_PASSWORD }}" \
      --team-id "${{ secrets.APPLE_TEAM_ID }}" \
      --wait
    xcrun stapler staple releases/AudioForge-*.dmg
```

---

## Testing Notarized Installer

### On Your Development Machine

1. **Move DMG to Downloads folder** (to simulate fresh download)
2. **Double-click DMG** → No warnings should appear
3. **Run Install AudioForge.command** → Should install without prompts
4. **Open a DAW** and load plugins → Should work normally

### On a Fresh macOS Machine

Test on a Mac that has never seen AudioForge before:

1. Download DMG from GitHub releases
2. Double-click DMG
3. **Expected**: No "unidentified developer" warning
4. Run Install AudioForge.command
5. Open Logic Pro / Reaper / Ableton
6. Load plugins
7. **Expected**: Plugins load without security prompts

### Common Issues

**Issue**: "This app is damaged and can't be opened"
- **Cause**: DMG not notarized or notarization ticket not stapled
- **Fix**: Run `xcrun stapler staple AudioForge-v1.1.0-macOS.dmg`

**Issue**: "Developer cannot be verified"
- **Cause**: Certificate expired or revoked
- **Fix**: Regenerate Developer ID Application certificate

**Issue**: Notarization fails with "Invalid Code Signature"
- **Cause**: Plugin not signed with hardened runtime (`--options runtime`)
- **Fix**: Re-sign plugin with `--options runtime` flag

---

## Cost Summary

| Item | Cost | Frequency |
|------|------|-----------|
| Apple Developer Program | $99 | Annual |
| **Total** | **$99/year** | - |

**One-time setup**: ~2-3 hours (enrollment, certificate setup, testing)
**Per-release overhead**: ~5-10 minutes (signing, notarization, verification)

---

## References

- [Apple Code Signing Guide](https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution)
- [notarytool Documentation](https://developer.apple.com/documentation/technotes/tn3147-migrating-to-the-latest-notarization-tool)
- [JUCE Code Signing](https://forum.juce.com/t/code-signing-and-notarization-on-macos/33447)

---

**Last Updated**: 2026-08-23
