# Installing LaTeX for PDF Conversion

To convert the AudioForge user manuals from Markdown to PDF, you need Pandoc and a LaTeX distribution.

## Quick Install (macOS)

### Option 1: BasicTeX (Recommended - Smaller)

BasicTeX is a minimal LaTeX distribution (~100 MB):

```bash
# Install BasicTeX via Homebrew
brew install --cask basictex

# Restart your terminal, or run:
eval "$(/usr/libexec/path_helper)"

# Verify installation
xelatex --version
```

### Option 2: Full MacTeX (Complete - Larger)

Full MacTeX includes everything (~4 GB):

```bash
# Install MacTeX via Homebrew
brew install --cask mactex

# Restart terminal
eval "$(/usr/libexec/path_helper)"
```

---

## Linux Installation

### Ubuntu/Debian

```bash
# Install LaTeX packages
sudo apt update
sudo apt install texlive-xetex texlive-fonts-recommended

# Verify installation
xelatex --version
```

### Fedora/RHEL

```bash
# Install LaTeX packages
sudo dnf install texlive-xetex texlive-collection-fontsrecommended

# Verify installation
xelatex --version
```

### Arch Linux

```bash
# Install LaTeX
sudo pacman -S texlive-most

# Verify installation
xelatex --version
```

---

## Windows Installation

### Option 1: MiKTeX (Recommended)

1. Download MiKTeX installer from https://miktex.org/download
2. Run the installer (MiKTeX will download packages on-demand)
3. Open Command Prompt and verify:
   ```cmd
   xelatex --version
   ```

### Option 2: TeX Live

1. Download TeX Live from https://www.tug.org/texlive/acquire-netinstall.html
2. Run the installer (large download, ~4 GB)
3. Add to PATH if needed
4. Verify in PowerShell:
   ```powershell
   xelatex --version
   ```

---

## Verify Pandoc Installation

After installing LaTeX, verify Pandoc is installed:

```bash
# Check Pandoc version
pandoc --version

# If not installed:
# macOS:   brew install pandoc
# Windows: choco install pandoc
# Linux:   sudo apt install pandoc
```

---

## Convert Manuals to PDF

Once both Pandoc and LaTeX are installed:

```bash
# Navigate to manuals directory
cd docs/manuals

# Run conversion script
./convert-to-pdf.sh
```

**Expected output**:
```
========================================
AudioForge Manual PDF Converter
========================================

✓ Pandoc found: pandoc 3.10.2
✓ XeLaTeX found

Found 10 manuals to convert

→ Converting SimpleGain-Manual.md...
✓ Created SimpleGain-Manual.pdf
→ Converting PanUtil-Manual.md...
✓ Created PanUtil-Manual.pdf
...

========================================
Conversion complete!
========================================

Success: 10 / 10

Created PDFs:
  SimpleGain-Manual.pdf (245K)
  PanUtil-Manual.pdf (189K)
  ...

Total size: 2.1M

Done!
```

---

## Manual Conversion (Individual Files)

If you prefer to convert manuals individually:

```bash
# Single manual conversion
pandoc SimpleGain-Manual.md -o SimpleGain-Manual.pdf \
    --pdf-engine=xelatex \
    --variable geometry:margin=1in \
    --variable fontsize=11pt \
    --toc

# Custom settings
pandoc BasicSynth-Manual.md -o BasicSynth-Manual.pdf \
    --pdf-engine=xelatex \
    --variable geometry:margin=0.75in \
    --variable fontsize=12pt \
    --variable colorlinks=true \
    --toc \
    --toc-depth=3
```

---

## Troubleshooting

### Error: "pandoc: xelatex not found"

**Solution**: LaTeX is not installed or not in PATH. Follow installation steps above.

### Error: "! LaTeX Error: File `[package].sty' not found"

**Solution**: Install missing LaTeX package:

**macOS (BasicTeX)**:
```bash
sudo tlmgr update --self
sudo tlmgr install [package]
```

**Linux**:
```bash
sudo apt install texlive-latex-extra
```

**Windows (MiKTeX)**: Packages install automatically on first use.

### Error: "pandoc: command not found"

**Solution**: Install Pandoc:
```bash
# macOS
brew install pandoc

# Linux
sudo apt install pandoc

# Windows
choco install pandoc
```

### Conversion is slow

**Expected**: First PDF conversion can take 30-60 seconds as LaTeX compiles fonts and packages. Subsequent conversions are faster (~5-10 seconds each).

---

## Alternative: Online Conversion

If you don't want to install LaTeX locally, use online converters:

1. **Markdown to PDF**: https://www.markdowntopdf.com/
   - Upload .md file
   - Download PDF

2. **Dillinger**: https://dillinger.io/
   - Paste Markdown content
   - Export → PDF

3. **HackMD**: https://hackmd.io/
   - Import Markdown
   - Export as PDF

**Note**: Online converters may not preserve custom formatting from the conversion script.

---

## File Sizes

Expected PDF sizes (approximate):

| Manual | Size |
|--------|------|
| SimpleGain | ~250 KB |
| PanUtil | ~190 KB |
| BasicSynth | ~280 KB |
| CleanDelay | ~170 KB |
| SimpleEQ | ~160 KB |
| SimpleComp | ~160 KB |
| WaveShaper | ~140 KB |
| Saturation | ~140 KB |
| ChorusFlanger | ~150 KB |
| Reverb | ~200 KB |
| **Total** | **~2.0 MB** |

---

## Next Steps

After conversion:
1. PDFs are created in `docs/manuals/`
2. Add to `.gitignore` if you don't want to commit them
3. Upload to website for user downloads
4. Link from documentation pages

---

**Last Updated**: 2026-08-23
