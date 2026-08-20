# AudioForge Deployment Guide

Complete guide to deploying AudioForge plugins to production.

## Overview

AudioForge plugins are distributed through FluxStudio's marketplace at `fluxstudio.art/audioforge`. The deployment process involves:

1. Building plugins locally
2. Uploading to DigitalOcean Spaces (CDN)
3. Updating FluxStudio database
4. Testing downloads

## Prerequisites

### Development Tools
- ✅ JUCE framework (installed via git submodule)
- ✅ CMake 3.15+
- ✅ C++17 compiler (Xcode on macOS)

### Deployment Tools
- AWS CLI: `brew install awscli`
- DigitalOcean CLI: `brew install doctl`
- PostgreSQL client: `brew install postgresql`

### Credentials Needed
1. **DigitalOcean Spaces** (for file hosting)
   - Access Key
   - Secret Key
   - Get from: https://cloud.digitalocean.com/account/api/spaces

2. **FluxStudio Database** (for metadata updates)
   - Database URL
   - Get from: DigitalOcean managed database console

## Step-by-Step Deployment

### 1. Build Plugins for Production

```bash
cd ~/Projects/Active/AudioForge

# Build SimpleGain
cd plugins/SimpleGain
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Build PanUtil
cd ../PanUtil
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Verify builds
ls -lh plugins/SimpleGain/build/SimpleGain_artefacts/VST3/
ls -lh plugins/PanUtil/build/PanUtil_artefacts/VST3/
```

**Expected output**:
- `SimpleGain.vst3/` (bundle directory, ~35MB)
- `PanUtil.vst3/` (bundle directory, ~35MB)

### 2. Upload to DigitalOcean Spaces

```bash
cd ~/Projects/Active/AudioForge

# Set credentials
export SPACES_KEY='your-spaces-access-key'
export SPACES_SECRET='your-spaces-secret-key'

# Optional: customize bucket/region
export SPACES_BUCKET='fluxstudio'
export SPACES_REGION='sfo3'

# Run upload script
./scripts/upload-to-spaces.sh
```

**Script will**:
- Package plugins into ZIP files
- Upload to `s3://fluxstudio/audioforge/{plugin}/{version}/`
- Generate CDN URLs
- Create SQL update script

**Output**:
```
✓ https://fluxstudio.sfo3.cdn.digitaloceanspaces.com/audioforge/simplegain/1.0.0/SimpleGain-1.0.0-macOS.vst3.zip
✓ https://fluxstudio.sfo3.cdn.digitaloceanspaces.com/audioforge/panutil/1.0.0/PanUtil-1.0.0-macOS.vst3.zip
```

### 3. Update Database

The upload script generates SQL in `/tmp/audioforge-upload/update_plugin_urls.sql`.

**Option A: Via psql command line**
```bash
# Get database URL from FluxStudio
cd ~/Projects/Active/FluxStudio
export DATABASE_URL=$(grep DATABASE_URL .env.production | cut -d '=' -f2)

# Run SQL update
psql $DATABASE_URL -f /tmp/audioforge-upload/update_plugin_urls.sql
```

**Option B: Via DigitalOcean Console**
1. Go to https://cloud.digitalocean.com/databases
2. Select FluxStudio database
3. Click "SQL Console"
4. Paste contents of `update_plugin_urls.sql`
5. Execute

**Verify**:
```sql
SELECT
    p.name,
    p.version,
    pv.download_url_mac,
    pv.file_size_mac
FROM plugins p
JOIN plugin_versions pv ON pv.plugin_id = p.id
WHERE pv.is_latest = true;
```

Should show URLs and file sizes for both plugins.

### 4. Deploy FluxStudio Frontend

```bash
cd ~/Projects/Active/FluxStudio

# Authenticate with DigitalOcean (one-time)
doctl auth init

# Deploy
npm run deploy
```

**Or wait for auto-deploy**:
- FluxStudio auto-deploys from GitHub main branch
- Check status: https://cloud.digitalocean.com/apps
- Deployment takes ~5-10 minutes

### 5. Verify Deployment

#### Test API
```bash
curl https://api.fluxstudio.art/api/audioforge/plugins | jq '.'
```

