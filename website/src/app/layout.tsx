import type { Metadata } from "next";
import localFont from "next/font/local";
import "./globals.css";

const geistSans = localFont({
  src: "./fonts/GeistVF.woff",
  variable: "--font-geist-sans",
  weight: "100 900",
});
const geistMono = localFont({
  src: "./fonts/GeistMonoVF.woff",
  variable: "--font-geist-mono",
  weight: "100 900",
});

export const metadata: Metadata = {
  title: "AudioForge - Professional VST3 Plugins",
  description: "Professional-quality audio plugins built with modern C++ and JUCE framework",
  metadataBase: new URL('https://audioforge.dev'),
  alternates: {
    canonical: '/',
  },
};

// JSON-LD structured data for the organization
const organizationSchema = {
  "@context": "https://schema.org",
  "@type": "Organization",
  "name": "AudioForge",
  "url": "https://audioforge.dev",
  "logo": "https://audioforge.dev/images/logo.png",
  "description": "Professional-quality audio plugins built with modern C++ and JUCE framework",
  "sameAs": [
    "https://github.com/yourusername/AudioForge"
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
  "description": "Professional-quality audio plugins built with modern C++ and JUCE framework"
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
      <body
        className={`${geistSans.variable} ${geistMono.variable} antialiased bg-background text-foreground`}
      >
        {children}
      </body>
    </html>
  );
}
