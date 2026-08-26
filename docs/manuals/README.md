# AudioForge User Manuals

This directory contains comprehensive user manuals for all 10 AudioForge plugins.

## Available Manuals

1. **[SimpleGain](SimpleGain-Manual.md)** - Gain/volume control with metering
2. **[PanUtil](PanUtil-Manual.md)** - Stereo panning and width control
3. **[BasicSynth](BasicSynth-Manual.md)** - Subtractive synthesizer
4. **[CleanDelay](CleanDelay-Manual.md)** - Stereo delay with ping-pong
5. **[SimpleEQ](SimpleEQ-Manual.md)** - 3-band parametric EQ
6. **[SimpleComp](SimpleComp-Manual.md)** - Compressor with auto makeup gain
7. **[WaveShaper](WaveShaper-Manual.md)** - Waveshaping distortion
8. **[Saturation](Saturation-Manual.md)** - Analog-style saturation
9. **[ChorusFlanger](ChorusFlanger-Manual.md)** - Modulation effects
10. **[Reverb](Reverb-Manual.md)** - Algorithmic reverb

## Formats

### Markdown (Current)

All manuals are written in Markdown format for:
- Easy version control (Git)
- Cross-platform compatibility
- Simple editing
- Web rendering (GitHub, website)

### Converting to PDF

To convert these manuals to PDF format for distribution:

#### Method 1: Python/WeasyPrint (Recommended - No LaTeX Required)

**Quick and easy**, no system dependencies needed:

```bash
cd docs/manuals

# Create virtual environment and install dependencies (first time only)
python3 -m venv .venv-pdf
source .venv-pdf/bin/activate
pip install weasyprint markdown pymdown-extensions

# Run conversion
python3 convert-to-pdf-python.py
```

**Expected output**:
```
========================================
AudioForge Manual PDF Converter
(Python/WeasyPrint)
========================================

Found 10 manuals to convert

→ Converting SimpleGain-Manual.md...
✓ Created SimpleGain-Manual.pdf (116 KB)

Success: 10 / 10
Total size: 0.5 MB

Done!
```

**Pros**:
- ✅ No LaTeX installation required
- ✅ Fast conversion (~5 seconds total)
- ✅ Works on all platforms (macOS, Linux, Windows)
- ✅ Smaller file sizes

**Cons**:
- ⚠️ Slightly different formatting than LaTeX

---

#### Method 2: Pandoc/XeLaTeX (Traditional - High Quality)

**Professional LaTeX output** with advanced typography:

**1. Install dependencies**:

See [INSTALL-LATEX.md](INSTALL-LATEX.md) for detailed installation instructions.

```bash
# macOS
brew install pandoc
brew install --cask basictex
eval "$(/usr/libexec/path_helper)"

# Linux (Ubuntu/Debian)
sudo apt install pandoc texlive-xetex texlive-fonts-recommended

# Windows
choco install pandoc
# Download MiKTeX from https://miktex.org/download
```

**2. Run conversion script**:

```bash
cd docs/manuals
./convert-to-pdf.sh
```

**Expected output**:
```
✓ Pandoc found: pandoc 3.10.2
✓ XeLaTeX found

Found 10 manuals to convert

→ Converting SimpleGain-Manual.md...
✓ Created SimpleGain-Manual.pdf

Success: 10 / 10
Total size: 2.1M

Done!
```

#### Manual Conversion (Individual Files)

**Convert single manual**:
```bash
cd docs/manuals
pandoc SimpleGain-Manual.md -o SimpleGain-Manual.pdf \
    --pdf-engine=xelatex \
    --variable geometry:margin=1in \
    --variable fontsize=11pt \
    --toc
```

#### Option 2: Online Converters

- **Markdown to PDF**: https://www.markdowntopdf.com/
- **Dillinger**: https://dillinger.io/ (export to PDF)

#### Option 3: VS Code Extensions

- **Markdown PDF** extension: Right-click `.md` file → "Markdown PDF: Export (PDF)"

### Converting to EPUB (eBook)

For accessibility and mobile reading:

```bash
pandoc SimpleGain-Manual.md -o SimpleGain-Manual.epub \
    --toc \
    --epub-metadata=metadata.xml
```

## Manual Structure

Each manual follows this structure:

1. **Overview**: What the plugin does, when to use it
2. **Quick Start**: Get started in 5 minutes
3. **User Interface**: UI layout and controls
4. **Parameters**: Detailed parameter descriptions
5. **Use Cases**: Real-world application examples
6. **Tips & Techniques**: Pro workflows and tricks
7. **Technical Specifications**: Audio specs, CPU usage
8. **Troubleshooting**: Common issues and solutions
9. **Support**: Where to get help

## Contributing

To improve or update manuals:

1. Edit the Markdown file directly
2. Follow the existing structure and tone
3. Use clear, concise language (avoid jargon)
4. Include practical examples and settings
5. Submit a pull request

## License

All manuals are licensed under **Creative Commons Attribution 4.0** (CC BY 4.0):
- ✅ Share, copy, redistribute
- ✅ Adapt, remix, transform
- ✅ Commercial use allowed
- ℹ️ Attribution required

---

**AudioForge Documentation**
© 2026 AudioForge
https://audioforge.fluxstudio.art
