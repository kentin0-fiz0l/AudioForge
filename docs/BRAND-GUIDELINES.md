# AudioForge Brand Guidelines

## Brand Identity: "Studio Precision"

AudioForge's visual identity combines the precision of professional audio engineering with the accessibility of open-source software. Our design language evokes vintage studio equipment while maintaining a modern, clean aesthetic.

---

## Core Brand Pillars

1. **Professional Audio Engineering** - Technical precision meets creative expression
2. **Analog Warmth + Digital Clarity** - Blend vintage gear aesthetics with modern clean design
3. **Open Source Philosophy** - Transparent, accessible, community-focused

---

## Color Palette

### Primary Colors

**Cyan (Digital Meter Glow)**
- Default: `#06b6d4`
- Light: `#22d3ee`
- Dark: `#0891b2`
- Usage: Primary CTAs, interactive elements, waveform visualizations
- Inspiration: Digital audio meters, oscilloscope displays

**Amber (VU Meter Warmth)**
- Default: `#f59e0b`
- Light: `#fbbf24`
- Dark: `#d97706`
- Usage: Accents, highlights, secondary interactions
- Inspiration: Vintage VU meters, analog warmth

### Neutral Palette

**Backgrounds (Studio Aesthetic)**
- Base: `#0a0e14` - Deep charcoal (mixing console black)
- Light: `#151b24` - Elevated surfaces
- Lighter: `#1f2937` - Cards, modals

**Foregrounds**
- Default: `#e4e7eb` - Clean white (readable)
- Muted: `#9ca3af` - Secondary text
- Dim: `#6b7280` - Tertiary text

### Semantic Colors

- Success: `#10b981` (Green - "armed for recording")
- Warning: `#f59e0b` (Amber)
- Error: `#ef4444` (Red - "clipping indicator")

### Spectrum Colors (Advanced Usage)

Used for frequency visualization and audio-specific UI:
- Low: `#10b981` (Green)
- Mid: `#f59e0b` (Amber)
- High: `#ef4444` (Red)

---

## Typography

### Font Families

**Display & Body Text**
- Family: Inter
- Weights: 400 (Regular), 500 (Medium), 600 (Semibold), 700 (Bold), 800 (Extrabold)
- Usage: Headlines, body text, navigation
- CDN: Google Fonts

**Technical/Monospace**
- Family: JetBrains Mono
- Weights: 400 (Regular), 500 (Medium), 600 (Semibold)
- Usage: Version numbers, code snippets, technical details
- CDN: Google Fonts

### Type Scale

```
Hero Headline:    4xl - 7xl (text-4xl md:text-6xl lg:text-7xl)
Section Headline: 3xl - 5xl (text-3xl md:text-4xl lg:text-5xl)
Card Title:       2xl (text-2xl)
Subsection:       xl (text-xl)
Body Large:       lg (text-lg)
Body:             base (text-base)
Small:            sm (text-sm)
Micro:            xs (text-xs)
```

---

## Logo & Icon

### Waveform Logo

The AudioForge logo features an 8-bar waveform visualization in a rounded rectangle container:

- **8 vertical bars** with varying heights creating a waveform pattern
- **Gradient fill** from amber (bottom) to cyan (top) on each bar
- **Rounded caps** on bars (border-radius: 50%)
- **Background**: Deep charcoal with subtle border
- **Glow effect** on hover with gradient overlay

**Sizes:**
- Small: 32×32px (navigation, favicon)
- Medium: 48×48px (hero sections)
- Large: 64×64px (marketing materials)

**Usage Guidelines:**
- Always maintain aspect ratio (1:1)
- Minimum size: 24×24px
- Clear space: 8px on all sides
- Never distort or rotate
- Animate on hover/interaction (scale, glow)

### Logo with Text

When paired with text:
- Logo on left, text on right
- Vertical center alignment
- 12px gap between icon and text
- Text: "Audio**Forge**" (Forge in cyan)

---

## Visual Elements

### Waveforms

Decorative waveform elements appear throughout the brand:

**Types:**
1. **Header Waveforms** - Subtle background SVG paths
2. **Visualizer Bars** - Animated vertical bars in plugin cards
3. **Separator Waveforms** - Horizontal gradients between sections

**Animation:**
- Pulse duration: 2-3s
- Easing: `cubic-bezier(0.4, 0, 0.6, 1)`
- Never distract from content

### Gradients

**Primary Gradient**
```css
linear-gradient(135deg, #06b6d4 0%, #0891b2 100%)
```

**Accent Gradient**
```css
linear-gradient(135deg, #f59e0b 0%, #d97706 100%)
```

**Mesh Background**
```css
radial-gradient(at 0% 0%, rgba(6, 182, 212, 0.1) 0px, transparent 50%),
radial-gradient(at 100% 0%, rgba(245, 158, 11, 0.1) 0px, transparent 50%),
radial-gradient(at 100% 100%, rgba(6, 182, 212, 0.1) 0px, transparent 50%),
radial-gradient(at 0% 100%, rgba(245, 158, 11, 0.1) 0px, transparent 50%)
```

### Glass Morphism

Background blur effects for depth:
```css
background: rgba(21, 27, 36, 0.8);
backdrop-filter: blur(12px);
```

### Glow Effects

**Primary Glow**
```css
box-shadow: 0 0 20px rgba(6, 182, 212, 0.3);
```

**Accent Glow**
```css
box-shadow: 0 0 20px rgba(245, 158, 11, 0.3);
```

