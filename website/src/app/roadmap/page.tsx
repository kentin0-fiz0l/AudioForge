import type { Metadata } from 'next';
import Link from 'next/link';
import { roadmap, type RoadmapPhase } from '@/lib/plugins';

export const metadata: Metadata = {
  title: 'Roadmap | AudioForge',
  description: 'What AudioForge has built so far and what comes next.',
  openGraph: {
    title: 'Roadmap | AudioForge',
    description: 'What AudioForge has built so far and what comes next.',
  },
};

type ItemStatus = RoadmapPhase['items'][number]['status'];

const phaseLabel: Record<RoadmapPhase['status'], string> = { complete: 'Done', current: 'Now', planned: 'Next' };
const phaseStyle: Record<RoadmapPhase['status'], string> = {
  complete: 'bg-green-500/20 text-green-400 border-green-500/50',
  current: 'bg-primary/20 text-primary border-primary/50',
  planned: 'bg-zinc-700/50 text-zinc-400 border-zinc-600',
};
const itemMark: Record<ItemStatus, string> = { complete: '✓', 'in-progress': '●', planned: '○' };
const itemStyle: Record<ItemStatus, string> = {
  complete: 'text-green-400',
  'in-progress': 'text-primary',
  planned: 'text-zinc-500',
};

export default function RoadmapPage() {
  return (
    <main className="min-h-screen bg-background text-foreground">
      <section className="border-b border-zinc-800">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 py-16 lg:py-24">
          <Link href="/" className="text-sm text-foreground-muted hover:text-primary transition-colors">← AudioForge</Link>
          <h1 className="text-4xl lg:text-5xl font-bold mt-6 mb-4 font-display">Roadmap</h1>
          <p className="text-lg text-foreground-muted max-w-2xl">
            What is built, what is being worked on now, and what comes next. AudioForge is free and open source under the MIT License.
          </p>
        </div>
      </section>

      <section className="max-w-4xl mx-auto px-4 sm:px-6 py-12 space-y-10">
        {roadmap.map((phase) => (
          <div key={phase.id} className="border border-zinc-800 rounded-xl p-6 bg-background-light">
            <div className="flex flex-wrap items-center gap-3 mb-2">
              <h2 className="text-2xl font-bold">{phase.name}</h2>
              <span className={`px-3 py-1 rounded-full text-xs font-medium border ${phaseStyle[phase.status]}`}>
                {phaseLabel[phase.status]}
              </span>
            </div>
            <p className="text-foreground-muted mb-6">{phase.goal}</p>
            <ul className="space-y-4">
              {phase.items.map((item) => (
                <li key={item.name} className="flex gap-3">
                  <span className={`mt-0.5 w-5 text-center ${itemStyle[item.status]}`} aria-hidden>{itemMark[item.status]}</span>
                  <div>
                    <div className="font-semibold">
                      {item.name}
                      {item.status === 'in-progress' && <span className="ml-2 text-xs font-normal text-primary">in progress</span>}
                    </div>
                    {item.features && (
                      <ul className="mt-1 text-sm text-foreground-muted space-y-0.5">
                        {item.features.map((feature) => <li key={feature}>{feature}</li>)}
                      </ul>
                    )}
                  </div>
                </li>
              ))}
            </ul>
          </div>
        ))}
      </section>
    </main>
  );
}
