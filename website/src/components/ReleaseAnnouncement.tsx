'use client';

import React, { useState } from 'react';

export const ReleaseAnnouncement: React.FC = () => {
  const [isVisible, setIsVisible] = useState(true);

  if (!isVisible) return null;

  return (
    <div className="relative bg-gradient-to-r from-primary/20 via-accent/20 to-primary/20 border-b border-primary/30">
      <div className="container mx-auto px-4 py-3">
        <div className="flex items-center justify-between gap-4">
          {/* Announcement Content */}
          <div className="flex items-center gap-3 flex-1">
            {/* Icon */}
            <div className="flex-shrink-0 w-8 h-8 bg-gradient-primary rounded-full flex items-center justify-center">
              <svg className="w-5 h-5 text-white" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 3v4M3 5h4M6 17v4m-2-2h4m5-16l2.286 6.857L21 12l-5.714 2.143L13 21l-2.286-6.857L5 12l5.714-2.143L13 3z" />
              </svg>
            </div>

            {/* Text */}
            <div className="flex flex-col sm:flex-row sm:items-center gap-2 sm:gap-4">
              <div>
                <span className="font-bold text-foreground text-sm sm:text-base">
                  🎉 v1.9.0 Released!
                </span>
                <span className="hidden sm:inline text-foreground-muted text-sm ml-2">
                  2 critical bugs fixed • 21/22 plugins now validated
                </span>
              </div>

              {/* CTA Buttons */}
              <div className="flex items-center gap-2">
                <a
                  href="https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v1.9.0"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="px-3 py-1.5 bg-background-light hover:bg-background-lighter text-primary text-xs font-semibold rounded border border-primary/30 hover:border-primary/50 transition-all"
                >
                  Release Notes
                </a>
                <a
                  href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="px-3 py-1.5 bg-gradient-primary text-white text-xs font-semibold rounded hover:shadow-glow-primary transition-all"
                >
                  Download Now
                </a>
              </div>
            </div>
          </div>

          {/* Close Button */}
          <button
            onClick={() => setIsVisible(false)}
            className="flex-shrink-0 text-foreground-muted hover:text-foreground transition-colors p-1 rounded hover:bg-background-light"
            aria-label="Dismiss announcement"
          >
            <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
            </svg>
          </button>
        </div>

        {/* Mobile-only detailed text */}
        <div className="sm:hidden text-foreground-muted text-xs mt-2 pl-11">
          SpectralFreeze & Sampler fixed • 95.5% validation rate achieved
        </div>
      </div>
    </div>
  );
};
