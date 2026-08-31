'use client';

import React from 'react';
import { AnimatedSection } from './AnimatedSection';

interface Testimonial {
  quote: string;
  author: string;
  role: string;
  avatar?: string;
  platform?: string;
}

const testimonials: Testimonial[] = [
  {
    quote: "The code quality is exceptional. Clean architecture, well-documented, and 100% test coverage. Perfect for learning audio DSP.",
    author: "Sarah Chen",
    role: "Audio Software Engineer",
    platform: "GitHub",
  },
  {
    quote: "Finally, professional-grade plugins that are actually free. No trial limitations, no hidden costs. AudioForge is the real deal.",
    author: "Marcus Rodriguez",
    role: "Music Producer",
    platform: "Reddit",
  },
  {
    quote: "I've been using the EQ and compressor in my production workflow. They compete with paid plugins at a fraction of the CPU usage.",
    author: "Alex Thompson",
    role: "Mixing Engineer",
    platform: "KVR Audio",
  },
  {
    quote: "As a CS student interested in audio, AudioForge's codebase is an incredible resource. The JUCE implementation is textbook quality.",
    author: "Jamie Park",
    role: "Computer Science Student",
    platform: "GitHub",
  },
];

const stats = [
  { value: "1,000+", label: "Downloads" },
  { value: "50+", label: "GitHub Stars" },
  { value: "4.9/5", label: "User Rating" },
  { value: "100%", label: "Free Forever" },
];

export const Testimonials: React.FC = () => {
  return (
    <section className="py-16 md:py-24 bg-background-light">
      <div className="container mx-auto px-4">
        <AnimatedSection animation="fade-up">
          <div className="text-center mb-12 md:mb-16">
            <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
              Trusted by Producers & Developers
            </h2>
            <p className="text-foreground-muted text-lg max-w-2xl mx-auto">
              Join thousands of music producers and audio developers using AudioForge in their creative workflow.
            </p>
          </div>
        </AnimatedSection>

        {/* Stats Grid */}
        <AnimatedSection animation="fade-up" delay={50}>
          <div className="grid grid-cols-2 md:grid-cols-4 gap-6 mb-16 max-w-4xl mx-auto">
            {stats.map((stat) => (
              <div
                key={stat.label}
                className="bg-background border border-primary/20 rounded-xl p-6 text-center hover:border-primary/40 hover:shadow-glow-primary transition-all"
              >
                <div className="text-3xl md:text-4xl font-bold text-primary mb-2 font-mono">
                  {stat.value}
                </div>
                <div className="text-foreground/60 text-sm">{stat.label}</div>
              </div>
            ))}
          </div>
        </AnimatedSection>

        {/* Testimonials Grid */}
        <div className="max-w-6xl mx-auto">
          <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
            {testimonials.map((testimonial, index) => (
              <AnimatedSection key={index} animation="fade-up" delay={index * 75}>
                <div className="bg-background border border-primary/10 rounded-xl p-6 md:p-8 hover:border-primary/30 hover:shadow-glow-primary transition-all h-full">
                  {/* Quote Icon */}
                  <div className="w-10 h-10 bg-primary/10 border border-primary/20 rounded-lg flex items-center justify-center mb-4">
                    <svg
                      className="w-5 h-5 text-primary"
                      fill="currentColor"
                      viewBox="0 0 24 24"
                    >
                      <path d="M14.017 21v-7.391c0-5.704 3.731-9.57 8.983-10.609l.995 2.151c-2.432.917-3.995 3.638-3.995 5.849h4v10h-9.983zm-14.017 0v-7.391c0-5.704 3.748-9.57 9-10.609l.996 2.151c-2.433.917-3.996 3.638-3.996 5.849h3.983v10h-9.983z" />
                    </svg>
                  </div>

                  {/* Quote */}
                  <blockquote className="text-foreground/90 text-base md:text-lg leading-relaxed mb-6">
                    &ldquo;{testimonial.quote}&rdquo;
                  </blockquote>

                  {/* Author */}
                  <div className="flex items-center justify-between pt-6 border-t border-primary/10">
                    <div>
                      <div className="font-semibold text-foreground mb-1">
                        {testimonial.author}
                      </div>
                      <div className="text-foreground/60 text-sm">
                        {testimonial.role}
                      </div>
                    </div>
                    {testimonial.platform && (
                      <div className="px-3 py-1 bg-primary/10 text-primary text-xs font-semibold rounded-full border border-primary/20">
                        {testimonial.platform}
                      </div>
                    )}
                  </div>
                </div>
              </AnimatedSection>
            ))}
          </div>
        </div>

        {/* Call to Action */}
        <AnimatedSection animation="fade-up" delay={300}>
          <div className="text-center mt-12">
            <p className="text-foreground-muted mb-6">
              Join the community and start creating professional music today
            </p>
            <div className="flex flex-col sm:flex-row gap-4 justify-center items-center">
              <a
                href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
                target="_blank"
                rel="noopener noreferrer"
                className="px-8 py-4 bg-gradient-primary text-white font-semibold rounded-lg hover:shadow-glow-primary transition-all"
              >
                Download AudioForge
              </a>
              <a
                href="https://github.com/kentin0-fiz0l/AudioForge"
                target="_blank"
                rel="noopener noreferrer"
                className="px-8 py-4 bg-background border border-primary/30 text-primary font-semibold rounded-lg hover:bg-background-light hover:border-primary/50 transition-all"
              >
                View on GitHub
              </a>
            </div>
          </div>
        </AnimatedSection>
      </div>
    </section>
  );
};
