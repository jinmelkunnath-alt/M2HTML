import { Layout } from './components/Layout';
import { useRoute } from './components/Router';
import { Home } from './pages/Home';
import { Playground } from './pages/Playground';
import { Documentation } from './pages/Documentation';
import { Examples } from './pages/Examples';
import { Benchmarks } from './pages/Benchmarks';
import { Architecture } from './pages/Architecture';
import { SimplePage } from './pages/SimplePage';
const copy: Record<string, [string,string]> = {
  '/marketplace': ['Plugin Marketplace', 'A local-first marketplace model for discovering and validating plugins from folders you control.'],
  '/downloads': ['Downloads', 'Build MarkForge from source or package it with the offline release scripts.'],
  '/releases': ['Release Notes', 'Track foundation, parser, renderer, CLI, and website milestones.'],
  '/roadmap': ['Roadmap', 'Prompt 6 completes audits, optimization, polish, and the 1.0.0 release.'],
  '/about': ['About', 'MarkForge is an open-source Markdown platform engineered like systems software.'],
  '/contributing': ['Contributing', 'Contributions should preserve public APIs, update tests, and keep the project self-contained.']
};
export function App() { const route = useRoute(); let page = route === '/' ? <Home/> : route === '/playground' ? <Playground/> : route === '/documentation' ? <Documentation/> : route === '/examples' ? <Examples/> : route === '/benchmarks' ? <Benchmarks/> : route === '/architecture' ? <Architecture/> : copy[route] ? <SimplePage title={copy[route][0]} body={copy[route][1]} /> : <SimplePage title="404" body="The requested page does not exist." />; return <Layout>{page}</Layout>; }