---

## Components

### Buttons

**Primary (CTA)**
- Background: `gradient-primary`
- Text: White
- Hover: Glow effect + slight scale
- Border radius: 8px
- Padding: 16px 32px

**Secondary**
- Background: `background-light`
- Text: `foreground`
- Border: `primary/30`
- Hover: Border intensity increase

**Disabled**
- Background: `background-light`
- Text: `foreground-dim`
- Border: `primary/10`
- Cursor: not-allowed

### Cards

**Plugin Card**
- Background: `background-light`
- Border: `primary/10`
- Border radius: 12px
- Hover: Border `primary/40` + glow shadow
- Ambient glow on hover (gradient overlay)

**Feature Card**
- Similar to Plugin Card
- Icon in primary color
- Hover: Icon scale + glow

### Navigation

- Fixed header
- Glass morphism background when scrolled
- Logo: Waveform icon + text
- Hover: Scale 105%

### Footer

- Background: `background-light`
- Top border: Decorative gradient line
- Logo integration
- Column layout (responsive)
- Pulsing status indicator

---

## Animation Guidelines

### Transitions

**Standard**
```css
transition: all 300ms cubic-bezier(0.4, 0, 0.2, 1);
```

**Fast**
```css
transition: all 150ms cubic-bezier(0.4, 0, 0.2, 1);
```

### Hover States

- Scale: 105-110%
- Glow: Opacity 0 → 100%
- Border: Intensity increase
- Never exceed 200ms delay

### Waveform Animation

```css
@keyframes waveform {
  0%, 100% { transform: scaleY(0.5); }
  50% { transform: scaleY(1.2); }
}
```

### Pulse

```css
@keyframes pulse {
  0%, 100% { opacity: 0.5; }
  50% { opacity: 1; }
}
```

---

## Accessibility

### Contrast Ratios

- Text on `background`: 15.8:1 (AAA)
- Text on `background-light`: 12.2:1 (AAA)
- Primary on `background`: 4.8:1 (AA)
- Accent on `background`: 5.2:1 (AA)

### Focus States

- Outline: `2px solid primary`
- Offset: `2px`
- Never remove focus indicators

### Motion

- Respect `prefers-reduced-motion`
- Provide static alternatives for animations

---

## Voice & Tone

### Voice Characteristics

- **Professional** but not stuffy
- **Technical** but accessible
- **Passionate** about open source
- **Confident** in quality

### Writing Style

**Do:**
- Use active voice
- Be specific and technical when needed
- Celebrate the community
- Explain complex concepts clearly

**Don't:**
- Use marketing hyperbole
- Make unverified claims
- Gatekeep technical knowledge
- Ignore accessibility

### Example Copy

**Good:**
> "13 professional VST3 and AU plugins for music production. Built with JUCE and modern C++."

**Bad:**
> "The world's most amazing, revolutionary audio plugins you'll ever use!"

---

## Asset Specifications

### Favicon

- Format: SVG (with PNG fallback)
- Size: 64×64px (scales to 16×16, 32×32)
- Design: Waveform logo on dark background

### Open Graph Images

- Size: 1200×630px
- Format: PNG or JPEG
- Include: Logo, tagline, gradient background
- Text: Large, readable at small sizes

### Screenshots

- Format: PNG
- Dimensions: 16:9 aspect ratio preferred
- Quality: High-res (2x for retina)
- Context: Show plugin in DAW environment

---

## Platform-Specific Guidelines

### Web

- Responsive breakpoints: 640px, 768px, 1024px, 1280px
- Mobile-first approach
- Touch targets: Minimum 44×44px
- Load Inter and JetBrains Mono from Google Fonts

### Social Media

**Profile Images:**
- Use waveform logo on dark background
- Square format (1:1)
- High contrast for small sizes

**Cover Images:**
- Gradient mesh background
- Logo + tagline
- Professional, technical aesthetic

### Documentation

- Syntax highlighting: Use primary/accent colors
- Code blocks: `background-light` with `primary/10` border
- Inline code: Monospace font, subtle background

---

## Don'ts

### Never:

- Use the old purple-pink gradient
- Rotate or distort the logo
- Change the waveform bar count (always 8)
- Use Comic Sans or other informal fonts
- Add drop shadows (use glows instead)
- Mix competing gradients
- Use low-contrast color combinations
- Ignore dark mode (AudioForge is dark-first)

---

## Implementation

### Tailwind Configuration

See `website/tailwind.config.ts` for the complete Tailwind setup including:
- Custom color palette
- Font configuration
- Animation keyframes
- Shadow utilities

### CSS Custom Properties

```css
:root {
  --background: #0a0e14;
  --foreground: #e4e7eb;
}
```

### Component Library

All components in `website/src/components/`:
- `Logo.tsx` - Waveform logo with animations
- `Hero.tsx` - Landing hero with gradients
- `Navigation.tsx` - Glass morphism header
- `PluginCard.tsx` - Audio-inspired cards
- `Footer.tsx` - Enhanced footer with logo

---

## Contact & Resources

- **Website**: https://audioforge-site.sfo3.cdn.digitaloceanspaces.com/index.html
- **GitHub**: https://github.com/kentin0-fiz0l/AudioForge
- **License**: MIT
- **Design System**: Tailwind CSS
- **Framework**: Next.js 14

---

**Version**: 1.0.0
**Last Updated**: 2026-08-31
**Status**: Live

