# AudioForge Analytics Setup

## Chosen Approach: GitHub Built-in Analytics

**Decision Date**: 2026-08-27
**Rationale**: Simple, free, no additional infrastructure required, sufficient for initial launch

## What We Track

### Download Metrics (via GitHub Releases API)
- Total downloads across all releases
- Downloads per release version
- Downloads per platform (macOS DMG, Windows .exe, Linux .deb/.rpm/AppImage)

### Community Metrics (via GitHub API)
- GitHub stars
- Issues opened/closed
- Pull requests
- Discussion activity (once enabled)

### Website Traffic (via GitHub Pages)
- Page views (limited data from GitHub Pages insights)
- Future: Consider Plausible/Umami for detailed website analytics

## How to Check Download Statistics

Run the analytics script:

```bash
./scripts/check-downloads.sh
```

Or query manually:

```bash
# Total downloads
gh api repos/kentin0-fiz0l/AudioForge/releases \
  --jq '[.[] | .assets[].download_count] | add'

# Downloads by release
gh api repos/kentin0-fiz0l/AudioForge/releases \
  --jq '.[] | {tag: .tag_name, downloads: [.assets[].download_count] | add}'

# Downloads by asset (specific release)
gh api repos/kentin0-fiz0l/AudioForge/releases/tags/v1.1.0 \
  --jq '.assets[] | {name: .name, downloads: .download_count}'
```

## Success Metrics (3 Months)

**Target Downloads** (from roadmap):
- ✅ 100+ total downloads
- ✅ Platform split: ~60% macOS, 30% Windows, 10% Linux

**Target Community**:
- ✅ 10+ GitHub stars
- ✅ 3+ active discussions
- ✅ 5+ external contributors

## Future Enhancements

If download volume increases significantly (500+), consider:

1. **Plausible Analytics** (self-hosted)
   - Real-time dashboard
   - Geographic distribution
   - Referral sources
   - Custom event tracking

2. **Umami Analytics** (self-hosted)
   - Simpler than Plausible
   - Privacy-focused
   - Easy Vercel deployment

3. **Google Analytics** (if needed for marketing)
   - Only if pursuing paid/commercial features
   - Not recommended for privacy reasons

## Monitoring Schedule

**Weekly** (Months 1-3):
- Run `./scripts/check-downloads.sh`
- Check GitHub Insights: https://github.com/kentin0-fiz0l/AudioForge/pulse
- Monitor discussions/issues

**Monthly** (After Month 3):
- Review download trends
- Platform distribution analysis
- Community health metrics

## Notes

- GitHub API rate limit: 5000 requests/hour (authenticated)
- Download counts update in real-time
- Historical data preserved indefinitely
- No PII collected (privacy-friendly)

---

**Last Updated**: 2026-08-27
