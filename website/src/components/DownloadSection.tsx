import React from 'react';

const LATEST = 'https://github.com/kentin0-fiz0l/AudioForge/releases/latest';

const platforms = [
  {
    name: 'macOS',
    detail: 'macOS 11 or later, Apple Silicon and Intel',
    file: '.pkg installer',
    formats: 'VST3, Audio Units and standalone apps',
    unsigned: 'If macOS won’t open the installer, go to System Settings > Privacy & Security and click Open Anyway.',
  },
  {
    name: 'Windows',
    detail: 'Windows 10 or 11, 64-bit',
    file: 'Setup.exe installer',
    formats: 'VST3 and standalone apps, with an uninstaller',
    unsigned: 'If SmartScreen appears, click More info, then Run anyway.',
  },
  {
    name: 'Linux',
    detail: 'x86_64',
    file: '.tar.gz with install.sh',
    formats: 'VST3 and standalone apps, installed for your user',
    unsigned: 'Unpack it and run ./install.sh.',
  },
];

export const DownloadSection: React.FC = () => (
  <section id="download" className="py-16 md:py-24 bg-background">
    <div className="container mx-auto px-4 max-w-5xl">
      <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display text-center">Download</h2>
      <p className="text-foreground-muted text-lg text-center max-w-2xl mx-auto mb-12">
        Every plugin, free and open source under the MIT License. Each release has an installer for your platform
        and a zip to copy by hand.
      </p>
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mb-10">
        {platforms.map((p) => (
          <a
            key={p.name}
            href={LATEST}
            className="block bg-background-light border border-primary/20 rounded-xl p-6 hover:border-primary/50 transition-colors"
          >
            <h3 className="text-2xl font-bold text-foreground mb-1">{p.name}</h3>
            <p className="text-foreground-muted text-sm mb-4">{p.detail}</p>
            <p className="text-foreground font-semibold">{p.file}</p>
            <p className="text-foreground-muted text-sm mb-4">{p.formats}</p>
            <p className="text-foreground/60 text-xs">{p.unsigned}</p>
          </a>
        ))}
      </div>
      <div className="text-center">
        <a
          href={LATEST}
          className="inline-block px-10 py-4 bg-gradient-primary text-white font-semibold text-lg rounded-lg hover:shadow-glow-primary transition-all"
        >
          Get the latest release
        </a>
        <p className="text-foreground/60 text-sm mt-4 max-w-2xl mx-auto">
          The installers are not code-signed (AudioForge has no Apple Developer ID or Windows certificate), so your
          system asks you to confirm once. Prefer to build it yourself? See{' '}
          <a href="/docs/#getting-started" className="text-primary hover:underline">Getting Started</a>.
        </p>
      </div>
    </div>
  </section>
);
