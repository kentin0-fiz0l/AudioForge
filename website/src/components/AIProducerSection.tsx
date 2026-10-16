import React from 'react';

const README = 'https://github.com/kentin0-fiz0l/AudioForge/tree/main/ableton/remote-scripts/AIProducer';

const abilities = [
  { title: 'Generate tracks', text: 'Drums, a bass line that follows the chords, chords and a lead, on new tracks with instruments loaded.' },
  { title: 'Mix from outside Live', text: 'Read and set faders, pans, sends and device parameters, and watch the meters.' },
  { title: 'Edit clips and the Arrangement', text: 'Read clips, set warping and markers, copy clips to the Arrangement and clear it.' },
  { title: 'Locators and playback', text: 'Place, list and jump to locators, and move the playhead to any beat.' },
];

export const AIProducerSection: React.FC = () => (
  <section id="ai-producer" className="py-16 md:py-24 bg-background-light">
    <div className="container mx-auto px-4 max-w-5xl">
      <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display text-center">
        AI Producer for Ableton Live
      </h2>
      <p className="text-foreground-muted text-lg text-center max-w-3xl mx-auto mb-12">
        A Live Control Surface script that lets a script or a chat assistant work on your Live set over OSC: build
        tracks, mix them, and edit clips and the Arrangement. It also records what you say about the sound, with what
        Live was doing at that moment, and that feedback is how AudioForge&apos;s plugins get fixed and improved.
      </p>
      <div className="grid grid-cols-1 sm:grid-cols-2 gap-6 mb-10">
        {abilities.map((a) => (
          <div key={a.title} className="bg-background border border-primary/10 rounded-xl p-6">
            <h3 className="text-lg font-bold text-foreground mb-2">{a.title}</h3>
            <p className="text-foreground-muted text-sm">{a.text}</p>
          </div>
        ))}
      </div>
      <p className="text-center">
        <a href={README} className="text-primary hover:underline font-semibold">
          Set it up in Live →
        </a>
      </p>
    </div>
  </section>
);
