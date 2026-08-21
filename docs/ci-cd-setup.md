# CI/CD and Quality Gates Setup

This document explains the continuous integration, code coverage, and branch protection setup for AudioForge.

## Overview

AudioForge uses GitHub Actions for automated testing, building, and code quality enforcement. The CI/CD pipeline ensures that all code changes:

1. Build successfully on all platforms (macOS, Windows, Linux)
2. Pass all 34 unit tests (100% pass rate required)
3. Meet code coverage standards (target: 85%+)
4. Are reviewed before merging to main

## GitHub Actions Workflows

### 1. Build and Test (`build-test.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`
- Manual workflow dispatch

**What it does:**
- Builds SimpleGain and PanUtil plugins on all platforms
- Runs the full test suite (34 tests)
- Uploads VST3 plugin artifacts
- Validates plugin structure

**Platforms:**
- macOS (latest)
- Windows (latest)
- Linux (Ubuntu latest)

### 2. Code Coverage (`code-coverage.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`
- Manual workflow dispatch

**What it does:**
- Builds tests with coverage instrumentation (gcov/lcov)
- Runs all tests and captures coverage data
- Filters out external code (JUCE, system headers)
- Generates HTML coverage report
- Uploads to Codecov for tracking
- Warns if coverage falls below 85% threshold

**Coverage exclusions:**
- JUCE framework code (`*/JUCE/*`)
- Test code itself (`*/tests/*`)
- System headers (`/usr/*`)
- Build artifacts (`*/build/*`)

**Viewing coverage reports:**
- **Codecov dashboard**: Check the badge in README.md
- **GitHub Actions artifacts**: Download HTML report from workflow runs
- **Locally**: Run `cmake -B build -DENABLE_COVERAGE=ON && cmake --build build && ./build/TestRunner` then generate report with lcov

## Branch Protection Rules

To enable quality gates, configure these branch protection rules on the `main` branch:

### GitHub Repository Settings → Branches → Add Rule

**Branch name pattern:** `main`

#### Protection Rules to Enable:

1. **Require a pull request before merging**
   - ✅ Require approvals: 1
   - ✅ Dismiss stale pull request approvals when new commits are pushed
   - ✅ Require review from Code Owners (optional)

2. **Require status checks to pass before merging**
   - ✅ Require branches to be up to date before merging
   - **Required status checks:**
     - `build-and-test (macOS)`
     - `build-and-test (Windows)`
     - `build-and-test (Linux)`
     - `coverage / Generate Coverage Report`

3. **Require conversation resolution before merging**
   - ✅ All conversations must be resolved

4. **Do not allow bypassing the above settings**
   - ✅ Do not allow force pushes
   - ✅ Do not allow deletions

5. **Restrict who can push to matching branches** (optional)
   - Add team members who can push directly (for emergencies)

### Additional Recommended Settings:

**General Settings:**
- Automatically delete head branches (keeps repo clean)
- Allow merge commits (or choose squash/rebase based on preference)

**Actions Settings:**
- Allow GitHub Actions to create pull requests (for automated updates)

## Setting Up Codecov

1. Go to [codecov.io](https://codecov.io) and sign in with GitHub
2. Add your AudioForge repository
3. Copy the Codecov token
4. In GitHub: Settings → Secrets and variables → Actions → New repository secret
   - Name: `CODECOV_TOKEN`
   - Value: [paste token]

## Local Development Workflow

### Running Tests Locally

```bash
# Standard test run
cd tests
cmake -B build
cmake --build build
./build/TestRunner

# With coverage
cd tests
cmake -B build -DENABLE_COVERAGE=ON
cmake --build build
./build/TestRunner

# Generate coverage report locally
lcov --capture --directory build --output-file coverage.info
lcov --remove coverage.info '*/JUCE/*' '*/tests/*' '/usr/*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_html
open coverage_html/index.html  # macOS
```

### Pre-commit Checklist

Before pushing code:

1. ✅ All tests pass locally: `./build/TestRunner`
2. ✅ Code builds on your platform without errors
3. ✅ New features have corresponding tests
4. ✅ Code follows project style (C++17, JUCE patterns)
5. ✅ No debug print statements or commented code
6. ✅ Updated documentation if needed

### Pull Request Process

1. Create a feature branch: `git checkout -b feature/my-feature`
2. Make changes and commit: `git commit -m "Add feature"`
3. Push to GitHub: `git push -u origin feature/my-feature`
4. Open a pull request on GitHub
5. Wait for CI checks to pass (builds + tests + coverage)
6. Address review feedback
7. Once approved and checks pass, merge to main

## Coverage Targets

**Current Status:** Setting up baseline
**Target:** 85% line coverage

**What counts toward coverage:**
- Plugin processor code (`plugins/*/Source/PluginProcessor.cpp`)
- Plugin editor code (`plugins/*/Source/PluginEditor.cpp`)
- Shared DSP library (`shared/dsp/*.h`)

**What doesn't count:**
- JUCE framework code
- Test code itself
- System headers
- Third-party libraries

## Troubleshooting

### Tests Fail Locally But Pass in CI

- Ensure you've pulled the latest JUCE submodule: `git submodule update --init --recursive`
- Check that you're using a similar compiler version
- Clear build directory and rebuild from scratch

### Coverage Report Shows 0%

- Make sure you built with `-DENABLE_COVERAGE=ON`
- Run the tests (they generate coverage data)
- Check that .gcda files were created in build directory

### CI Workflow Fails to Upload Coverage

- Verify `CODECOV_TOKEN` is set in repository secrets
- Check Codecov service status
- Review workflow logs for specific error messages

## Future Enhancements

- [ ] Add static analysis (clang-tidy, cppcheck)
- [ ] Add memory leak detection (valgrind on Linux)
- [ ] Add performance benchmarks
- [ ] Add macOS code signing for releases
- [ ] Add Windows installer creation
- [ ] Add automatic changelog generation

## References

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Codecov Documentation](https://docs.codecov.io/)
- [LCOV Documentation](https://github.com/linux-test-project/lcov)
- [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md)
