import { useEffect, useState } from 'react';
export type Command = { id: string; label: string; run: () => void; keywords?: string };
export function useCommandPalette(commands: Command[]) {
  const [open, setOpen] = useState(false); const [query, setQuery] = useState('');
  useEffect(() => { const onKey = (e: KeyboardEvent) => { if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'k') { e.preventDefault(); setOpen(v => !v); } }; window.addEventListener('keydown', onKey); return () => window.removeEventListener('keydown', onKey); }, []);
  const filtered = commands.filter(c => `${c.label} ${c.keywords || ''}`.toLowerCase().includes(query.toLowerCase()));
  return { open, setOpen, query, setQuery, filtered };
}
