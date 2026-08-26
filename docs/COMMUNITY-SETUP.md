# Community Infrastructure Setup Guide

This guide covers the remaining community infrastructure setup for AudioForge that requires manual configuration.

---

## ✅ Completed

- [x] GitHub Release v1.1.0 published
- [x] Issue templates (bug report, feature request)
- [x] Issue template configuration
- [x] Community guidelines (CONTRIBUTING.md, CODE_OF_CONDUCT.md, SECURITY.md)
- [x] CHANGELOG.md

---

## 🔄 Pending Setup

### 1. Enable GitHub Discussions

**Why**: Provides a community forum for Q&A, ideas, and announcements separate from issues.

**Steps**:

1. Go to https://github.com/kentin0-fiz0l/AudioForge/settings
2. Scroll down to **"Features"** section
3. Check the box next to **"Discussions"**
4. Click **"Set up discussions"**
5. GitHub will create a welcome discussion post
6. Customize categories:
   - **General** - General discussion
   - **Q&A** - Questions and answers
   - **Ideas** - Feature requests and suggestions
   - **Show and tell** - User projects and creations
   - **Announcements** - Official announcements (locked to maintainers)

**Recommended Categories**:
```
📢 Announcements (Locked to maintainers)
   - Release notes
   - Important updates

💬 General
   - General AudioForge discussion

❓ Q&A
   - Installation help
   - Plugin usage questions
   - DAW integration support

💡 Ideas
   - Feature requests
   - Plugin suggestions
   - Workflow improvements

🎵 Show and Tell
   - Projects made with AudioForge
   - Sound design showcases
   - Preset sharing

🛠️ Development
   - Contributing to AudioForge
   - Building from source
   - Bug investigations
```

**Expected Time**: 5-10 minutes

---

### 2. Set Up Download Analytics

**Why**: Track downloads by platform (macOS/Windows/Linux) to understand user base and prioritize development.

**Recommended Service**: **Plausible Analytics** (privacy-focused, lightweight)

**Alternative**: Umami (self-hosted, free)

---

#### Option A: Plausible Analytics (Recommended)

**Free Tier**: No (starts at $9/month for 10k pageviews)
**Self-Hosted**: Yes (free, Docker)

**Steps for Self-Hosted**:

1. **Set up Plausible server** (requires a VPS like DigitalOcean, AWS, or Hetzner):

```bash
# Clone Plausible hosting repo
git clone https://github.com/plausible/hosting
cd hosting

# Configure
cp plausible-conf.env.example plausible-conf.env
nano plausible-conf.env

# Set:
# BASE_URL=https://analytics.yourdomain.com
# SECRET_KEY_BASE=<generate with: openssl rand -base64 64>

# Start with Docker Compose
docker-compose up -d
```

2. **Add tracking to website**:

In `website/src/app/layout.tsx` or `website/public/index.html`:

```html
<script defer data-domain="audioforge.fluxstudio.art"
  src="https://analytics.yourdomain.com/js/script.js"></script>
```

3. **Track downloads** with custom events:

```javascript
// In download buttons
onClick={() => {
  plausible('Download', {
    props: {
      platform: 'macOS',
      version: 'v1.1.0',
      format: 'DMG'
    }
  });
  // Then trigger actual download
}}
```

4. **View dashboard**: https://analytics.yourdomain.com

**Expected Time**: 30-60 minutes (self-hosted setup)

---

#### Option B: Umami Analytics (Self-Hosted, Free)

**Free Tier**: Yes (self-hosted)
**Privacy**: GDPR compliant, no cookies

**Steps**:

1. **Deploy Umami**:

```bash
# Option 1: Railway (easiest - 1-click deploy)
# Visit https://railway.app/template/umami
# Connect GitHub, deploy

# Option 2: Vercel + PlanetScale (free tier)
git clone https://github.com/umami-software/umami.git
cd umami
# Follow setup at https://umami.is/docs/running-on-vercel
```

2. **Add tracking script**:

```html
<script async defer
  data-website-id="YOUR-WEBSITE-ID"
  src="https://your-umami-instance.vercel.app/umami.js">
</script>
```

3. **Track download events**:

```javascript
// Track downloads
umami.track('download', {
  platform: 'macOS',
  version: 'v1.1.0'
});
```

**Expected Time**: 15-30 minutes (Railway/Vercel deploy)

---

#### Option C: GitHub Release Download Counts (Built-in, Free)

**Why**: GitHub automatically tracks release download counts.

**Steps**:

1. Visit https://github.com/kentin0-fiz0l/AudioForge/releases
2. View download counts for each asset under release
3. Use GitHub API to fetch stats programmatically:

```bash
# Get download stats for all releases
gh api repos/kentin0-fiz0l/AudioForge/releases \
  --jq '.[] | {tag: .tag_name, downloads: [.assets[].download_count] | add}'
```

**Limitations**:
- Only tracks release assets
- No real-time dashboard
- No platform breakdown (unless you create separate assets per platform)

**Expected Time**: 0 minutes (already enabled)

---

### 3. Community Health Check

Verify GitHub's Community Standards:

1. Go to https://github.com/kentin0-fiz0l/AudioForge/community
2. Check that all items are green:
   - [x] Description ✅
   - [x] README ✅
   - [x] Code of conduct ✅
   - [x] Contributing ✅
   - [x] License ✅
   - [x] Security policy ✅
   - [ ] Issue templates ✅
   - [ ] Pull request template (optional)
   - [ ] Discussions (pending - see step 1)

**Expected Time**: 2 minutes

---

### 4. Social Media & Promotion (Optional)

**Create Social Profiles**:

- **Twitter/X**: @AudioForgeVST
- **Reddit**: Post to r/WeAreTheMusicMakers, r/audioengineering, r/edmproduction
- **Discord**: Create AudioForge Discord server for real-time support
- **YouTube**: Channel for video tutorials

**Reddit Self-Promotion Guidelines**:
- Follow 10:1 rule (10 helpful comments for every 1 self-promotion post)
- Share in "Feedback Friday" threads
- Focus on helping users, not just promoting

**Expected Time**: 1-2 hours (optional)

---

## 📊 Analytics Dashboard Setup

### Recommended Metrics to Track

**Download Metrics**:
- Total downloads (all-time)
- Downloads by platform (macOS/Windows/Linux)
- Downloads by version
- Geographic distribution (optional)

**Website Metrics**:
- Page views (home, docs, downloads)
- Bounce rate
- Time on page
- Most viewed manuals

**Community Metrics**:
- GitHub stars
- Issues opened/closed
- Discussions started/answered
- Contributors

**Dashboard Tools**:
- **Plausible/Umami**: Website + download tracking
- **GitHub Insights**: https://github.com/kentin0-fiz0l/AudioForge/pulse
- **GitHub Traffic**: https://github.com/kentin0-fiz0l/AudioForge/graphs/traffic

---

## 🎯 Success Metrics (3 Months)

**Downloads**:
- ✅ 100+ total downloads
- ✅ 50% macOS, 40% Windows, 10% Linux split

**Community**:
- ✅ 50+ GitHub stars
- ✅ 3+ external contributors
- ✅ 10+ active discussions

**Documentation**:
- ✅ 500+ manual page views
- ✅ 5+ video tutorial views (1000+ each)
- ✅ Low bounce rate on docs pages (<40%)

---

## Next Steps

1. **Enable GitHub Discussions** (5 min)
2. **Set up analytics** (choose Plausible, Umami, or GitHub-only)
3. **Promote v1.1.0 release** on Reddit, forums, social media
4. **Monitor community health** weekly

---

**Last Updated**: 2026-08-25
