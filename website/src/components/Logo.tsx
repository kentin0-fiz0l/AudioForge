import React from 'react';

interface LogoProps {
  size?: 'sm' | 'md' | 'lg';
  showText?: boolean;
  animate?: boolean;
}

export const Logo: React.FC<LogoProps> = ({
  size = 'md',
  showText = true,
  animate = false
}) => {
  const sizes = {
    sm: { container: 'w-8 h-8', text: 'text-lg' },
    md: { container: 'w-12 h-12', text: 'text-2xl' },
    lg: { container: 'w-16 h-16', text: 'text-3xl' },
  };

  const { container, text } = sizes[size];

  return (
    <div className="flex items-center space-x-3 group">
      {/* Waveform Icon */}
      <div className={`${container} relative`}>
        {/* Background glow */}
        <div className="absolute inset-0 bg-gradient-primary opacity-20 blur-xl rounded-lg group-hover:opacity-30 transition-opacity" />

        {/* Main container */}
        <div className="relative w-full h-full bg-background-light border border-primary/30 rounded-lg overflow-hidden group-hover:border-primary/50 transition-all">
          {/* Animated waveform bars */}
          <div className="absolute inset-0 flex items-center justify-center gap-[2px] px-1">
            {[0.3, 0.7, 1, 0.5, 0.8, 0.4, 0.9, 0.6].map((height, i) => (
              <div
                key={i}
                className={`flex-1 bg-gradient-to-t from-primary to-accent rounded-full ${
                  animate ? 'animate-waveform' : ''
                }`}
                style={{
                  height: `${height * 100}%`,
                  animationDelay: `${i * 0.1}s`,
                }}
              />
            ))}
          </div>
        </div>
      </div>

      {/* Text */}
      {showText && (
        <span className={`${text} font-bold text-foreground group-hover:text-primary transition-colors font-display tracking-tight`}>
          Audio<span className="text-primary">Forge</span>
        </span>
      )}
    </div>
  );
};
