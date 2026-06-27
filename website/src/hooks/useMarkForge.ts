import { useEffect, useMemo, useState } from 'react';
import { ParseResult, runMarkForge } from '../wasm/markforgeWasm';
export function useMarkForge(markdown: string, renderer: string) {
  const [result, setResult] = useState<ParseResult | null>(null);
  const [loading, setLoading] = useState(true);
  useEffect(() => { let active = true; setLoading(true); const id = setTimeout(() => { runMarkForge(markdown, renderer).then(r => { if (active) setResult(r); }).finally(() => active && setLoading(false)); }, 20); return () => { active = false; clearTimeout(id); }; }, [markdown, renderer]);
  return useMemo(() => ({ result, loading }), [result, loading]);
}
