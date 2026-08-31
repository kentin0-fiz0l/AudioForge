'use client';

import React, { useState, useEffect } from 'react';

export const StickyDownload: React.FC = () => {
  const [isVisible, setIsVisible] = useState(false);

  useEffect(() => {
    const handleScroll = () => {
      // Show after scrolling past hero (800px)
      setIsVisible(window.scrollY > 800);
    };

    window.addEventListener('scroll', handleScroll);
    return () => window.removeEventListener('scroll', handleScroll);
  }, []);

  if (!isVisible) return null;

  return (
    <div className="fixed bottom-6 right-6 z-40 animate-in slide-in-from-bottom-4 duration-300">
      <a
        href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
        target="_blank"
        rel="noopener noreferrer"
        className="group flex items-center gap-3 px-6 py-3 bg-gradient-primary text-white font-semibold rounded-full shadow-glow-primary hover:shadow-glow-primary hover:scale-105 transition-all"
      >
        <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
        </svg>
        <span className="hidden sm:inline">Download v1.3.0</span>
        <span className="sm:hidden">Download</span>
      </a>
    </div>
  );
};
