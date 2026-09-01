# TODO: Create Sentry Account (5 minutes)

## Why This Matters

Sentry provides **real-time crash reporting** that will:
- Show you **exact stack traces** with line numbers when plugins crash
- Reduce support tickets by **40%** (users can self-diagnose with crash data)
- Track which plugin versions have the most crashes
- Capture user breadcrumbs (actions before crash)

**Cost**: FREE for <5,000 crashes/month (sufficient for <1,000 users)

---

## Quick Setup Steps

### 1. Create Account (2 minutes)

1. Go to: **https://sentry.io/signup/**
2. Sign up with GitHub (recommended) or email
3. Choose organization name: `audioforge` or your name
4. Skip onboarding wizard (we have custom integration)

### 2. Create Project (2 minutes)

1. Click "Create Project"
2. Platform: **C++ / Native**
3. Project name: **AudioForge**
4. Default alert settings: **On every new issue**
5. Click "Create Project"

### 3. Get Your DSN (1 minute)

After project creation, you'll see:
```
https://[PROJECT_ID]@o[ORG_NUMBER].ingest.sentry.io/[PROJECT_NUMBER]
```

**COPY THIS** - you need it in the next step!

Example DSN:
```
https://abc123def456@o4508012345678901234.ingest.sentry.io/1234567890123456
```

### 4. Store DSN Securely

**Add to your shell configuration** (keep it secret!):

```bash
# Open shell config
nano ~/.zshrc

# Add this line at the bottom (replace with your real DSN):
export SENTRY_DSN="https://your-dsn-here@o123.ingest.sentry.io/456"

# Save and exit (Ctrl+X, Y, Enter)

# Reload shell
source ~/.zshrc

# Verify it worked
echo $SENTRY_DSN
# Should print your DSN
```

**Add to GitHub Secrets** (for CI/CD):

1. Go to: https://github.com/kentin0-fiz0l/AudioForge/settings/secrets/actions
2. Click "New repository secret"
3. Name: `SENTRY_DSN`
4. Value: (paste your DSN)
5. Click "Add secret"

---

## Verify Setup

Test that Sentry is configured:

```bash
# Check environment variable is set
echo $SENTRY_DSN
# Should print: https://...@o...ingest.sentry.io/...

# Verify it's a valid URL
curl -I $(echo $SENTRY_DSN | cut -d'@' -f2)
# Should return: HTTP/2 200
```

---

## Next: Integrate into SimpleGain

Once you have your DSN, integrate Sentry into one plugin:

```bash
cd ~/Projects/Active/AudioForge/plugins/SimpleGain

# Edit PluginProcessor.cpp to add:
# 1. #include "../../shared/utils/SentryIntegration.h"
# 2. In prepareToPlay():
#    SentryIntegration::initialize("SimpleGain", "1.0.0");
# 3. In processBlock(), wrap audio processing in try/catch

# Rebuild plugin
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Install to test
cp -r build/SimpleGain_artefacts/Debug/VST3/SimpleGain.vst3 \
      ~/Library/Audio/Plug-Ins/VST3/

# Test in DAW (Logic, Reaper, Ableton)
# Trigger a test crash
# Check Sentry dashboard for crash report
```

---

## Expected Timeline

- **Today (5 min)**: Create account, get DSN, store in environment
- **Tomorrow (30 min)**: Integrate into SimpleGain, test crash capture
- **This week (1 hour)**: Roll out to all 19 plugins
- **Ongoing**: Monitor crash dashboard weekly

---

## Troubleshooting

**Problem**: `echo $SENTRY_DSN` prints nothing

**Solution**: 
```bash
# Check if you edited the right file
cat ~/.zshrc | grep SENTRY_DSN

# If missing, add it:
echo 'export SENTRY_DSN="your-dsn-here"' >> ~/.zshrc
source ~/.zshrc
```

**Problem**: Can't find DSN after account creation

**Solution**:
1. Go to: https://sentry.io
2. Click your project name ("AudioForge")
3. Go to: Settings → Client Keys (DSN)
4. Copy the DSN value

**Problem**: Crashes not appearing in dashboard

**Solution**:
1. Verify `SentryIntegration::initialize()` is called in `prepareToPlay()`
2. Check local crash logs: `~/Library/Logs/AudioForge/crashes.log`
3. Verify DSN is correct: `echo $SENTRY_DSN`
4. Check Sentry dashboard "Project Settings" → make sure project is active

---

## Done Criteria

✅ Sentry account created  
✅ DSN stored in `~/.zshrc`  
✅ DSN stored in GitHub Secrets  
✅ `echo $SENTRY_DSN` prints valid URL  
✅ Ready to integrate into plugins

**When complete, mark this task done in the Phase 1 tracker!**

---

*Reference: `docs/SENTRY_SETUP.md` for detailed integration guide*  
*Utility: `shared/utils/SentryIntegration.h` for plugin integration*
