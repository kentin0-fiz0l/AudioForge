# AudioForge Website

Official marketing website and documentation for AudioForge VST3 plugins.

## Overview

AudioForge is a collection of professional-quality audio plugins built with modern C++ and the JUCE framework. This website serves as the landing page, plugin showcase, and documentation hub for all AudioForge plugins.

## Tech Stack

- **Framework**: Next.js 14 with App Router
- **Language**: TypeScript
- **Styling**: Tailwind CSS
- **Deployment**: Static site export (Vercel or GitHub Pages)

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
│   │   ├── layout.tsx            # Root layout
│   │   ├── globals.css           # Global styles
│   │   ├── plugins/
│   │   │   ├── page.tsx          # Plugin listing
│   │   │   └── [slug]/
│   │   │       └── page.tsx      # Individual plugin pages
│   │   ├── documentation/
│   │   │   └── page.tsx          # Documentation
│   │   └── roadmap/
│   │       └── page.tsx          # Roadmap
│   ├── components/
│   │   ├── Header.tsx            # Navigation header
│   │   ├── Footer.tsx            # Footer
│   │   ├── PluginCard.tsx        # Plugin card component
│   │   └── ...                   # Other components
│   └── data/
│       └── plugins.ts            # Plugin data
├── public/
│   ├── images/                   # Static images
│   ├── sitemap.xml              # SEO sitemap
│   └── robots.txt               # Crawler instructions
├── next.config.mjs              # Next.js configuration
├── tailwind.config.ts           # Tailwind configuration
├── vercel.json                  # Vercel deployment config
└── package.json                 # Dependencies
```

## Build

### Build for Production

```bash
npm run build
```

This generates a static site in the `out/` directory.

### Test Production Build Locally

```bash
# Install a static server
npm install -g serve

# Serve the production build
serve out
```

## Deployment

### Deploy to Vercel (Recommended)

1. Install Vercel CLI:
```bash
npm install -g vercel
```

2. Deploy:
```bash
vercel
```

3. For production deployment:
```bash
vercel --prod
```

### Deploy to GitHub Pages

1. Add to package.json scripts:
```json
{
  "scripts": {
    "deploy": "npm run build && gh-pages -d out"
  }
}
```

2. Install gh-pages:
```bash
npm install --save-dev gh-pages
```

3. Deploy:
```bash
npm run deploy
```

### Deploy to Other Static Hosts

The `out/` directory contains a complete static site that can be deployed to:
- Netlify
- Cloudflare Pages
- AWS S3 + CloudFront
- Any static file host

Simply upload the contents of the `out/` directory to your hosting provider.

## Environment Variables

No environment variables are required for the static site. All configuration is done in the code.

## SEO Optimization

The site includes:
- Comprehensive meta tags in layout.tsx and page.tsx files
- Sitemap.xml for search engine crawling
- Robots.txt for crawler control
- Semantic HTML structure
- Open Graph tags for social sharing
- JSON-LD structured data (ready to add)

## Performance

- Static site generation for instant page loads
- Optimized fonts with local hosting
- Lazy-loaded images
- Minimal JavaScript bundle
- Tailwind CSS for efficient styling

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

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test locally
5. Submit a pull request

## License

MIT License - see LICENSE file for details

## Contact

For questions or support, please open an issue on GitHub.

## Related Projects

- [AudioForge Plugins](../plugins/) - The actual VST3 plugins
- [AudioForge Documentation](https://audioforge.dev/documentation/) - Full documentation

## Roadmap

See the [Roadmap page](https://audioforge.dev/roadmap/) for upcoming features and planned plugins.
