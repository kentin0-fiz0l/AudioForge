'use client';

import React from 'react';
import { AnimatedSection } from './AnimatedSection';

interface DocCard {
  title: string;
  description: string;
  icon: React.ReactNode;
  category: string;
  link: string;
}

const docCards: DocCard[] = [
  {
    title: 'Quick Start Guide',
    description: 'Get up and running with AudioForge in under 5 minutes. Installation, setup, and your first plugin.',
    category: 'Getting Started',
    link: '/docs',
    icon: (
      <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 10V3L4 14h7v7l9-11h-7z" />
      </svg>
    ),
  },
  {
    title: 'Plugin Reference',
    description: 'Complete documentation for all 22 plugins. Parameters, controls, use cases, and audio examples.',
    category: 'Reference',
    link: '/docs',
    icon: (
      <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6.253v13m0-13C10.832 5.477 9.246 5 7.5 5S4.168 5.477 3 6.253v13C4.168 18.477 5.754 18 7.5 18s3.332.477 4.5 1.253m0-13C13.168 5.477 14.754 5 16.5 5c1.747 0 3.332.477 4.5 1.253v13C19.832 18.477 18.247 18 16.5 18c-1.746 0-3.332.477-4.5 1.253" />
      </svg>
    ),
  },
  {
    title: 'Audio DSP Concepts',
    description: 'Learn the fundamentals of digital signal processing with real-world examples from our codebase.',
    category: 'Learning',
    link: '/docs/learning',
    icon: (
      <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9.663 17h4.673M12 3v1m6.364 1.636l-.707.707M21 12h-1M4 12H3m3.343-5.657l-.707-.707m2.828 9.9a5 5 0 117.072 0l-.548.547A3.374 3.374 0 0014 18.469V19a2 2 0 11-4 0v-.531c0-.895-.356-1.754-.988-2.386l-.548-.547z" />
      </svg>
    ),
  },
  {
    title: 'API Documentation',
    description: 'Comprehensive C++ API reference for developers building on AudioForge or contributing code.',
    category: 'Development',
    link: '/docs',
    icon: (
      <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M10 20l4-16m4 4l4 4-4 4M6 16l-4-4 4-4" />
      </svg>
    ),
  },
];

export const DocsPreview: React.FC = () => {
  return (
    <section className="py-16 md:py-24 bg-background">
      <div className="container mx-auto px-4">
        <AnimatedSection animation="fade-up">
          <div className="text-center mb-12 md:mb-16">
            <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
              Documentation
            </h2>
            <p className="text-foreground-muted text-lg max-w-2xl mx-auto">
              Everything you need to master AudioForge, from beginner tutorials to advanced DSP concepts.
            </p>
          </div>
        </AnimatedSection>

        <div className="max-w-6xl mx-auto">
          {/* Doc Cards Grid */}
          <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-12">
            {docCards.map((card, index) => (
              <AnimatedSection key={card.title} animation="fade-up" delay={index * 50}>
                <a
                  href={card.link}
                  className="block h-full bg-background-light border border-primary/10 rounded-xl p-6 hover:border-primary/40 hover:shadow-glow-primary transition-all duration-300 group"
                >
                  {/* Category Badge */}
                  <div className="flex items-center justify-between mb-4">
                    <span className="px-3 py-1 bg-primary/10 text-primary text-xs font-semibold rounded-full border border-primary/20">
                      {card.category}
                    </span>
                    <div className="w-10 h-10 bg-primary/10 border border-primary/20 rounded-lg flex items-center justify-center text-primary group-hover:bg-primary/20 group-hover:scale-110 transition-all">
                      {card.icon}
                    </div>
                  </div>

                  {/* Content */}
                  <h3 className="text-xl font-bold text-foreground mb-2 group-hover:text-primary transition-colors">
                    {card.title}
                  </h3>
                  <p className="text-foreground/70 text-sm leading-relaxed mb-4">
                    {card.description}
                  </p>

                  {/* Read More Link */}
                  <div className="flex items-center text-primary font-semibold text-sm group-hover:gap-2 transition-all">
                    Read More
                    <svg
                      className="w-4 h-4 ml-1 group-hover:translate-x-1 transition-transform"
                      fill="none"
                      stroke="currentColor"
                      viewBox="0 0 24 24"
                    >
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
                    </svg>
                  </div>
                </a>
              </AnimatedSection>
            ))}
          </div>

          {/* Featured Documentation Highlights */}
          <AnimatedSection animation="fade-up" delay={200}>
            <div className="bg-gradient-to-br from-primary/5 to-accent/5 border border-primary/20 rounded-2xl p-8 md:p-10">
              <div className="grid grid-cols-1 md:grid-cols-3 gap-8">
                {/* Highlight 1 */}
                <div className="text-center">
                  <div className="w-12 h-12 bg-primary/20 border border-primary/40 rounded-lg flex items-center justify-center text-primary mx-auto mb-4">
                    <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
                    </svg>
                  </div>
                  <h4 className="text-foreground font-bold mb-2">100+ Pages</h4>
                  <p className="text-foreground/60 text-sm">
                    Comprehensive documentation covering every aspect of the plugin suite
                  </p>
                </div>

                {/* Highlight 2 */}
                <div className="text-center">
                  <div className="w-12 h-12 bg-accent/20 border border-accent/40 rounded-lg flex items-center justify-center text-accent mx-auto mb-4">
                    <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M14 10l-2 1m0 0l-2-1m2 1v2.5M20 7l-2 1m2-1l-2-1m2 1v2.5M14 4l-2-1-2 1M4 7l2-1M4 7l2 1M4 7v2.5M12 21l-2-1m2 1l2-1m-2 1v-2.5M6 18l-2-1v-2.5M18 18l2-1v-2.5" />
                    </svg>
                  </div>
                  <h4 className="text-foreground font-bold mb-2">Code Examples</h4>
                  <p className="text-foreground/60 text-sm">
                    Real-world C++ examples with explanations of DSP algorithms
                  </p>
                </div>

                {/* Highlight 3 */}
                <div className="text-center">
                  <div className="w-12 h-12 bg-primary/20 border border-primary/40 rounded-lg flex items-center justify-center text-primary mx-auto mb-4">
                    <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 15l-2 5L9 9l11 4-5 2zm0 0l5 5M7.188 2.239l.777 2.897M5.136 7.965l-2.898-.777M13.95 4.05l-2.122 2.122m-5.657 5.656l-2.12 2.122" />
                    </svg>
                  </div>
                  <h4 className="text-foreground font-bold mb-2">Always Current</h4>
                  <p className="text-foreground/60 text-sm">
                    Updated with every release, versioned for historical reference
                  </p>
                </div>
              </div>

              {/* CTA */}
              <div className="text-center mt-8">
                <a
                  href="/docs"
                  className="inline-flex items-center gap-2 px-6 py-3 bg-background border border-primary/30 text-primary rounded-lg hover:bg-background-lighter hover:border-primary/50 transition-all font-semibold"
                >
                  Browse All Documentation
                  <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
                  </svg>
                </a>
              </div>
            </div>
          </AnimatedSection>
        </div>
      </div>
    </section>
  );
};
