import type { Metadata } from "next";
import { SITE_URL } from "@/lib/site";
import { Navigation } from "@/components/Navigation";
import "./globals.css";

export const metadata: Metadata = {
  title: "AudioForge - 60 Professional VST3 & AU Plugins",
  description: "60 professional VST3 and AU plugins with universal preset management and MIDI Learn. Free, open source, and professional-grade.",
  metadataBase: new URL(SITE_URL),
  icons: {
    icon: '/favicon.svg',
    apple: '/favicon.svg',
  },
  alternates: {
    canonical: '/index.html',
  },
  openGraph: {
    type: 'website',
    locale: 'en_US',
    url: `${SITE_URL}/`,
    title: 'AudioForge - 60 Professional VST3 & AU Plugins',
    description: '60 professional audio plugins with universal preset management and MIDI Learn. Built with JUCE, free and open source.',
    siteName: 'AudioForge',
  },
  twitter: {
    card: 'summary_large_image',
    title: 'AudioForge - 60 Professional VST3 & AU Plugins',
    description: '60 professional audio plugins with universal preset management and MIDI Learn. Built with JUCE, free and open source.',
  },
};

// JSON-LD structured data for the organization
const organizationSchema = {
  "@context": "https://schema.org",
  "@type": "Organization",
  "name": "AudioForge",
  "url": `${SITE_URL}/`,
  "description": "Professional-quality audio plugins built with modern C++ and JUCE framework",
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
