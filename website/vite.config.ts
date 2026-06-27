import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig(({ mode }) => ({
  plugins: [react()],
  base: mode === 'production' ? '/M2HTML/' : '/',
  build: {
    outDir: 'dist',
    sourcemap: mode !== 'production',
    assetsDir: 'assets',
    rollupOptions: {
      output: {
        manualChunks: {
          editor: ['@monaco-editor/react', 'monaco-editor'],
          motion: ['framer-motion']
        }
      }
    }
  },
  server: {
    host: '127.0.0.1',
    port: 5173
  }
}));
