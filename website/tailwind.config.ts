import type { Config } from "tailwindcss";

const config: Config = {
  content: [
    "./src/app/**/*.{js,ts,jsx,tsx,mdx}",
    "./src/components/**/*.{js,ts,jsx,tsx,mdx}",
  ],
  darkMode: 'class',
  theme: {
    extend: {
      colors: {
        // Base colors - Deep charcoal studio aesthetic
        background: {
          DEFAULT: '#0a0e14',
          light: '#151b24',
          lighter: '#1f2937',
        },
        foreground: {
          DEFAULT: '#e4e7eb',
          muted: '#9ca3af',
          dim: '#6b7280',
        },

        // Primary - Cyan (digital meter/oscilloscope glow)
        primary: {
          DEFAULT: '#06b6d4',
          light: '#22d3ee',
          dark: '#0891b2',
          glow: 'rgba(6, 182, 212, 0.5)',
        },

        // Accent - Amber (VU meter warm glow)
        accent: {
          DEFAULT: '#f59e0b',
          light: '#fbbf24',
          dark: '#d97706',
          glow: 'rgba(245, 158, 11, 0.5)',
        },

        // Semantic colors
        success: '#10b981',
        warning: '#f59e0b',
        error: '#ef4444',

        // Audio-specific colors
        waveform: '#06b6d4',
        spectrum: {
          low: '#10b981',
          mid: '#f59e0b',
          high: '#ef4444',
        },
      },

      fontFamily: {
        sans: ['Inter', 'system-ui', 'sans-serif'],
        mono: ['JetBrains Mono', 'Consolas', 'Monaco', 'monospace'],
        display: ['Inter', 'system-ui', 'sans-serif'],
      },

      backgroundImage: {
        'gradient-radial': 'radial-gradient(var(--tw-gradient-stops))',
        'gradient-primary': 'linear-gradient(135deg, #06b6d4 0%, #0891b2 100%)',
        'gradient-accent': 'linear-gradient(135deg, #f59e0b 0%, #d97706 100%)',
        'gradient-studio': 'linear-gradient(180deg, #0a0e14 0%, #151b24 100%)',
        'gradient-mesh': `
          radial-gradient(at 0% 0%, rgba(6, 182, 212, 0.1) 0px, transparent 50%),
          radial-gradient(at 100% 0%, rgba(245, 158, 11, 0.1) 0px, transparent 50%),
          radial-gradient(at 100% 100%, rgba(6, 182, 212, 0.1) 0px, transparent 50%),
          radial-gradient(at 0% 100%, rgba(245, 158, 11, 0.1) 0px, transparent 50%)
        `,
      },

      boxShadow: {
        'glow-primary': '0 0 20px rgba(6, 182, 212, 0.3)',
        'glow-accent': '0 0 20px rgba(245, 158, 11, 0.3)',
        'inner-glow': 'inset 0 0 20px rgba(6, 182, 212, 0.1)',
      },

      animation: {
        'pulse-slow': 'pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite',
        'waveform': 'waveform 2s ease-in-out infinite',
        'glow': 'glow 2s ease-in-out infinite',
      },

      keyframes: {
        waveform: {
          '0%, 100%': { transform: 'scaleY(1)' },
          '50%': { transform: 'scaleY(1.5)' },
        },
        glow: {
          '0%, 100%': { opacity: '0.5' },
          '50%': { opacity: '1' },
        },
      },
    },
  },
  plugins: [],
};
export default config;
