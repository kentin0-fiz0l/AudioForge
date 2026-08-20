# AudioForge Deployment Scripts

Scripts for building, packaging, and deploying AudioForge plugins.

## upload-to-spaces.sh

Packages built plugins and uploads them to DigitalOcean Spaces (S3-compatible storage).

### Prerequisites

1. **AWS CLI** (for Spaces upload):
   ```bash
   brew install awscli
   ```

2. **DigitalOcean Spaces credentials**:
   - Log in to DigitalOcean
   - Go to API → Spaces Keys
   - Create a new key pair
   - Save the key and secret

3. **Built plugins**:
   ```bash
   # Build SimpleGain
   cd plugins/SimpleGain
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release

   # Build PanUtil
   cd ../PanUtil
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

### Usage

```bash
# Set credentials (one-time)
export SPACES_KEY='your-spaces-access-key'
export SPACES_SECRET='your-spaces-secret-key'

# Optional: customize bucket/region
export SPACES_BUCKET='fluxstudio'  # default
export SPACES_REGION='sfo3'         # default

# Run upload script
./scripts/upload-to-spaces.sh
```

### What It Does

1. **Packages plugins** into ZIP archives:
   - `SimpleGain-1.0.0-macOS.vst3.zip`
   - `PanUtil-1.0.0-macOS.vst3.zip`

2. **Uploads to Spaces**:
   - Uploads to `s3://fluxstudio/audioforge/{plugin}/{version}/`
   - Sets public-read ACL for downloads
   - Generates CDN URLs

3. **Creates SQL update**:
   - Generates SQL to update `plugin_versions` table
   - Includes download URLs and file sizes
   - Ready to run on production database

### Output

The script creates:
- `/tmp/audioforge-upload/` - Packaged ZIP files
- `/tmp/audioforge-upload/update_plugin_urls.sql` - Database update script

### Example Output

```
==========================================
AudioForge Plugin Upload
==========================================

Packaging plugins...

1. SimpleGain v1.0.0
-------------------
✓ Packaging macOS VST3...
  Size: 12.3M

2. PanUtil v1.0.0
----------------
✓ Packaging macOS VST3...
  Size: 11.8M

Uploading to DigitalOcean Spaces...

↑ Uploading SimpleGain-1.0.0-macOS.vst3.zip...
✓ https://fluxstudio.sfo3.cdn.digitaloceanspaces.com/audioforge/simplegain/1.0.0/SimpleGain-1.0.0-macOS.vst3.zip

↑ Uploading PanUtil-1.0.0-macOS.vst3.zip...
✓ https://fluxstudio.sfo3.cdn.digitaloceanspaces.com/audioforge/panutil/1.0.0/PanUtil-1.0.0-macOS.vst3.zip

==========================================
Upload Complete!
==========================================

CDN URLs:
  • https://fluxstudio.sfo3.cdn.digitaloceanspaces.com/audioforge/simplegain/1.0.0/SimpleGain-1.0.0-macOS.vst3.zip
  • https://fluxstudio.sfo3.cdn.digitaloceanspaces.com/audioforge/panutil/1.0.0/PanUtil-1.0.0-macOS.vst3.zip

Next steps:
  1. Run the SQL update on production database
  2. Test downloads at https://fluxstudio.art/audioforge
```

## Updating Production Database

After uploading files, update the database with download URLs:

```bash
# Get the SQL file
SQL_FILE="/tmp/audioforge-upload/update_plugin_urls.sql"

# Run on production database
psql $DATABASE_URL -f $SQL_FILE

# Or manually via DigitalOcean console:
# 1. Go to your managed database
# 2. Open SQL console
# 3. Paste contents of update_plugin_urls.sql
```

## Adding New Plugins

When adding a new plugin:

1. **Build the plugin**:
   ```bash
   cd plugins/YourPlugin
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

2. **Add packaging to script**:
   Edit `upload-to-spaces.sh` and add a new section like:
   ```bash
   # Package YourPlugin
   YOURPLUGIN_DIR="$PLUGINS_DIR/YourPlugin/build/YourPlugin_artefacts"
   # ... packaging logic
   ```

3. **Run upload script**:
   ```bash
   ./scripts/upload-to-spaces.sh
   ```

## Troubleshooting

### "AWS CLI not found"
```bash
brew install awscli
```

### "Unable to locate credentials"
Make sure you've set the environment variables:
```bash
export SPACES_KEY='your-key'
export SPACES_SECRET='your-secret'
```

### "Plugin not built"
Build the plugin first:
```bash
cd plugins/SimpleGain
cmake -B build && cmake --build build --config Release
```

### Upload fails with 403
Check your Spaces credentials are correct and have write permissions.

### Database update fails
Make sure you're connected to the correct database and have write permissions.

## Security Notes

- Never commit Spaces credentials to git
- Use environment variables or a `.env` file (gitignored)
- Rotate keys periodically
- Use read-only keys for downloads, write keys only for uploads

## Next Steps

After successful upload:
1. ✅ Files are hosted on Spaces CDN
2. ✅ Database has download URLs
3. ✅ Marketplace shows download buttons
4. 🎉 Users can download plugins!
