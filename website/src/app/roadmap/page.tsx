import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Roadmap | AudioForge',
  description: 'AudioForge development roadmap: from learning fundamentals to building a complete plugin suite and establishing a sustainable business.',
  openGraph: {
    title: 'Roadmap | AudioForge',
    description: 'AudioForge development roadmap: from learning fundamentals to building a complete plugin suite and establishing a sustainable business.',
  },
};

interface RoadmapPhase {
  id: number;
  title: string;
  status: 'current' | 'next' | 'future';
  goal: string;
  timeline?: string;
  items: {
    name: string;
    status: 'complete' | 'in-progress' | 'planned';
    timeline?: string;
    description?: string;
    features?: string[];
  }[];
}

const roadmapData: RoadmapPhase[] = [
  {
    id: 1,
    title: 'Foundation & Learning',
    status: 'current',
    goal: 'Build 3 simple plugins to learn JUCE and DSP fundamentals',
    items: [
      {
        name: 'SimpleGain',
        status: 'complete',
        description: 'Basic gain control plugin with real-time metering',
        features: [
          'Gain control (-60 to +12 dB)',
          'Real-time level meter',
          'Smooth parameter changes',
          'Clean rotary knob UI',
          'VST3 + Standalone builds',
        ],
      },
      {
        name: 'PanUtil',
        status: 'planned',
        timeline: '1-2 weeks',
        description: 'Stereo panning and width control utility',
        features: [
          'Stereo pan control (-100% L to +100% R)',
          'Constant-power panning algorithm',
          'Balance mode vs Pan mode',
          'Width control (stereo widening/narrowing)',
          'Dual meters (L/R)',
          'Phase correlation meter',
        ],
      },
      {
        name: 'BasicSynth',
        status: 'planned',
        timeline: '2-3 weeks',
        description: 'Simple subtractive synthesizer',
        features: [
          '3 oscillator waveforms (sine, saw, square)',
          'ADSR envelope',
          'Low-pass filter with resonance',
          'MIDI note handling',
          'Polyphony (4-8 voices)',
          'Simple visual keyboard',
        ],
      },
    ],
  },
  {
    id: 2,
    title: 'Core Plugin Suite',
    status: 'next',
    goal: 'Build useful, professional-quality effects',
    items: [
      {
        name: 'CleanDelay',
        status: 'planned',
        description: 'Versatile delay effect with tempo sync',
        features: [
          'Delay time (1ms - 2 seconds)',
          'Feedback control',
          'Wet/dry mix',
          'Sync to DAW tempo',
          'Stereo ping-pong mode',
          'High-pass filter in feedback path',
        ],
      },
      {
        name: 'SimpleEQ',
        status: 'planned',
        description: 'Parametric equalizer with visual feedback',
        features: [
          '3-band parametric EQ',
          'High-pass and low-pass filters',
          'Visual frequency response curve',
          'Spectrum analyzer',
        ],
      },
      {
        name: 'SimpleComp',
        status: 'planned',
        description: 'Clean and transparent compressor',
        features: [
          'Threshold, ratio, attack, release',
          'Automatic makeup gain',
          'Gain reduction meter',
          'Soft knee option',
        ],
      },
    ],
  },
  {
    id: 3,
    title: 'Distribution & Website',
    status: 'future',
    goal: 'Get plugins into users\' hands, build community',
    items: [
      {
        name: 'Distribution Infrastructure',
        status: 'planned',
        features: [
          'Website with downloads',
          'User documentation for each plugin',
          'Video tutorials (YouTube)',
          'Code-signing certificates (macOS + Windows)',
          'Installers (macOS .pkg, Windows .exe)',
          'AU (Audio Units) support for macOS',
          'Linux builds',
          'Beta testing program',
        ],
      },
    ],
  },
  {
    id: 4,
    title: 'Advanced Plugins',
    status: 'future',
    goal: 'Build unique, creative tools',
    items: [
      {
        name: 'Creative Effects',
        status: 'planned',
        description: 'Innovative and unique audio processing tools',
        features: [
          'Granular synthesis plugin',
          'Convolution reverb',
          'Spectral processing tools',
          'Creative modulation effects',
          'Wavetable synthesizer',
        ],
      },
    ],
  },
  {
    id: 5,
    title: 'Monetization',
    status: 'future',
    goal: 'Generate revenue while keeping core plugins free',
    items: [
      {
        name: 'Business Model',
        status: 'planned',
        description: 'Sustainable revenue while maintaining free tier',
        features: [
          'Free tier: SimpleGain, PanUtil, BasicSynth, CleanDelay, SimpleEQ, SimpleComp',
          'Premium tier: Advanced synths and effects ($29-49)',
          'Expansion packs (presets, wavetables)',
          'Early access to new plugins',
        ],
      },
    ],
  },
];

