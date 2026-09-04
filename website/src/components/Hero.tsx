import React from 'react';
import { Logo } from './Logo';

interface HeroProps {
  className?: string;
}

export const Hero: React.FC<HeroProps> = ({ className = '' }) => {
  return (
    <section className={`relative overflow-hidden bg-background ${className}`}>
      {/* Animated gradient mesh background */}
      <div className="absolute inset-0 bg-gradient-mesh opacity-50" />

      {/* Waveform decoration */}
      <div className="absolute inset-0 opacity-10">
        <svg className="w-full h-full" xmlns="http://www.w3.org/2000/svg">
          <defs>
            <linearGradient id="waveGradient" x1="0%" y1="0%" x2="100%" y2="0%">
              <stop offset="0%" stopColor="#06b6d4" stopOpacity="0.5" />
              <stop offset="50%" stopColor="#f59e0b" stopOpacity="0.5" />
              <stop offset="100%" stopColor="#06b6d4" stopOpacity="0.5" />
            </linearGradient>
          </defs>
          <path
            d="M0,200 Q100,150 200,200 T400,200 T600,200 T800,200 T1000,200 T1200,200 T1400,200 T1600,200"
            stroke="url(#waveGradient)"
            strokeWidth="2"
            fill="none"
            className="animate-pulse-slow"
          />
          <path
            d="M0,250 Q100,220 200,250 T400,250 T600,250 T800,250 T1000,250 T1200,250 T1400,250 T1600,250"
            stroke="url(#waveGradient)"
            strokeWidth="2"
            fill="none"
            className="animate-pulse-slow"
            style={{ animationDelay: '0.5s' }}
          />
        </svg>
      </div>

      <div className="relative container mx-auto px-4 py-20 md:py-32">
        <div className="max-w-4xl mx-auto text-center">
          {/* Logo showcase */}
          <div className="flex justify-center mb-8">
            <Logo size="lg" showText={false} animate={true} />
          </div>

          {/* Headline */}
          <h1 className="text-4xl md:text-6xl lg:text-7xl font-bold text-foreground mb-6 font-display">
            Professional Audio Plugins,
            <span className="block mt-2 bg-gradient-to-r from-primary via-accent to-primary bg-clip-text text-transparent animate-pulse-slow">
              Free & Open Source
            </span>
          </h1>

          {/* Validation Badge */}
          <div className="inline-flex items-center gap-2 px-6 py-3 mb-6 bg-green-500/10 border border-green-500/30 rounded-full">
            <svg className="w-5 h-5 text-green-400" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
            <span className="text-green-400 font-semibold text-sm md:text-base">
              🎉 100% Validated (22/22 at Maximum Strictness)
            </span>
          </div>

          {/* Subheadline */}
          <p className="text-lg md:text-xl text-foreground-muted mb-8 max-w-2xl mx-auto leading-relaxed">
            22 professional VST3 and AU plugins + 121 factory presets. From essential utilities to advanced spectral effects, built with JUCE and modern C++.
          </p>

          {/* CTA Buttons */}
          <div className="flex flex-col sm:flex-row gap-4 justify-center items-center mb-12">
            <a
              href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
              target="_blank"
              rel="noopener noreferrer"
              className="group relative px-8 py-4 bg-gradient-primary text-white font-semibold rounded-lg transition-all shadow-glow-primary hover:shadow-glow-primary hover:scale-105 w-full sm:w-auto text-center overflow-hidden"
            >
              {/* Button glow effect */}
              <div className="absolute inset-0 bg-gradient-to-r from-primary-light to-primary opacity-0 group-hover:opacity-100 transition-opacity" />
              <span className="relative flex items-center justify-center gap-2">
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
                </svg>
                Download v1.9.0
              </span>
            </a>
            <a
              href="https://github.com/kentin0-fiz0l/AudioForge"
              target="_blank"
              rel="noopener noreferrer"
              className="px-8 py-4 bg-background-light text-foreground font-semibold rounded-lg hover:bg-background-lighter transition-all border border-primary/30 hover:border-primary/50 w-full sm:w-auto text-center"
            >
              View on GitHub
            </a>
          </div>

          {/* Platform Badges */}
          <div className="flex flex-wrap gap-3 justify-center items-center">
            <span className="px-4 py-2 bg-background-light text-foreground-muted rounded-full text-sm font-medium border border-primary/20 hover:border-primary/40 transition-colors font-mono">
              VST3
            </span>
            <span className="px-4 py-2 bg-background-light text-foreground-muted rounded-full text-sm font-medium border border-primary/20 hover:border-primary/40 transition-colors font-mono">
              AU (macOS)
            </span>
            <span className="px-4 py-2 bg-background-light text-foreground-muted rounded-full text-sm font-medium border border-primary/20 hover:border-primary/40 transition-colors font-mono">
              macOS / Windows / Linux
            </span>
            <span className="px-4 py-2 bg-background-light text-foreground-muted rounded-full text-sm font-medium border border-primary/20 hover:border-primary/40 transition-colors font-mono">
              MIT License
            </span>
            <span className="px-4 py-2 bg-gradient-primary text-white rounded-full text-sm font-medium border border-primary shadow-glow-primary font-mono">
              100% Test Coverage
            </span>
          </div>

          {/* Waveform visualizer decoration */}
          <div className="mt-16 flex justify-center gap-1 opacity-30">
            {Array.from({ length: 50 }).map((_, i) => (
              <div
                key={i}
                className="w-1 bg-gradient-to-t from-primary to-accent rounded-full animate-waveform"
                style={{
                  height: `${20 + Math.random() * 60}px`,
                  animationDelay: `${i * 0.05}s`,
                }}
              />
            ))}
          </div>
        </div>
      </div>
    </section>
  );
};
