import { notFound } from 'next/navigation';
import { Metadata } from 'next';
import Link from 'next/link';
import Image from 'next/image';
import { getPluginById } from '@/lib/plugins';

interface PluginPageProps {
  params: Promise<{ slug: string }>;
}

// Generate static params for SimpleGain and PanUtil
export async function generateStaticParams() {
  return [
    { slug: 'simplegain' },
    { slug: 'panutil' },
  ];
}

// Generate metadata for SEO
export async function generateMetadata({ params }: PluginPageProps): Promise<Metadata> {
  const { slug } = await params;
  const plugin = getPluginById(slug);

  if (!plugin) {
    return {
      title: 'Plugin Not Found',
    };
  }

  return {
    title: `${plugin.name} - ${plugin.tagline} | AudioForge`,
    description: plugin.description,
    openGraph: {
      title: `${plugin.name} - AudioForge`,
      description: plugin.description,
      type: 'website',
    },
  };
}

export default async function PluginPage({ params }: PluginPageProps) {
  const { slug } = await params;
  const plugin = getPluginById(slug);

  if (!plugin) {
    notFound();
  }

  const isReleased = plugin.status === 'released';

  return (
    <div className="min-h-screen bg-background">
      {/* Hero Section */}
      <section className="relative bg-gradient-to-br from-primary/10 via-background to-accent/10 border-b border-foreground/10">
        <div className="container mx-auto px-6 py-16 lg:py-24">
          <div className="grid lg:grid-cols-2 gap-12 items-center">
            {/* Left: Plugin Info */}
            <div>
              {/* Breadcrumb */}
              <nav className="flex items-center gap-2 text-sm text-foreground/60 mb-6">
                <Link href="/" className="hover:text-primary transition-colors">
                  Home
                </Link>
                <span>/</span>
                <Link href="/#plugins" className="hover:text-primary transition-colors">
                  Plugins
                </Link>
                <span>/</span>
                <span className="text-foreground">{plugin.name}</span>
              </nav>

              {/* Title & Badge */}
              <div className="flex items-start gap-4 mb-4">
                <h1 className="text-5xl lg:text-6xl font-bold bg-gradient-primary bg-clip-text text-transparent">
                  {plugin.name}
                </h1>
                <span className="px-3 py-1 bg-green-500/20 text-green-400 border border-green-500/30 rounded-full text-xs font-medium mt-2">
                  v{plugin.version}
                </span>
              </div>

              <p className="text-xl lg:text-2xl text-accent font-medium mb-6">
                {plugin.tagline}
              </p>

              <p className="text-lg text-foreground/80 mb-8">
                {plugin.description}
              </p>

              {/* CTA Buttons */}
              <div className="flex flex-wrap gap-4">
                {isReleased && plugin.downloadUrl ? (
                  <a
                    href={plugin.downloadUrl}
                    className="px-8 py-4 bg-gradient-primary text-white text-lg font-semibold rounded-lg hover:opacity-90 transition-opacity shadow-lg shadow-primary/20"
                  >
                    Download for Free
                  </a>
                ) : (
                  <button
                    disabled
                    className="px-8 py-4 bg-foreground/5 text-foreground/40 text-lg font-semibold rounded-lg cursor-not-allowed border border-foreground/10"
                  >
                    Coming Soon
                  </button>
                )}
                {plugin.docsUrl && (
                  <Link
                    href={plugin.docsUrl}
                    className="px-8 py-4 bg-foreground/10 text-foreground text-lg font-semibold rounded-lg hover:bg-foreground/20 transition-colors border border-foreground/20"
                  >
                    View Documentation
                  </Link>
                )}
              </div>
            </div>

            {/* Right: Plugin Screenshot/Mockup */}
            <div className="relative">
              <div className="aspect-video rounded-xl bg-gradient-to-br from-primary/20 to-accent/20 border border-foreground/10 flex items-center justify-center overflow-hidden shadow-2xl">
                {plugin.screenshots && plugin.screenshots.length > 0 ? (
                  <Image
                    src={plugin.screenshots[0]}
                    alt={`${plugin.name} interface`}
                    width={800}
                    height={450}
                    className="w-full h-full object-cover"
                  />
                ) : (
                  <div className="text-center p-8">
                    <div className="text-8xl font-bold bg-gradient-primary bg-clip-text text-transparent mb-4">
                      {plugin.name.substring(0, 2)}
                    </div>
                    <p className="text-foreground/40">Screenshot coming soon</p>
                  </div>
                )}
              </div>
            </div>
          </div>
        </div>
      </section>

      {/* Features Deep Dive */}
      <section className="py-16 lg:py-24 border-b border-foreground/10">
        <div className="container mx-auto px-6">
          <h2 className="text-4xl font-bold mb-4 text-foreground">Features</h2>
          <p className="text-foreground/60 mb-12 max-w-3xl">
            Everything you need to know about what makes {plugin.name} powerful and easy to use.
          </p>

          <div className="grid md:grid-cols-2 gap-8">
            {plugin.features.map((feature, idx) => (
              <div
                key={idx}
                className="p-6 bg-foreground/5 border border-foreground/10 rounded-lg hover:border-primary/30 transition-colors"
              >
                <h3 className="text-xl font-bold text-primary mb-2">
                  {feature.name}
                </h3>
                <p className="text-foreground/70">
                  {feature.description}
                </p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Use Cases (if available) */}
      {plugin.useCases && plugin.useCases.length > 0 && (
        <section className="py-16 lg:py-24 bg-foreground/5 border-b border-foreground/10">
          <div className="container mx-auto px-6">
            <h2 className="text-4xl font-bold mb-4 text-foreground">Use Cases</h2>
            <p className="text-foreground/60 mb-12 max-w-3xl">
              Real-world scenarios where {plugin.name} shines.
            </p>

            <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
              {plugin.useCases.map((useCase, idx) => (
                <div
                  key={idx}
                  className="p-6 bg-background border border-foreground/10 rounded-lg"
                >
                  <div className="flex items-start gap-3">
                    <span className="text-2xl text-primary mt-1">→</span>
                    <p className="text-foreground/80">
                      {useCase}
                    </p>
                  </div>
                </div>
              ))}
            </div>
          </div>
        </section>
      )}

      {/* Technical Details */}
      <section className="py-16 lg:py-24 border-b border-foreground/10">
        <div className="container mx-auto px-6">
          <h2 className="text-4xl font-bold mb-4 text-foreground">Technical Details</h2>
          <p className="text-foreground/60 mb-12 max-w-3xl">
            Parameters, algorithms, and the DSP under the hood.
          </p>

          {/* Parameters */}
          <div className="mb-12">
            <h3 className="text-2xl font-bold mb-6 text-foreground">Parameters</h3>
            <div className="space-y-4">
              {plugin.parameters.map((param, idx) => (
                <div
                  key={idx}
                  className="p-6 bg-foreground/5 border border-foreground/10 rounded-lg"
                >
                  <div className="flex flex-wrap items-start justify-between gap-4 mb-2">
                    <h4 className="text-lg font-bold text-primary">
                      {param.name}
                    </h4>
                    <span className="px-3 py-1 bg-accent/20 text-accent text-sm font-mono rounded">
                      {param.range}
                    </span>
                  </div>
                  <p className="text-foreground/70">
                    {param.description}
                  </p>
                </div>
              ))}
            </div>
          </div>

          {/* Algorithms (if available) */}
          {plugin.algorithms && plugin.algorithms.length > 0 && (
            <div>
              <h3 className="text-2xl font-bold mb-6 text-foreground">DSP Algorithms</h3>
              <div className="space-y-6">
                {plugin.algorithms.map((algo, idx) => (
                  <div
                    key={idx}
                    className="p-6 bg-foreground/5 border border-foreground/10 rounded-lg"
                  >
                    <h4 className="text-xl font-bold text-primary mb-2">
                      {algo.name}
                    </h4>
                    <p className="text-foreground/70 mb-4">
                      {algo.description}
                    </p>
                    {algo.code && (
                      <pre className="p-4 bg-background border border-foreground/20 rounded-lg overflow-x-auto">
                        <code className="text-sm text-foreground/80 font-mono">
                          {algo.code}
                        </code>
                      </pre>
                    )}
                  </div>
                ))}
              </div>
            </div>
          )}

          {/* Learning Focus (if available) */}
          {plugin.learningFocus && plugin.learningFocus.length > 0 && (
            <div className="mt-12">
              <h3 className="text-2xl font-bold mb-6 text-foreground">Learning Focus</h3>
              <p className="text-foreground/60 mb-6">
                What you&apos;ll learn by studying this plugin&apos;s source code:
              </p>
              <div className="grid md:grid-cols-2 gap-3">
                {plugin.learningFocus.map((topic, idx) => (
                  <div
                    key={idx}
                    className="flex items-start gap-3 p-3 bg-background border border-foreground/10 rounded-lg"
                  >
                    <span className="text-primary mt-0.5">✓</span>
                    <span className="text-foreground/80">{topic}</span>
                  </div>
                ))}
              </div>
            </div>
          )}
        </div>
      </section>

      {/* Installation Guide */}
      <section className="py-16 lg:py-24 bg-foreground/5 border-b border-foreground/10">
        <div className="container mx-auto px-6">
          <h2 className="text-4xl font-bold mb-4 text-foreground">Installation</h2>
          <p className="text-foreground/60 mb-12 max-w-3xl">
            Get {plugin.name} up and running in your DAW in minutes.
          </p>

          <div className="max-w-4xl space-y-8">
            {/* macOS */}
            <div className="p-6 bg-background border border-foreground/10 rounded-lg">
              <h3 className="text-xl font-bold text-primary mb-4 flex items-center gap-2">
                <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 24 24">
                  <path d="M18.71 19.5c-.83 1.24-1.71 2.45-3.05 2.47-1.34.03-1.77-.79-3.29-.79-1.53 0-2 .77-3.27.82-1.31.05-2.3-1.32-3.14-2.53C4.25 17 2.94 12.45 4.7 9.39c.87-1.52 2.43-2.48 4.12-2.51 1.28-.02 2.5.87 3.29.87.78 0 2.26-1.07 3.81-.91.65.03 2.47.26 3.64 1.98-.09.06-2.17 1.28-2.15 3.81.03 3.02 2.65 4.03 2.68 4.04-.03.07-.42 1.44-1.38 2.83M13 3.5c.73-.83 1.94-1.46 2.94-1.5.13 1.17-.34 2.35-1.04 3.19-.69.85-1.83 1.51-2.95 1.42-.15-1.15.41-2.35 1.05-3.11z"/>
                </svg>
                macOS
              </h3>
              <ol className="space-y-3 text-foreground/80">
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">1</span>
                  <span>Download the <code className="px-2 py-0.5 bg-foreground/10 rounded text-sm font-mono">.vst3</code> file</span>
                </li>
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">2</span>
                  <span>Copy to <code className="px-2 py-0.5 bg-foreground/10 rounded text-sm font-mono">~/Library/Audio/Plug-Ins/VST3/</code></span>
                </li>
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">3</span>
                  <span>Restart your DAW and rescan plugins</span>
                </li>
              </ol>
            </div>

            {/* Windows */}
            <div className="p-6 bg-background border border-foreground/10 rounded-lg">
              <h3 className="text-xl font-bold text-primary mb-4 flex items-center gap-2">
                <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 24 24">
                  <path d="M0 3.449L9.75 2.1v9.451H0m10.949-9.602L24 0v11.4H10.949M0 12.6h9.75v9.451L0 20.699M10.949 12.6H24V24l-12.9-1.801"/>
                </svg>
                Windows
              </h3>
              <ol className="space-y-3 text-foreground/80">
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">1</span>
                  <span>Download the <code className="px-2 py-0.5 bg-foreground/10 rounded text-sm font-mono">.vst3</code> file</span>
                </li>
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">2</span>
                  <span>Copy to <code className="px-2 py-0.5 bg-foreground/10 rounded text-sm font-mono">C:\Program Files\Common Files\VST3\</code></span>
                </li>
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">3</span>
                  <span>Restart your DAW and rescan plugins</span>
                </li>
              </ol>
            </div>

            {/* Linux */}
            <div className="p-6 bg-background border border-foreground/10 rounded-lg">
              <h3 className="text-xl font-bold text-primary mb-4 flex items-center gap-2">
                <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 24 24">
                  <path d="M12.504 0c-.155 0-.315.008-.48.021-4.226.333-3.105 4.807-3.17 6.298-.076 1.092-.3 1.953-1.05 3.02-.885 1.051-2.127 2.75-2.716 4.521-.278.84-.41 1.684-.287 2.489a2.5 2.5 0 00-.096.25c-.206.732.028 1.768.484 2.574.456.807 1.128 1.436 1.921 1.696.793.261 1.745.233 2.604-.28.859-.514 1.526-1.389 1.732-2.12.206-.732-.028-1.768-.484-2.575s-1.128-1.436-1.921-1.696c-.204-.067-.411-.11-.615-.127.055-.57.342-1.188.945-1.681.844-.69 1.989-1.089 3.23-1.089 2.04 0 3.882.955 5.244 2.317.68.681 1.283 1.436 1.791 2.217.126-.309.186-.646.163-.993-.11-1.665-.927-3.23-2.317-4.622-1.39-1.392-2.955-2.208-4.62-2.318-.838-.056-1.643.063-2.356.35-.058-.104-.121-.203-.187-.3a2.503 2.503 0 00-.097-.25c-.206-.731.028-1.767.484-2.574.456-.806 1.128-1.436 1.921-1.696.793-.26 1.745-.233 2.604.28.859.514 1.526 1.389 1.732 2.121.206.731-.028 1.767-.484 2.574-.456.806-1.128 1.436-1.921 1.696a2.5 2.5 0 00-.615.127c.055.57.342 1.188.945 1.681.844.69 1.989 1.089 3.23 1.089 2.04 0 3.882-.955 5.244-2.317 1.362-1.362 2.178-2.926 2.317-4.622.14-1.695-.403-3.383-1.696-4.676-1.293-1.293-2.98-1.836-4.676-1.696z"/>
                </svg>
                Linux
              </h3>
              <ol className="space-y-3 text-foreground/80">
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">1</span>
                  <span>Download the <code className="px-2 py-0.5 bg-foreground/10 rounded text-sm font-mono">.vst3</code> file</span>
                </li>
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">2</span>
                  <span>Copy to <code className="px-2 py-0.5 bg-foreground/10 rounded text-sm font-mono">~/.vst3/</code></span>
                </li>
                <li className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 bg-primary/20 text-primary rounded-full flex items-center justify-center text-sm font-bold">3</span>
                  <span>Restart your DAW and rescan plugins</span>
                </li>
              </ol>
            </div>
          </div>
        </div>
      </section>

      {/* Download CTA */}
      <section className="py-16 lg:py-24">
        <div className="container mx-auto px-6">
          <div className="max-w-4xl mx-auto text-center">
            <h2 className="text-4xl lg:text-5xl font-bold mb-6 bg-gradient-primary bg-clip-text text-transparent">
              Ready to try {plugin.name}?
            </h2>
            <p className="text-xl text-foreground/70 mb-8">
              {isReleased
                ? `Download now for free, or browse more plugins on the FluxStudio marketplace.`
                : `This plugin is currently ${plugin.status === 'in-development' ? 'in development' : 'planned'}. Check back soon!`
              }
            </p>
            <div className="flex flex-wrap gap-4 justify-center">
              {isReleased && plugin.downloadUrl ? (
                <>
                  <a
                    href={plugin.downloadUrl}
                    className="px-8 py-4 bg-gradient-primary text-white text-lg font-semibold rounded-lg hover:opacity-90 transition-opacity shadow-lg shadow-primary/20"
                  >
                    Download {plugin.name}
                  </a>
                  <a
                    href="https://fluxstudio.app/marketplace"
                    className="px-8 py-4 bg-foreground/10 text-foreground text-lg font-semibold rounded-lg hover:bg-foreground/20 transition-colors border border-foreground/20"
                  >
                    Browse FluxStudio Marketplace
                  </a>
                </>
              ) : (
                <Link
                  href="/#plugins"
                  className="px-8 py-4 bg-gradient-primary text-white text-lg font-semibold rounded-lg hover:opacity-90 transition-opacity shadow-lg shadow-primary/20"
                >
                  View All Plugins
                </Link>
              )}
            </div>
          </div>
        </div>
      </section>
    </div>
  );
}
