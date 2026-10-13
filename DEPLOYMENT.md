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

Plugins are built and tested by GitHub Actions on every change. Releases are meant to carry
installers for macOS, Windows and Linux, built by the "Build and Sign Release" workflow; that
workflow is not producing files yet, so for now plugins are built from source (see the README).

## Retired

- The DigitalOcean Spaces bucket `audioforge-site` and `website/deploy.sh` (the site moved to
  App Platform, 2026-10-13).
- `scripts/upload-to-spaces.sh` and the FluxStudio download database: the `fluxstudio` bucket
  they used no longer exists.
