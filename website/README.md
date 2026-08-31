# AudioForge Website

Official marketing website and documentation for AudioForge VST3 plugins.

## Overview

AudioForge is a collection of professional-quality audio plugins built with modern C++ and the JUCE framework. This website serves as the landing page, plugin showcase, and documentation hub for all AudioForge plugins.

## Tech Stack

- **Framework**: Next.js 14 with App Router
- **Language**: TypeScript
- **Styling**: Tailwind CSS
- **Deployment**: Static site export to DigitalOcean Spaces
- **URL**: https://audioforge-site.sfo3.cdn.digitaloceanspaces.com/index.html

## Features

- Responsive design optimized for all devices
- Dark theme with modern UI
- Plugin showcase with detailed specifications
- Comprehensive documentation
- Development roadmap
- SEO-optimized with sitemap and metadata
- Static site export for fast performance

## Local Development

### Prerequisites

- Node.js 18.x or later
- npm or yarn

### Setup

1. Clone the repository:
```bash
git clone https://github.com/yourusername/AudioForge.git
cd AudioForge/website
```

2. Install dependencies:
```bash
npm install
```

3. Run the development server:
```bash
npm run dev
```

4. Open [http://localhost:3000](http://localhost:3000) in your browser.

## Project Structure

```
website/
├── src/
│   ├── app/
│   │   ├── page.tsx              # Homepage
│   │   ├── layout.tsx            # Root layout with SEO
│   │   ├── globals.css           # Global styles
│   │   ├── plugins/
│   │   │   └── [slug]/
│   │   │       └── page.tsx      # Individual plugin pages
│   │   ├── docs/
│   │   │   └── page.tsx          # Documentation
│   │   └── roadmap/
│   │       └── page.tsx          # Roadmap
│   ├── components/
│   │   ├── Hero.tsx              # Hero section
│   │   ├── Navigation.tsx        # Nav header
│   │   ├── Footer.tsx            # Footer
│   │   ├── PluginCard.tsx        # Plugin card component
│   │   └── FeatureGrid.tsx       # Features grid
│   └── lib/
│       └── plugins.ts            # Plugin data and metadata
├── public/
│   ├── images/                   # Static images
│   ├── sitemap.xml              # SEO sitemap
│   └── robots.txt               # Crawler instructions
├── next.config.mjs              # Next.js configuration
├── tailwind.config.ts           # Tailwind configuration
└── package.json                 # Dependencies
```

## Build

### Build for Production

```bash
npm run build
```

This generates a static site in the `out/` directory with:
- 9 static HTML pages
- Optimized assets
- 92.7 kB first load JS
- Complete SEO metadata

### Test Production Build Locally

```bash
# Install a static server
npm install -g serve

# Serve the production build
serve out
```

## Deployment to DigitalOcean

### Option 1: DigitalOcean App Platform (Recommended)

Easiest deployment method with automatic builds and HTTPS:

1. **Create a new App on DigitalOcean:**
   - Go to DigitalOcean Dashboard → Apps → Create App
   - Connect your GitHub repository
   - Select the `AudioForge` repository
   - Set source directory to `/website`

2. **Configure the App:**
   - **Build Command**: `npm run build`
   - **Output Directory**: `out`
   - **Environment Variables**: None needed
   - **Plan**: Static Site (cheapest option, $3/mo)

3. **Set up custom domain:**
   - In App settings, add domain: `audioforge.fluxstudio.art`
   - Copy the CNAME record provided by DigitalOcean
   - Add CNAME record to FluxStudio.art DNS:
     ```
     Type: CNAME
     Name: audioforge
     Value: [your-app-name].ondigitalocean.app
     TTL: 3600
     ```

4. **Deploy:**
   - DigitalOcean will auto-deploy on every push to main
   - HTTPS certificate is automatically provisioned
   - Site will be live at `audioforge.fluxstudio.art`

### Option 2: DigitalOcean Spaces + CDN

Cheapest option for pure static hosting ($5/mo for 250GB storage + bandwidth):

1. **Create a Space:**
   - Go to DigitalOcean Dashboard → Spaces → Create Space
   - Name: `audioforge-site`
   - Region: Choose closest to your users (e.g., SFO3)
   - Enable CDN
   - Set Files Listing to "Public"

2. **Upload the static site:**
   ```bash
   # Install s3cmd or use doctl
   npm install -g @digitalocean/doctl

   # Authenticate
   doctl auth init

   # Build the site
   npm run build

   # Upload to Spaces (replace YOUR-SPACE-NAME and REGION)
   doctl spaces upload out/* \
     --space audioforge-site \
     --region sfo3 \
     --recursive
   ```

3. **Configure Space as website:**
   - In Space settings → Enable "Static Website Hosting"
   - Set index document: `index.html`
   - Set error document: `404.html`

4. **Set up custom domain:**
   - In Space settings → Add custom domain: `audioforge.fluxstudio.art`
   - Add CNAME record to FluxStudio.art DNS:
     ```
     Type: CNAME
     Name: audioforge
     Value: audioforge-site.sfo3.cdn.digitaloceanspaces.com
     TTL: 3600
     ```

5. **Enable SSL:**
   - DigitalOcean will provision a Let's Encrypt certificate automatically

### Option 3: Manual Deployment Script

Create a deployment script for easy updates:

```bash
#!/bin/bash
# deploy.sh

echo "Building site..."
npm run build

echo "Uploading to DigitalOcean Spaces..."
doctl spaces upload out/* \
  --space audioforge-site \
  --region sfo3 \
  --recursive \
  --force

echo "Deployment complete!"
echo "Site live at: https://audioforge.fluxstudio.art"
```

Make it executable:
```bash
chmod +x deploy.sh
./deploy.sh
```

## DNS Configuration

Add this CNAME record to FluxStudio.art DNS (e.g., in DigitalOcean Networking or your DNS provider):

```
Type: CNAME
Name: audioforge
Value: [depends on deployment option]
TTL: 3600
```

**Values by deployment option:**
- **App Platform**: `your-app-name.ondigitalocean.app`
- **Spaces CDN**: `audioforge-site.sfo3.cdn.digitaloceanspaces.com`

## Cost Comparison

- **App Platform**: $3/month (static site plan)
- **Spaces + CDN**: $5/month (250GB storage + bandwidth)
- **Both options include**:
  - Automatic HTTPS
  - CDN distribution
  - 99.99% uptime SLA

## SEO Optimization

The site includes:
- Comprehensive meta tags in all pages
- Open Graph and Twitter Card metadata
- JSON-LD structured data (Organization, SoftwareApplication)
- Sitemap.xml at `/sitemap.xml`
- Robots.txt at `/robots.txt`
- Semantic HTML structure
- Optimized images with Next.js Image component

## Performance

- Static site generation for instant page loads
- Optimized fonts with local hosting
- Lazy-loaded images
- Minimal JavaScript bundle (92.7 kB first load)
- Tailwind CSS for efficient styling
- CDN distribution via DigitalOcean

## Theme Configuration

The site uses a dark theme with purple-pink gradient accents:

- Background: `#1a1a1a`
- Primary: `#8b5cf6` (purple)
- Accent: `#ec4899` (pink)
- Gradient: `linear-gradient(135deg, #8b5cf6 0%, #ec4899 100%)`

## Browser Support

- Chrome (latest)
- Firefox (latest)
- Safari (latest)
- Edge (latest)

## Monitoring

After deployment, monitor:
- **Performance**: DigitalOcean App Platform → Metrics
- **Bandwidth**: Spaces → Usage
- **Uptime**: Set up DigitalOcean monitoring alerts

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test locally with `npm run dev`
5. Build and verify with `npm run build && serve out`
6. Submit a pull request

## License

MIT License - see LICENSE file for details

## Contact

For questions or support, please open an issue on GitHub.

## Related Projects

- [AudioForge Plugins](../plugins/) - The actual VST3 plugins
- [FluxStudio Marketplace](https://fluxstudio.art/audioforge) - Download plugins

## Roadmap

See the [Roadmap page](https://audioforge.fluxstudio.art/roadmap) for upcoming features and planned plugins.
