#!/bin/bash
#
# Format all AudioForge C++ source files with clang-format
# Run with --check to verify without modifying files
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_ROOT"

CHECK_ONLY=false
if [ "$1" = "--check" ] || [ "$1" = "-c" ]; then
    CHECK_ONLY=true
fi

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Code Formatting${NC}"
if [ "$CHECK_ONLY" = true ]; then
    echo -e "${BLUE}Mode: CHECK (no modifications)${NC}"
else
    echo -e "${BLUE}Mode: FORMAT (will modify files)${NC}"
fi
echo -e "${BLUE}======================================${NC}"
echo

# Find all C++ source and header files, excluding build directories and JUCE
FILES=$(find plugins shared -type f \( -name "*.cpp" -o -name "*.h" \) \
    ! -path "*/build/*" \
    ! -path "*/builds/*" \
    ! -path "*_artefacts/*" \
    ! -path "*/JuceLibraryCode/*" \
    ! -path "*/JUCE/*")

TOTAL_FILES=$(echo "$FILES" | wc -l | tr -d ' ')
FORMATTED=0
NEEDS_FORMATTING=0

echo -e "${BLUE}Found $TOTAL_FILES files to check${NC}"
echo

if [ "$CHECK_ONLY" = true ]; then
    # Check mode - verify formatting without modifying
    for file in $FILES; do
        if ! clang-format --dry-run --Werror "$file" > /dev/null 2>&1; then
            echo -e "${YELLOW}⚠ Needs formatting: $file${NC}"
            NEEDS_FORMATTING=$((NEEDS_FORMATTING + 1))
        fi
    done

    if [ $NEEDS_FORMATTING -eq 0 ]; then
        echo -e "${GREEN}✓ All $TOTAL_FILES files are correctly formatted!${NC}"
        exit 0
    else
        echo
        echo -e "${RED}✗ $NEEDS_FORMATTING files need formatting${NC}"
        echo -e "${BLUE}Run without --check to format them:${NC}"
        echo -e "${BLUE}  ./scripts/format-code.sh${NC}"
        exit 1
    fi
else
    # Format mode - modify files
    for file in $FILES; do
        if ! clang-format --dry-run --Werror "$file" > /dev/null 2>&1; then
            echo -e "${YELLOW}Formatting: $file${NC}"
            clang-format -i "$file"
            FORMATTED=$((FORMATTED + 1))
        fi
    done

    if [ $FORMATTED -eq 0 ]; then
        echo -e "${GREEN}✓ All $TOTAL_FILES files already correctly formatted!${NC}"
    else
        echo
        echo -e "${GREEN}✓ Formatted $FORMATTED files${NC}"
        echo -e "${BLUE}Review changes with: git diff${NC}"
        echo -e "${BLUE}Commit changes with: git add -A && git commit -m \"Apply clang-format\"${NC}"
    fi
fi
