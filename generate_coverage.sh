#!/bin/bash
set -e

# AudioForge Code Coverage Generation Script
# This script builds the test suite with coverage enabled,
# runs all tests, and generates HTML coverage reports.

echo "==================================="
echo "AudioForge Coverage Report Generator"
echo "==================================="

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Navigate to tests directory
cd "$(dirname "$0")/tests"

echo -e "${BLUE}Step 1: Cleaning previous build...${NC}"
rm -rf build
rm -rf coverage_report
mkdir -p build

echo -e "${BLUE}Step 2: Configuring with coverage enabled...${NC}"
cmake -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug

echo -e "${BLUE}Step 3: Building test suite...${NC}"
cmake --build build

echo -e "${BLUE}Step 4: Running tests...${NC}"
./build/TestRunner

echo -e "${BLUE}Step 5: Generating coverage data...${NC}"
# Capture coverage info
lcov --capture \
     --directory build \
     --output-file build/coverage.info \
     --rc branch_coverage=1 \
     --ignore-errors inconsistent,gcov,format,count,unsupported,deprecated

echo -e "${BLUE}Step 6: Filtering coverage data...${NC}"
# Remove external code from coverage
lcov --remove build/coverage.info \
     '*/JUCE/*' \
     '*/tests/*' \
     '*/usr/*' \
     '*/Applications/*' \
     '*/Library/*' \
     --output-file build/coverage_filtered.info \
     --rc branch_coverage=1 \
     --ignore-errors inconsistent,format,unused

echo -e "${BLUE}Step 7: Generating HTML report...${NC}"
# Generate HTML report
genhtml build/coverage_filtered.info \
        --output-directory coverage_report \
        --title "AudioForge Code Coverage" \
        --legend \
        --branch-coverage \
        --rc branch_coverage=1 \
        --ignore-errors inconsistent,corrupt,format,empty,category,unsupported

echo ""
echo -e "${GREEN}==================================="
echo "Coverage Report Generated!"
echo "===================================${NC}"
echo ""

# Display coverage summary
lcov --summary build/coverage_filtered.info \
     --rc branch_coverage=1 \
     --ignore-errors inconsistent,corrupt,format

echo ""
echo -e "${YELLOW}HTML report available at:${NC}"
echo "  $(pwd)/coverage_report/index.html"
echo ""
echo -e "${YELLOW}To view the report:${NC}"
echo "  open coverage_report/index.html"
echo ""
