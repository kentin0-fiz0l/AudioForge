import type { Metadata } from "next";
import { SITE_URL } from "@/lib/site";
import { Navigation } from "@/components/Navigation";
import "./globals.css";

const TITLE = 'AudioForge: 60 free, open-source audio plugins';
const DESCRIPTION = '60 free, open-source VST3 and AU plugins (effects, instruments and MIDI tools) for macOS, Windows and Linux, built with JUCE.';

export const metadata: Metadata = {
  title: TITLE,
  description: DESCRIPTION,
  metadataBase: new URL(SITE_URL),
  icons: {
    icon: '/favicon.svg',
    apple: '/favicon.svg',
  },
  alternates: {
    canonical: '/',
  },
  openGraph: {
    type: 'website',
    locale: 'en_US',
    url: `${SITE_URL}/`,
    title: TITLE,
    description: DESCRIPTION,
    siteName: 'AudioForge',
    // Made from website/scripts/og-image.html
    images: [{ url: '/og-image.png', width: 1200, height: 630, alt: 'AudioForge: 60 free audio plugins, open source' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: TITLE,
    description: DESCRIPTION,
    images: ['/og-image.png'],
  },
};

// JSON-LD structured data for the organization
const organizationSchema = {
  "@context": "https://schema.org",
  "@type": "Organization",
  "name": "AudioForge",
  "url": `${SITE_URL}/`,
  "description": "Free, open-source audio plugins built with JUCE and modern C++",
  "sameAs": [
    "https://github.com/kentin0-fiz0l/AudioForge"
  ]
};

// JSON-LD structured data for software application
const softwareSchema = {
  "@context": "https://schema.org",
  "@type": "SoftwareApplication",
  "name": "AudioForge VST3 Plugins",
  "applicationCategory": "MultimediaApplication",
  "operatingSystem": "Windows, macOS, Linux",
  "offers": {
    "@type": "Offer",
    "price": "0",
    "priceCurrency": "USD"
  },
  "description": "60 free, open-source VST3 and AU plugins built with JUCE and modern C++17."
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" className="dark">
      <head>
        <script
          type="application/ld+json"
          dangerouslySetInnerHTML={{
            __html: JSON.stringify(organizationSchema)
          }}
        />
        <script
          type="application/ld+json"
          dangerouslySetInnerHTML={{
            __html: JSON.stringify(softwareSchema)
          }}
        />
      </head>
      <body className="antialiased bg-background text-foreground min-h-screen">
        <Navigation />
        {children}
      </body>
    </html>
  );
}
