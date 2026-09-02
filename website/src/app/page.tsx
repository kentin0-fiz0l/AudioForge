'use client';

import { useState, useMemo } from 'react';
import { Hero } from '@/components/Hero';
import { PluginCard } from '@/components/PluginCard';
import { FeatureGrid } from '@/components/FeatureGrid';
import { Footer } from '@/components/Footer';
import { Stats } from '@/components/Stats';
import { StickyDownload } from '@/components/StickyDownload';
import { ScrollToTop } from '@/components/ScrollToTop';
import { AnimatedSection } from '@/components/AnimatedSection';
import { FAQ } from '@/components/FAQ';
import { Newsletter } from '@/components/Newsletter';
import { DocsPreview } from '@/components/DocsPreview';
import { Testimonials } from '@/components/Testimonials';
import { PluginFilter, type PluginCategory } from '@/components/PluginFilter';
import { plugins, getCurrentPhase, roadmap } from '@/lib/plugins';

export default function Home() {
  const [activeCategory, setActiveCategory] = useState<PluginCategory>('all');
  const [searchQuery, setSearchQuery] = useState('');

  const currentPhase = getCurrentPhase();

  // Filter plugins based on category and search
  const filteredPlugins = useMemo(() => {
    return plugins.filter((plugin) => {
      // Status filter
      const isReleased = plugin.status === 'released';

      // Category filter
      const matchesCategory = activeCategory === 'all' || plugin.category === activeCategory;

      // Search filter
      const matchesSearch =
        searchQuery === '' ||
        plugin.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
        plugin.tagline.toLowerCase().includes(searchQuery.toLowerCase()) ||
        plugin.description.toLowerCase().includes(searchQuery.toLowerCase());

      return isReleased && matchesCategory && matchesSearch;
    });
  }, [activeCategory, searchQuery]);

  const upcomingPlugins = plugins.filter(p => p.status === 'in-development' || p.status === 'planned');

  return (
    <main className="min-h-screen">
      {/* Sticky Download Button */}
      <StickyDownload />

      {/* Scroll to Top Button */}
      <ScrollToTop />

      {/* Hero Section */}
      <Hero />

      {/* Stats Section */}
      <Stats />

      {/* Plugin Showcase */}
      <section id="plugins" className="py-16 md:py-24 bg-background-light">
        <div className="container mx-auto px-4">
          {/* Section Header */}
          <AnimatedSection animation="fade-up">
            <div className="text-center mb-12 md:mb-16">
              <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
                Our Plugins
              </h2>
              <p className="text-foreground-muted text-lg max-w-2xl mx-auto">
                Professional audio tools for every step of your production workflow.
              </p>
            </div>
          </AnimatedSection>

          {/* Plugin Filter */}
          <AnimatedSection animation="fade-up">
            <PluginFilter
              onFilterChange={setActiveCategory}
              onSearchChange={setSearchQuery}
              activeCategory={activeCategory}
              searchQuery={searchQuery}
            />
          </AnimatedSection>

          {/* Filtered Plugins Grid */}
          {filteredPlugins.length > 0 ? (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 md:gap-8 mb-12">
              {filteredPlugins.map((plugin, index) => (
                <AnimatedSection key={plugin.id} animation="fade-up" delay={index * 50}>
                  <PluginCard plugin={plugin} />
                </AnimatedSection>
              ))}
            </div>
          ) : (
            <AnimatedSection animation="fade-up">
              <div className="text-center py-16">
                <div className="w-16 h-16 mx-auto mb-4 bg-primary/10 border border-primary/20 rounded-full flex items-center justify-center">
                  <svg className="w-8 h-8 text-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z" />
                  </svg>
                </div>
                <h3 className="text-xl font-bold text-foreground mb-2">No plugins found</h3>
                <p className="text-foreground/60 mb-6">
                  Try adjusting your search or filter to find what you&apos;re looking for.
                </p>
                <button
                  onClick={() => {
                    setActiveCategory('all');
                    setSearchQuery('');
                  }}
                  className="px-6 py-3 bg-gradient-primary text-white font-semibold rounded-lg hover:shadow-glow-primary transition-all"
                >
                  Clear Filters
                </button>
              </div>
            </AnimatedSection>
          )}

          {/* Upcoming Plugins Grid */}
          {upcomingPlugins.length > 0 && (
            <>
              <AnimatedSection animation="fade-up">
                <h3 className="text-2xl font-bold text-foreground mb-6">Coming Soon</h3>
              </AnimatedSection>
              <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 md:gap-8">
                {upcomingPlugins.map((plugin, index) => (
                  <AnimatedSection key={plugin.id} animation="fade-up" delay={index * 50}>
                    <PluginCard plugin={plugin} />
                  </AnimatedSection>
                ))}
              </div>
            </>
          )}
        </div>
      </section>

      {/* Value Propositions */}
      <FeatureGrid />

      {/* Documentation Preview */}
      <DocsPreview />

      {/* Roadmap Teaser */}
      <section className="py-16 md:py-24 bg-background-light">
        <div className="container mx-auto px-4">
          {/* Section Header */}
          <div className="text-center mb-12 md:mb-16">
            <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
              Roadmap
            </h2>
            <p className="text-foreground-muted text-lg max-w-2xl mx-auto">
              We&apos;re building a complete suite of production tools. Here&apos;s what we&apos;re working on.
            </p>
          </div>

          {/* Current Phase */}
          {currentPhase && (
            <div className="max-w-4xl mx-auto mb-12">
              <div className="bg-background border border-primary/30 rounded-2xl p-8 shadow-glow-primary">
                <div className="flex items-center gap-3 mb-4">
                  <span className="px-4 py-1.5 bg-primary/20 text-primary rounded-full text-sm font-semibold border border-primary font-mono">
                    Current Phase
                  </span>
                  <h3 className="text-2xl md:text-3xl font-bold text-foreground font-display">
                    {currentPhase.name}
                  </h3>
                </div>
                <p className="text-foreground-muted text-lg mb-6">{currentPhase.goal}</p>
                {currentPhase.timeline && (
                  <p className="text-foreground/60 mb-6">Timeline: {currentPhase.timeline}</p>
                )}

                {/* Current Phase Items */}
                <div className="space-y-4">
                  {currentPhase.items.map((item, idx) => (
                    <div key={idx} className="flex items-start gap-3">
                      {/* Status Icon */}
                      <div className="mt-1 flex-shrink-0">
                        {item.status === 'complete' && (
                          <div className="w-6 h-6 rounded-full bg-green-500/20 border border-green-500/50 flex items-center justify-center">
                            <span className="text-green-400 text-sm">✓</span>
                          </div>
                        )}
                        {item.status === 'in-progress' && (
                          <div className="w-6 h-6 rounded-full bg-yellow-500/20 border border-yellow-500/50 flex items-center justify-center">
                            <span className="text-yellow-400 text-xs">●</span>
                          </div>
                        )}
                        {item.status === 'planned' && (
                          <div className="w-6 h-6 rounded-full bg-blue-500/20 border border-blue-500/50 flex items-center justify-center">
                            <span className="text-blue-400 text-xs">○</span>
                          </div>
                        )}
                      </div>

                      {/* Item Content */}
                      <div className="flex-1">
                        <h4 className="text-foreground font-semibold mb-1">{item.name}</h4>
                        {item.features && item.features.length > 0 && (
                          <ul className="text-foreground/60 text-sm space-y-0.5">
                            {item.features.map((feature, featureIdx) => (
                              <li key={featureIdx} className="flex items-start">
                                <span className="text-primary mr-2">•</span>
                                <span>{feature}</span>
                              </li>
                            ))}
                          </ul>
                        )}
                      </div>
                    </div>
                  ))}
                </div>
              </div>
            </div>
          )}

          {/* Upcoming Phases Preview */}
          <div className="max-w-4xl mx-auto">
            <h3 className="text-2xl font-bold text-foreground mb-6 text-center">What&apos;s Next</h3>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
              {roadmap
                .filter(phase => phase.status === 'planned')
                .slice(0, 2)
                .map((phase) => (
                  <div
                    key={phase.id}
                    className="bg-foreground/5 border border-foreground/10 rounded-xl p-6 hover:border-primary/30 transition-colors"
                  >
                    <h4 className="text-xl font-bold text-foreground mb-2">{phase.name}</h4>
                    <p className="text-foreground/70 text-sm mb-4">{phase.goal}</p>
                    <div className="space-y-2">
                      {phase.items.slice(0, 3).map((item, itemIdx) => (
                        <div key={itemIdx} className="flex items-start gap-2">
                          <span className="text-primary text-xs mt-1">▹</span>
                          <span className="text-foreground/60 text-sm">{item.name}</span>
                        </div>
                      ))}
                      {phase.items.length > 3 && (
                        <p className="text-foreground/40 text-xs italic">
                          +{phase.items.length - 3} more items...
                        </p>
                      )}
                    </div>
                  </div>
                ))}
            </div>
          </div>
        </div>
      </section>

      {/* Community & Support */}
      <section className="py-16 md:py-24 bg-background">
        <div className="container mx-auto px-4">
          <div className="max-w-4xl mx-auto">
            {/* Section Header */}
            <div className="text-center mb-12">
              <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
                Community & Support
              </h2>
              <p className="text-foreground-muted text-lg">
                Join our growing community of producers and developers.
              </p>
            </div>

            {/* Community Cards */}
            <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-12">
              {/* GitHub */}
              <div className="bg-background-light border border-primary/10 rounded-xl p-8 hover:border-primary/40 transition-all duration-300 hover:shadow-glow-primary">
                <div className="text-4xl mb-4">💻</div>
                <h3 className="text-2xl font-bold text-foreground mb-3">Open Source</h3>
                <p className="text-foreground/70 mb-6">
                  All code is MIT licensed and available on GitHub. Report bugs, request features, or contribute to the project.
                </p>
                <a
                  href="https://github.com/kentin0-fiz0l/AudioForge"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="inline-flex items-center text-primary hover:text-primary-light font-semibold transition-colors"
                >
                  View on GitHub
                  <span className="ml-2">→</span>
                </a>
              </div>

              {/* Documentation */}
              <div className="bg-background-light border border-primary/10 rounded-xl p-8 hover:border-primary/40 transition-all duration-300 hover:shadow-glow-primary">
                <div className="text-4xl mb-4">📚</div>
                <h3 className="text-2xl font-bold text-foreground mb-3">Documentation</h3>
                <p className="text-foreground/70 mb-6">
                  Comprehensive guides for using the plugins and learning audio DSP. Perfect for producers and developers alike.
                </p>
                <a
                  href="/docs"
                  className="inline-flex items-center text-primary hover:text-primary-light font-semibold transition-colors"
                >
                  Read the Docs
                  <span className="ml-2">→</span>
                </a>
              </div>

              {/* Support */}
              <div className="bg-background-light border border-primary/10 rounded-xl p-8 hover:border-primary/40 transition-all duration-300 hover:shadow-glow-primary">
                <div className="text-4xl mb-4">💬</div>
                <h3 className="text-2xl font-bold text-foreground mb-3">Get Help</h3>
                <p className="text-foreground/70 mb-6">
                  Questions? Issues? We&apos;re here to help. Open an issue on GitHub or join our community discussions.
                </p>
                <a
                  href="https://github.com/kentin0-fiz0l/AudioForge/issues"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="inline-flex items-center text-primary hover:text-primary-light font-semibold transition-colors"
                >
                  Get Support
                  <span className="ml-2">→</span>
                </a>
              </div>

              {/* Learning */}
              <div className="bg-background-light border border-primary/10 rounded-xl p-8 hover:border-primary/40 transition-all duration-300 hover:shadow-glow-primary">
                <div className="text-4xl mb-4">🎓</div>
                <h3 className="text-2xl font-bold text-foreground mb-3">Learn Audio DSP</h3>
                <p className="text-foreground/70 mb-6">
                  Clean, well-documented code with 100% test coverage (136 passing tests). Study real-world implementations of audio algorithms.
                </p>
                <a
                  href="/docs/learning"
                  className="inline-flex items-center text-primary hover:text-primary-light font-semibold transition-colors"
                >
                  Start Learning
                  <span className="ml-2">→</span>
                </a>
              </div>
            </div>

            {/* CTA Section */}
            <div className="bg-background border border-primary/30 rounded-2xl p-8 md:p-12 text-center shadow-glow-primary">
              <h3 className="text-2xl md:text-3xl font-bold text-foreground mb-4 font-display">
                Ready to Try AudioForge?
              </h3>
              <p className="text-foreground-muted text-lg mb-8 max-w-2xl mx-auto">
                Download AudioForge v1.8.0 with 22 plugins (VST3 & AU) and start making music today. Free forever, no strings attached.
              </p>
              <a
                href="https://github.com/kentin0-fiz0l/AudioForge/releases/latest"
                target="_blank"
                rel="noopener noreferrer"
                className="inline-block px-10 py-4 bg-gradient-primary text-white font-semibold text-lg rounded-lg hover:shadow-glow-primary transition-all"
              >
                Download v1.8.0
              </a>
            </div>
          </div>
        </div>
      </section>

      {/* Testimonials Section */}
      <Testimonials />

      {/* FAQ Section */}
      <FAQ />

      {/* Newsletter Signup */}
      <Newsletter />

      <Footer />
    </main>
  );
}
