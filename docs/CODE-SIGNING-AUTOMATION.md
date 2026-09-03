# Code Signing Automation Guide

Complete guide to automate AudioForge code signing and notarization in GitHub Actions.

## Overview

**Goal**: Zero manual signing steps - every tagged release automatically builds, signs, and notarizes DMG installers.

**What Gets Automated**:
- ✅ Build all plugins on macOS runner
- ✅ Sign all VST3/AU bundles with Developer ID
- ✅ Create DMG installer
- ✅ Sign DMG with Developer ID
- ✅ Notarize DMG with Apple
- ✅ Staple notarization ticket to DMG
- ✅ Upload signed DMG to GitHub Release

**Time Estimate**: ~20 minutes per release (automated)

---

## Prerequisites

1. **Apple Developer Program** ($99/year) - Already enrolled ✅
2. **Developer ID Certificate** - Already created ✅
3. **Notarization credentials** - App-specific password needed

---

## One-Time Setup (30 minutes)

### Step 1: Export Developer ID Certificate

On your local Mac:

```bash
# List your certificates
security find-identity -v -p codesigning | grep "Developer ID Application"

# Export certificate to .p12 file
# Replace "Your Name" with the exact name from the certificate
security export -k login.keychain \
  -t identities \
  -f pkcs12 \
  -o ~/Desktop/AudioForge-DevID.p12 \
  "Developer ID Application: Your Name (TEAM_ID)"
```

