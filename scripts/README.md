# AudioForge Deployment Scripts

Scripts for building, packaging, and deploying AudioForge plugins.

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
