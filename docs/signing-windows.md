# Windows Code Signing Guide

This guide covers how to sign AudioForge plugins and installer for Windows using Authenticode code signing, eliminating SmartScreen warnings.

## Prerequisites

### 1. Purchase Code Signing Certificate

**Cost**: $200-400/year (varies by vendor and certificate type)

**Certificate Types**:
1. **Standard Code Signing** (~$200/year)
   - Digital signature validates identity
   - SmartScreen warnings on first run (until reputation builds)
   - Suitable for initial release

2. **EV (Extended Validation) Code Signing** (~$400/year)
   - Higher trust level, instant SmartScreen reputation
   - Requires USB hardware token (physical security)
   - **Recommended** for professional distribution

**Recommended Vendors**:
- [DigiCert](https://www.digicert.com/signing/code-signing-certificates) - $200-400/year
- [Sectigo (formerly Comodo)](https://sectigo.com/ssl-certificates-tls/code-signing) - $180-350/year
- [SSL.com](https://www.ssl.com/code-signing/) - $200-400/year
- [GlobalSign](https://www.globalsign.com/en/code-signing-certificate) - $250-450/year

**Purchase Process** (varies by vendor):
1. Choose certificate type (Standard or EV)
2. Provide business/personal validation documents
   - For individuals: Government-issued ID, proof of address
   - For organizations: Business registration, D&B number
3. Complete validation (2-7 days for Standard, 3-14 days for EV)
4. Receive certificate (.pfx file) or USB token (EV)

### 2. Install Windows SDK (for signtool)

**On Windows**:

Download and install **Windows SDK**:
- https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/

Or install via **Visual Studio Installer**:
1. Open Visual Studio Installer
2. Modify your installation
3. Select **Individual components** tab
4. Check **Windows 10 SDK** or **Windows 11 SDK**

**Verify installation**:
```powershell
signtool /?
```

If not found, add to PATH:
```powershell
$env:Path += ";C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64"
```

### 3. Install NSIS (Nullsoft Scriptable Install System)

**Download NSIS**:
- https://nsis.sourceforge.io/Download

**Install**:
1. Run `nsis-3.09-setup.exe` (or latest version)
2. Accept defaults
3. NSIS will be installed to `C:\Program Files (x86)\NSIS`

**Verify installation**:
```powershell
makensis /VERSION
```

Add to PATH if needed:
```powershell
$env:Path += ";C:\Program Files (x86)\NSIS"
```

---

## Signing Workflow

### 1. Build Installer with NSIS

**Navigate to installers directory**:
```powershell
cd AudioForge\installers\windows
```

**Compile installer**:
```powershell
makensis audioforge.nsi
```

**Expected output**:
```
Processed 1 file, writing output:
Output: AudioForge-Setup-v1.1.0.exe
Install: 10 files, 25.3 MB
Uninstall: 10 files
```

**Result**: `AudioForge-Setup-v1.1.0.exe` created in `installers/windows/`

### 2. Sign Plugins and Installer

**Using PowerShell script** (recommended):
```powershell
.\sign.ps1 -CertificatePath "C:\path\to\certificate.pfx" -CertificatePassword "password"
```

**Using environment variables**:
```powershell
$env:WINDOWS_CERT_PATH = "C:\path\to\certificate.pfx"
$env:WINDOWS_CERT_PASSWORD = "password"
.\sign.ps1
```

**Manual signing** (signtool directly):
```powershell
# Sign installer
signtool sign /f certificate.pfx /p password `
    /tr http://timestamp.digicert.com `
    /td sha256 /fd sha256 `
    AudioForge-Setup-v1.1.0.exe

# Verify signature
signtool verify /pa /v AudioForge-Setup-v1.1.0.exe
```

**Expected output**:
```
Successfully signed: AudioForge-Setup-v1.1.0.exe

Signature verified successfully
```

### 3. Sign VST3 Plugins (Optional but Recommended)

```powershell
# Navigate to VST3 directory
cd ..\..\releases\staging\AudioForge Plugins\VST3

# Sign each plugin
Get-ChildItem -Filter "*.vst3" -Recurse -File | ForEach-Object {
    signtool sign /f C:\path\to\certificate.pfx /p password `
        /tr http://timestamp.digicert.com `
        /td sha256 /fd sha256 `
        $_.FullName
}
```

---

## Certificate Management

### Importing Certificate to Windows Certificate Store

**Import .pfx certificate**:
```powershell
# Via GUI
certmgr.msc
# Right-click "Personal" → "All Tasks" → "Import"
# Select .pfx file, enter password

# Via PowerShell
$cert = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2
$cert.Import("C:\path\to\certificate.pfx", "password", [System.Security.Cryptography.X509Certificates.X509KeyStorageFlags]::DefaultKeySet)
$store = New-Object System.Security.Cryptography.X509Certificates.X509Store("My", "CurrentUser")
$store.Open("ReadWrite")
$store.Add($cert)
$store.Close()
```

**Sign using certificate from store** (no password needed):
```powershell
signtool sign /n "Your Company Name" `
    /tr http://timestamp.digicert.com `
    /td sha256 /fd sha256 `
    AudioForge-Setup-v1.1.0.exe
```

### EV Certificate (USB Token)

**For EV certificates**:
1. Insert USB token
2. Install token drivers (provided by vendor)
3. Sign using token-specific certificate name:

```powershell
# List available certificates
signtool sign /?

# Sign using USB token certificate
signtool sign /n "Your Company Name" `
    /tr http://timestamp.digicert.com `
    /td sha256 /fd sha256 /v `
    AudioForge-Setup-v1.1.0.exe
```

---

## SmartScreen and Reputation Building

### Standard Certificate (Initial Warnings)

With a **Standard Code Signing Certificate**, Windows SmartScreen will show warnings on first run:

```
"Windows protected your PC"
"Microsoft Defender SmartScreen prevented an unrecognized app from starting"
```

**Why this happens**:
- New certificate has no reputation with Microsoft
- Windows needs time to build trust through user downloads

**How to build reputation**:
1. **Get downloads**: 100+ unique users installing
2. **Time**: 2-4 weeks minimum
3. **No malware reports**: Clean security record
4. **Consistent signing**: Use same certificate for all releases

**User workaround** (until reputation builds):
1. Click "More info"
2. Click "Run anyway"

### EV Certificate (Instant Reputation)

With an **EV Code Signing Certificate**:
- ✅ **No SmartScreen warnings** from day one
- ✅ Instant trust, no reputation building needed
- ✅ Professional appearance

**Recommendation**: If budget allows, invest in EV certificate ($400/year) to avoid user friction.

---

## Testing Signed Installer

### On Your Development Machine

1. **Build and sign installer**:
   ```powershell
   makensis audioforge.nsi
   .\sign.ps1 -CertificatePath "certificate.pfx" -CertificatePassword "password"
   ```

2. **Verify signature**:
   ```powershell
   signtool verify /pa /v AudioForge-Setup-v1.1.0.exe
   ```

3. **Check certificate details**:
   - Right-click `AudioForge-Setup-v1.1.0.exe`
   - Select **Properties** → **Digital Signatures** tab
   - Select signature → **Details**
   - Verify: "This digital signature is OK"

### On a Fresh Windows Machine

Test on a Windows 10/11 VM or fresh machine:

1. **Download installer** from GitHub releases
2. **Right-click installer** → **Properties** → **Digital Signatures**
   - Expected: Certificate details visible
3. **Run installer**:
   - **Standard cert**: May show SmartScreen warning (click "More info" → "Run anyway")
   - **EV cert**: No warnings, installs cleanly
4. **Verify installation**:
   - Check `C:\Program Files\Common Files\VST3\` for plugins
   - Open DAW (Reaper, FL Studio, Ableton) and rescan plugins
   - Load plugins and test

### Testing SmartScreen Behavior

**Simulate fresh download**:
```powershell
# Add "Zone.Identifier" to file (simulates download)
Set-Content -Path "AudioForge-Setup-v1.1.0.exe:Zone.Identifier" -Value "[ZoneTransfer]`r`nZoneId=3"
```

**Run installer** and observe SmartScreen behavior.

---

## CI/CD Integration (GitHub Actions)

### 1. Add GitHub Secrets

Go to **Settings** → **Secrets and variables** → **Actions** → **New repository secret**:

- `WINDOWS_CERT_BASE64`: Base64-encoded .pfx file
  ```powershell
  [Convert]::ToBase64String([IO.File]::ReadAllBytes("certificate.pfx")) | Set-Clipboard
  ```
- `WINDOWS_CERT_PASSWORD`: Certificate password

### 2. Update .github/workflows/release.yml

Add Windows signing steps:

```yaml
- name: Import Code Signing Certificate (Windows)
  if: runner.os == 'Windows'
  run: |
    $certBytes = [Convert]::FromBase64String("${{ secrets.WINDOWS_CERT_BASE64 }}")
    [IO.File]::WriteAllBytes("certificate.pfx", $certBytes)

- name: Build NSIS Installer (Windows)
  if: runner.os == 'Windows'
  run: |
    makensis installers\windows\audioforge.nsi

- name: Sign Installer (Windows)
  if: runner.os == 'Windows'
  run: |
    signtool sign /f certificate.pfx /p "${{ secrets.WINDOWS_CERT_PASSWORD }}" `
      /tr http://timestamp.digicert.com /td sha256 /fd sha256 `
      installers\windows\AudioForge-Setup-v1.1.0.exe

    # Verify signature
    signtool verify /pa /v installers\windows\AudioForge-Setup-v1.1.0.exe

- name: Upload Installer
  if: runner.os == 'Windows'
  uses: actions/upload-artifact@v3
  with:
    name: AudioForge-Windows-Installer
    path: installers/windows/AudioForge-Setup-v1.1.0.exe
```

---

## Troubleshooting

### Issue: "SignTool Error: No certificates were found"

**Cause**: Certificate not imported to certificate store, or .pfx file path incorrect

**Fix**:
```powershell
# Verify certificate file exists
Test-Path C:\path\to\certificate.pfx

# Import to certificate store
certmgr.msc
# Right-click "Personal" → "All Tasks" → "Import" → Select .pfx
```

### Issue: "Error: An error occurred while attempting to sign"

**Cause**: Incorrect password, or certificate expired

**Fix**:
```powershell
# Verify password by importing certificate
$cert = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2
$cert.Import("certificate.pfx", "password", "DefaultKeySet")

# Check certificate expiration
certutil -dump certificate.pfx
```

### Issue: "Timestamping failed"

**Cause**: Timestamp server unreachable (network issue)

**Fix**:
```powershell
# Try alternate timestamp server
signtool sign /f certificate.pfx /p password `
    /tr http://timestamp.sectigo.com `
    /td sha256 /fd sha256 `
    AudioForge-Setup-v1.1.0.exe
```

**Alternate timestamp servers**:
- `http://timestamp.digicert.com` (DigiCert)
- `http://timestamp.sectigo.com` (Sectigo)
- `http://timestamp.globalsign.com/scripts/timstamp.dll` (GlobalSign)

### Issue: SmartScreen warning persists after signing

**Cause**: Certificate has no reputation yet (Standard cert only)

**Fix**:
1. Verify signature is valid:
   ```powershell
   signtool verify /pa /v AudioForge-Setup-v1.1.0.exe
   ```
2. Build reputation through downloads (100+ users, 2-4 weeks)
3. Or upgrade to EV certificate for instant reputation

---

## Cost Summary

| Item | Cost | Frequency |
|------|------|-----------|
| Standard Code Signing Cert | $180-250 | Annual |
| EV Code Signing Cert | $350-450 | Annual |
| NSIS | Free | - |
| Windows SDK | Free | - |
| **Total (Standard)** | **$180-250/year** | - |
| **Total (EV)** | **$350-450/year** | - |

**Recommendation**: Start with Standard certificate ($200), upgrade to EV ($400) if budget allows and SmartScreen warnings impact user adoption.

**One-time setup**: ~2-3 hours (certificate purchase, validation, testing)
**Per-release overhead**: ~2-5 minutes (build installer, sign, verify)

---

## References

- [Microsoft Code Signing Guide](https://docs.microsoft.com/en-us/windows/win32/seccrypto/cryptography-tools)
- [SignTool Documentation](https://docs.microsoft.com/en-us/windows/win32/seccrypto/signtool)
- [NSIS Documentation](https://nsis.sourceforge.io/Docs/)
- [SmartScreen FAQ](https://docs.microsoft.com/en-us/windows/security/threat-protection/microsoft-defender-smartscreen/microsoft-defender-smartscreen-overview)

---

**Last Updated**: 2026-08-23
