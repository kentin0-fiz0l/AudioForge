/**
 * Plugin metadata for AudioForge
 * Extracted from documentation: README.md, plugin READMEs, and ROADMAP.md
 */

export interface PluginFeature {
  name: string;
  description: string;
}

export interface PluginParameter {
  name: string;
  range: string;
  description: string;
}

export interface Plugin {
  id: string;
  name: string;
  version: string;
  status: 'released' | 'in-development' | 'planned';
  tagline: string;
  description: string;
  features: PluginFeature[];
  parameters: PluginParameter[];
  useCases?: string[];
  learningFocus?: string[];
  algorithms?: {
    name: string;
    description: string;
    code?: string;
  }[];
  screenshots?: string[];
  downloadUrl?: string;
  docsUrl?: string;
}

export const plugins: Plugin[] = [
  {
    id: 'simplegain',
    name: 'SimpleGain',
    version: '1.0.0',
    status: 'released',
    tagline: 'Clean gain/volume control with metering',
    description: 'A basic gain/volume control plugin with smooth parameter changes and real-time level metering. Perfect for learning audio buffer processing and JUCE fundamentals.',
    features: [
      {
        name: 'Adjustable Gain',
        description: 'Control gain from -60 dB to +12 dB with smooth transitions'
      },
      {
        name: 'Real-time Level Meter',
        description: 'Visual feedback of audio levels with 30 FPS updates'
      },
      {
        name: 'Parameter Smoothing',
        description: 'No clicks or pops with 50ms ramp time for parameter changes'
      },
      {
        name: 'Clean Interface',
        description: 'Simple, intuitive rotary knob design'
      }
    ],
    parameters: [
      {
        name: 'Gain',
        range: '-60 dB to +12 dB',
        description: 'Main volume control with decibel scaling'
      }
    ],
    learningFocus: [
      'Audio buffer processing',
      'Decibel to linear conversion',
      'Parameter smoothing',
      'Level metering',
      'Sample-by-sample processing',
      'AudioProcessor basics',
      'AudioParameterFloat usage',
      'GUI with AudioProcessorEditor',
      'Timer-based UI updates',
      'Thread-safe communication (atomics)'
    ],
    screenshots: [],
    downloadUrl: '/downloads/SimpleGain-v1.0.0',
    docsUrl: '/docs/plugins/simplegain'
  },
  {
    id: 'panutil',
    name: 'PanUtil',
    version: '1.0.0',
    status: 'released',
    tagline: 'Stereo panning utility with balance and width controls',
    description: 'Professional stereo panning and width control with constant-power panning algorithm and dual metering. Features both Pan and Balance modes for different use cases.',
    features: [
      {
        name: 'Pan Control',
        description: 'Position audio anywhere in the stereo field from 100% left to 100% right'
      },
      {
        name: 'Width Control',
        description: 'Adjust stereo width from mono (0%) to ultra-wide (200%)'
      },
      {
        name: 'Dual Modes',
        description: 'Pan Mode with constant-power panning or Balance Mode for stereo sources'
      },
      {
        name: 'Visual Feedback',
        description: 'Real-time pan position and L/R level meters'
      }
    ],
    parameters: [
      {
        name: 'Pan',
        range: '-1.0 (100% L) to +1.0 (100% R)',
        description: 'Stereo pan position with center at 0'
      },
      {
        name: 'Width',
        range: '0% (mono) to 200% (ultra-wide)',
        description: 'Stereo width control using M/S processing'
      },
      {
        name: 'Mode',
        range: 'Pan Mode / Balance Mode',
        description: 'Pan Mode for mono sources, Balance Mode for stereo sources'
      }
    ],
    useCases: [
      'Fix off-center recordings - Center a vocal that was recorded slightly to one side',
      'Create space in mix - Pan instruments to different positions',
      'Stereo widening - Make synths/pads sound bigger with width >100%',
      'Mono compatibility - Check mix at 0% width',
      'Creative effects - Extreme panning/width for special effects'
    ],
    learningFocus: [
      'Stereo processing (L/R channels)',
      'Constant-power panning algorithm',
      'M/S (Mid/Side) processing for width control',
      'Dual-channel metering',
      'Pan vs Balance behavior',
      'Multi-channel buffer processing',
      'ComboBox parameter (mode selector)',
      'Advanced GUI with visualization',
      'Multiple sliders and controls'
    ],
    algorithms: [
      {
        name: 'Constant-Power Panning',
        description: 'Ensures perceived loudness stays constant as you pan across the stereo field',
        code: `// Convert pan (-1 to +1) to angle (0 to π/2)
float angle = (pan + 1.0) * 0.25 * π;
leftGain = cos(angle);
rightGain = sin(angle);`
      },
      {
        name: 'Width Control (M/S Processing)',
        description: 'Mid/Side processing for stereo width adjustment',
        code: `// Convert L/R to M/S
mid = (L + R) * 0.5;    // Mono sum
side = (L - R) * 0.5;   // Stereo difference

// Scale side by width
side *= width;  // 0.0 = mono, 1.0 = normal, 2.0 = wide

// Convert back to L/R
L = mid + side;
R = mid - side;`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/PanUtil-v1.0.0',
    docsUrl: '/docs/plugins/panutil'
  },
  {
    id: 'basicsynth',
    name: 'BasicSynth',
    version: '0.1.0',
    status: 'released',
    tagline: 'Simple subtractive synthesizer for learning synthesis fundamentals',
    description: 'A polyphonic subtractive synthesizer with 3 oscillator waveforms, ADSR envelope, and resonant low-pass filter. Perfect for learning MIDI processing, voice management, and sound synthesis.',
    features: [
      {
        name: '3 Oscillator Waveforms',
        description: 'Sine, Sawtooth, and Square waveforms for different timbres'
      },
      {
        name: 'ADSR Envelope Generator',
        description: 'Shape amplitude over time with Attack, Decay, Sustain, and Release controls'
      },
      {
        name: 'Resonant Low-Pass Filter',
        description: 'Biquad filter with cutoff (20Hz-20kHz) and resonance (Q: 0.5-10.0) for tone shaping'
      },
      {
        name: '8-Voice Polyphony',
        description: 'Play chords with round-robin voice stealing when exceeding 8 voices'
      },
      {
        name: 'Per-Voice Processing',
        description: 'Each voice has independent oscillator, envelope, and filter state'
      },
      {
        name: 'Real-time Output Metering',
        description: 'Visual feedback of audio levels with gradient display'
      }
    ],
    parameters: [
      {
        name: 'Waveform',
        range: 'Sine / Sawtooth / Square',
        description: 'Oscillator waveform selection'
      },
      {
        name: 'Volume',
        range: '0.0 to 1.0',
        description: 'Master output volume'
      },
      {
        name: 'Attack',
        range: '1ms to 2000ms',
        description: 'Envelope attack time (log scale)'
      },
      {
        name: 'Decay',
        range: '1ms to 2000ms',
        description: 'Envelope decay time (log scale)'
      },
      {
        name: 'Sustain',
        range: '0% to 100%',
        description: 'Envelope sustain level'
      },
      {
        name: 'Release',
        range: '1ms to 5000ms',
        description: 'Envelope release time (log scale)'
      },
      {
        name: 'Filter Cutoff',
        range: '20Hz to 20kHz',
        description: 'Low-pass filter cutoff frequency (log scale, 1kHz midpoint)'
      },
      {
        name: 'Filter Resonance',
        range: '0.5 to 10.0',
        description: 'Filter resonance (Q factor). 0.707 = Butterworth, higher = resonant peak'
      }
    ],
    useCases: [
      'Pad sounds - Slow attack, long release, low cutoff with mild resonance',
      'Bass sounds - Fast attack, short release, low cutoff with high resonance',
      'Pluck sounds - Instant attack, no sustain, medium cutoff',
      'Lead sounds - Fast attack, high sustain, sweepable filter with resonance',
      'Learning synthesis - Experiment with oscillators, envelopes, and filters'
    ],
    learningFocus: [
      'MIDI note processing (note on/off messages)',
      'Polyphonic voice management and voice stealing',
      'Oscillator waveform generation (sine, saw, square)',
      'ADSR envelope implementation (linear attack/decay/release)',
      'Biquad filter design (low-pass with resonance)',
      'Per-voice state management',
      'Optimization techniques (cached filter coefficients)',
      'Professional UI design with color-coded controls'
    ],
    algorithms: [
      {
        name: 'ADSR Envelope',
        description: 'Linear attack and release, linear decay from 1.0 to sustain level',
        code: `// Attack phase
envelopeLevel = min(1.0, envelopeTime / attack);

// Decay phase
decayAmount = (1.0 - sustain) * (envelopeTime / decay);
envelopeLevel = 1.0 - min(decayAmount, 1.0 - sustain);

// Sustain phase
envelopeLevel = sustain;

// Release phase
releaseProgress = envelopeTime / release;
envelopeLevel = max(0.0, releaseStartLevel * (1.0 - releaseProgress));`
      },
      {
        name: 'Voice Stealing',
        description: 'Round-robin voice allocation when all 8 voices are active',
        code: `// Find free voice or steal oldest
voice = findFreeVoice();
if (voice == null) {
    voice = voices[nextVoiceIndex];
    nextVoiceIndex = (nextVoiceIndex + 1) % 8;
}
voice.noteOn(midiNote, velocity, sampleRate);`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/BasicSynth-v0.1.0',
    docsUrl: '/docs/plugins/basicsynth'
  },
  {
    id: 'cleandelay',
    name: 'CleanDelay',
    version: '0.1.0',
    status: 'released',
    tagline: 'Professional stereo delay with ping-pong mode',
    description: 'A versatile stereo delay effect for creating rhythmic echoes, ambient textures, and stereo width enhancement. Features clean circular buffer implementation with feedback control and ping-pong mode for alternating L/R delays.',
    features: [
      {
        name: 'Adjustable Delay Time',
        description: '1ms to 2000ms with logarithmic scaling for musical control'
      },
      {
        name: 'Feedback Control',
        description: '0% to 99% feedback for controlled echo repetitions'
      },
      {
        name: 'Wet/Dry Mix',
        description: 'Seamless blending from 100% dry to 100% wet'
      },
      {
        name: 'Ping-Pong Mode',
        description: 'Alternating left/right delays for wide stereo imaging'
      },
      {
        name: 'Real-time Metering',
        description: 'Input and output level monitoring'
      },
      {
        name: 'Clean Signal Path',
        description: 'High-quality circular buffer implementation'
      }
    ],
    parameters: [
      {
        name: 'Delay Time',
        range: '1ms to 2000ms',
        description: 'Time between echoes (log scale for musical control)'
      },
      {
        name: 'Feedback',
        range: '0% to 99%',
        description: 'Amount of delayed signal fed back into the delay line'
      },
      {
        name: 'Mix (Wet)',
        range: '0% to 100%',
        description: 'Balance between dry (original) and wet (delayed) signal'
      },
      {
        name: 'Ping-Pong',
        range: 'Off / On',
        description: 'Enable alternating left/right delays for stereo effect'
      }
    ],
    useCases: [
      'Vocal slapback - Quick doubling effect (80-120ms, low feedback)',
      'Rhythmic eighth note delay - Syncopated echoes (250ms @ 120 BPM, ping-pong on)',
      'Ambient wash - Lush delay clouds (500-800ms, high feedback, ping-pong on)',
      'Haas effect - Stereo width enhancement (10-30ms, no feedback)',
      'Dub-style echo throw - Classic dub delays (375ms dotted eighth, automate mix)',
      'Guitar lead enhancement - Fill out single-note lines (375-500ms, medium feedback)'
    ],
    learningFocus: [
      'Circular buffer implementation for delay lines',
      'Cross-channel feedback for ping-pong stereo effect',
      'Wet/dry signal mixing',
      'Buffer index wrapping and modulo arithmetic',
      'Multi-channel processing with shared write position',
      'Real-time parameter control (delay time, feedback, mix)',
      'State persistence (save/restore delay parameters)',
      'Professional UI with color-coded controls'
    ],
    algorithms: [
      {
        name: 'Circular Buffer Delay',
        description: 'Efficient delay line using modulo arithmetic for buffer wrapping',
        code: `// Write input to delay buffer
delayBuffer[writePosition] = input + (feedback * delayedSample);

// Read from delay buffer (delayed by delaySamples)
readPosition = (writePosition - delaySamples + MAX_DELAY_SAMPLES) % MAX_DELAY_SAMPLES;
delayedSample = delayBuffer[readPosition];

// Advance write position
writePosition = (writePosition + 1) % MAX_DELAY_SAMPLES;`
      },
      {
        name: 'Ping-Pong Stereo Delay',
        description: 'Cross-channel feedback for alternating L/R echoes',
        code: `// Left channel gets feedback from right channel
leftFeedback = pingPong ? rightDelayBuffer[readPos] : leftDelayBuffer[readPos];
leftDelayBuffer[writePos] = leftInput + (leftFeedback * feedback);

// Right channel gets feedback from left channel
rightFeedback = pingPong ? leftDelayBuffer[readPos] : rightDelayBuffer[readPos];
rightDelayBuffer[writePos] = rightInput + (rightFeedback * feedback);`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/CleanDelay-v0.1.0',
    docsUrl: '/docs/plugins/cleandelay'
  },
  {
    id: 'simpleeq',
    name: 'SimpleEQ',
    version: '0.1.0',
    status: 'in-development',
    tagline: '3-band parametric EQ',
    description: 'Beginner-friendly parametric EQ with visual frequency response curve and spectrum analyzer. Essential for tone shaping and frequency balance.',
    features: [
      {
        name: '3-band Parametric EQ',
        description: 'Low, mid, and high frequency bands with adjustable Q'
      },
      {
        name: 'High-pass and Low-pass Filters',
        description: 'Remove unwanted low or high frequencies'
      },
      {
        name: 'Visual Frequency Response',
        description: 'See the EQ curve in real-time'
      },
      {
        name: 'Spectrum Analyzer',
        description: 'View the frequency content of your audio'
      }
    ],
    parameters: [
      {
        name: 'Low Freq',
        range: '20Hz to 500Hz',
        description: 'Low band center frequency'
      },
      {
        name: 'Low Gain',
        range: '-12dB to +12dB',
        description: 'Low band gain'
      },
      {
        name: 'Mid Freq',
        range: '200Hz to 5kHz',
        description: 'Mid band center frequency'
      },
      {
        name: 'Mid Gain',
        range: '-12dB to +12dB',
        description: 'Mid band gain'
      },
      {
        name: 'High Freq',
        range: '2kHz to 20kHz',
        description: 'High band center frequency'
      },
      {
        name: 'High Gain',
        range: '-12dB to +12dB',
        description: 'High band gain'
      }
    ],
    screenshots: [],
    docsUrl: '/docs/plugins/simpleeq'
  },
  {
    id: 'simplecomp',
    name: 'SimpleComp',
    version: '0.1.0',
    status: 'in-development',
    tagline: 'Basic compressor',
    description: 'Straightforward dynamic range compressor with automatic makeup gain and visual gain reduction metering.',
    features: [
      {
        name: 'Threshold, Ratio, Attack, Release',
        description: 'All essential compressor controls'
      },
      {
        name: 'Automatic Makeup Gain',
        description: 'Compensate for volume reduction automatically'
      },
      {
        name: 'Gain Reduction Meter',
        description: 'See how much compression is being applied'
      },
      {
        name: 'Soft Knee Option',
        description: 'Gentler compression transition'
      }
    ],
    parameters: [
      {
        name: 'Threshold',
        range: '-60dB to 0dB',
        description: 'Level at which compression begins'
      },
      {
        name: 'Ratio',
        range: '1:1 to 20:1',
        description: 'Amount of compression applied'
      },
      {
        name: 'Attack',
        range: '0.1ms to 100ms',
        description: 'How quickly compression responds'
      },
      {
        name: 'Release',
        range: '10ms to 1000ms',
        description: 'How quickly compression releases'
      },
      {
        name: 'Knee',
        range: 'Hard / Soft',
        description: 'Compression curve shape'
      }
    ],
    screenshots: [],
    docsUrl: '/docs/plugins/simplecomp'
  }
];

/**
 * Roadmap phases with timeline and goals
 */
export interface RoadmapPhase {
  id: string;
  name: string;
  status: 'complete' | 'current' | 'planned';
  goal: string;
  timeline?: string;
  items: {
    name: string;
    status: 'complete' | 'in-progress' | 'planned';
    features?: string[];
  }[];
  successMetrics?: string[];
}

export const roadmap: RoadmapPhase[] = [
  {
    id: 'phase-1',
    name: 'Phase 1: Infrastructure Foundation',
    status: 'complete',
    goal: 'Build 3 simple plugins to learn JUCE and DSP fundamentals',
    items: [
      {
        name: 'Project setup with JUCE',
        status: 'complete'
      },
      {
        name: 'SimpleGain plugin',
        status: 'complete',
        features: ['Gain control', 'Level meter', 'Parameter smoothing', 'Clean UI']
      },
      {
        name: 'PanUtil plugin',
        status: 'complete',
        features: ['Pan control', 'Width control', 'Dual modes', 'Visual feedback']
      },
      {
        name: 'Shared DSP library',
        status: 'complete',
        features: ['5 reusable components']
      },
      {
        name: 'Testing framework',
        status: 'complete',
        features: ['JUCE UnitTestRunner', '54 test suites', '100% pass rate']
      },
      {
        name: 'CI/CD pipeline',
        status: 'complete',
        features: ['GitHub Actions', 'Multi-platform builds']
      },
      {
        name: 'Documentation',
        status: 'complete'
      }
    ],
    successMetrics: [
      '3 working plugins',
      'Understanding of core DSP concepts',
      'Comfortable with JUCE framework'
    ]
  },
  {
    id: 'phase-2',
    name: 'Phase 2: Core Plugin Suite',
    status: 'current',
    goal: 'Build useful, professional-quality effects',
    timeline: '2-3 months',
    items: [
      {
        name: 'BasicSynth',
        status: 'complete',
        features: [
          '3 oscillator waveforms (sine, sawtooth, square)',
          'ADSR envelope generator',
          'Resonant low-pass filter (cutoff + Q)',
          'MIDI note handling with velocity',
          '8-voice polyphony with round-robin voice stealing',
          'Per-voice filtering and envelope',
          'Real-time output metering'
        ]
      },
      {
        name: 'CleanDelay',
        status: 'complete',
        features: [
          'Delay time (1ms - 2 seconds) with log scaling',
          'Feedback control (0-99%)',
          'Wet/dry mix (0-100%)',
          'Stereo ping-pong mode (alternating L/R)',
          'Input/output level metering',
          'Circular buffer implementation (192k samples)'
        ]
      },
      {
        name: 'SimpleEQ',
        status: 'planned',
        features: [
          '3-band parametric EQ',
          'High-pass and low-pass filters',
          'Visual frequency response curve',
          'Spectrum analyzer'
        ]
      },
      {
        name: 'SimpleComp',
        status: 'planned',
        features: [
          'Threshold, ratio, attack, release',
          'Automatic makeup gain',
          'Gain reduction meter',
          'Soft knee option'
        ]
      }
    ],
    successMetrics: [
      'Plugins that sound professional',
      'No CPU spikes or crashes',
      'Positive feedback from beta testers'
    ]
  },
  {
    id: 'phase-3',
    name: 'Phase 3: Distribution & Website',
    status: 'planned',
    goal: 'Get plugins into users\' hands, build community',
    timeline: '6 months',
    items: [
      {
        name: 'Website with downloads',
        status: 'in-progress',
        features: ['Simple static site']
      },
      {
        name: 'User documentation',
        status: 'planned',
        features: ['User documentation for each plugin']
      },
      {
        name: 'Video tutorials',
        status: 'planned',
        features: ['YouTube tutorials']
      },
      {
        name: 'Code-signing certificates',
        status: 'planned',
        features: ['macOS + Windows signing']
      },
      {
        name: 'Installers',
        status: 'planned',
        features: ['macOS .pkg', 'Windows .exe']
      },
      {
        name: 'AU support',
        status: 'planned',
        features: ['Audio Units for macOS']
      },
      {
        name: 'Linux builds',
        status: 'planned'
      },
      {
        name: 'Beta testing program',
        status: 'planned'
      }
    ],
    successMetrics: [
      '100+ downloads in first month',
      'Active user feedback/bug reports',
      'Small community forming'
    ]
  },
  {
    id: 'phase-4',
    name: 'Phase 4: Advanced Plugins',
    status: 'planned',
    goal: 'Build unique, creative tools',
    items: [
      {
        name: 'Granular synthesis plugin',
        status: 'planned'
      },
      {
        name: 'Convolution reverb',
        status: 'planned'
      },
      {
        name: 'Spectral processing tools',
        status: 'planned'
      },
      {
        name: 'Creative modulation effects',
        status: 'planned'
      },
      {
        name: 'Wavetable synthesizer',
        status: 'planned'
      }
    ]
  },
  {
    id: 'phase-5',
    name: 'Phase 5: Monetization',
    status: 'planned',
    goal: 'Generate revenue while keeping core plugins free',
    items: [
      {
        name: 'Free Tier',
        status: 'planned',
        features: [
          'SimpleGain, PanUtil, BasicSynth stay free forever',
          'CleanDelay, SimpleEQ, SimpleComp free'
        ]
      },
      {
        name: 'Premium Tier ($29-49)',
        status: 'planned',
        features: [
          'Advanced synths and effects',
          'Expansion packs (presets, wavetables)',
          'Early access to new plugins'
        ]
      }
    ],
    successMetrics: [
      'First paying customer',
      '$100/month revenue by month 6',
      '$500/month revenue by end of year 1'
    ]
  }
];

/**
 * Get plugins by status
 */
export function getPluginsByStatus(status: Plugin['status']): Plugin[] {
  return plugins.filter(p => p.status === status);
}

/**
 * Get plugin by ID
 */
export function getPluginById(id: string): Plugin | undefined {
  return plugins.find(p => p.id === id);
}

/**
 * Get current roadmap phase
 */
export function getCurrentPhase(): RoadmapPhase | undefined {
  return roadmap.find(p => p.status === 'current');
}

/**
 * Project information
 */
export const projectInfo = {
  name: 'AudioForge',
  tagline: 'Professional-quality DAW plugins, free and open source',
  description: 'AudioForge is a collection of high-quality audio plugins for music production. Starting with essential utilities and effects, we\'re building a suite of tools that producers actually want to use.',
  techStack: {
    framework: 'JUCE 7.x',
    language: 'C++17',
    formats: ['VST3', 'AU (coming)', 'AAX (coming)'],
    platforms: ['macOS', 'Windows', 'Linux']
  },
  github: 'https://github.com/yourusername/AudioForge',
  license: 'MIT',
  testCoverage: '100%',
  totalTests: 54,
  totalPlugins: plugins.length,
  releasedPlugins: getPluginsByStatus('released').length
};
