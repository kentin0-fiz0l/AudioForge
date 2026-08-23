# AudioForge Windows Code Signing Script
# Signs VST3 plugins and installer executable with Authenticode certificate

param(
    [Parameter(Mandatory=$false)]
    [string]$CertificatePath = "",

    [Parameter(Mandatory=$false)]
    [string]$CertificatePassword = "",

    [Parameter(Mandatory=$false)]
    [string]$TimestampServer = "http://timestamp.digicert.com"
)

$ErrorActionPreference = "Stop"

# Colors for output
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

Write-Host "==========================================" -ForegroundColor Blue
Write-Host "AudioForge Windows Code Signing" -ForegroundColor Blue
Write-Host "==========================================" -ForegroundColor Blue
Write-Host ""

# Check if certificate path is provided
if (-not $CertificatePath) {
    Write-ColorOutput Yellow "ERROR: Certificate path not provided"
    Write-Host "Usage: .\sign.ps1 -CertificatePath 'C:\path\to\certificate.pfx' -CertificatePassword 'password'"
    Write-Host ""
    Write-Host "Or set environment variables:"
    Write-Host "  `$env:WINDOWS_CERT_PATH = 'C:\path\to\certificate.pfx'"
    Write-Host "  `$env:WINDOWS_CERT_PASSWORD = 'password'"
    exit 1
}

# Check if certificate exists
if (-not (Test-Path $CertificatePath)) {
    Write-ColorOutput Red "ERROR: Certificate file not found: $CertificatePath"
    exit 1
}

# Find VST3 plugins
$VST3Path = "..\..\releases\staging\AudioForge Plugins\VST3"
if (-not (Test-Path $VST3Path)) {
    Write-ColorOutput Red "ERROR: VST3 directory not found: $VST3Path"
    Write-Host "Run build-installer.sh first to create staging directory"
    exit 1
}

# Sign VST3 plugins
Write-Host "→ Signing VST3 plugins..." -ForegroundColor Blue
$pluginCount = 0

Get-ChildItem -Path $VST3Path -Filter "*.vst3" -Recurse -File | ForEach-Object {
    Write-Host "  Signing $($_.Name)..." -ForegroundColor Yellow

    & signtool sign `
        /f $CertificatePath `
        /p $CertificatePassword `
        /tr $TimestampServer `
        /td sha256 `
        /fd sha256 `
        $_.FullName

    if ($LASTEXITCODE -eq 0) {
        Write-ColorOutput Green "  ✓ Signed $($_.Name)"
        $pluginCount++
    } else {
        Write-ColorOutput Red "  ✗ Failed to sign $($_.Name)"
        exit 1
    }
}

Write-Host ""
Write-ColorOutput Green "✓ Successfully signed $pluginCount VST3 plugins"

# Sign installer executable (if it exists)
$InstallerPath = "AudioForge-Setup-v1.1.0.exe"
if (Test-Path $InstallerPath) {
    Write-Host ""
    Write-Host "→ Signing installer executable..." -ForegroundColor Blue

    & signtool sign `
        /f $CertificatePath `
        /p $CertificatePassword `
        /tr $TimestampServer `
        /td sha256 `
        /fd sha256 `
        $InstallerPath

    if ($LASTEXITCODE -eq 0) {
        Write-ColorOutput Green "✓ Signed $InstallerPath"
    } else {
        Write-ColorOutput Red "✗ Failed to sign $InstallerPath"
        exit 1
    }

    # Verify signature
    Write-Host ""
    Write-Host "→ Verifying signature..." -ForegroundColor Blue
    & signtool verify /pa /v $InstallerPath

    if ($LASTEXITCODE -eq 0) {
        Write-ColorOutput Green "✓ Signature verified successfully"
    } else {
        Write-ColorOutput Red "✗ Signature verification failed"
        exit 1
    }
} else {
    Write-ColorOutput Yellow "⚠ Installer executable not found: $InstallerPath"
    Write-Host "Run makensis on audioforge.nsi first to create the installer"
}

Write-Host ""
Write-ColorOutput Green "==========================================="
Write-ColorOutput Green "All files signed successfully!"
Write-ColorOutput Green "==========================================="
