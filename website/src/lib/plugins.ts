/**
 * The plugin catalog. Generated from the plugins themselves by website/scripts/build-catalog.mjs
 * (names, types and formats from each CMakeLists.txt, parameters from the source, the written
 * part from plugins/<Name>/plugin.json), so it lists every plugin and cannot drift.
 */
import catalog from './catalog.json';

export interface PluginFeature {
  name: string;
  description: string;
}

export interface PluginParameter {
  name: string;
  range: string;
  description?: string;
}

export type PluginCategory = 'utility' | 'creative' | 'mixing' | 'synthesis' | 'midi';
export type PluginType = 'instrument' | 'effect' | 'midi';
export type BadgeType = 'fixed' | 'experimental' | 'new' | 'validated' | 'popular';

export const pluginTypeLabel: Record<PluginType, string> = {
  instrument: 'Instrument',
  effect: 'Effect',
  midi: 'MIDI tool',
};

export interface Plugin {
  id: string;
  folder: string;
  name: string;
  type: PluginType;
  formats: string[];
  status: 'released' | 'in-development' | 'planned';
  category: PluginCategory;
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
  graphic?: string;
  badges?: BadgeType[];
  demo?: string; // an MP3 made by tools/render-demos.sh
}

type CatalogEntry = Omit<Plugin, 'status' | 'docsUrl' | 'badges'> & { experimental?: boolean };

export const plugins: Plugin[] = (catalog as CatalogEntry[]).map(({ experimental, ...entry }) => ({
  ...entry,
  status: 'released',
  docsUrl: `/plugins/${entry.id}/`,
  badges: experimental ? ['experimental'] : undefined,
}));

/**
 * Roadmap phases and their goals
 */
export interface RoadmapPhase {
  id: string;
  name: string;
  status: 'complete' | 'current' | 'planned';
  goal: string;
  items: {
    name: string;
    status: 'complete' | 'in-progress' | 'planned';
    features?: string[];
  }[];
}

export const roadmap: RoadmapPhase[] = [
  {
    id: 'suite',
    name: 'The plugin suite',
    status: 'complete',
    goal: '60 plugins built with JUCE and modern C++17',
    items: [
      { name: 'Effects', status: 'complete', features: ['EQ, compressors, limiter, gate, multiband', 'Delays, reverbs, modulation, saturation', 'Granular and spectral effects', 'Vocal tools: compressor, de-esser, doubler, harmonizer, vocoder'] },
      { name: 'Synths and instruments', status: 'complete', features: ['Subtractive, FM, wavetable, pad and mono synths', 'Drum and percussion synths', 'Acoustic models: bass, brass, strings, electric piano, organ, koto, sitar, shakuhachi'] },
      { name: 'MIDI tools', status: 'complete', features: ['Arpeggiator, chord generator, harmonizer, groove quantizer and more'] },
      { name: 'Formats', status: 'complete', features: ['VST3 on macOS, Windows and Linux', 'AU on macOS', 'Standalone apps'] },
    ],
  },
  {
    id: 'quality',
    name: 'Testing and quality',
    status: 'complete',
    goal: 'Every change is built and tested before it is merged',
    items: [
      { name: 'Automated tests', status: 'complete', features: ['290+ test suites on macOS, Windows and Linux for every change', 'Every plugin stress-tested at extreme settings'] },
      { name: 'Tuned from real sessions', status: 'complete', features: ['Feedback from using the plugins in Ableton Live is logged and drives fixes'] },
    ],
  },
  {
    id: 'ai-producer',
    name: 'AI Producer for Ableton Live',
    status: 'complete',
    goal: 'A Live control script that builds and edits sessions on request',
    items: [
      { name: 'Generate tracks', status: 'complete', features: ['Drums, bass that follows the chords, chords and lead'] },
      { name: 'Control Live', status: 'complete', features: ['Mixer, devices and meters', 'Clips, warping and Arrangement edits', 'Locators and playback position'] },
    ],
  },
  {
    id: 'distribution',
    name: 'Getting the plugins to people',
    status: 'current',
    goal: 'Install AudioForge without building it yourself',
    items: [
      { name: 'Website with the full catalog', status: 'in-progress', features: ['A page for every one of the 60 plugins'] },
      { name: 'Installers attached to every release', status: 'in-progress', features: ['macOS, Windows and Linux'] },
      { name: 'Code signing and notarization', status: 'planned' },
      { name: 'A user guide for each plugin', status: 'planned' },
    ],
  },
  {
    id: 'next-plugins',
    name: 'Next plugins',
    status: 'planned',
    goal: 'Gaps found while producing real shows with AudioForge',
    items: [
      { name: 'Hand and Latin percussion', status: 'planned' },
      { name: 'Tempo-synced pattern gate', status: 'planned' },
      { name: 'Riser and build generator', status: 'planned' },
      { name: '808 with glide, and a supersaw', status: 'planned' },
    ],
  },
];

/**
 * Get plugin by ID
 */
export function getPluginById(id: string): Plugin | undefined {
  return plugins.find(p => p.id === id);
}
