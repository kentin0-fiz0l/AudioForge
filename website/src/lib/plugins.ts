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
    status: 'released',
    tagline: '3-band parametric EQ with HPF/LPF',
    description: 'Professional 3-band parametric equalizer with optional high-pass and low-pass filters. Features shelf and peaking filters for precise tone shaping across the frequency spectrum.',
    features: [
      {
        name: 'Low Shelf (20Hz-500Hz)',
        description: 'Boost or cut bass frequencies with smooth shelving curve (±12dB, Q=0.707)'
      },
      {
        name: 'Parametric Mid (200Hz-5kHz)',
        description: 'Precise mid-range control with peaking filter (±12dB, Q=1.0)'
      },
      {
        name: 'High Shelf (2kHz-20kHz)',
        description: 'Shape high frequencies with smooth shelving curve (±12dB, Q=0.707)'
      },
      {
        name: 'Optional High-Pass Filter',
        description: 'Remove unwanted low frequencies (20Hz-500Hz, 12dB/octave)'
      },
      {
        name: 'Optional Low-Pass Filter',
        description: 'Tame excessive high frequencies (2kHz-20kHz, 12dB/octave)'
      },
      {
        name: 'Real-time Metering',
        description: 'Input and output level monitoring with gradient display'
      }
    ],
    parameters: [
      {
        name: 'Low Freq',
        range: '20Hz to 500Hz',
        description: 'Low shelf transition frequency (log scale)'
      },
      {
        name: 'Low Gain',
        range: '-12dB to +12dB',
        description: 'Low shelf gain amount'
      },
      {
        name: 'Mid Freq',
        range: '200Hz to 5kHz',
        description: 'Mid band center frequency (log scale)'
      },
      {
        name: 'Mid Gain',
        range: '-12dB to +12dB',
        description: 'Mid band gain amount'
      },
      {
        name: 'High Freq',
        range: '2kHz to 20kHz',
        description: 'High shelf transition frequency (log scale)'
      },
      {
        name: 'High Gain',
        range: '-12dB to +12dB',
        description: 'High shelf gain amount'
      },
      {
        name: 'HPF Enabled',
        range: 'Off / On',
        description: 'Enable/disable high-pass filter'
      },
      {
        name: 'HPF Freq',
        range: '20Hz to 500Hz',
        description: 'High-pass filter cutoff frequency'
      },
      {
        name: 'LPF Enabled',
        range: 'Off / On',
        description: 'Enable/disable low-pass filter'
      },
      {
        name: 'LPF Freq',
        range: '2kHz to 20kHz',
        description: 'Low-pass filter cutoff frequency'
      }
    ],
    useCases: [
      'Remove rumble - Enable HPF at 40-80Hz to clean up low-end mud',
      'Tame harsh vocals - Cut 2-4kHz by 3-6dB to reduce sibilance',
      'Add air - Boost 8-12kHz shelf by 2-4dB for sparkle',
      'Telephone effect - HPF at 300Hz + LPF at 3kHz for lo-fi sound',
      'Bass enhancement - Boost 80-120Hz shelf by 3-6dB for weight',
      'De-essing - Enable LPF at 8kHz to reduce harsh highs'
    ],
    learningFocus: [
      'Biquad filter design (shelf, peaking, HPF, LPF)',
      'Filter coefficient caching for performance',
      'Cascaded filter processing (5 filters in series)',
      'Shelf vs peaking filter behavior',
      'Per-channel filter state management',
      'Professional EQ UI design with color coding',
      'Parameter smoothing for click-free adjustments',
      'State persistence across sessions'
    ],
    algorithms: [
      {
        name: 'Low Shelf Filter',
        description: 'Smooth bass boost/cut with Butterworth response (Q=0.707)',
        code: `// Calculate shelf coefficients
A = pow(10.0, gainDb / 40.0);  // Convert dB to linear
w0 = 2π * frequency / sampleRate;
alpha = sin(w0) / (2.0 * Q);
beta = sqrt(A) / Q;

// Biquad coefficients
a0 = (A+1) + (A-1)*cos(w0) + beta*sin(w0);
b0 = A*((A+1) - (A-1)*cos(w0) + beta*sin(w0)) / a0;
b1 = 2*A*((A-1) - (A+1)*cos(w0)) / a0;
b2 = A*((A+1) - (A-1)*cos(w0) - beta*sin(w0)) / a0;`
      },
      {
        name: 'Peaking EQ',
        description: 'Bell curve for precise mid-range control (Q=1.0)',
        code: `// Peaking filter for mid band
A = pow(10.0, gainDb / 40.0);
w0 = 2π * frequency / sampleRate;
alpha = sin(w0) / (2.0 * Q);

a0 = 1.0 + alpha/A;
b0 = (1.0 + alpha*A) / a0;
b1 = (-2.0 * cos(w0)) / a0;
b2 = (1.0 - alpha*A) / a0;`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/SimpleEQ-v0.1.0',
    docsUrl: '/docs/plugins/simpleeq'
  },
  {
    id: 'simplecomp',
    name: 'SimpleComp',
    version: '1.0.0',
    status: 'released',
    tagline: 'Dynamic range compressor with automatic makeup gain',
    description: 'Professional-quality dynamic range compressor with envelope-based gain reduction, hard/soft knee modes, and real-time gain reduction metering. Features automatic makeup gain to maintain consistent output levels.',
    features: [
      {
        name: 'Envelope Follower Compression',
        description: 'Smooth gain reduction with attack and release envelope following'
      },
      {
        name: 'Hard and Soft Knee Modes',
        description: 'Choose between immediate compression (hard knee) or gradual transition (6dB soft knee)'
      },
      {
        name: 'Automatic Makeup Gain',
        description: 'Intelligent compensation (50%) based on threshold and ratio settings'
      },
      {
        name: 'Real-time Gain Reduction Meter',
        description: 'Visual feedback with gradient display (0 to -20dB range, thread-safe)'
      },
      {
        name: 'Professional UI',
        description: 'Vertical sliders for threshold/ratio, rotary knobs for attack/release'
      },
      {
        name: 'Flexible Timing Controls',
        description: 'Attack (0.1-100ms) and Release (10-1000ms) with log scaling for fine control'
      }
    ],
    parameters: [
      {
        name: 'Threshold',
        range: '-60dB to 0dB',
        description: 'Level at which compression begins (default: -20dB)'
      },
      {
        name: 'Ratio',
        range: '1:1 to 20:1',
        description: 'Amount of compression applied (default: 4:1, slight curve for low ratios)'
      },
      {
        name: 'Attack',
        range: '0.1ms to 100ms',
        description: 'How quickly compression responds to level increases (log scale, default: 10ms)'
      },
      {
        name: 'Release',
        range: '10ms to 1000ms',
        description: 'How quickly compression releases when level drops (log scale, default: 100ms)'
      },
      {
        name: 'Knee',
        range: 'Hard / Soft',
        description: 'Compression curve shape (hard = immediate, soft = 6dB gradual transition)'
      }
    ],
    useCases: [
      'Taming vocal dynamics and evening out performances',
      'Controlling drum transients and peak levels',
      'Adding punch and sustain to bass and guitars',
      'Mastering bus compression for glue and cohesion',
      'Limiting peaks before broadcast or streaming',
      'Learning fundamental compression concepts and envelope following'
    ],
    learningFocus: [
      'Envelope follower design with attack/release coefficients',
      'Gain reduction calculation (threshold, ratio, knee)',
      'Hard vs. soft knee compression curves',
      'Automatic makeup gain algorithms',
      'Thread-safe metering with std::atomic',
      'Exponential time constants for envelope smoothing',
      'Per-channel state management for stereo processing',
      'Parameter smoothing with log-scaled controls'
    ],
    algorithms: [
      {
        name: 'Envelope Follower',
        description: 'Tracks input level with exponential attack/release coefficients calculated from time parameters',
        code: 'float attackCoeff = std::exp(-1.0f / (attackMs * 0.001f * sampleRate));\nfloat releaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * sampleRate));\n\nfloat coeff = (inputLevelDB > linearToDb(envelope)) ? attackCoeff : releaseCoeff;\nenvelope = coeff * envelope + (1.0f - coeff) * inputLevel;'
      },
      {
        name: 'Soft Knee Gain Reduction',
        description: 'Gradual compression transition using quadratic curve in 6dB knee region',
        code: 'float kneeStart = threshold - knee / 2.0f;\nfloat kneeEnd = threshold + knee / 2.0f;\n\nif (inputLevelDB >= kneeEnd) {\n  // Above knee: full compression\n  float overDB = inputLevelDB - threshold;\n  return overDB - (overDB / ratio);\n} else {\n  // In knee region: quadratic curve\n  float kneeInput = inputLevelDB - kneeStart;\n  float kneeRatio = kneeInput / knee;\n  float overDB = inputLevelDB - threshold;\n  float gainReduction = overDB - (overDB / ratio);\n  return gainReduction * kneeRatio * kneeRatio;\n}'
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/SimpleComp-v1.0.0',
    docsUrl: '/docs/plugins/simplecomp'
  },
  {
    id: 'waveshaper',
    name: 'WaveShaper',
    version: '1.0.0',
    status: 'released',
    tagline: 'Waveshaping distortion with 5 curve modes and tone control',
    description: 'Professional waveshaping distortion plugin with multiple saturation curves, drive control, and integrated tone filtering. Features real-time curve visualization to see how each mode shapes your audio.',
    features: [
      {
        name: '5 Waveshaping Modes',
        description: 'Soft Clip, Hard Clip, Tanh, Cubic, and Asymmetric curves for different distortion characters'
      },
      {
        name: 'Drive Control (0-100%)',
        description: 'Variable drive amount up to 11x gain multiplier for intense saturation'
      },
      {
        name: 'Real-time Curve Visualization',
        description: 'See the waveshaping transfer function update live as you change modes'
      },
      {
        name: 'Mix Control (Dry/Wet)',
        description: 'Blend original and distorted signals for parallel processing (0-100%)'
      },
      {
        name: 'Tone Filter',
        description: 'Integrated low-pass filter (500Hz-20kHz) to tame harsh highs'
      },
      {
        name: 'Output Gain',
        description: 'Level compensation from -12dB to +12dB for gain staging'
      },
      {
        name: 'DC Blocker',
        description: 'Automatic DC offset removal at 20Hz for clean output'
      },
      {
        name: 'Input/Output Metering',
        description: 'Real-time level monitoring with 30 FPS updates'
      }
    ],
    parameters: [
      {
        name: 'Drive',
        range: '0% to 100%',
        description: 'Amount of drive/distortion (0-10x multiplier)'
      },
      {
        name: 'Mix',
        range: '0% to 100%',
        description: 'Dry/wet blend (0% = dry, 100% = wet)'
      },
      {
        name: 'Tone',
        range: '0% to 100%',
        description: 'Low-pass filter cutoff (500Hz to 20kHz)'
      },
      {
        name: 'Shape',
        range: '5 modes',
        description: 'Waveshaping curve type (Soft Clip, Hard Clip, Tanh, Cubic, Asymmetric)'
      },
      {
        name: 'Output',
        range: '-12dB to +12dB',
        description: 'Output level compensation'
      }
    ],
    useCases: [
      'Gentle saturation - Soft Clip mode with low drive (10-30%) for warmth',
      'Hard digital clipping - Hard Clip mode with high drive for aggressive distortion',
      'Analog-style warmth - Tanh mode with medium drive (40-60%) for tube-like saturation',
      'Harmonic enhancement - Cubic mode for adding odd harmonics',
      'Asymmetric tube emulation - Asymmetric mode for vintage warmth with even harmonics',
      'Parallel distortion - Mix control at 50% for New York-style parallel processing',
      'Taming harsh distortion - Use Tone control to roll off high frequencies after saturation'
    ],
    learningFocus: [
      'Waveshaping algorithms (polynomial, tanh, clipping)',
      'Transfer function visualization',
      'Per-sample nonlinear processing',
      'Harmonic distortion (odd vs even harmonics)',
      'Soft vs hard clipping behavior',
      'DC offset and blocking filters',
      'Parallel processing techniques',
      'Post-distortion tone shaping'
    ],
    algorithms: [
      {
        name: 'Soft Clip',
        description: 'Smooth saturation using polynomial approximation that approaches ±1 asymptotically',
        code: `// Soft clipping formula
if (|input| < 1.0)
    return input;  // Linear pass-through
else
    sign = input > 0 ? 1.0 : -1.0;
    saturated = 1.0 - 1.0 / (|input| + 1.0);
    return sign * saturated;`
      },
      {
        name: 'Hard Clip',
        description: 'Digital clipping at ±1 threshold for aggressive distortion',
        code: `// Hard clipping
return clamp(input, -1.0, 1.0);`
      },
      {
        name: 'Tanh Saturation',
        description: 'Classic hyperbolic tangent curve for smooth saturation',
        code: `// Tanh saturation
return tanh(input);  // Approaches ±1 asymptotically`
      },
      {
        name: 'Cubic Waveshaping',
        description: 'Polynomial distortion (x - x³/3) that adds odd harmonics',
        code: `// Cubic waveshaping
output = input - (input³ / 3.0);
return clamp(output, -1.0, 1.0);`
      },
      {
        name: 'Asymmetric Waveshaping',
        description: 'Different curves for positive and negative signals, adding even harmonics for warmth',
        code: `// Asymmetric waveshaping
if (input > 0.0)
    return tanh(input * 1.5) * 0.85;  // Harder positive
else
    return tanh(input * 0.8) * 0.95;  // Softer negative`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/WaveShaper-v1.0.0',
    docsUrl: '/docs/plugins/waveshaper'
  },
  {
    id: 'saturation',
    name: 'Saturation',
    version: '1.0.0',
    status: 'released',
    tagline: 'Multi-mode analog saturation with harmonic enhancement',
    description: 'Professional analog saturation plugin with 5 distinct saturation modes, each modeling different analog hardware characteristics. Features real-time harmonic analysis, oversampling for aliasing-free distortion, and comprehensive tone shaping controls.',
    features: [
      {
        name: '5 Saturation Modes',
        description: 'Tape (soft, even harmonics), Tube (asymmetric warmth), Transistor (hard clipping), Transformer (gentle H2), and Diode (asymmetric hard clipping)'
      },
      {
        name: 'Real-time Harmonic Analyzer',
        description: 'Visual feedback showing H1-H8 harmonic content with color-coded bars (orange=even/warm, cyan=odd/bright)'
      },
      {
        name: 'Oversampling (Off/2x/4x)',
        description: 'High-quality oversampling using polyphase IIR filters to eliminate aliasing artifacts'
      },
      {
        name: 'Drive Control (0-100%)',
        description: 'Variable drive amount up to 10x gain multiplier for intense saturation'
      },
      {
        name: 'Dual Tone Controls',
        description: 'Independent low and high shelf filters (±12dB) at 200Hz and 5kHz for pre/post tone shaping'
      },
      {
        name: 'Mix Control (Dry/Wet)',
        description: 'Blend original and saturated signals for parallel processing (0-100%)'
      },
      {
        name: 'DC Blocker',
        description: 'Automatic DC offset removal at 20Hz for clean output'
      },
      {
        name: 'Input/Output Metering',
        description: 'Real-time level monitoring with 30 FPS updates'
      }
    ],
    parameters: [
      {
        name: 'Drive',
        range: '0% to 100%',
        description: 'Amount of drive/saturation (maps to 1-10x gain multiplier)'
      },
      {
        name: 'Mode',
        range: '5 modes',
        description: 'Saturation algorithm type (Tape, Tube, Transistor, Transformer, Diode)'
      },
      {
        name: 'Tone Low',
        range: '-12dB to +12dB',
        description: 'Low shelf filter at 200Hz (Q=0.707)'
      },
      {
        name: 'Tone High',
        range: '-12dB to +12dB',
        description: 'High shelf filter at 5kHz (Q=0.707)'
      },
      {
        name: 'Mix',
        range: '0% to 100%',
        description: 'Dry/wet blend (0% = dry, 100% = wet)'
      },
      {
        name: 'Oversampling',
        range: 'Off / 2x / 4x',
        description: 'Oversampling factor for aliasing reduction'
      }
    ],
    useCases: [
      'Add warmth to digital recordings - Tape mode with low drive (10-30%) and even harmonics',
      'Vintage tube warmth - Tube mode with medium drive (40-60%) for asymmetric saturation',
      'Aggressive distortion - Transistor mode with high drive for hard clipping and odd harmonics',
      'Subtle glue and cohesion - Transformer mode with low drive for gentle H2 enrichment',
      'Creative effects - Diode mode for strong asymmetric saturation and unique harmonics',
      'Parallel saturation - Mix at 30-50% for New York-style parallel processing',
      'Aliasing-free overdriven tones - Use 2x or 4x oversampling for clean high-gain saturation'
    ],
    learningFocus: [
      'Analog saturation modeling (tape, tube, transistor, transformer, diode)',
      'Harmonic distortion (even vs odd harmonics)',
      'Oversampling techniques for aliasing reduction',
      'Polyphase IIR filter design',
      'Real-time FFT analysis for harmonic content',
      'Shelf filter design for tone shaping',
      'DC blocking filters',
      'Custom UI components (harmonic analyzer visualization)',
      'Per-sample nonlinear processing'
    ],
    algorithms: [
      {
        name: 'Tape Saturation',
        description: 'Soft compression with even harmonics (H2, H4, H6) for warmth',
        code: `// Tape saturation formula
const float x = input * 0.7f;
if (abs(x) < 0.1f) return input;

const float absX = abs(x);
const float sign = (x > 0.0f) ? 1.0f : -1.0f;
const float compressed = absX / (1.0f + absX * 0.6f);
return sign * compressed * 1.4f;`
      },
      {
        name: 'Tube Saturation',
        description: 'Asymmetric waveshaping for warm tube-like saturation (H2, H3, H4)',
        code: `// Tube saturation (asymmetric)
if (input > 0.0f)
    return tanh(input * 1.2f) * 0.9f;  // Harder positive
else
    return tanh(input * 0.8f) * 1.05f; // Softer negative`
      },
      {
        name: 'Transistor Saturation',
        description: 'Hard clipping with odd harmonics (H3, H5, H7) for aggressive tone',
        code: `// Transistor saturation
if (abs(input) < 0.7f)
    return input;  // Linear region

const float sign = (input > 0.0f) ? 1.0f : -1.0f;
const float absInput = abs(input);
const float hardClip = 0.7f + (absInput - 0.7f) * 0.3f;
return sign * min(hardClip, 1.2f);`
      },
      {
        name: 'Transformer Saturation',
        description: 'Gentle compression with H2 dominant harmonic for subtle warmth',
        code: `// Transformer saturation
const float asymmetry = 0.15f;
const float asymmetricInput = input + input * input * asymmetry;
return tanh(asymmetricInput * 0.6f) * 1.2f;`
      },
      {
        name: 'Diode Saturation',
        description: 'Asymmetric hard clipping for strong H2, H3, H5 harmonics',
        code: `// Diode saturation (very asymmetric)
if (input > 0.0f)
    return min(input, 0.7f);  // Hard clip positive at 0.7
else
    return max(input, -1.2f); // Soft clip negative at -1.2`
      },
      {
        name: 'Oversampling',
        description: 'Polyphase IIR oversampling for aliasing-free saturation',
        code: `// Upsample by factor (2x or 4x)
auto oversampledBlock = oversampler->processSamplesUp(audioBlock);

// Process at higher sample rate
processSaturation(oversampledBlock);

// Downsample back to original rate
auto outputBlock = juce::dsp::AudioBlock<float>(buffer);
oversampler->processSamplesDown(outputBlock);`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/Saturation-v1.0.0',
    docsUrl: '/docs/plugins/saturation'
  },
  {
    id: 'chorusflanger',
    name: 'Chorus/Flanger',
    version: '1.0.0',
    status: 'released',
    tagline: 'LFO-based modulation effects for movement and depth',
    description: 'Classic chorus and flanger effects using LFO-modulated delay lines. Create everything from subtle voice doubling to dramatic swept comb filtering with adjustable modulation, stereo width, and real-time LFO visualization.',
    features: [
      {
        name: 'Dual Effect Modes',
        description: 'Chorus (wide, slow modulation) and Flanger (narrow, fast modulation with feedback) in one plugin'
      },
      {
        name: 'Real-time LFO Visualizer',
        description: 'See the modulation waveform and L/R phase positions with color-coded indicators (orange=left, cyan=right)'
      },
      {
        name: 'Adjustable LFO Rate (0.1-10 Hz)',
        description: 'Control modulation speed with log scaling from slow chorusing to fast flanging'
      },
      {
        name: 'Modulation Depth (0-100%)',
        description: 'Variable depth control for subtle to extreme pitch modulation'
      },
      {
        name: 'Dual LFO Waveforms',
        description: 'Sine (smooth) and Triangle (linear) waveforms for different modulation characters'
      },
      {
        name: 'Stereo Width Control',
        description: 'Phase offset between L/R LFOs (0-100%) creates wide stereo movement'
      },
      {
        name: 'Feedback Control (0-95%)',
        description: 'Adjustable feedback for flanger resonance and comb filtering intensity'
      },
      {
        name: 'Mix Control',
        description: 'Blend dry and wet signals for parallel processing (0-100%)'
      }
    ],
    parameters: [
      {
        name: 'Rate',
        range: '0.1 to 10 Hz',
        description: 'LFO frequency with log scaling centered at 1 Hz'
      },
      {
        name: 'Depth',
        range: '0% to 100%',
        description: 'Modulation amount (delay time variation)'
      },
      {
        name: 'Mode',
        range: 'Chorus / Flanger',
        description: 'Chorus: 15ms base + ±10ms modulation | Flanger: 3ms base + ±2.5ms modulation'
      },
      {
        name: 'Feedback',
        range: '0% to 95%',
        description: 'Amount of output fed back into delay line (more prominent in flanger mode)'
      },
      {
        name: 'Mix',
        range: '0% to 100%',
        description: 'Dry/wet balance (0% = dry, 100% = wet)'
      },
      {
        name: 'Waveform',
        range: 'Sine / Triangle',
        description: 'LFO waveform shape (Sine = smooth, Triangle = linear)'
      },
      {
        name: 'Stereo Width',
        range: '0% to 100%',
        description: 'Phase offset between L/R LFOs for stereo movement (50% = 90° offset)'
      }
    ],
    useCases: [
      'Vocal doubling - Chorus mode with slow rate (0.3-0.8 Hz), medium depth for thick vocals',
      'Guitar widening - Chorus mode with stereo width at 75-100% for spacious guitar tones',
      'Synth movement - Flanger mode with medium rate (1-3 Hz) and high feedback for sweeping pads',
      'Classic jet flanger - Flanger mode with fast rate (5-8 Hz), high feedback, and high depth',
      'Subtle enhancement - Chorus mode with low depth (20-30%) and mix at 30% for gentle thickness',
      'Extreme modulation - Flanger mode with max depth and feedback for dramatic comb filtering',
      'Stereo auto-pan - High stereo width creates alternating L/R movement'
    ],
    learningFocus: [
      'LFO (Low-Frequency Oscillator) implementation',
      'Circular delay buffer with write position tracking',
      'Fractional delay using linear interpolation',
      'Feedback loops in delay-based effects',
      'Stereo processing with phase offset',
      'Time-based modulation effects',
      'Real-time visualization of LFO state',
      'Preventing feedback instability'
    ],
    algorithms: [
      {
        name: 'LFO Generation',
        description: 'Sine and triangle waveforms for smooth and linear modulation',
        code: `// Sine LFO
float lfoSine = sin(phase * 2π);

// Triangle LFO
float lfoTriangle = 4.0f * abs(phase - 0.5f) - 1.0f;

// Phase advancement
phase += (rate / sampleRate);
if (phase >= 1.0f) phase -= 1.0f;`
      },
      {
        name: 'Fractional Delay Interpolation',
        description: 'Linear interpolation between samples for smooth pitch changes',
        code: `// Calculate read position (fractional)
float readPos = writePos - delayInSamples;
while (readPos < 0.0f) readPos += bufferSize;

// Get integer and fractional parts
int readPosInt = (int)readPos;
float frac = readPos - readPosInt;

// Linear interpolation
int nextPos = (readPosInt + 1) % bufferSize;
float sample1 = buffer[readPosInt];
float sample2 = buffer[nextPos];
return sample1 + frac * (sample2 - sample1);`
      },
      {
        name: 'Modulated Delay',
        description: 'LFO modulates delay time for pitch variation',
        code: `// Calculate modulated delay time
float baseDelay = (mode == CHORUS) ? 15ms : 3ms;
float modRange = (mode == CHORUS) ? 10ms : 2.5ms;

float lfoValue = getLFO(phase, waveform);  // -1 to +1
float delayTime = baseDelay + lfoValue * modRange * depth;

// Read from delay buffer with interpolation
float delayed = getInterpolatedSample(buffer, delayTime);`
      },
      {
        name: 'Feedback Processing',
        description: 'Feedback creates comb filtering for flanger resonance',
        code: `// Read delayed sample
float delayed = getInterpolatedSample(buffer, delayTime);

// Write to delay buffer (input + feedback)
buffer[writePos] = input + delayed * feedback;

// Mix dry/wet
output = input * (1.0 - mix) + delayed * mix;`
      },
      {
        name: 'Stereo Phase Offset',
        description: 'Independent L/R LFO phases create stereo movement',
        code: `// Left channel LFO
lfoPhaseLeft += rateIncrement;
if (lfoPhaseLeft >= 1.0f) lfoPhaseLeft -= 1.0f;

// Right channel with phase offset
float phaseOffset = stereoWidth * 0.5f;  // 0-0.5 (0-180°)
lfoPhaseRight = lfoPhaseLeft + phaseOffset;
if (lfoPhaseRight >= 1.0f) lfoPhaseRight -= 1.0f;`
      }
    ],
    screenshots: [],
    downloadUrl: '/downloads/ChorusFlanger-v1.0.0',
    docsUrl: '/docs/plugins/chorusflanger'
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
        features: ['JUCE UnitTestRunner', '67 test suites', '100% pass rate']
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
    status: 'complete',
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
        status: 'complete',
        features: [
          'Low shelf (20Hz-500Hz, ±12dB)',
          'Parametric mid band (200Hz-5kHz, ±12dB, Q=1.0)',
          'High shelf (2kHz-20kHz, ±12dB)',
          'Optional high-pass filter (20Hz-500Hz, 12dB/oct)',
          'Optional low-pass filter (2kHz-20kHz, 12dB/oct)',
          'Biquad filter cascading with coefficient caching',
          'Per-channel filter state for stereo processing'
        ]
      },
      {
        name: 'SimpleComp',
        status: 'complete',
        features: [
          'Threshold (-60dB to 0dB), Ratio (1:1 to 20:1)',
          'Attack (0.1-100ms), Release (10-1000ms)',
          'Envelope follower with exponential coefficients',
          'Hard and soft knee modes (6dB soft knee)',
          'Automatic makeup gain (50% compensation)',
          'Real-time gain reduction metering (thread-safe)',
          'Professional gradient UI with vertical sliders and rotary knobs'
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
    name: 'Phase 3: Distribution & Expansion',
    status: 'current',
    goal: 'Expand plugin suite and improve distribution',
    timeline: '3-6 months',
    items: [
      {
        name: 'WaveShaper',
        status: 'complete',
        features: [
          '5 waveshaping modes (Soft Clip, Hard Clip, Tanh, Cubic, Asymmetric)',
          'Drive control (0-10x multiplier)',
          'Real-time curve visualization',
          'Mix control for parallel processing',
          'Tone filter (500Hz-20kHz)',
          'Output gain compensation (-12 to +12 dB)',
          'DC blocker and input/output metering'
        ]
      },
      {
        name: 'Saturation',
        status: 'complete',
        features: [
          '5 saturation modes (Tape, Tube, Transistor, Transformer, Diode)',
          'Drive control (0-10x multiplier)',
          'Real-time harmonic analyzer (H1-H8 visualization)',
          'Oversampling (Off/2x/4x) for aliasing reduction',
          'Dual tone controls (±12dB low/high shelves)',
          'Mix control for parallel processing',
          'DC blocker and input/output metering'
        ]
      },
      {
        name: 'Chorus/Flanger',
        status: 'complete',
        features: [
          'Dual effect modes (Chorus and Flanger)',
          'Adjustable LFO rate (0.1-10 Hz) and depth (0-100%)',
          'Real-time LFO visualizer with L/R phase indicators',
          'Two LFO waveforms (Sine and Triangle)',
          'Stereo width control (0-100% phase offset)',
          'Feedback control (0-95%) for flanger resonance',
          'Mix control for parallel processing'
        ]
      },
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
  github: 'https://github.com/kentin0-fiz0l/AudioForge',
  license: 'MIT',
  testCoverage: '100%',
  totalTests: 123,
  totalPlugins: plugins.length,
  releasedPlugins: getPluginsByStatus('released').length
};