const milestones = [
  {
    category: 'Build System',
    items: [
      { name: 'CMake configuration for JUCE', status: 'complete' as const },
      { name: 'VST3 builds', status: 'complete' as const },
      { name: 'AU (Audio Units) support', status: 'planned' as const },
      { name: 'AAX (Pro Tools) support', status: 'planned' as const },
      { name: 'Automated testing', status: 'planned' as const },
      { name: 'CI/CD pipeline (GitHub Actions)', status: 'planned' as const },
    ],
  },
  {
    category: 'Code Quality',
    items: [
      { name: 'Unit tests for DSP algorithms', status: 'planned' as const },
      { name: 'Integration tests', status: 'planned' as const },
      { name: 'Code coverage reporting', status: 'planned' as const },
      { name: 'Static analysis', status: 'planned' as const },
      { name: 'Performance profiling', status: 'planned' as const },
    ],
  },
  {
    category: 'Documentation',
    items: [
      { name: 'DSP basics guide', status: 'complete' as const },
      { name: 'JUCE patterns guide', status: 'complete' as const },
      { name: 'Contributing guidelines', status: 'planned' as const },
      { name: 'API documentation (Doxygen)', status: 'planned' as const },
      { name: 'Plugin user manuals', status: 'planned' as const },
    ],
  },
];

const successMetrics = [
  {
    phase: 'Phase 1 (Learning)',
    metrics: [
      '3 working plugins',
      'Understanding of core DSP concepts',
      'Comfortable with JUCE framework',
    ],
  },
  {
    phase: 'Phase 2 (Quality)',
    metrics: [
      'Plugins that sound professional',
      'No CPU spikes or crashes',
      'Positive feedback from beta testers',
    ],
  },
  {
    phase: 'Phase 3 (Distribution)',
    metrics: [
      '100+ downloads in first month',
      'Active user feedback/bug reports',
      'Small community forming',
    ],
  },
  {
    phase: 'Phase 4 (Revenue)',
    metrics: [
      'First paying customer',
      '$100/month revenue by month 6',
      '$500/month revenue by end of year 1',
    ],
  },
];

function StatusBadge({ status }: { status: 'complete' | 'in-progress' | 'planned' | 'current' | 'next' | 'future' }) {
  const styles = {
    complete: 'bg-green-500/20 text-green-400 border-green-500/50',
    'in-progress': 'bg-orange-500/20 text-orange-400 border-orange-500/50',
    planned: 'bg-zinc-700/50 text-zinc-400 border-zinc-600',
    current: 'bg-orange-500/20 text-orange-400 border-orange-500/50',
    next: 'bg-blue-500/20 text-blue-400 border-blue-500/50',
    future: 'bg-zinc-700/50 text-zinc-400 border-zinc-600',
  };

  const labels = {
    complete: 'Complete',
    'in-progress': 'In Progress',
    planned: 'Planned',
    current: 'Current',
    next: 'Next',
    future: 'Future',
  };

  return (
    <span className={`px-3 py-1 rounded-full text-xs font-medium border ${styles[status]}`}>
      {labels[status]}
    </span>
  );
}

function CheckIcon({ checked }: { checked: boolean }) {
  return (
    <span className={`inline-flex items-center justify-center w-5 h-5 rounded-full border-2 ${
      checked ? 'bg-green-500 border-green-500' : 'border-zinc-600'
    }`}>
      {checked && (
        <svg className="w-3 h-3 text-white" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={3} d="M5 13l4 4L19 7" />
        </svg>
      )}
    </span>
  );
}

