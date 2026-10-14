# AudioForge Deployment

## Website

The website (`website/`, a Next.js static export) is served by DigitalOcean App Platform,
app `audioforge`, on the free static-site tier: https://fluxstudio-audioforge-y59di.ondigitalocean.app/

It is rebuilt and redeployed on every push to `main` (`.do/app.yaml`, `deploy_on_push`).
There is nothing to run by hand. To apply a change to the app itself:

```bash
doctl apps update 4d7780b8-5fd3-4e02-a80e-ae122993edb2 --spec .do/app.yaml
```

To check a build before merging:

```bash
cd website && npm ci && npm run build   # output in website/out
```

## Plugins

Plugins are built and tested by GitHub Actions on every change. `.github/workflows/release.yml`
builds every plugin on macOS (universal), Windows and Linux and attaches the installers to a
GitHub release:

- `AudioForge-<version>-macOS.pkg` and `.zip` (VST3, AU, standalone apps)
- `AudioForge-<version>-Windows-Setup.exe` and `.zip` (VST3, standalone apps)
- `AudioForge-<version>-Linux-x86_64.tar.gz` (VST3, standalone apps, `install.sh`)
- `SHA256SUMS.txt`

To release, update `CHANGELOG.md` with a `## [X.Y.Z]` section, then push a tag:

```bash
git tag vX.Y.Z && git push origin vX.Y.Z      # builds and publishes the release (about an hour)
```

To try a change to the workflow, run it by hand; it makes a draft release only collaborators see:

```bash
gh workflow run release.yml --ref <branch> -f version=0.0.0-test
```

The installers are not code-signed (no Apple Developer ID or Windows certificate), so macOS asks
for "Open Anyway" and Windows SmartScreen for "Run anyway" once; the release notes say so.
`build-sign-release.yml` (signing and notarizing) runs only by hand and needs those certificates.

## Retired

- The DigitalOcean Spaces bucket `audioforge-site` and `website/deploy.sh` (the site moved to
  App Platform, 2026-10-13).
- `scripts/upload-to-spaces.sh` and the FluxStudio download database: the `fluxstudio` bucket
  they used no longer exists.
