import React from 'react';

interface Feature {
  icon: string;
  title: string;
  description: string;
}

interface FeatureGridProps {
  className?: string;
}

const features: Feature[] = [
  {
    icon: '🎚️',
    title: 'Professional Quality',
    description: 'Built with JUCE framework and modern C++17. Same technology used by industry-leading plugins.',
  },
  {
    icon: '🆓',
    title: 'Free & Open Source',
    description: 'MIT licensed. Use, modify, and learn from the source code. No subscriptions, no locked features.',
  },
  {
    icon: '🔬',
    title: 'Learning-Focused',
    description: 'Comprehensive documentation and clean code. Perfect for studying audio DSP and plugin development.',
  },
  {
    icon: '⚡',
    title: 'High Performance',
    description: 'Optimized algorithms with no CPU spikes or crashes. Extensively tested with 88% test coverage.',
  },
  {
    icon: '🎨',
    title: 'Clean Interface',
    description: 'Intuitive, distraction-free UI design. Focus on your music, not on figuring out the plugin.',
  },
  {
    icon: '🌍',
    title: 'Cross-Platform',
    description: 'Works on macOS, Windows, and Linux. VST3 support with AU and AAX formats coming soon.',
  },
];

export const FeatureGrid: React.FC<FeatureGridProps> = ({ className = '' }) => {
  return (
    <section className={`py-16 md:py-24 bg-background ${className}`}>
      <div className="container mx-auto px-4">
        {/* Section Header */}
        <div className="text-center mb-12 md:mb-16">
          <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4">
            Why AudioForge?
          </h2>
          <p className="text-foreground/70 text-lg max-w-2xl mx-auto">
            Professional-grade audio plugins that don&apos;t compromise on quality or transparency.
          </p>
        </div>

        {/* Feature Grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 md:gap-8">
          {features.map((feature, idx) => (
            <div
              key={idx}
              className="group bg-foreground/5 border border-foreground/10 rounded-xl p-6 hover:border-primary/50 transition-all duration-300 hover:shadow-lg hover:shadow-primary/10"
            >
              {/* Icon */}
              <div className="text-4xl mb-4 group-hover:scale-110 transition-transform duration-300">
                {feature.icon}
              </div>

              {/* Title */}
              <h3 className="text-xl font-bold text-foreground mb-3 group-hover:text-primary transition-colors">
                {feature.title}
              </h3>

              {/* Description */}
              <p className="text-foreground/70 text-sm leading-relaxed">
                {feature.description}
              </p>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
};
