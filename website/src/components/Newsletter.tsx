'use client';

import React, { useState } from 'react';
import { AnimatedSection } from './AnimatedSection';

export const Newsletter: React.FC = () => {
  const [email, setEmail] = useState('');
  const [status, setStatus] = useState<'idle' | 'loading' | 'success' | 'error'>('idle');

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setStatus('loading');

    // For now, just show success message
    // In production, this would integrate with an email service
    setTimeout(() => {
      setStatus('success');
      setEmail('');
      setTimeout(() => setStatus('idle'), 5000);
    }, 1000);
  };

  return (
    <section className="py-16 md:py-24 bg-gradient-to-br from-background via-background-light to-background">
      <div className="container mx-auto px-4">
        <div className="max-w-4xl mx-auto">
          <AnimatedSection animation="fade-up">
            {/* Decorative waveform background */}
            <div className="relative bg-background-light border border-primary/20 rounded-2xl p-8 md:p-12 overflow-hidden">
              {/* Background decoration */}
              <div className="absolute inset-0 opacity-5">
                <svg className="w-full h-full" viewBox="0 0 800 400" preserveAspectRatio="none">
                  <path
                    d="M0,200 Q200,100 400,200 T800,200"
                    fill="none"
                    stroke="currentColor"
                    strokeWidth="2"
                    className="text-primary"
                  />
                  <path
                    d="M0,250 Q200,150 400,250 T800,250"
                    fill="none"
                    stroke="currentColor"
                    strokeWidth="2"
                    className="text-accent"
                  />
                </svg>
              </div>

              {/* Content */}
              <div className="relative z-10 text-center">
                <div className="inline-flex items-center gap-2 px-4 py-2 bg-primary/10 border border-primary/30 rounded-full mb-6">
                  <span className="w-2 h-2 bg-primary rounded-full animate-pulse"></span>
                  <span className="text-primary text-sm font-semibold font-mono">STAY UPDATED</span>
                </div>

                <h2 className="text-3xl md:text-4xl font-bold text-foreground mb-4 font-display">
                  Get AudioForge Updates
                </h2>
                <p className="text-foreground-muted text-lg mb-8 max-w-2xl mx-auto">
                  Subscribe to receive release notifications, new plugin announcements, and exclusive audio DSP insights.
                </p>

                {status === 'success' ? (
                  <div className="inline-flex items-center gap-3 px-6 py-4 bg-green-500/10 border border-green-500/30 rounded-lg text-green-400">
                    <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 13l4 4L19 7" />
                    </svg>
                    <span className="font-semibold">Thanks! You&apos;re subscribed.</span>
                  </div>
                ) : (
                  <form onSubmit={handleSubmit} className="max-w-md mx-auto">
                    <div className="flex flex-col sm:flex-row gap-3">
                      <input
                        type="email"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        placeholder="your@email.com"
                        required
                        disabled={status === 'loading'}
                        className="flex-1 px-4 py-3 bg-background border border-primary/30 text-foreground rounded-lg focus:outline-none focus:ring-2 focus:ring-primary/50 focus:border-primary/50 transition-all placeholder:text-foreground/40 disabled:opacity-50"
                      />
                      <button
                        type="submit"
                        disabled={status === 'loading'}
                        className="px-6 py-3 bg-gradient-primary text-white font-semibold rounded-lg hover:shadow-glow-primary transition-all disabled:opacity-50 disabled:cursor-not-allowed whitespace-nowrap"
                      >
                        {status === 'loading' ? (
                          <span className="flex items-center gap-2">
                            <svg className="animate-spin w-4 h-4" fill="none" viewBox="0 0 24 24">
                              <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4"></circle>
                              <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                            </svg>
                            Subscribing...
                          </span>
                        ) : (
                          'Subscribe'
                        )}
                      </button>
                    </div>
                    <p className="text-foreground/50 text-xs mt-4">
                      No spam, ever. Unsubscribe anytime. We respect your privacy.
                    </p>
                  </form>
                )}

                {/* Trust indicators */}
                <div className="mt-8 flex flex-wrap items-center justify-center gap-6 text-foreground/60 text-sm">
                  <div className="flex items-center gap-2">
                    <svg className="w-4 h-4 text-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
                    </svg>
                    Privacy First
                  </div>
                  <div className="flex items-center gap-2">
                    <svg className="w-4 h-4 text-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 10V3L4 14h7v7l9-11h-7z" />
                    </svg>
                    Instant Updates
                  </div>
                  <div className="flex items-center gap-2">
                    <svg className="w-4 h-4 text-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
                    </svg>
                    No Spam
                  </div>
                </div>
              </div>
            </div>
          </AnimatedSection>
        </div>
      </div>
    </section>
  );
};
