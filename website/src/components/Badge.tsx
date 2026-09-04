import React from 'react';

export type BadgeType = 'fixed' | 'experimental' | 'new' | 'validated' | 'popular';

interface BadgeProps {
  type: BadgeType;
  text?: string;
  className?: string;
}

const badgeStyles: Record<BadgeType, {
  bg: string;
  text: string;
  border: string;
  icon?: React.ReactNode;
}> = {
  fixed: {
    bg: 'bg-green-500/10',
    text: 'text-green-400',
    border: 'border-green-500/30',
    icon: (
      <svg className="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
      </svg>
    ),
  },
  experimental: {
    bg: 'bg-yellow-500/10',
    text: 'text-yellow-400',
    border: 'border-yellow-500/30',
    icon: (
      <svg className="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
      </svg>
    ),
  },
  new: {
    bg: 'bg-blue-500/10',
    text: 'text-blue-400',
    border: 'border-blue-500/30',
    icon: (
      <svg className="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 3v4M3 5h4M6 17v4m-2-2h4m5-16l2.286 6.857L21 12l-5.714 2.143L13 21l-2.286-6.857L5 12l5.714-2.143L13 3z" />
      </svg>
    ),
  },
  validated: {
    bg: 'bg-green-500/10',
    text: 'text-green-400',
    border: 'border-green-500/30',
    icon: (
      <svg className="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0121 12a11.955 11.955 0 01-2.382 7.016m-4.846 1.966A7.955 7.955 0 0112 21a7.955 7.955 0 01-1.772-.203m2.5-8.333V5" />
      </svg>
    ),
  },
  popular: {
    bg: 'bg-purple-500/10',
    text: 'text-purple-400',
    border: 'border-purple-500/30',
    icon: (
      <svg className="w-3.5 h-3.5" fill="currentColor" viewBox="0 0 24 24">
        <path d="M11.049 2.927c.3-.921 1.603-.921 1.902 0l1.519 4.674a1 1 0 00.95.69h4.915c.969 0 1.371 1.24.588 1.81l-3.976 2.888a1 1 0 00-.363 1.118l1.518 4.674c.3.922-.755 1.688-1.538 1.118l-3.976-2.888a1 1 0 00-1.176 0l-3.976 2.888c-.783.57-1.838-.197-1.538-1.118l1.518-4.674a1 1 0 00-.363-1.118l-3.976-2.888c-.784-.57-.38-1.81.588-1.81h4.914a1 1 0 00.951-.69l1.519-4.674z" />
      </svg>
    ),
  },
};

const defaultText: Record<BadgeType, string> = {
  fixed: 'Fixed in v1.9.0',
  experimental: 'Experimental',
  new: 'New',
  validated: 'Validated',
  popular: 'Popular',
};

export const Badge: React.FC<BadgeProps> = ({ type, text, className = '' }) => {
  const style = badgeStyles[type];
  const displayText = text || defaultText[type];

  return (
    <div
      className={`inline-flex items-center gap-1.5 px-3 py-1 ${style.bg} ${style.text} ${style.border} border rounded-full text-xs font-semibold ${className}`}
    >
      {style.icon}
      <span>{displayText}</span>
    </div>
  );
};

// Convenience components for specific badge types
export const FixedBadge: React.FC<{ className?: string }> = ({ className }) => (
  <Badge type="fixed" className={className} />
);

export const ExperimentalBadge: React.FC<{ className?: string }> = ({ className }) => (
  <Badge type="experimental" className={className} />
);

export const NewBadge: React.FC<{ className?: string }> = ({ className }) => (
  <Badge type="new" className={className} />
);

export const ValidatedBadge: React.FC<{ className?: string }> = ({ className }) => (
  <Badge type="validated" className={className} />
);

export const PopularBadge: React.FC<{ className?: string }> = ({ className }) => (
  <Badge type="popular" className={className} />
);