You'll be prompted to:
1. Enter your **Mac password** (to access keychain)
2. Set a **certificate password** (remember this - you'll need it for GitHub Secrets)

**Result**: `AudioForge-DevID.p12` on your Desktop

---

### Step 2: Convert Certificate to Base64

```bash
# Convert .p12 to base64 for GitHub Secrets storage
base64 -i ~/Desktop/AudioForge-DevID.p12 | pbcopy
```

This copies the base64 string to your clipboard. **Save it** - you'll paste it into GitHub Secrets.

---

### Step 3: Gather Notarization Credentials

You need three values:

1. **Apple ID**: Your developer account email (e.g., `you@example.com`)
2. **Team ID**: Found at https://developer.apple.com/account (e.g., `J6Z4A7W2G9`)
3. **App-Specific Password**:
   - Go to https://appleid.apple.com
   - Security → App-Specific Passwords
   - Generate new password named "AudioForge GitHub Actions"
   - Copy the password (e.g., `abcd-efgh-ijkl-mnop`)

---

### Step 4: Add GitHub Secrets

Go to your repository: **Settings** → **Secrets and variables** → **Actions** → **New repository secret**

Add these 5 secrets:

| Secret Name | Value | Example |
|-------------|-------|---------|
| `MACOS_CERTIFICATE` | Base64 certificate from Step 2 | `MIIKcQIBAzCCCjc...` |
| `MACOS_CERTIFICATE_PWD` | Password you set when exporting .p12 | `YourSecurePassword123` |
| `APPLE_ID` | Your Apple Developer email | `you@example.com` |
| `APPLE_TEAM_ID` | Team ID from developer.apple.com | `J6Z4A7W2G9` |
| `APPLE_APP_PASSWORD` | App-specific password from appleid.apple.com | `abcd-efgh-ijkl-mnop` |

**Security Note**: These secrets are encrypted and only accessible to GitHub Actions. Never commit them to the repository.

---

### Step 5: Test the Workflow

Once secrets are added, test the workflow:

```bash
# Create a test tag
git tag -a v1.8.1-test -m "Test automated signing"
git push origin v1.8.1-test

# Or trigger manually via GitHub UI:
# Actions → "Build and Sign Release" → Run workflow
```

Expected workflow steps:
1. ✅ Checkout code
2. ✅ Import certificate to runner keychain
3. ✅ Build all plugins
4. ✅ Sign plugins with Developer ID
5. ✅ Create DMG
6. ✅ Sign DMG
7. ✅ Notarize DMG (~5-10 minutes wait)
8. ✅ Staple notarization ticket
9. ✅ Upload to GitHub Release

**Total time**: ~20 minutes (build: 5 min, notarization wait: 10 min)

---

## Workflow File

The automation workflow is at: `.github/workflows/release.yml`

**Triggers**:
- **Automatic**: Push any tag matching `v*.*.*` (e.g., `v1.8.1`, `v2.0.0`)
- **Manual**: GitHub Actions UI → "Run workflow"

---

## Troubleshooting

### "Certificate not found" error

**Cause**: Certificate wasn't imported correctly to runner keychain.

**Fix**: Verify `MACOS_CERTIFICATE` and `MACOS_CERTIFICATE_PWD` secrets are correct.

```bash
# Re-export and re-encode certificate
security export -k login.keychain -t identities -f pkcs12 \
  -o ~/Desktop/AudioForge-DevID.p12 "Developer ID Application: ..."
base64 -i ~/Desktop/AudioForge-DevID.p12 | pbcopy
# Update GitHub Secret
```

---

### "Notarization failed: Invalid credentials"

**Cause**: Apple ID, Team ID, or App-Specific Password is incorrect.

**Fix**: Verify all three notarization secrets:

1. Check `APPLE_ID` matches your developer account email
2. Verify `APPLE_TEAM_ID` at https://developer.apple.com/account
3. Regenerate `APPLE_APP_PASSWORD`:
   - Go to https://appleid.apple.com
   - Delete old app-specific password
   - Create new one
   - Update GitHub Secret

---

### "Notarization stuck in progress"

**Cause**: Apple's notarization service can take 5-30 minutes.

**Fix**: Wait. The workflow polls every 30 seconds with a 1-hour timeout.

Check status manually:
```bash
xcrun notarytool history --keychain-profile "audioforge-notarize"
```

---

### DMG opens with "unidentified developer" warning

**Cause**: Notarization ticket wasn't stapled to DMG.

**Fix**: Verify stapling step succeeded in workflow logs. Manually staple:

```bash
xcrun stapler staple AudioForge-v1.8.1-macOS.dmg
xcrun stapler validate AudioForge-v1.8.1-macOS.dmg
```

---

## Local Testing (Before Automation)

Test the complete signing workflow locally first:

```bash
# 1. Build and sign plugins
CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)" \
  ./installers/build-installer.sh 1.8.1

# 2. Create and sign DMG
./installers/create-dmg.sh 1.8.1

# 3. Notarize DMG
./installers/notarize-dmg.sh releases/installers/AudioForge-v1.8.1-macOS.dmg

# 4. Verify
spctl -a -t open --context context:primary-signature \
  -v releases/installers/AudioForge-v1.8.1-macOS.dmg
```

Expected: `accepted`

---

## Cost & Limits

**Apple Developer Program**: $99/year (already enrolled)

**Notarization Limits**:
- **Free tier**: Unlimited notarization submissions
- **Wait time**: 5-30 minutes per submission (usually ~10 min)
- **GitHub Actions minutes**: 2,000 free minutes/month for public repos

**Estimated monthly cost**: $0 (within free tiers)

---

## Security Best Practices

1. **Never commit certificates or passwords** to the repository
2. **Use GitHub Secrets** for all sensitive credentials
3. **Rotate app-specific passwords** every 6-12 months
4. **Limit certificate access** - only export when necessary
5. **Delete .p12 files** after uploading to GitHub Secrets
6. **Enable 2FA** on Apple ID

---

## Future Improvements

1. **Parallel notarization**: Notarize plugins and DMG separately to save time
2. **Caching**: Cache JUCE build to speed up plugin compilation
3. **Multi-platform**: Extend workflow to build Windows installers
4. **Version bumping**: Auto-increment version numbers from git tags

---

## References

- [Apple Code Signing Guide](https://developer.apple.com/support/code-signing/)
- [Apple Notarization Guide](https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution)
- [GitHub Actions: macOS Signing](https://docs.github.com/en/actions/deployment/deploying-xcode-applications/installing-an-apple-certificate-on-macos-runners-for-xcode-development)
- [JUCE Code Signing](https://github.com/juce-framework/JUCE/blob/master/docs/macOS%20Signing%20and%20Notarization.md)
