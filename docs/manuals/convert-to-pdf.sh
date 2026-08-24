#!/bin/bash
set -e

# AudioForge Manual PDF Converter
# Converts all Markdown manuals to professional PDF format

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}AudioForge Manual PDF Converter${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if Pandoc is installed
if ! command -v pandoc &> /dev/null; then
    echo -e "${RED}Error: Pandoc is not installed${NC}"
    echo ""
    echo "Install Pandoc:"
    echo "  macOS:   brew install pandoc"
    echo "  Windows: choco install pandoc"
    echo "  Linux:   sudo apt install pandoc"
    echo ""
    exit 1
fi

# Check if XeLaTeX is installed
if ! command -v xelatex &> /dev/null; then
    echo -e "${RED}Error: XeLaTeX is not installed${NC}"
    echo ""
    echo "Install BasicTeX (macOS):"
    echo "  brew install --cask basictex"
    echo "  eval \"\$(/usr/libexec/path_helper)\""
    echo ""
    echo "Install LaTeX (Linux):"
    echo "  sudo apt install texlive-xetex texlive-fonts-recommended"
    echo ""
    echo "Install LaTeX (Windows):"
    echo "  Download MiKTeX from https://miktex.org/download"
    echo ""
    exit 1
fi

echo -e "${GREEN}✓ Pandoc found: $(pandoc --version | head -n1)${NC}"
echo -e "${GREEN}✓ XeLaTeX found${NC}"
echo ""

# Count manuals
MANUAL_COUNT=$(ls *-Manual.md 2>/dev/null | wc -l | tr -d ' ')

if [ "$MANUAL_COUNT" -eq 0 ]; then
    echo -e "${RED}Error: No manual files found (*-Manual.md)${NC}"
    exit 1
fi

echo -e "${BLUE}Found ${MANUAL_COUNT} manuals to convert${NC}"
echo ""

# Convert each manual
SUCCESS_COUNT=0
FAIL_COUNT=0

for file in *-Manual.md; do
    if [ -f "$file" ]; then
        PDF_FILE="${file%.md}.pdf"
        echo -e "${YELLOW}→ Converting ${file}...${NC}"

        # Convert with Pandoc
        if pandoc "$file" -o "$PDF_FILE" \
            --pdf-engine=xelatex \
            --variable geometry:margin=1in \
            --variable fontsize=11pt \
            --variable colorlinks=true \
            --variable linkcolor=blue \
            --variable urlcolor=blue \
            --variable toccolor=black \
            --toc \
            --toc-depth=2 \
            --metadata title="AudioForge $(basename "$file" -Manual.md) User Manual" \
            --metadata date="$(date +%Y-%m-%d)" \
            --metadata author="AudioForge" \
            2>/dev/null; then

            echo -e "${GREEN}✓ Created ${PDF_FILE}${NC}"
            SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
        else
            echo -e "${RED}✗ Failed to convert ${file}${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi
    fi
done

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Conversion complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "${GREEN}Success: ${SUCCESS_COUNT} / ${MANUAL_COUNT}${NC}"

if [ $FAIL_COUNT -gt 0 ]; then
    echo -e "${RED}Failed:  ${FAIL_COUNT} / ${MANUAL_COUNT}${NC}"
fi

echo ""
echo "PDF files created in: $(pwd)"
echo ""

# List created PDFs
if [ $SUCCESS_COUNT -gt 0 ]; then
    echo -e "${BLUE}Created PDFs:${NC}"
    ls -lh *.pdf 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
    echo ""
fi

# Calculate total size
if [ $SUCCESS_COUNT -gt 0 ]; then
    TOTAL_SIZE=$(du -sh *.pdf 2>/dev/null | awk '{sum+=$1} END {print sum}')
    echo -e "${GREEN}Total size: $(du -ch *.pdf 2>/dev/null | tail -1 | awk '{print $1}')${NC}"
fi

echo ""
echo -e "${BLUE}Done!${NC}"