export default function RoadmapPage() {
  return (
    <div className="min-h-screen bg-zinc-950 text-white">
      {/* Hero Section */}
      <section className="border-b border-zinc-800 bg-gradient-to-b from-zinc-900 to-zinc-950">
        <div className="max-w-7xl mx-auto px-6 py-16 lg:py-24">
          <h1 className="text-4xl lg:text-5xl font-bold mb-4">Development Roadmap</h1>
          <p className="text-xl text-zinc-400 max-w-3xl">
            From learning fundamentals to building a complete plugin suite and establishing a sustainable business
          </p>
        </div>
      </section>

      {/* Phases Timeline */}
      <section className="border-b border-zinc-800">
        <div className="max-w-7xl mx-auto px-6 py-16">
          <div className="space-y-12">
            {roadmapData.map((phase, index) => (
              <div key={phase.id} className="relative">
                {/* Timeline connector */}
                {index < roadmapData.length - 1 && (
                  <div className="absolute left-6 top-24 w-0.5 h-full bg-zinc-800 -z-10" />
                )}

                {/* Phase header */}
                <div className="flex items-start gap-6 mb-6">
                  <div className={`flex-shrink-0 w-12 h-12 rounded-full flex items-center justify-center border-2 font-bold text-lg ${
                    phase.status === 'current'
                      ? 'bg-orange-500/20 border-orange-500 text-orange-400'
                      : phase.status === 'next'
                      ? 'bg-blue-500/20 border-blue-500 text-blue-400'
                      : 'bg-zinc-800 border-zinc-700 text-zinc-500'
                  }`}>
                    {phase.id}
                  </div>
                  <div className="flex-1">
                    <div className="flex items-center gap-3 mb-2">
                      <h2 className="text-2xl font-bold">{phase.title}</h2>
                      <StatusBadge status={phase.status} />
                    </div>
                    <p className="text-zinc-400 text-lg"><strong className="text-zinc-300">Goal:</strong> {phase.goal}</p>
                  </div>
                </div>

                {/* Phase items */}
                <div className="ml-18 space-y-6">
                  {phase.items.map((item, itemIndex) => (
                    <div key={itemIndex} className="border border-zinc-800 rounded-lg p-6 bg-zinc-900/50">
                      <div className="flex items-start justify-between mb-4">
                        <div>
                          <h3 className="text-xl font-semibold mb-1">{item.name}</h3>
                          {item.timeline && (
                            <p className="text-sm text-zinc-500">Timeline: {item.timeline}</p>
                          )}
                        </div>
                        <StatusBadge status={item.status} />
                      </div>

                      {item.description && (
                        <p className="text-zinc-400 mb-4">{item.description}</p>
                      )}

                      {item.features && item.features.length > 0 && (
                        <ul className="space-y-2">
                          {item.features.map((feature, featureIndex) => (
                            <li key={featureIndex} className="flex items-start gap-3 text-zinc-400">
                              <CheckIcon checked={item.status === 'complete'} />
                              <span>{feature}</span>
                            </li>
                          ))}
                        </ul>
                      )}
                    </div>
                  ))}
                </div>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Technical Milestones */}
      <section className="border-b border-zinc-800">
        <div className="max-w-7xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">Technical Milestones</h2>
          <div className="grid md:grid-cols-3 gap-6">
            {milestones.map((milestone, index) => (
              <div key={index} className="border border-zinc-800 rounded-lg p-6 bg-zinc-900/50">
                <h3 className="text-xl font-semibold mb-4 text-orange-500">{milestone.category}</h3>
                <ul className="space-y-3">
                  {milestone.items.map((item, itemIndex) => (
                    <li key={itemIndex} className="flex items-start gap-3 text-sm">
                      <CheckIcon checked={item.status === 'complete'} />
                      <span className={item.status === 'complete' ? 'text-zinc-400' : 'text-zinc-500'}>
                        {item.name}
                      </span>
                    </li>
                  ))}
                </ul>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Success Metrics */}
      <section className="border-b border-zinc-800">
        <div className="max-w-7xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">Success Metrics</h2>
          <div className="grid md:grid-cols-2 gap-6">
            {successMetrics.map((metric, index) => (
              <div key={index} className="border border-zinc-800 rounded-lg p-6 bg-zinc-900/50">
                <h3 className="text-xl font-semibold mb-4 text-orange-500">{metric.phase}</h3>
                <ul className="space-y-2">
                  {metric.metrics.map((item, itemIndex) => (
                    <li key={itemIndex} className="flex items-start gap-3 text-zinc-400">
                      <span className="text-orange-500 mt-1">→</span>
                      <span>{item}</span>
                    </li>
                  ))}
                </ul>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Next Steps */}
      <section>
        <div className="max-w-7xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">Next Steps</h2>
          <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-6">
            <div className="border border-orange-500/50 rounded-lg p-6 bg-orange-500/5">
              <div className="text-sm text-orange-500 font-medium mb-2">This Week</div>
              <p className="text-white">Start PanUtil plugin</p>
            </div>
            <div className="border border-zinc-800 rounded-lg p-6 bg-zinc-900/50">
              <div className="text-sm text-zinc-500 font-medium mb-2">This Month</div>
              <p className="text-zinc-300">Complete PanUtil, start BasicSynth</p>
            </div>
            <div className="border border-zinc-800 rounded-lg p-6 bg-zinc-900/50">
              <div className="text-sm text-zinc-500 font-medium mb-2">Next 3 Months</div>
              <p className="text-zinc-300">Complete Phase 1, start Phase 2</p>
            </div>
            <div className="border border-zinc-800 rounded-lg p-6 bg-zinc-900/50">
              <div className="text-sm text-zinc-500 font-medium mb-2">6 Months</div>
              <p className="text-zinc-300">Launch website with first public downloads</p>
            </div>
          </div>

          <div className="mt-12 p-6 border border-zinc-800 rounded-lg bg-zinc-900/50">
            <h3 className="text-xl font-semibold mb-4 text-orange-500">Resources Needed</h3>
            <div className="grid md:grid-cols-2 gap-4 text-zinc-400">
              <ul className="space-y-2">
                <li className="flex items-start gap-2">
                  <span className="text-orange-500">•</span>
                  <span>Audio interface for testing</span>
                </li>
                <li className="flex items-start gap-2">
                  <span className="text-orange-500">•</span>
                  <span>Reference plugins for comparison</span>
                </li>
                <li className="flex items-start gap-2">
                  <span className="text-orange-500">•</span>
                  <span>Beta testers (producers/musicians)</span>
                </li>
              </ul>
              <ul className="space-y-2">
                <li className="flex items-start gap-2">
                  <span className="text-orange-500">•</span>
                  <span>Code signing certificates ($99/year Apple, $200-400 Windows)</span>
                </li>
                <li className="flex items-start gap-2">
                  <span className="text-orange-500">•</span>
                  <span>Web hosting for downloads ($5-10/month)</span>
                </li>
              </ul>
            </div>
          </div>
        </div>
      </section>
    </div>
  );
}
