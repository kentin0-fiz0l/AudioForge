# GitHub Actions Workflows

This directory contains CI/CD workflows for the AudioForge project.

## Workflows Overview

### 1. Build and Test (`build-test.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`
- Manual trigger via workflow_dispatch

**What it does:**
- Builds all plugins (SimpleGain, PanUtil) on macOS, Windows, and Linux
- Uses matrix strategy for parallel platform builds
- Caches JUCE build artifacts for faster subsequent builds
- Runs tests (when available)
- Uploads build artifacts for 7 days
- Validates plugin structure

**Optimization features:**
- Parallel builds across platforms
- JUCE dependency caching
- CMake parallel compilation (`--parallel`)
- Fail-fast disabled to see all platform results
- Concurrency groups to cancel outdated runs

**Platform-specific:**
- **macOS**: Uses latest macOS runner, Xcode toolchain
- **Windows**: Uses latest Windows runner, MSVC toolchain
- **Linux**: Installs audio dev dependencies (ALSA, JACK, X11, etc.)

### 2. Release (`release.yml`)

**Triggers:**
- Git tag push matching `v*.*.*` (e.g., `v1.0.0`)
- Manual trigger with version input

**What it does:**
1. Creates GitHub Release with changelog
2. Builds plugins for all platforms in parallel
3. Packages plugins into ZIP files:
   - Individual plugin ZIPs (e.g., `SimpleGain-1.0.0-macOS.zip`)
   - Combined bundle (e.g., `AudioForge-1.0.0-macOS.zip`)
4. Uploads all ZIPs as release assets

**Release workflow:**
```
Tag pushed → Create Release → Build (macOS/Windows/Linux) → Package → Upload Assets
```

**Artifacts produced per platform:**
- `SimpleGain-{version}-{platform}.zip`
- `PanUtil-{version}-{platform}.zip`
- `AudioForge-{version}-{platform}.zip` (combined bundle)

### 3. Code Quality (`code-quality.yml`)

**Triggers:**
- Push to `main` or `develop`
- Pull requests
- Manual trigger

**Checks performed:**
- **Format Check**: clang-format verification
- **Lint**: clang-tidy static analysis
- **Security Scan**: Trivy vulnerability scanning
- **Dependencies**: JUCE submodule verification

## Using the Workflows

### Running a build

Push or create a PR:
```bash
git push origin main
```

Or trigger manually:
1. Go to Actions tab in GitHub
2. Select "Build and Test"
3. Click "Run workflow"

### Creating a release

**Method 1: Tag push (recommended)**
```bash
git tag v1.0.0
git push origin v1.0.0
```

**Method 2: Manual trigger**
1. Go to Actions → Release
2. Click "Run workflow"
3. Enter version (e.g., `1.0.0`)

### Downloading artifacts

After a successful build:
1. Go to Actions tab
2. Click on the workflow run
3. Scroll to "Artifacts" section
4. Download `AudioForge-macOS`, `AudioForge-Windows`, or `AudioForge-Linux`

## Caching Strategy

### JUCE Build Cache

We cache JUCE build artifacts to speed up builds:
- Cache key includes OS and JUCE directory hash
- Restore keys allow partial cache hits
- Typical speedup: 5-10 minutes on subsequent builds

**Cache locations:**
- `plugins/*/build/_deps`
- `plugins/*/build/JUCE`

### Cache invalidation

Cache is automatically invalidated when:
- JUCE submodule is updated
- OS/platform changes

## Build Times

Approximate build times (no cache):

| Platform | SimpleGain | PanUtil | Total |
|----------|------------|---------|-------|
| macOS    | 8 min      | 8 min   | 16 min |
| Windows  | 10 min     | 10 min  | 20 min |
| Linux    | 7 min      | 7 min   | 14 min |

With cache: ~5-8 minutes per platform

## Optimization Tips

### 1. Parallel builds
Both plugins build in parallel steps, not sequentially.

### 2. Matrix strategy
All platforms build simultaneously, not one after another.

### 3. Fail-fast disabled
See results from all platforms even if one fails.

### 4. Concurrency groups
Cancel outdated workflow runs when new commits are pushed.

### 5. Conditional steps
Linux dependencies only install on Linux runners.

## Environment Variables

| Variable | Value | Purpose |
|----------|-------|---------|
| `BUILD_TYPE` | `Release` | CMake build configuration |
| `CMAKE_VERSION` | `3.15.x` | CMake version for consistency |

## Secrets Required

Currently none. Future additions:

- `SPACES_ACCESS_KEY` - DigitalOcean Spaces for CDN uploads
- `SPACES_SECRET_KEY` - DigitalOcean Spaces secret
- `DATABASE_URL` - FluxStudio database for metadata updates

## Troubleshooting

### Build fails on Linux

**Error**: Missing audio libraries
**Fix**: Dependencies are installed in workflow. Check if new JUCE modules require additional packages.

### Build fails on Windows

**Error**: MSBuild not found
**Fix**: Ensure `windows-latest` runner is used (includes MSVC).

### Cache not working

**Error**: Builds still slow
**Fix**:
1. Check cache hit/miss in workflow logs
2. Verify JUCE submodule hasn't changed
3. Clear cache in Settings → Actions → Caches

### Release artifacts missing

**Error**: ZIP files not uploaded
**Fix**: Check build logs for VST3 output paths. Ensure plugins built successfully.

## Future Improvements

### Planned additions:
- [ ] AU (Audio Unit) format for macOS
- [ ] AAX format for Pro Tools
- [ ] Code signing for macOS/Windows
- [ ] Automated plugin validation (PluginVal)
- [ ] Performance benchmarks
- [ ] Memory leak detection (Valgrind/AddressSanitizer)
- [ ] Upload to DigitalOcean Spaces (CDN)
- [ ] Update FluxStudio database on release
- [ ] Automated changelog generation
- [ ] Version bumping automation

### Testing enhancements:
- [ ] JUCE UnitTestRunner integration
- [ ] Plugin loading tests
- [ ] Audio processing correctness tests
- [ ] UI rendering tests (headless)

## Workflow File Locations

- `.github/workflows/build-test.yml` - CI builds
- `.github/workflows/release.yml` - Release automation
- `.github/workflows/code-quality.yml` - Code quality checks
- `.github/dependabot.yml` - Dependency updates

## Related Documentation

- [DEPLOYMENT.md](../../DEPLOYMENT.md) - Manual deployment guide
- [QUICKSTART.md](../../QUICKSTART.md) - Getting started
- [README.md](../../README.md) - Project overview

## Support

For CI/CD issues:
1. Check workflow logs in Actions tab
2. Search existing GitHub Issues
3. Create new issue with workflow run link
