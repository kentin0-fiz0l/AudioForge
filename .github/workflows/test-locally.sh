#!/bin/bash
# Test GitHub Actions workflows locally using act
# https://github.com/nektos/act

set -e

echo "=== AudioForge CI/CD Local Testing ==="
echo ""

# Check if act is installed
if ! command -v act &> /dev/null; then
    echo "❌ 'act' is not installed"
    echo ""
    echo "Install with: brew install act"
    echo "or visit: https://github.com/nektos/act"
    exit 1
fi

echo "✓ 'act' is installed"
echo ""

# Function to run workflow
run_workflow() {
    local workflow=$1
    local job=$2

    echo "=== Testing $workflow${job:+ ($job)} ==="

    if [ -n "$job" ]; then
        act -W ".github/workflows/$workflow" -j "$job" --dry-run
    else
        act -W ".github/workflows/$workflow" --dry-run
    fi

    echo ""
}

# Test each workflow
echo "1. Testing build-test workflow..."
run_workflow "build-test.yml"

echo "2. Testing release workflow..."
run_workflow "release.yml"

echo "3. Testing code-quality workflow..."
run_workflow "code-quality.yml"

echo "=== All workflows validated successfully ==="
echo ""
echo "To actually run a workflow locally:"
echo "  act -W .github/workflows/build-test.yml"
echo ""
echo "To run a specific job:"
echo "  act -W .github/workflows/build-test.yml -j build-and-test"
echo ""
echo "To run with a specific matrix:"
echo "  act -W .github/workflows/build-test.yml -j build-and-test --matrix os:macos-latest"
