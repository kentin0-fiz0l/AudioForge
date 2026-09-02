'use client';

import React, { useState } from 'react';
import { AnimatedSection } from './AnimatedSection';

interface FAQItem {
  question: string;
  answer: string;
}

const faqs: FAQItem[] = [
  {
    question: 'Are AudioForge plugins really free?',
    answer: 'Yes! All AudioForge plugins are 100% free and open source under the MIT License. No hidden costs, no subscriptions, no feature limitations. Use them for personal or commercial projects.',
  },
  {
    question: 'What DAWs are supported?',
    answer: 'AudioForge plugins work in any DAW that supports VST3 format (Ableton Live, FL Studio, Reaper, Bitwig, etc.) or AU format on macOS (Logic Pro, GarageBand, Ableton, etc.). Standalone versions are also available.',
  },
  {
    question: 'Do I need to create an account to download?',
    answer: 'No account required! Just click download and you\'ll get the plugins immediately. We respect your privacy and don\'t collect personal information.',
  },
  {
    question: 'How do I install the plugins?',
    answer: 'Download the installer for your platform, run it, and the plugins will be automatically placed in your system plugin folder. Restart your DAW and rescan plugins. Detailed installation guides are available for each platform.',
  },
  {
    question: 'Are the plugins compatible with Apple Silicon (M1/M2/M3)?',
    answer: 'Yes! Our macOS builds are universal binaries that run natively on both Intel and Apple Silicon Macs, providing optimal performance on all systems.',
  },
  {
    question: 'Can I use AudioForge plugins commercially?',
    answer: 'Absolutely! The MIT License allows you to use our plugins in any project, including commercial music production, sound design, podcasts, or film scoring. No attribution required (though appreciated!).',
  },
  {
    question: 'How are these different from paid plugins?',
    answer: 'AudioForge plugins focus on essential, well-executed features with clean code and 100% test coverage. While paid plugins might have more complex UIs or exotic features, our plugins excel at core DSP tasks with professional quality.',
  },
  {
    question: 'Can I contribute to the project?',
    answer: 'Yes! AudioForge is open source. Report bugs, request features, submit pull requests, or help with documentation on GitHub. Check our CONTRIBUTING.md guide to get started.',
  },
  {
    question: 'Will you add AAX support for Pro Tools?',
    answer: 'AAX support is planned for v1.5.0. It requires an iLok developer account and additional certification. Follow our roadmap or star the repo on GitHub to stay updated.',
  },
  {
    question: 'How stable are the plugins?',
    answer: 'Very stable! All 22 plugins have 100% test coverage with 136 passing tests. They undergo continuous integration testing on macOS, Windows, and Linux before each release. Our code signing ensures integrity.',
  },
];

export const FAQ: React.FC = () => {
  const [openIndex, setOpenIndex] = useState<number | null>(null);

  const toggleFAQ = (index: number) => {
    setOpenIndex(openIndex === index ? null : index);
  };

  return (
    <section className="py-16 md:py-24 bg-background">
      <div className="container mx-auto px-4">
        <AnimatedSection animation="fade-up">
          <div className="text-center mb-12 md:mb-16">
            <h2 className="text-3xl md:text-4xl lg:text-5xl font-bold text-foreground mb-4 font-display">
              Frequently Asked Questions
            </h2>
            <p className="text-foreground-muted text-lg max-w-2xl mx-auto">
              Everything you need to know about AudioForge plugins.
            </p>
          </div>
        </AnimatedSection>

        <div className="max-w-3xl mx-auto space-y-3">
          {faqs.map((faq, index) => (
            <AnimatedSection key={index} animation="fade-up" delay={index * 30}>
              <div className="border border-primary/10 rounded-xl overflow-hidden bg-background-light hover:border-primary/30 transition-all">
                <button
                  onClick={() => toggleFAQ(index)}
                  className="w-full px-6 py-4 text-left flex items-center justify-between gap-4 group"
                >
                  <span className="text-lg font-semibold text-foreground group-hover:text-primary transition-colors">
                    {faq.question}
                  </span>
                  <svg
                    className={`w-5 h-5 text-primary flex-shrink-0 transition-transform duration-300 ${
                      openIndex === index ? 'rotate-180' : ''
                    }`}
                    fill="none"
                    stroke="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 9l-7 7-7-7" />
                  </svg>
                </button>

                <div
                  className={`overflow-hidden transition-all duration-300 ease-in-out ${
                    openIndex === index ? 'max-h-96 opacity-100' : 'max-h-0 opacity-0'
                  }`}
                >
                  <div className="px-6 pb-4 text-foreground-muted leading-relaxed">
                    {faq.answer}
                  </div>
                </div>
              </div>
            </AnimatedSection>
          ))}
        </div>

        {/* Still have questions CTA */}
        <AnimatedSection animation="fade-up" delay={300}>
          <div className="mt-12 text-center">
            <p className="text-foreground-muted mb-4">
              Still have questions?
            </p>
            <a
              href="https://github.com/kentin0-fiz0l/AudioForge/discussions"
              target="_blank"
              rel="noopener noreferrer"
              className="inline-flex items-center gap-2 px-6 py-3 bg-background-light border border-primary/30 text-primary rounded-lg hover:bg-background-lighter hover:border-primary/50 transition-all font-medium"
            >
              <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M8 12h.01M12 12h.01M16 12h.01M21 12c0 4.418-4.03 8-9 8a9.863 9.863 0 01-4.255-.949L3 20l1.395-3.72C3.512 15.042 3 13.574 3 12c0-4.418 4.03-8 9-8s9 3.582 9 8z" />
              </svg>
              Ask in GitHub Discussions
            </a>
          </div>
        </AnimatedSection>
      </div>
    </section>
  );
};