Should return:
```json
{
  "success": true,
  "data": [
    {
      "name": "SimpleGain",
      "slug": "simplegain",
      "version": "1.0.0",
      ...
    },
    {
      "name": "PanUtil",
      "slug": "panutil",
      "version": "1.0.0",
      ...
    }
  ]
}
```

#### Test Frontend
Visit: https://fluxstudio.art/audioforge

Should see:
- ✅ Both plugins listed
- ✅ Download buttons visible
- ✅ Clicking downloads a ZIP file
- ✅ ZIP contains VST3 plugin

#### Test Downloads
```bash
# Download SimpleGain
curl -L -O "$(curl -s https://api.fluxstudio.art/api/audioforge/plugins/simplegain | jq -r '.data.versions[0].downloadUrlMac')"

# Verify ZIP
unzip -l SimpleGain-1.0.0-macOS.vst3.zip
```

## Deployment Checklist

### Pre-Deployment
- [ ] All plugins build successfully
- [ ] Plugins tested locally
- [ ] Git commits are pushed
- [ ] Changelog updated

### During Deployment
- [ ] Plugins uploaded to Spaces
- [ ] CDN URLs generated
- [ ] Database updated with URLs
- [ ] FluxStudio deployed

### Post-Deployment
- [ ] API returns correct data
- [ ] Marketplace page loads
- [ ] Download buttons work
- [ ] Downloaded plugins install correctly
- [ ] Analytics tracking works

## Rollback Procedure

If something goes wrong:

### Rollback Database
```sql
-- Mark versions as not latest
UPDATE plugin_versions
SET is_latest = false
WHERE version = '1.0.0';

-- Or delete versions entirely
DELETE FROM plugin_versions
WHERE version = '1.0.0';
```

### Rollback FluxStudio Deployment
```bash
cd ~/Projects/Active/FluxStudio

# Revert to previous commit
git revert HEAD
git push origin main

# Or use DigitalOcean console to rollback
```

### Remove Spaces Files
```bash
aws s3 rm s3://fluxstudio/audioforge/simplegain/1.0.0/ \
  --recursive \
  --endpoint-url=https://sfo3.digitaloceanspaces.com
```

## Monitoring

### Check Download Stats
```sql
SELECT
    p.name,
    p.download_count,
    COUNT(pd.id) as tracked_downloads,
    COUNT(DISTINCT pd.ip_address) as unique_ips
FROM plugins p
LEFT JOIN plugin_downloads pd ON pd.plugin_id = p.id
GROUP BY p.id, p.name
ORDER BY p.download_count DESC;
```

### Check CDN Bandwidth
- DigitalOcean Console → Spaces → fluxstudio → Metrics
- Monitor bandwidth usage and costs

### Error Monitoring
- FluxStudio has Sentry integration
- Check: https://sentry.io/organizations/fluxstudio/

## Cost Estimates

### DigitalOcean Spaces
- Storage: $5/month for 250GB
- Transfer: $0.01/GB after 1TB free
- Estimate: $5-10/month for AudioForge files

### Database
- Included in FluxStudio managed database
- No additional cost for plugin metadata

### Total Monthly Cost
- **Development**: $0 (local only)
- **Production**: ~$5-10/month (Spaces bandwidth)

## Troubleshooting

### "Plugin not found" in marketplace
- Check database: `SELECT * FROM plugins WHERE slug = 'simplegain';`
- Verify `is_published = true`
- Check API: `curl https://api.fluxstudio.art/api/audioforge/plugins`

### Download button doesn't work
- Check `plugin_versions` table has `download_url_mac`
- Verify CDN URL is accessible: `curl -I <url>`
- Check CORS headers on Spaces

### Upload fails with 403
- Verify Spaces credentials
- Check key has write permissions
- Try regenerating key in DigitalOcean console

### Database migration fails
- Check if tables already exist
- Verify database credentials
- Run migration manually from SQL file

## Next Plugin Release

When releasing v1.1.0 or adding new plugins:

1. Update version in CMakeLists.txt
2. Build new version
3. Run upload script (auto-detects version)
4. Upload script creates new version in database
5. Old versions remain available

## Support

- GitHub Issues: https://github.com/yourusername/AudioForge/issues
- FluxStudio Support: support@fluxstudio.art
- Documentation: https://fluxstudio.art/docs/audioforge

---

Last updated: 2026-08-20
