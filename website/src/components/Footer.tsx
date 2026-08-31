import React from 'react';
import { Logo } from './Logo';

export const Footer: React.FC = () => {
  const currentYear = new Date().getFullYear();

  return (
    <footer className="relative py-12 bg-background-light border-t border-primary/10">
      {/* Decorative waveform at top */}
      <div className="absolute top-0 left-0 right-0 h-px bg-gradient-to-r from-transparent via-primary/50 to-transparent" />

      <div className="container mx-auto px-4">
        <div className="max-w-6xl mx-auto">
          {/* Main footer content */}
          <div className="grid grid-cols-1 md:grid-cols-4 gap-8 mb-8">
            {/* Brand column */}
            <div className="md:col-span-2">
              <Logo size="sm" />
              <p className="text-foreground-muted text-sm mt-4 leading-relaxed max-w-md">
                Professional VST3 and AU plugins for music production. Built with JUCE and modern C++, free and open source forever.
              </p>
              <div className="flex gap-4 mt-6">
                <a
                  href="https://github.com/kentin0-fiz0l/AudioForge"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="text-foreground-muted hover:text-primary transition-colors"
                  aria-label="GitHub"
                >
                  <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 24 24">
                    <path fillRule="evenodd" d="M12 2C6.477 2 2 6.484 2 12.017c0 4.425 2.865 8.18 6.839 9.504.5.092.682-.217.682-.483 0-.237-.008-.868-.013-1.703-2.782.605-3.369-1.343-3.369-1.343-.454-1.158-1.11-1.466-1.11-1.466-.908-.62.069-.608.069-.608 1.003.07 1.531 1.032 1.531 1.032.892 1.53 2.341 1.088 2.91.832.092-.647.35-1.088.636-1.338-2.22-.253-4.555-1.113-4.555-4.951 0-1.093.39-1.988 1.029-2.688-.103-.253-.446-1.272.098-2.65 0 0 .84-.27 2.75 1.026A9.564 9.564 0 0112 6.844c.85.004 1.705.115 2.504.337 1.909-1.296 2.747-1.027 2.747-1.027.546 1.379.202 2.398.1 2.651.64.7 1.028 1.595 1.028 2.688 0 3.848-2.339 4.695-4.566 4.943.359.309.678.92.678 1.855 0 1.338-.012 2.419-.012 2.747 0 .268.18.58.688.482A10.019 10.019 0 0022 12.017C22 6.484 17.522 2 12 2z" clipRule="evenodd" />
                  </svg>
                </a>
              </div>
            </div>

            {/* Resources column */}
            <div>
              <h3 className="text-foreground font-semibold mb-4 font-display">Resources</h3>
              <ul className="space-y-2">
                <li>
                  <a href="/docs" className="text-foreground-muted hover:text-primary transition-colors text-sm">
                    Documentation
                  </a>
                </li>
                <li>
                  <a href="/roadmap" className="text-foreground-muted hover:text-primary transition-colors text-sm">
                    Roadmap
                  </a>
                </li>
                <li>
                  <a
                    href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
                    target="_blank"
                    rel="noopener noreferrer"
                    className="text-foreground-muted hover:text-primary transition-colors text-sm"
                  >
                    Download
                  </a>
                </li>
              </ul>
            </div>

            {/* Community column */}
            <div>
              <h3 className="text-foreground font-semibold mb-4 font-display">Community</h3>
              <ul className="space-y-2">
                <li>
                  <a
                    href="https://github.com/kentin0-fiz0l/AudioForge/discussions"
                    target="_blank"
                    rel="noopener noreferrer"
                    className="text-foreground-muted hover:text-primary transition-colors text-sm"
                  >
                    Discussions
                  </a>
                </li>
                <li>
                  <a
                    href="https://github.com/kentin0-fiz0l/AudioForge/issues"
                    target="_blank"
                    rel="noopener noreferrer"
                    className="text-foreground-muted hover:text-primary transition-colors text-sm"
                  >
                    Issues
                  </a>
                </li>
                <li>
                  <a
                    href="https://github.com/kentin0-fiz0l/AudioForge"
                    target="_blank"
                    rel="noopener noreferrer"
                    className="text-foreground-muted hover:text-primary transition-colors text-sm"
                  >
                    GitHub
                  </a>
                </li>
              </ul>
            </div>
          </div>

          {/* Bottom bar */}
          <div className="pt-8 border-t border-primary/10">
            <div className="flex flex-col md:flex-row justify-between items-center gap-4">
              <p className="text-foreground-muted text-sm">
                © {currentYear} AudioForge. Licensed under the{' '}
                <a
                  href="https://opensource.org/licenses/MIT"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="text-primary hover:text-primary-light transition-colors"
                >
                  MIT License
                </a>
              </p>
              <p className="text-foreground-dim text-xs font-mono flex items-center gap-2">
                <span className="inline-block w-2 h-2 bg-primary rounded-full animate-pulse" />
                Built with JUCE framework and C++17
              </p>
            </div>
          </div>
        </div>
      </div>
    </footer>
  );
};
