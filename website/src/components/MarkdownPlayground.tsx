import { useMemo, useState } from 'react';
import Editor from '@monaco-editor/react';
import { Tabs, Button, Panel } from './ui';
import { useMarkForge } from '../hooks/useMarkForge';
import { useCommandPalette } from '../hooks/useCommandPalette';
import { CommandPalette } from './CommandPalette';
import { PerformanceDashboard } from './PerformanceDashboard';
import { downloadFile } from '../hooks/useLocalFile';
import { loadSettings, saveRecent, saveSettings } from '../utils/storage';
import { examples } from '../data/examples';
export function MarkdownPlayground() {
  const [markdown, setMarkdown] = useState(examples[0].content); const [tab, setTab] = useState('Preview'); const [view, setView] = useState('Split'); const [settings, setSettings] = useState(loadSettings()); const { result } = useMarkForge(markdown, settings.renderer);
  const commands = useMemo(() => [
    { id: 'format', label: 'Format document', run: () => setMarkdown(markdown.trim() + '\n') },
    { id: 'theme', label: 'Toggle theme', run: () => { const next = settings.theme === 'dark' ? 'light' : 'dark'; setSettings({ ...settings, theme: next }); saveSettings({ ...settings, theme: next }); document.documentElement.dataset.theme = next; } },
    { id: 'docs', label: 'Open documentation', run: () => { history.pushState(null, '', '/documentation'); dispatchEvent(new Event('popstate')); } },
    { id: 'save', label: 'Save recent file', run: () => saveRecent('playground.md', markdown) }
  ], [markdown, settings]);
  const palette = useCommandPalette(commands);
  const output = result ? { HTML: result.html, AST: JSON.stringify(result.ast, null, 2), 'Token Stream': JSON.stringify(result.tokens, null, 2), Diagnostics: result.diagnostics.join('\n') || 'No diagnostics', Performance: JSON.stringify(result.metrics, null, 2), Configuration: JSON.stringify(settings, null, 2), 'Rendered JSON': result.json, 'Rendered XML': result.xml, 'Rendered Plain Text': result.text } : {};
  return <div className="space-y-4"><CommandPalette open={palette.open} query={palette.query} setQuery={palette.setQuery} commands={palette.filtered} close={() => palette.setOpen(false)} /><div className="flex flex-wrap items-center justify-between gap-3"><div className="flex gap-2"><Button onClick={() => setView('Split')}>Split</Button><Button variant="ghost" onClick={() => setView('Editor')}>Editor</Button><Button variant="ghost" onClick={() => setView('Preview')}>Preview</Button></div><div className="flex gap-2"><Button variant="ghost" onClick={() => navigator.clipboard.writeText(result?.html || '')}>Copy HTML</Button><Button onClick={() => downloadFile('markforge.html', result?.html || '', 'text/html')}>Export HTML</Button></div></div><div className={`grid gap-4 ${view === 'Split' ? 'lg:grid-cols-2' : 'grid-cols-1'}`}>{view !== 'Preview' && <Panel title="Markdown Editor"><Editor height="62vh" language="markdown" theme="vs-dark" value={markdown} onChange={v => setMarkdown(v || '')} options={{ wordWrap: settings.wordWrap ? 'on' : 'off', fontSize: settings.fontSize, fontFamily: settings.fontFamily, minimap: { enabled: true }, lineNumbers: 'on', tabSize: settings.tabWidth, automaticLayout: true }} /></Panel>}{view !== 'Editor' && <Panel title="Live Preview"><article className="prose-markforge min-h-[62vh] overflow-auto" dangerouslySetInnerHTML={{ __html: result?.html || '' }} /></Panel>}</div><PerformanceDashboard metrics={result?.metrics} /><Panel title="Developer Panels"><Tabs tabs={Object.keys(output)} active={tab} onChange={setTab} /><pre className="mt-4 max-h-96 overflow-auto rounded-2xl bg-black/40 p-4 text-xs">{output[tab as keyof typeof output]}</pre></Panel></div>;
}
