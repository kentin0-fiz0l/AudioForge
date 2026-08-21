import React from 'react';
import Image from 'next/image';
import type { Plugin } from '@/lib/plugins';

interface PluginCardProps {
  plugin: Plugin;
  className?: string;
}

const statusColors = {
  released: 'bg-green-500/20 text-green-400 border-green-500/30',
  'in-development': 'bg-yellow-500/20 text-yellow-400 border-yellow-500/30',
  planned: 'bg-blue-500/20 text-blue-400 border-blue-500/30',
};

const statusLabels = {
  released: 'Released',
  'in-development': 'In Development',
  planned: 'Planned',
};

export const PluginCard: React.FC<PluginCardProps> = ({ plugin, className = '' }) => {
  const isReleased = plugin.status === 'released';

  return (
    <article className={`group bg-foreground/5 border border-foreground/10 rounded-xl overflow-hidden hover:border-primary/50 transition-all duration-300 hover:shadow-lg hover:shadow-primary/10 ${className}`}>
      {/* Screenshot/Placeholder */}
      <div className="relative h-48 bg-gradient-to-br from-primary/20 to-accent/20 flex items-center justify-center overflow-hidden">
        {plugin.screenshots && plugin.screenshots.length > 0 ? (
          <Image
            src={plugin.screenshots[0]}
            alt={`${plugin.name} screenshot`}
            fill
            className="object-cover"
          />
        ) : (
          <div className="text-center p-6">
            <div className="text-5xl font-bold bg-gradient-primary bg-clip-text text-transparent mb-2">
              {plugin.name.substring(0, 2)}
            </div>
            <p className="text-foreground/40 text-sm">Preview coming soon</p>
          </div>
        )}

        {/* Status Badge */}
        <div className="absolute top-3 right-3">
          <span className={`px-3 py-1 rounded-full text-xs font-medium border backdrop-blur-sm ${statusColors[plugin.status]}`}>
            {statusLabels[plugin.status]}
          </span>
        </div>
      </div>

      {/* Content */}
      <div className="p-6">
        {/* Header */}
        <div className="mb-3">
          <div className="flex items-start justify-between mb-2">
            <h3 className="text-2xl font-bold text-foreground group-hover:text-primary transition-colors">
              {plugin.name}
            </h3>
            <span className="text-sm text-foreground/60 ml-2 mt-1">
              v{plugin.version}
            </span>
          </div>
          <p className="text-accent text-sm font-medium">
            {plugin.tagline}
          </p>
        </div>

        {/* Description */}
        <p className="text-foreground/70 text-sm mb-4 line-clamp-2">
          {plugin.description}
        </p>

        {/* Features */}
        <div className="mb-5">
          <h4 className="text-xs uppercase tracking-wider text-foreground/50 mb-2 font-semibold">
            Key Features
          </h4>
          <ul className="space-y-1">
            {plugin.features.slice(0, 3).map((feature, idx) => (
              <li key={idx} className="flex items-start text-sm text-foreground/80">
                <span className="text-primary mr-2 mt-0.5">•</span>
                <span>{feature.name}</span>
              </li>
            ))}
          </ul>
        </div>

        {/* Actions */}
        <div className="flex gap-2">
          {isReleased && plugin.downloadUrl && (
            <a
              href={plugin.downloadUrl}
              className="flex-1 px-4 py-2 bg-gradient-primary text-white text-sm font-semibold rounded-lg hover:opacity-90 transition-opacity text-center"
            >
              Download
            </a>
          )}
          {plugin.docsUrl && (
            <a
              href={plugin.docsUrl}
              className={`px-4 py-2 bg-foreground/10 text-foreground text-sm font-semibold rounded-lg hover:bg-foreground/20 transition-colors border border-foreground/20 text-center ${
                isReleased ? '' : 'flex-1'
              }`}
            >
              Docs
            </a>
          )}
          {!isReleased && (
            <button
              disabled
              className="flex-1 px-4 py-2 bg-foreground/5 text-foreground/40 text-sm font-semibold rounded-lg cursor-not-allowed border border-foreground/10"
            >
              Coming Soon
            </button>
          )}
        </div>
      </div>
    </article>
  );
};
