# Sentry Crash Reporting Setup Guide

## Overview

Sentry provides real-time crash reporting and error tracking for AudioForge plugins. This gives us visibility into production failures and helps prioritize bug fixes.

## Benefits

- **40% fewer support tickets** via automated crash data
- **Stack traces** with exact line numbers
- **Breadcrumbs** showing user actions before crash
- **Release tracking** (which version crashed?)
- **User impact metrics** (how many users affected?)

## Setup Steps

### 1. Create Sentry Account (5 minutes)

1. Go to https://sentry.io/signup/
2. Choose "Free" plan (5,000 events/month, sufficient for <1,000 users)
3. Create a new project:
   - Platform: **C++ / Native**
   - Name: **AudioForge**
   - Alert frequency: **On every new issue**

### 2. Get Your DSN

After project creation, Sentry provides a DSN (Data Source Name):
```
https://[PROJECT_ID]@[ORG].ingest.sentry.io/[PROJECT_NUMBER]
```

**Important**: Keep this DSN secret! It's used to send crash reports.

### 3. Store DSN Securely

**For local development:**
```bash
# Add to .zshrc or .bashrc (never commit to git)
export SENTRY_DSN="your-dsn-here"
```

**For CI/CD (GitHub Actions):**
1. Go to repository Settings → Secrets → Actions
2. Add secret: `SENTRY_DSN` with your DSN value
3. Reference in workflows: `${{ secrets.SENTRY_DSN }}`

### 4. Integrate Sentry SDK

We use **sentry-native** (C/C++ SDK). Integration points:

**Option A: Header-only (recommended for quick start)**
```cpp
// In PluginProcessor.cpp
#include "sentry.h"

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize Sentry on first plugin load
    static bool sentryInitialized = false;
    if (!sentryInitialized)
    {
        sentry_options_t* options = sentry_options_new();
        sentry_options_set_dsn(options, std::getenv("SENTRY_DSN"));
        sentry_options_set_release(options, "audioforge@1.5.0");
        sentry_options_set_environment(options, "production");
        sentry_init(options);
        sentryInitialized = true;
    }
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    try
    {
        // Your existing processing code here
    }
    catch (const std::exception& e)
    {
        // Capture crash with context
        sentry_capture_event(sentry_value_new_message_event(
            SENTRY_LEVEL_ERROR,
            "processBlock",
            e.what()
        ));
        throw; // Re-throw to let DAW handle crash
    }
}
```

**Option B: CMake integration (production-ready)**

Add to plugin's `CMakeLists.txt`:
```cmake
# Fetch sentry-native
include(FetchContent)
FetchContent_Declare(
    sentry
    GIT_REPOSITORY https://github.com/getsentry/sentry-native.git
    GIT_TAG 0.6.6
)
FetchContent_MakeAvailable(sentry)

# Link against plugin target
target_link_libraries(${PROJECT_NAME} PRIVATE sentry)
```

### 5. Test Crash Capture

**Trigger a test crash:**
```cpp
// In PluginEditor.cpp, add a debug button
if (debugCrashButton.isDown())
{
    sentry_capture_event(sentry_value_new_message_event(
        SENTRY_LEVEL_INFO,
        "test",
        "Manual crash test from AudioForge"
    ));
    throw std::runtime_error("Test crash for Sentry validation");
}
```

**Verify in Sentry dashboard:**
1. Trigger the crash in your DAW
2. Check https://sentry.io/issues/
3. You should see the crash within 5 minutes

### 6. Production Best Practices

**What to capture:**
- ✅ Exceptions in `processBlock()` (audio thread crashes)
- ✅ Exceptions in `prepareToPlay()` (initialization failures)
- ✅ Exceptions in `setStateInformation()` (preset loading failures)
- ✅ Assertion failures in debug builds

**What NOT to capture:**
- ❌ Expected errors (invalid MIDI notes, out-of-range parameters)
- ❌ User input validation failures
- ❌ Debug logging

**Privacy:**
- ✅ Crash reports include: plugin version, OS version, stack trace
- ❌ No user data (audio, MIDI, preset names, file paths)
- ✅ Sentry GDPR-compliant, EU data residency available

### 7. Monitoring & Alerts

**Set up alerts:**
1. Sentry → Project Settings → Alerts
2. Create alert rule:
   - **Trigger**: "An issue is first seen"
   - **Action**: Send email to `your-email@example.com`
   - **Frequency**: Immediate

**Dashboard widgets:**
- Crash-free sessions (target: >99%)
- Most common crashes (fix these first)
- Crashes by plugin version
- Crashes by OS version

### 8. CI/CD Integration

Add to `.github/workflows/build-test.yml`:

```yaml
- name: Initialize Sentry Release
  if: github.event_name == 'release'
  run: |
    curl -sL https://sentry.io/get-cli/ | bash
    export SENTRY_AUTH_TOKEN=${{ secrets.SENTRY_AUTH_TOKEN }}
    sentry-cli releases new "audioforge@${{ github.ref_name }}"
    sentry-cli releases set-commits "audioforge@${{ github.ref_name }}" --auto
    sentry-cli releases finalize "audioforge@${{ github.ref_name }}"
```

This tracks which commits are in each release, enabling source-map lookup in crash reports.

## Cost Analysis

**Free Tier:**
- 5,000 events/month
- 1 month data retention
- 1 project
- Unlimited users

**Sufficient for**: <1,000 active users at <0.5% crash rate

**Paid Tier** ($26/month):
- 50,000 events/month
- 90 days retention
- Advanced search/filtering

**When to upgrade**: >1,000 active users or >200 crashes/month

## Success Metrics

After 1 month of Sentry integration:
- ✅ Know exact crash rate (target: <1%)
- ✅ Top 5 crashes identified and fixed
- ✅ 40% reduction in "plugin crashes my DAW" support tickets
- ✅ Stack traces with line numbers (no more guessing)

## Troubleshooting

**Problem**: No crashes appearing in Sentry dashboard

**Solutions**:
1. Verify DSN is correct: `echo $SENTRY_DSN`
2. Check network connectivity (Sentry requires HTTPS out)
3. Verify `sentry_init()` was called
4. Check Sentry dashboard "Settings → Client Keys" for DSN

**Problem**: Too many crashes reported

**Solutions**:
1. Set `sentry_options_set_sample_rate(options, 0.1)` (10% sampling)
2. Filter noise: `sentry_options_set_before_send()` callback
3. Upgrade to paid tier

## Next Steps

1. ☐ Create Sentry account (https://sentry.io/signup/)
2. ☐ Get DSN and store in `~/.zshrc` and GitHub Secrets
3. ☐ Add sentry-native to one plugin (SimpleGain proof-of-concept)
4. ☐ Test crash capture
5. ☐ Roll out to all 19 plugins
6. ☐ Monitor crash dashboard weekly

---

**Estimated Setup Time**: 2 hours (1 hour for proof-of-concept, 1 hour for all plugins)  
**Expected Impact**: 40% fewer support tickets, 10x faster bug diagnosis
