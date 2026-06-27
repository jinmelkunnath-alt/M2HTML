import type { Config } from 'tailwindcss';
export default {
  content: ['./index.html', './src/**/*.{ts,tsx}'],
  darkMode: ['class', '[data-theme="dark"]'],
  theme: {
    extend: {
      colors: {
        forge: { 50: '#eef8ff', 400: '#38bdf8', 500: '#0ea5e9', 700: '#0369a1' },
        ink: '#070b13'
      },
      boxShadow: { glow: '0 0 60px rgba(14,165,233,.25)' }
    }
  },
  plugins: []
} satisfies Config;
