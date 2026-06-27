export type DocSection = { slug: string; title: string; body: string };
export const docs: DocSection[] = [
  { slug: 'getting-started', title: 'Getting Started', body: 'Install or build MarkForge, then run tools/markforge render-html README.md.' },
  { slug: 'architecture', title: 'Architecture', body: 'MarkForge is a compiler-style pipeline: tokenizer, token stream, recursive descent parser, AST, renderer manager and exporters.' },
  { slug: 'parser', title: 'Parser', body: 'The parser creates a stable AST and reports diagnostics without stopping at the first issue.' },
  { slug: 'ast', title: 'AST', body: 'The AST owns node IDs, parent links, child lists, sibling navigation, visitors and statistics.' },
  { slug: 'renderer', title: 'Renderer', body: 'Renderers consume only the AST and produce HTML, JSON, XML, text or formatted Markdown.' },
  { slug: 'configuration', title: 'Configuration', body: 'Layered configuration supports defaults, profiles and local overrides.' },
  { slug: 'plugin-sdk', title: 'Plugin SDK', body: 'Plugins are local, self-contained extensions using stable metadata and lifecycle hooks.' },
  { slug: 'cli', title: 'CLI', body: 'The professional CLI includes parse, render, lint, benchmark, profile, watch, serve and inspect commands.' },
  { slug: 'faq', title: 'FAQ', body: 'MarkForge requires no API keys, cloud services, remote databases or external backend.' }
];
export function searchDocs(query: string): DocSection[] { const q = query.toLowerCase(); return docs.filter(d => d.title.toLowerCase().includes(q) || d.body.toLowerCase().includes(q)); }
