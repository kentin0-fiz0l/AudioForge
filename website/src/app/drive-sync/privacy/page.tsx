import type { Metadata } from 'next';
import Link from 'next/link';

// The privacy policy Google asks for before a personal OAuth app can leave Testing (see ../page.tsx).
export const metadata: Metadata = {
  title: 'Drive Sync privacy policy | personal tool',
  description: 'What the Drive Sync personal tool does with Google data.',
  robots: { index: false, follow: false },
};

const points = [
  ['Who uses it', 'Only the owner of this site, with their own Google account. It is not offered to or used by anyone else.'],
  ['What it can reach', 'Through Google’s drive.file permission, only the Drive files and folders it created itself. It cannot see the rest of the Drive, Gmail, contacts or any other Google data.'],
  ['What it does', 'Copies the owner’s files from their computer into their own Drive, and lists the folders it made so it can add to them.'],
  ['Where data goes', 'Nowhere else. Files travel directly between the owner’s computer and Google. No data is collected, stored on any server, shared, sold or used for advertising or training.'],
  ['Sign-in tokens', 'Kept only in rclone’s configuration file on the owner’s computer.'],
  ['Removing access', 'The owner can revoke it at any time at myaccount.google.com/permissions.'],
] as const;

export default function DriveSyncPrivacyPage() {
  return (
    <main className="min-h-screen bg-background text-foreground">
      <section className="max-w-3xl mx-auto px-4 sm:px-6 py-16 lg:py-24 space-y-8">
        <div>
          <Link href="/drive-sync/" className="text-sm text-foreground-muted hover:text-primary transition-colors">← Drive Sync</Link>
          <h1 className="text-4xl font-bold mt-6 font-display">Privacy policy</h1>
          <p className="text-foreground-muted mt-2">Effective 8 November 2026</p>
        </div>
        <dl className="space-y-6">
          {points.map(([term, text]) => (
            <div key={term}>
              <dt className="font-semibold">{term}</dt>
              <dd className="text-foreground-muted mt-1">{text}</dd>
            </div>
          ))}
        </dl>
        <p className="text-foreground-muted">
          Its use of information received from Google APIs follows the Google API Services User Data Policy, including
          the Limited Use requirements.
        </p>
      </section>
    </main>
  );
}
