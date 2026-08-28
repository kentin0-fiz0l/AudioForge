# AudioForge Windows Build Environment Setup Script
# Run this script in PowerShell on the Windows VM

Write-Host "======================================"
Write-Host "AudioForge Windows Build Setup"
Write-Host "======================================"

# Check if running as Administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "ERROR: Please run this script as Administrator" -ForegroundColor Red
    exit 1
}

# 1. Install Chocolatey (package manager)
Write-Host "`n[1/5] Installing Chocolatey package manager..."
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# 2. Install Git
Write-Host "`n[2/5] Installing Git for Windows..."
choco install git -y

# 3. Install CMake
Write-Host "`n[3/5] Installing CMake..."
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y

# 4. Install NSIS
Write-Host "`n[4/5] Installing NSIS..."
choco install nsis -y

# 5. Refresh environment variables
Write-Host "`n[5/5] Refreshing environment variables..."
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

Write-Host "`n======================================"
Write-Host "Setup Complete!" -ForegroundColor Green
Write-Host "======================================"

Write-Host "`nInstalled tools:"
Write-Host "- Git: $(git --version)"
Write-Host "- CMake: $(cmake --version | Select-Object -First 1)"
Write-Host "- NSIS: $(makensis /VERSION)"
Write-Host "- Visual Studio: Already installed in VM"

Write-Host "`nNext steps:"
Write-Host "1. Close and reopen PowerShell"
Write-Host "2. Clone AudioForge repository:"
Write-Host "   git clone https://github.com/kentin0-fiz0l/AudioForge.git"
Write-Host "3. Follow BUILD-INSTRUCTIONS.md"
