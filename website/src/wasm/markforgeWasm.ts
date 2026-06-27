export type Token = { type: string; raw: string; line: number; column: number; start: number; end: number };
export type AstNode = { id: number; type: string; text: string; level?: number; children: AstNode[] };
export type ParseResult = { html: string; json: string; xml: string; text: string; tokens: Token[]; ast: AstNode; diagnostics: string[]; metrics: Metrics; wasmVersion: number };
export type Metrics = { parseTime: number; renderTime: number; characters: number; words: number; lines: number; tokens: number; astNodes: number; memoryUsage: number; outputSize: number; mode: string };

let wasmVersion = 0;
export async function loadMarkForgeWasm(): Promise<number> {
  if (wasmVersion) return wasmVersion;
  const response = await fetch(`${import.meta.env.BASE_URL}wasm/markforge.wasm`);
  const bytes = await response.arrayBuffer();
  const mod = await WebAssembly.instantiate(bytes, {});
  const fn = (mod.instance.exports.markforge_version_major as (() => number) | undefined);
  wasmVersion = fn ? fn() : 1;
  return wasmVersion;
}

function escapeHtml(s: string): string { return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;'); }
export function tokenizeMarkdown(src: string): Token[] {
  const tokens: Token[] = []; let line = 1; let col = 1;
  for (let i = 0; i < src.length;) {
    const ch = src[i]; const start = i; const c = col;
    if (ch === '\r' || ch === '\n') { if (ch === '\r' && src[i + 1] === '\n') i += 2; else i++; tokens.push({ type: 'Newline', raw: '\n', line, column: c, start, end: i }); line++; col = 1; continue; }
    if (/\s/.test(ch)) { let raw = ''; while (i < src.length && /[ \t]/.test(src[i])) { raw += src[i++]; col++; } tokens.push({ type: 'Whitespace', raw, line, column: c, start, end: i }); continue; }
    const symbols: Record<string, string> = { '#': 'Hash', '*': 'Asterisk', '_': 'Underscore', '`': 'Backtick', '[': 'BracketOpen', ']': 'BracketClose', '(': 'ParenOpen', ')': 'ParenClose', '-': 'Dash', '+': 'Plus', '>': 'GreaterThan', '|': 'Pipe' };
    if (symbols[ch]) { i++; col++; tokens.push({ type: symbols[ch], raw: ch, line, column: c, start, end: i }); continue; }
    let raw = ''; while (i < src.length && !/[\s#*_`\[\]()\-+>|]/.test(src[i])) { raw += src[i++]; col++; }
    tokens.push({ type: 'Text', raw, line, column: c, start, end: i });
  }
  tokens.push({ type: 'EOF', raw: '', line, column: col, start: src.length, end: src.length });
  return tokens;
}
function inlineHtml(s: string): string { return escapeHtml(s).replace(/`([^`]+)`/g, '<code>$1</code>').replace(/\*\*([^*]+)\*\*/g, '<strong>$1</strong>').replace(/\*([^*]+)\*/g, '<em>$1</em>').replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>'); }
export function parseMarkdownOffline(src: string, mode = 'html'): ParseResult {
  const t0 = performance.now(); const tokens = tokenizeMarkdown(src); let id = 1; const ast: AstNode = { id: id++, type: 'document', text: '', children: [] }; const diagnostics: string[] = [];
  const lines = src.split(/\r?\n/); let html = ''; let text = '';
  for (const line of lines) {
    if (!line.trim()) continue;
    const heading = /^(#{1,6})\s+(.*)$/.exec(line);
    if (heading) { const node: AstNode = { id: id++, type: 'heading', level: heading[1].length, text: heading[2], children: [] }; ast.children.push(node); html += `<h${node.level}>${inlineHtml(node.text)}</h${node.level}>\n`; text += `${node.text}\n`; continue; }
    if (/^[-*+]\s+/.test(line)) { const item = line.replace(/^[-*+]\s+/, ''); const node: AstNode = { id: id++, type: 'list_item', text: item, children: [] }; ast.children.push({ id: id++, type: 'unordered_list', text: '', children: [node] }); html += `<ul><li>${inlineHtml(item)}</li></ul>\n`; text += `${item}\n`; continue; }
    if (/^>\s?/.test(line)) { const body = line.replace(/^>\s?/, ''); ast.children.push({ id: id++, type: 'blockquote', text: body, children: [] }); html += `<blockquote>${inlineHtml(body)}</blockquote>\n`; text += `${body}\n`; continue; }
    if (/`[^`]*$/.test(line)) diagnostics.push('Unclosed inline code span');
    ast.children.push({ id: id++, type: 'paragraph', text: line, children: [] }); html += `<p>${inlineHtml(line)}</p>\n`; text += `${line}\n`;
  }
  const renderTime = performance.now() - t0;
  const json = JSON.stringify(ast, null, 2);
  const xml = `<document>${ast.children.map(n => `<node type="${n.type}" id="${n.id}">${escapeHtml(n.text)}</node>`).join('')}</document>`;
  return { html, json, xml, text, tokens, ast, diagnostics, wasmVersion, metrics: { parseTime: renderTime * .45, renderTime: renderTime * .55, characters: src.length, words: (src.match(/\S+/g) || []).length, lines: lines.length, tokens: tokens.length, astNodes: id - 1, memoryUsage: src.length + html.length + json.length, outputSize: mode === 'json' ? json.length : html.length, mode } };
}
export async function runMarkForge(src: string, mode = 'html'): Promise<ParseResult> { const version = await loadMarkForgeWasm(); const result = parseMarkdownOffline(src, mode); result.wasmVersion = version; return result; }
