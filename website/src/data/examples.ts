export type Example = { slug: string; title: string; difficulty: 'Simple' | 'Intermediate' | 'Advanced'; content: string };
export const examples: Example[] = [
  { slug: 'simple', title: 'Simple Document', difficulty: 'Simple', content: '# Hello MarkForge\n\nThis is **bold**, *italic*, and `code`.\n' },
  { slug: 'tables', title: 'Tables', difficulty: 'Intermediate', content: '# Table\n\n| Name | Value |\n| --- | ---: |\n| Parse | Fast |\n| Render | Deterministic |\n' },
  { slug: 'nested-lists', title: 'Nested Lists', difficulty: 'Intermediate', content: '# Lists\n\n- Parser\n- AST\n- Renderers\n' },
  { slug: 'math', title: 'Math and Code', difficulty: 'Advanced', content: '# Math\n\nInline math $a^2+b^2=c^2$ and fenced code.\n\n```c\nprintf("MarkForge");\n```\n' },
  { slug: 'callouts', title: 'Callouts', difficulty: 'Advanced', content: '# Callout\n\n> [!NOTE]\n> MarkForge runs offline.\n' },
  { slug: 'complex', title: 'Complex Document', difficulty: 'Advanced', content: '# Architecture\n\n## Pipeline\n\nMarkdown → Tokenizer → Parser → AST → Renderer.\n\n- HTML\n- JSON\n- XML\n- Text\n\n[Documentation](./docs)\n' }
];
