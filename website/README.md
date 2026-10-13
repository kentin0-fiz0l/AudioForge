# AudioForge Website

Official marketing website and documentation for AudioForge VST3 plugins.

## Overview

AudioForge is a collection of professional-quality audio plugins built with modern C++ and the JUCE framework. This website serves as the landing page, plugin showcase, and documentation hub for all AudioForge plugins.

## Tech Stack

- **Framework**: Next.js 14 with App Router
- **Language**: TypeScript
- **Styling**: Tailwind CSS
- **Deployment**: Static site export to DigitalOcean Spaces
- **URL**: https://fluxstudio-audioforge-y59di.ondigitalocean.app/

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

## Deployment

The site is served by DigitalOcean App Platform (app `audioforge`, free static-site tier) at
https://fluxstudio-audioforge-y59di.ondigitalocean.app/ and redeploys on every push to `main`. The spec is `.do/app.yaml` at the repo root;
see `DEPLOYMENT.md`. Nothing is deployed by hand.

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
