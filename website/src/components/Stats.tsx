'use client';

import React from 'react';
import { AnimatedSection } from './AnimatedSection';

interface Stat {
  value: string;
  label: string;
  icon: React.ReactNode;
  description: string;
}

const stats: Stat[] = [
  {
    value: '22',
    label: 'Plugins',
    description: 'VST3 & AU formats',
    icon: (
      <svg className="w-8 h-8" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 11H5m14 0a2 2 0 012 2v6a2 2 0 01-2 2H5a2 2 0 01-2-2v-6a2 2 0 012-2m14 0V9a2 2 0 00-2-2M5 11V9a2 2 0 012-2m0 0V5a2 2 0 012-2h6a2 2 0 012 2v2M7 7h10" />
      </svg>
    ),
  },
  {
    value: '100%',
    label: 'Test Coverage',
    description: '136 passing tests',
    icon: (
      <svg className="w-8 h-8" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
      </svg>
    ),
  },
  {
    value: '3',
    label: 'Platforms',
    description: 'macOS, Windows, Linux',
    icon: (
      <svg className="w-8 h-8" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M21 12a9 9 0 01-9 9m9-9a9 9 0 00-9-9m9 9H3m9 9a9 9 0 01-9-9m9 9c1.657 0 3-4.03 3-9s-1.343-9-3-9m0 18c-1.657 0-3-4.03-3-9s1.343-9 3-9m-9 9a9 9 0 019-9" />
      </svg>
    ),
  },
  {
    value: 'MIT',
    label: 'License',
    description: 'Free forever',
    icon: (
      <svg className="w-8 h-8" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
      </svg>
    ),
  },
];

export const Stats: React.FC = () => {
  return (
    <section className="py-16 md:py-20 bg-background relative overflow-hidden">
      {/* Background decoration */}
      <div className="absolute inset-0 opacity-5">
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-full h-full max-w-4xl">
          <div className="absolute inset-0 bg-gradient-to-r from-primary via-accent to-primary blur-3xl animate-pulse-slow" />
        </div>
      </div>

      <div className="container mx-auto px-4 relative">
        <AnimatedSection animation="fade-in">
          <div className="grid grid-cols-2 md:grid-cols-4 gap-6 md:gap-8">
            {stats.map((stat, index) => (
              <AnimatedSection
                key={stat.label}
                animation="fade-up"
                delay={index * 100}
                className="text-center"
              >
                <div className="group p-6 bg-background-light border border-primary/10 rounded-xl hover:border-primary/30 hover:shadow-glow-primary transition-all duration-300">
                  {/* Icon */}
                  <div className="inline-flex items-center justify-center w-16 h-16 mb-4 text-primary group-hover:scale-110 transition-transform">
                    {stat.icon}
                  </div>

                  {/* Value */}
                  <div className="text-3xl md:text-4xl font-bold text-foreground mb-1 font-display">
                    {stat.value}
                  </div>

                  {/* Label */}
                  <div className="text-sm font-semibold text-primary mb-1">
                    {stat.label}
                  </div>

                  {/* Description */}
                  <div className="text-xs text-foreground-muted">
                    {stat.description}
                  </div>
                </div>
              </AnimatedSection>
            ))}
          </div>
        </AnimatedSection>
      </div>
    </section>
  );
};
