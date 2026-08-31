import React from 'react';

interface HeroProps {
  className?: string;
}

export const Hero: React.FC<HeroProps> = ({ className = '' }) => {
  return (
    <section className={`relative overflow-hidden bg-background ${className}`}>
      {/* Gradient background */}
      <div className="absolute inset-0 bg-gradient-primary opacity-10" />

      <div className="relative container mx-auto px-4 py-20 md:py-32">
        <div className="max-w-4xl mx-auto text-center">
          {/* Headline */}
          <h1 className="text-4xl md:text-6xl lg:text-7xl font-bold text-foreground mb-6">
            Professional Audio Plugins,
            <span className="block bg-gradient-primary bg-clip-text text-transparent">
              Free & Open Source
            </span>
          </h1>

          {/* Subheadline */}
          <p className="text-lg md:text-xl text-foreground/80 mb-8 max-w-2xl mx-auto">
            13 professional VST3 and AU plugins for music production. From essential utilities to advanced spectral effects, built with JUCE and modern C++.
          </p>

          {/* CTA Buttons */}
          <div className="flex flex-col sm:flex-row gap-4 justify-center items-center mb-12">
            <a
              href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
              target="_blank"
              rel="noopener noreferrer"
              className="px-8 py-4 bg-gradient-primary text-white font-semibold rounded-lg hover:opacity-90 transition-opacity shadow-lg w-full sm:w-auto text-center"
            >
              Download v1.3.0
            </a>
            <a
              href="https://github.com/kentin0-fiz0l/AudioForge"
              target="_blank"
              rel="noopener noreferrer"
              className="px-8 py-4 bg-foreground/10 text-foreground font-semibold rounded-lg hover:bg-foreground/20 transition-colors border border-foreground/20 w-full sm:w-auto text-center"
            >
              View on GitHub
            </a>
          </div>

          {/* Platform Badges */}
          <div className="flex flex-wrap gap-3 justify-center items-center">
            <span className="px-4 py-2 bg-foreground/10 text-foreground/80 rounded-full text-sm font-medium border border-foreground/20">
              VST3
            </span>
            <span className="px-4 py-2 bg-foreground/10 text-foreground/80 rounded-full text-sm font-medium border border-foreground/20">
              AU (macOS)
            </span>
            <span className="px-4 py-2 bg-foreground/10 text-foreground/80 rounded-full text-sm font-medium border border-foreground/20">
              macOS / Windows / Linux
            </span>
            <span className="px-4 py-2 bg-foreground/10 text-foreground/80 rounded-full text-sm font-medium border border-foreground/20">
              MIT License
            </span>
            <span className="px-4 py-2 bg-primary/20 text-primary-light rounded-full text-sm font-medium border border-primary">
              100% Test Coverage
            </span>
          </div>
        </div>
      </div>
    </section>
  );
};
