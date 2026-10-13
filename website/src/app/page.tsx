'use client';

import { useState, useMemo } from 'react';
import { Hero } from '@/components/Hero';
import { PluginCard } from '@/components/PluginCard';
import { Footer } from '@/components/Footer';
import { Stats } from '@/components/Stats';
import { StickyDownload } from '@/components/StickyDownload';
import { ScrollToTop } from '@/components/ScrollToTop';
import { FAQ } from '@/components/FAQ';
import { AIProducerSection } from '@/components/AIProducerSection';
import { DownloadSection } from '@/components/DownloadSection';
import { PluginFilter, type PluginCategory } from '@/components/PluginFilter';
import { plugins } from '@/lib/plugins';

export default function Home() {
  const [activeCategory, setActiveCategory] = useState<PluginCategory>('all');
  const [searchQuery, setSearchQuery] = useState('');

  const filteredPlugins = useMemo(() => {
    const query = searchQuery.toLowerCase();
    return plugins.filter((plugin) =>
      (activeCategory === 'all' || plugin.category === activeCategory)
      && (query === ''
        || plugin.name.toLowerCase().includes(query)
        || plugin.tagline.toLowerCase().includes(query)
        || plugin.description.toLowerCase().includes(query)));
  }, [activeCategory, searchQuery]);

  return (
    <main className="min-h-screen">
      <StickyDownload />
      <ScrollToTop />
      <Hero />
      <Stats />

      <section id="plugins" className="py-16 md:py-24 bg-background-light">
        <div className="container mx-auto px-4">
          <div className="text-center mb-12">
            <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
              The plugins
            </h2>
            <p className="text-foreground-muted text-lg max-w-2xl mx-auto">
              All {plugins.length}: effects, instruments and MIDI tools. Every one is free.
            </p>
          </div>

          <PluginFilter
            onFilterChange={setActiveCategory}
            onSearchChange={setSearchQuery}
            activeCategory={activeCategory}
            searchQuery={searchQuery}
          />

          {filteredPlugins.length > 0 ? (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 md:gap-8">
              {filteredPlugins.map((plugin) => (
                <PluginCard key={plugin.id} plugin={plugin} />
              ))}
            </div>
          ) : (
            <div className="text-center py-16">
              <h3 className="text-xl font-bold text-foreground mb-2">No plugins found</h3>
              <p className="text-foreground/60 mb-6">Try another search or category.</p>
              <button
                onClick={() => {
                  setActiveCategory('all');
                  setSearchQuery('');
                }}
                className="px-6 py-3 bg-gradient-primary text-white font-semibold rounded-lg hover:shadow-glow-primary transition-all"
              >
                Clear filters
              </button>
            </div>
          )}
        </div>
      </section>

      <AIProducerSection />
      <DownloadSection />
      <FAQ />
      <Footer />
    </main>
  );
}
