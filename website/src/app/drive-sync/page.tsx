import type { Metadata } from 'next';
import Link from 'next/link';

// The homepage Google asks for before a personal OAuth app (the site owner's own rclone client, which copies
// their files to their own Google Drive) can leave Testing. Not part of AudioForge: kept out of the sitemap
// and out of search.
export const metadata: Metadata = {
  title: 'Drive Sync | personal tool',
  description: 'A personal tool that copies its owner’s files to their own Google Drive.',
  robots: { index: false, follow: false },
};

export default function DriveSyncPage() {
  return (
    <main className="min-h-screen bg-background text-foreground">
      <section className="max-w-3xl mx-auto px-4 sm:px-6 py-16 lg:py-24 space-y-6">
        <h1 className="text-4xl font-bold font-display">Drive Sync</h1>
        <p className="text-lg text-foreground-muted">
          A personal tool, used by this site&rsquo;s owner only. It is the owner&rsquo;s own Google sign-in for{' '}
          <a href="https://rclone.org" className="text-primary hover:underline">rclone</a>, the open-source file copier,
          which they use to copy their own files from their computer to their own Google Drive: rehearsal video and
          project files.
        </p>
        <p className="text-foreground-muted">
          It asks Google only for access to the Drive files it creates itself, and it is not offered to anyone else.
        </p>
        <p>
          <Link href="/drive-sync/privacy/" className="text-primary hover:underline">Privacy policy</Link>
        </p>
      </section>
    </main>
  );
}
