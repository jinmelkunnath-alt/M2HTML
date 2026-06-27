import { describe, expect, it } from 'vitest';
import { parseMarkdownOffline, tokenizeMarkdown } from '../wasm/markforgeWasm';
describe('MarkForge playground parser', () => {
  it('tokenizes markdown', () => { expect(tokenizeMarkdown('# Hello').some(t => t.type === 'Hash')).toBe(true); });
  it('renders html and ast', () => { const r = parseMarkdownOffline('# Hello\n\nText'); expect(r.html).toContain('<h1>'); expect(r.ast.children.length).toBeGreaterThan(1); });
});
