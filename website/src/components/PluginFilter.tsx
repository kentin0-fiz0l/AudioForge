'use client';

import React, { useState } from 'react';

interface PluginFilterProps {
  onFilterChange: (filter: string) => void;
  onSearchChange: (search: string) => void;
}

export const PluginFilter: React.FC<PluginFilterProps> = ({
  onFilterChange,
  onSearchChange,
}) => {
  const [activeFilter, setActiveFilter] = useState('all');
  const [searchTerm, setSearchTerm] = useState('');

  const filters = [
    { id: 'all', label: 'All Plugins', count: 13 },
    { id: 'released', label: 'Released', count: 13 },
    { id: 'utility', label: 'Utility', count: 3 },
    { id: 'effects', label: 'Effects', count: 7 },
    { id: 'synthesis', label: 'Synthesis', count: 3 },
  ];

  const handleFilterClick = (filterId: string) => {
    setActiveFilter(filterId);
    onFilterChange(filterId);
  };

  const handleSearchChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    setSearchTerm(value);
    onSearchChange(value);
  };

  return (
    <div className="mb-8 space-y-4">
      {/* Search bar */}
      <div className="relative max-w-md">
        <div className="absolute inset-y-0 left-0 flex items-center pl-4 pointer-events-none">
          <svg className="w-5 h-5 text-foreground-muted" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z" />
          </svg>
        </div>
        <input
          type="text"
          value={searchTerm}
          onChange={handleSearchChange}
          placeholder="Search plugins..."
          className="w-full pl-12 pr-4 py-3 bg-background-light border border-primary/20 rounded-lg text-foreground placeholder-foreground-dim focus:outline-none focus:border-primary/50 focus:ring-2 focus:ring-primary/20 transition-all"
        />
      </div>

      {/* Filter buttons */}
      <div className="flex flex-wrap gap-2">
        {filters.map((filter) => (
          <button
            key={filter.id}
            onClick={() => handleFilterClick(filter.id)}
            className={`px-4 py-2 rounded-lg font-medium text-sm transition-all ${
              activeFilter === filter.id
                ? 'bg-gradient-primary text-white shadow-glow-primary'
                : 'bg-background-light text-foreground-muted border border-primary/20 hover:border-primary/40 hover:text-foreground'
            }`}
          >
            {filter.label}
            <span className={`ml-2 ${activeFilter === filter.id ? 'text-white/80' : 'text-foreground-dim'}`}>
              ({filter.count})
            </span>
          </button>
        ))}
      </div>
    </div>
  );
};
