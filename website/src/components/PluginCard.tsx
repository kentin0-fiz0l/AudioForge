import React from 'react';
import Image from 'next/image';
import type { Plugin } from '@/lib/plugins';
import { Badge } from '@/components/Badge';

interface PluginCardProps {
  plugin: Plugin;
  className?: string;
}

const statusColors = {
  released: 'bg-success/20 text-success border-success/30',
  'in-development': 'bg-warning/20 text-warning border-warning/30',
  planned: 'bg-primary/20 text-primary border-primary/30',
};

const statusLabels = {
  released: 'Released',
  'in-development': 'In Development',
  planned: 'Planned',
};

export const PluginCard: React.FC<PluginCardProps> = ({ plugin, className = '' }) => {
  const isReleased = plugin.status === 'released';

  return (
    <article className={`group relative bg-background-light border border-primary/10 rounded-xl overflow-hidden hover:border-primary/40 transition-all duration-300 hover:shadow-glow-primary ${className}`}>
      {/* Ambient glow effect */}
      <div className="absolute inset-0 bg-gradient-to-br from-primary/5 via-transparent to-accent/5 opacity-0 group-hover:opacity-100 transition-opacity duration-300" />

      {/* Graphic/Screenshot/Placeholder */}
      <div className="relative h-48 bg-gradient-to-br from-background to-background-light flex items-center justify-center overflow-hidden border-b border-primary/10">
        {plugin.graphic ? (
          <Image
            src={plugin.graphic}
            alt={`${plugin.name} visual`}
            width={400}
            height={300}
            className="object-contain w-full h-full opacity-90 group-hover:opacity-100 group-hover:scale-105 transition-all duration-300"
          />
        ) : plugin.screenshots && plugin.screenshots.length > 0 ? (
          <Image
            src={plugin.screenshots[0]}
            alt={`${plugin.name} screenshot`}
            fill
            className="object-cover opacity-90 group-hover:opacity-100 transition-opacity"
          />
        ) : (
          <div className="text-center p-6">
            {/* Waveform placeholder */}
            <div className="flex justify-center gap-1 mb-4">
              {Array.from({ length: 12 }).map((_, i) => (
                <div
                  key={i}
                  className="w-1.5 bg-gradient-to-t from-primary to-accent rounded-full"
                  style={{
                    height: `${30 + Math.random() * 40}px`,
                  }}
                />
              ))}
            </div>
            <p className="text-foreground-dim text-sm font-mono">Preview coming soon</p>
          </div>
        )}

        {/* Status Badge */}
        <div className="absolute top-3 right-3">
          <span className={`px-3 py-1 rounded-full text-xs font-semibold border backdrop-blur-md ${statusColors[plugin.status]} font-mono`}>
            {statusLabels[plugin.status]}
          </span>
        </div>
      </div>

      {/* Content */}
      <div className="relative p-6">
        {/* Header */}
        <div className="mb-4">
          <div className="flex items-start justify-between mb-2">
            <h3 className="text-2xl font-bold text-foreground group-hover:text-primary transition-colors font-display">
              {plugin.name}
            </h3>
            <span className="text-sm text-foreground-dim ml-2 mt-1 font-mono">
              v{plugin.version}
            </span>
          </div>
          <p className="text-accent text-sm font-medium">
            {plugin.tagline}
          </p>

          {/* Badges */}
          {plugin.badges && plugin.badges.length > 0 && (
            <div className="flex flex-wrap gap-2 mt-3">
              {plugin.badges.map((badge, idx) => (
                <Badge key={idx} type={badge} />
              ))}
            </div>
          )}
        </div>

        {/* Description */}
        <p className="text-foreground-muted text-sm mb-5 line-clamp-2 leading-relaxed">
          {plugin.description}
        </p>

        {/* Features with audio-inspired bullets */}
        <div className="mb-6">
          <h4 className="text-xs uppercase tracking-wider text-foreground-dim mb-3 font-semibold font-mono">
            Key Features
          </h4>
          <ul className="space-y-2">
            {plugin.features.slice(0, 3).map((feature, idx) => (
              <li key={idx} className="flex items-start text-sm text-foreground-muted">
                <svg className="w-4 h-4 text-primary mr-2 mt-0.5 flex-shrink-0" fill="currentColor" viewBox="0 0 20 20">
                  <circle cx="10" cy="10" r="3" />
                </svg>
                <span>{feature.name}</span>
              </li>
            ))}
          </ul>
        </div>

        {/* Waveform separator */}
        <div className="h-px bg-gradient-to-r from-transparent via-primary/30 to-transparent mb-5" />

        {/* Actions */}
        <div className="flex gap-3">
          {isReleased && plugin.downloadUrl && (
            <a
              href={plugin.downloadUrl}
              className="group/btn relative flex-1 px-4 py-2.5 bg-gradient-primary text-white text-sm font-semibold rounded-lg transition-all hover:shadow-glow-primary text-center overflow-hidden"
            >
              <div className="absolute inset-0 bg-gradient-to-r from-primary-light to-primary opacity-0 group-hover/btn:opacity-100 transition-opacity" />
              <span className="relative flex items-center justify-center gap-2">
                <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
                </svg>
                Download
              </span>
            </a>
          )}
          {plugin.docsUrl && (
            <a
              href={plugin.docsUrl}
              className={`px-4 py-2.5 bg-background text-foreground text-sm font-semibold rounded-lg hover:bg-background-lighter transition-colors border border-primary/30 hover:border-primary/50 text-center ${
                isReleased ? '' : 'flex-1'
              }`}
            >
              Docs
            </a>
          )}
          {!isReleased && (
            <button
              disabled
              className="flex-1 px-4 py-2.5 bg-background text-foreground-dim text-sm font-semibold rounded-lg cursor-not-allowed border border-primary/10"
            >
              Coming Soon
            </button>
          )}
        </div>
      </div>
    </article>
  );
};
