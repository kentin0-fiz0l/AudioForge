'use client';

import React, { useState } from 'react';

export type PluginCategory = 'all' | 'utility' | 'creative' | 'mixing' | 'synthesis';

interface PluginFilterProps {
  onFilterChange: (category: PluginCategory) => void;
  onSearchChange: (query: string) => void;
  activeCategory: PluginCategory;
  searchQuery: string;
}

const categories: { id: PluginCategory; label: string; icon: string }[] = [
  { id: 'all', label: 'All Plugins', icon: '🎵' },
  { id: 'utility', label: 'Utility', icon: '🔧' },
  { id: 'creative', label: 'Creative', icon: '🎨' },
  { id: 'mixing', label: 'Mixing', icon: '🎚️' },
  { id: 'synthesis', label: 'Synthesis', icon: '🎹' },
];

export const PluginFilter: React.FC<PluginFilterProps> = ({
  onFilterChange,
  onSearchChange,
  activeCategory,
  searchQuery,
}) => {
  const [isFocused, setIsFocused] = useState(false);

  return (
    <div className="mb-12">
      {/* Search Bar */}
      <div className="max-w-2xl mx-auto mb-8">
        <div
          className={`relative transition-all duration-300 ${
            isFocused ? 'scale-105' : 'scale-100'
          }`}
        >
          <input
            type="text"
            placeholder="Search plugins..."
            value={searchQuery}
            onChange={(e) => onSearchChange(e.target.value)}
            onFocus={() => setIsFocused(true)}
            onBlur={() => setIsFocused(false)}
            className={`w-full px-6 py-4 pl-14 bg-background-light border rounded-xl text-foreground placeholder:text-foreground/40 focus:outline-none transition-all ${
              isFocused
                ? 'border-primary/50 shadow-glow-primary'
                : 'border-primary/20'
            }`}
          />
          <svg
            className={`absolute left-5 top-1/2 -translate-y-1/2 w-5 h-5 transition-colors ${
              isFocused ? 'text-primary' : 'text-foreground/40'
            }`}
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth={2}
              d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z"
            />
          </svg>
          {searchQuery && (
            <button
              onClick={() => onSearchChange('')}
              className="absolute right-5 top-1/2 -translate-y-1/2 w-6 h-6 flex items-center justify-center text-foreground/60 hover:text-foreground transition-colors"
              aria-label="Clear search"
            >
              <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
              </svg>
            </button>
          )}
        </div>
      </div>

      {/* Category Filters */}
      <div className="flex flex-wrap gap-3 justify-center">
        {categories.map((category) => (
          <button
            key={category.id}
            onClick={() => onFilterChange(category.id)}
            className={`px-6 py-3 rounded-lg font-semibold transition-all duration-300 flex items-center gap-2 ${
              activeCategory === category.id
                ? 'bg-gradient-primary text-white shadow-glow-primary scale-105'
                : 'bg-background-light text-foreground/80 border border-primary/20 hover:border-primary/40 hover:text-foreground'
            }`}
          >
            <span className="text-lg">{category.icon}</span>
            <span>{category.label}</span>
            {activeCategory === category.id && (
              <div className="w-2 h-2 bg-white rounded-full animate-pulse" />
            )}
          </button>
        ))}
      </div>

      {/* Active Filters Display */}
      {(searchQuery || activeCategory !== 'all') && (
        <div className="mt-6 flex flex-wrap items-center justify-center gap-3">
          <span className="text-foreground/60 text-sm">Active filters:</span>
          {activeCategory !== 'all' && (
            <div className="px-4 py-2 bg-primary/10 border border-primary/30 text-primary rounded-full text-sm font-semibold flex items-center gap-2">
              {categories.find((c) => c.id === activeCategory)?.label}
              <button
                onClick={() => onFilterChange('all')}
                className="hover:text-primary-light transition-colors"
                aria-label="Clear category filter"
              >
                <svg className="w-3 h-3" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            </div>
          )}
          {searchQuery && (
            <div className="px-4 py-2 bg-accent/10 border border-accent/30 text-accent rounded-full text-sm font-semibold flex items-center gap-2">
              Search: &ldquo;{searchQuery}&rdquo;
              <button
                onClick={() => onSearchChange('')}
                className="hover:text-accent-light transition-colors"
                aria-label="Clear search"
              >
                <svg className="w-3 h-3" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            </div>
          )}
          <button
            onClick={() => {
              onFilterChange('all');
              onSearchChange('');
            }}
            className="text-foreground/60 hover:text-foreground text-sm font-semibold transition-colors underline"
          >
            Clear all
          </button>
        </div>
      )}
    </div>
  );
};
